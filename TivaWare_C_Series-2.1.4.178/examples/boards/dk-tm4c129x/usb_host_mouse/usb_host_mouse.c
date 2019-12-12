//*****************************************************************************
//
// usb_host_mouse.c - An example using that supports a mouse.
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the DK-TM4C129X Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhhid.h"
#include "usblib/host/usbhhidmouse.h"
#include "drivers/pinout.h"
#include "mouse_ui.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>USB Host mouse example(usb_host_mouse)</h1>
//!
//! This example application demonstrates how to support a USB mouse using
//! the DK-TM4C129X development board.  This application supports only a
//! standard mouse HID device, but can report on the types of other devices
//! that are connected without having the ability to access them.
//! The bottom left status bar reports the
//! type of device attached.  The user interface for the application is
//! handled in the mouse_ui.c file while the usb_host_mouse.c file
//! handles start up and the USB interface.
//!
//! The application can be recompiled to run using and external USB phy to
//! implement a high speed host using an external USB phy.  To use the external
//! phy the application must be built with \b USE_ULPI defined.  This disables
//! the internal phy and the connector on the DK-TM4C129X board and enables the
//! connections to the external ULPI phy pins on the DK-TM4C129X board.
//
//*****************************************************************************

//*****************************************************************************
//
// The size of the host controller's memory pool in bytes.
//
//*****************************************************************************
#define HCD_MEMORY_SIZE         128

//*****************************************************************************
//
// The global application status structure.
//
//*****************************************************************************
tMouseStatus g_sStatus;

//*****************************************************************************
//
// The memory pool to provide to the Host controller driver.
//
//*****************************************************************************
uint8_t g_pui8HCDPool[HCD_MEMORY_SIZE * MAX_USB_DEVICES];

//*****************************************************************************
//
// Declare the USB Events driver interface.
//
//*****************************************************************************
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

//*****************************************************************************
//
// The global that holds all of the host drivers in use in the application.
// In this case, only the HID class is loaded.
//
//*****************************************************************************
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_sUSBHIDClassDriver,
    &g_sUSBEventDriver
};

//*****************************************************************************
//
// This global holds the number of class drivers in the g_ppHostClassDrivers
// list.
//
//*****************************************************************************
static const uint32_t g_ui32NumHostClassDrivers =
                  sizeof(g_ppHostClassDrivers) / sizeof(tUSBHostClassDriver *);

//*****************************************************************************
//
// The global value used to store the mouse instance value.
//
//*****************************************************************************
static tUSBHMouse *g_psMouse;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// This enumerated type is used to hold the states of the mouse.
//
//*****************************************************************************
enum
{
    //
    // No device is present.
    //
    eStateNoDevice,

    //
    // Mouse has been detected and needs to be initialized in the main loop.
    //
    eStateMouseInit,

    //
    // Mouse is connected and waiting for events.
    //
    eStateMouseConnected,
}
g_iMouseState;

//*****************************************************************************
//
// This is the callback from the USB HID mouse handler.
//
// pvCBData is ignored by this function.
// ui32Event is one of the valid events for a mouse device.
// ui32MsgParam is defined by the event that occurs.
// pvMsgData is a pointer to data that is defined by the event that
// occurs.
//
// This function will be called to inform the application when a mouse has
// been plugged in or removed and any time a mouse movement or button pressed
// has occurred.
//
// This function will return 0.
//
//*****************************************************************************
void
MouseCallback(tUSBHMouse *psMouse, uint32_t ui32Event, uint32_t ui32MsgParam,
              void *pvMsgData)
{
    int32_t i32Pos;

    switch(ui32Event)
    {
        //
        // New mouse detected.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Proceed to the eStateMouseInit state so that the main loop
            // can finish initializing the mouse since USBHMouseInit() cannot
            // be called from within a callback.
            //
            g_iMouseState = eStateMouseInit;

            break;
        }

        //
        // Mouse has been unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Change the state so that the main loop knows that a device is no
            // longer present.
            //
            g_iMouseState = eStateNoDevice;

            //
            // Need to clear out any held buttons.
            //
            g_sStatus.ui32Buttons = 0;

            g_sStatus.bUpdate = true;

            break;
        }

        //
        // New button press detected.
        //
        case USBH_EVENT_HID_MS_PRESS:
        {
            //
            // Save the new button that was pressed.
            //
            g_sStatus.ui32Buttons |= ui32MsgParam;

            g_sStatus.bUpdate = true;

            break;
        }

        //
        // A button was released on a HID mouse.
        //
        case USBH_EVENT_HID_MS_REL:
        {
            //
            // Remove the button from the pressed state.
            //
            g_sStatus.ui32Buttons &= ~ui32MsgParam;

            g_sStatus.bUpdate = true;

            break;
        }

        //
        // The HID mouse detected movement in the X direction.
        //
        case USBH_EVENT_HID_MS_X:
        {
            //
            // Update the cursor X position.
            // This has to be calculated as an intermediate signed value
            // because the position can go negative with fast movements.
            //
            i32Pos = g_sStatus.ui32XPos + (int8_t)ui32MsgParam;

            if(i32Pos < 0)
            {
                g_sStatus.ui32XPos = MOUSE_MIN_X;
            }
            else
            {
                g_sStatus.ui32XPos = (uint32_t)i32Pos;
            }

            if(g_sStatus.ui32XPos < MOUSE_MIN_X)
            {
                g_sStatus.ui32XPos = MOUSE_MIN_X;
            }

            if(g_sStatus.ui32XPos > MOUSE_MAX_X)
            {
                g_sStatus.ui32XPos = MOUSE_MAX_X;
            }

            g_sStatus.bUpdate = true;

            break;
        }

        //
        // The HID mouse detected movement in the Y direction.
        //
        case USBH_EVENT_HID_MS_Y:
        {
            //
            // Update the cursor Y position.
            // This has to be calculated as an intermediate signed value
            // because the position can go negative with fast movements.
            //
            i32Pos = (int32_t)g_sStatus.ui32YPos + (int8_t)ui32MsgParam;

            if(i32Pos < 0)
            {
                g_sStatus.ui32YPos = MOUSE_MIN_Y;
            }
            else
            {
                g_sStatus.ui32YPos = (uint32_t)i32Pos;
            }

            if(g_sStatus.ui32YPos < MOUSE_MIN_Y)
            {
                g_sStatus.ui32YPos = MOUSE_MIN_Y;
            }

            if(g_sStatus.ui32YPos > MOUSE_MAX_Y)
            {
                g_sStatus.ui32YPos = MOUSE_MAX_Y;
            }

            g_sStatus.bUpdate = true;

            break;
        }
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// The main routine for handling the USB mouse.
//
//*****************************************************************************
void
MouseMain(void)
{
    switch(g_iMouseState)
    {
        //
        // This state is entered when they mouse is first detected.
        //
        case eStateMouseInit:
        {
            //
            // Initialized the newly connected mouse.
            //
            USBHMouseInit(g_psMouse);

            //
            // Proceed to the mouse connected state.
            //
            g_iMouseState = eStateMouseConnected;

            break;
        }
        case eStateMouseConnected:
        default:
        {
            if(g_sStatus.bUpdate == true)
            {
                g_sStatus.bUpdate = false;

                UIUpdateStatus();
            }
            break;
        }
    }
}

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the mouse device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
//*****************************************************************************
void
USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    switch(pEventInfo->ui32Event)
    {
        case USB_EVENT_UNKNOWN_CONNECTED:
        case USB_EVENT_CONNECTED:
        {
            //
            // Save the device instance data.
            //
            g_sStatus.ui32Instance = pEventInfo->ui32Instance;
            g_sStatus.bConnected = true;

            //
            // Update the port status for the new device.
            //
            UIUpdateStatus();

            break;
        }
        //
        // A device has been unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Device is no longer connected.
            //
            g_sStatus.bConnected = false;

            //
            // Update the port status for the new device.
            //
            UIUpdateStatus();

            break;
        }
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// The main application loop.
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32SysClock, ui32PLLRate;
#ifdef USE_ULPI
    uint32_t ui32Setting;
#endif

    //
    // Set the application to run at 120 MHz with a PLL frequency of 480 MHz.
    //
    ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                           SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                           SYSCTL_CFG_VCO_480), 120000000);

    //
    // Set the part pin out appropriately for this device.
    //
    PinoutSet();

#ifdef USE_ULPI
    //
    // Switch the USB ULPI Pins over.
    //
    USBULPIPinoutSet();

    //
    // Enable USB ULPI with high speed support.
    //
    ui32Setting = USBLIB_FEATURE_ULPI_HS;
    USBOTGFeatureSet(0, USBLIB_FEATURE_USBULPI, &ui32Setting);

    //
    // Setting the PLL frequency to zero tells the USB library to use the
    // external USB clock.
    //
    ui32PLLRate = 0;
#else
    //
    // Save the PLL rate used by this application.
    //
    SysCtlVCOGet(SYSCTL_XTAL_25MHZ, &ui32PLLRate);
#endif

    //
    // Initialize the connection status.
    //
    g_sStatus.bConnected = false;
    g_sStatus.ui32Buttons = 0;

    //
    // Enable Clocking to the USB controller.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Enable Interrupts
    //
    ROM_IntMasterEnable();

    //
    // Initialize the USB stack mode and pass in a mode callback.
    //
    USBStackModeSet(0, eUSBModeHost, 0);

    //
    // Register the host class drivers.
    //
    USBHCDRegisterDrivers(0, g_ppHostClassDrivers, g_ui32NumHostClassDrivers);

    //
    // Open an instance of the mouse driver.  The mouse does not need
    // to be present at this time, this just save a place for it and allows
    // the applications to be notified when a mouse is present.
    //
    g_psMouse = USBHMouseOpen(MouseCallback, 0, 0);

    //
    // Initialize the power configuration. This sets the power enable signal
    // to be active high and does not enable the power fault.
    //
    USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);

    //
    // Tell the USB library the CPU clock and the PLL frequency.  This is a
    // new requirement for TM4C129 devices.
    //
    USBHCDFeatureSet(0, USBLIB_FEATURE_CPUCLK, &ui32SysClock);
    USBHCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &ui32PLLRate);

    //
    // Initialize the USB controller for Host mode.
    //
    USBHCDInit(0, g_pui8HCDPool, sizeof(g_pui8HCDPool));

    //
    // Initialize the UI elements.
    //
    UIInit(ui32SysClock);

    //
    // The main loop for the application.
    //
    while(1)
    {
        //
        // Call the USB library to let non-interrupt code run.
        //
        USBHCDMain();

        //
        // Call the mouse and mass storage main routines.
        //
        MouseMain();
    }
}
