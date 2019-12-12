//*****************************************************************************
//
// boot_demo_emac_flash.c - Ethernet boot loader example application.
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

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_flash.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/flash.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "utils/ustdlib.h"
#include "utils/lwiplib.h"
#include "utils/swupdate.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "drivers/frame.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "drivers/pinout.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Ethernet Boot Loader Demo (boot_demo_emac_flash)</h1>
//!
//! An example to demonstrate the use of remote update signaling with the
//! flash-based Ethernet boot loader.  This application configures the Ethernet
//! controller and acquires an IP address which is displayed on the screen
//! along with the board's MAC address.  It then listens for a ``magic packet''
//! telling it that a firmware upgrade request is being made and, when this
//! packet is received, transfers control into the boot loader to perform the
//! upgrade.
//!
//! This application is intended for use with flash-based ethernet boot
//! loader (boot_emac_flash).
//! 
//! The link address for this application is set to 0x4000, the link address
//! has to be multiple of the flash erase block size(16KB=0x4000).
//! You may change this address to a 16KB boundary higher than the last
//! address occupied by the boot loader binary as long as you also rebuild the
//! boot loader itself after modifying its bl_config.h file to set
//! APP_START_ADDRESS to the same value.
//!
//! The boot_demo_flash application can be used along with this application to
//! easily demonstrate that the boot loader is actually updating the on-chip
//! flash.
//!
//*****************************************************************************

//*****************************************************************************
//
// The number of SysTick ticks per second.
//
//*****************************************************************************
#define TICKS_PER_SECOND 100

//*****************************************************************************
//
// A global flag used to indicate if a remote firmware update request has been
// received.
//
//*****************************************************************************
static volatile bool g_bFirmwareUpdate = false;

//*****************************************************************************
//
// Buffers used to hold the Ethernet MAC and IP addresses for the board.
//
//*****************************************************************************
#define SIZE_MAC_ADDR_BUFFER 32
#define SIZE_IP_ADDR_BUFFER 32
char g_pcMACAddr[SIZE_MAC_ADDR_BUFFER];
char g_pcIPAddr[SIZE_IP_ADDR_BUFFER];

//*****************************************************************************
//
// Forward reference to various widget structures.
//
//*****************************************************************************
extern tCanvasWidget g_sBackground;

//*****************************************************************************
//
// The canvas widget used to display the board's Ethernet IP address.
//
//*****************************************************************************
Canvas(g_sIPAddr, &g_sBackground, 0, 0,
       &g_sKentec320x240x16_SSD2119, 10, 180, 300, 20,
       (CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT),
       ClrBlack, 0, ClrWhite, g_psFontCmss18b, g_pcIPAddr, 0, 0);

//*****************************************************************************
//
// The canvas widget used to display the board's Ethernet MAC address.  This
// is required if using the Ethernet boot loader.
//
//*****************************************************************************
Canvas(g_sMACAddr, &g_sBackground, &g_sIPAddr, 0,
       &g_sKentec320x240x16_SSD2119, 10, 200, 300, 20,
       (CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT),
       ClrBlack, 0, ClrWhite, g_psFontCmss18b, g_pcMACAddr, 0, 0);

//*****************************************************************************
//
// The canvas widget used to display the current status.
//
//*****************************************************************************
Canvas(g_sStatus, &g_sBackground, &g_sMACAddr, 0,
       &g_sKentec320x240x16_SSD2119,  60, 110, 200, 40,
       (CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT),
       ClrBlack, 0, ClrWhite, g_psFontCmss22b, "Waiting for signal.", 0, 0);

//*****************************************************************************
//
// The canvas widget acting as the background to the display.
//
//*****************************************************************************
Canvas(g_sBackground, WIDGET_ROOT, 0, &g_sStatus,
       &g_sKentec320x240x16_SSD2119, 10, 23, 300, (240 - 33),
       CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0);

//*****************************************************************************
//
// The system clock frequency.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

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
// This is the handler for this SysTick interrupt.  We use this to provide the
// required timer call to the lwIP stack.
//
//*****************************************************************************
void
SysTickHandler(void)
{
    //
    // Call the lwIP timer.
    //
    lwIPTimer(1000 / TICKS_PER_SECOND);
}

//*****************************************************************************
//
// This function is called by the software update module whenever a remote
// host requests to update the firmware on this board.  We set a flag that
// will cause the main loop to exit and transfer control to the bootloader.
//
// IMPORTANT:
// Note that this callback is made in interrupt context and, since it is not
// permitted to transfer control to the boot loader from within an interrupt,
// we can't just call SoftwareUpdateBegin() here.
//
//*****************************************************************************
void SoftwareUpdateRequestCallback(void)
{
    g_bFirmwareUpdate = true;
}

//*****************************************************************************
//
// Perform the initialization steps required to start up the Ethernet controller
// and lwIP stack.
//
//*****************************************************************************
void
SetupForEthernet(void)
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACAddr[6];

    //
    // Configure SysTick for a 100Hz interrupt.
    //
    ROM_SysTickPeriodSet(g_ui32SysClock / TICKS_PER_SECOND);
    ROM_SysTickEnable();
    ROM_SysTickIntEnable();

    //
    // Get the MAC address from the UART0 and UART1 registers in NV ram.
    //
    ROM_FlashUserGet(&ui32User0, &ui32User1);

    //
    // Convert the 24/24 split MAC address from NV ram into a MAC address
    // array.
    //
    pui8MACAddr[0] = ui32User0 & 0xff;
    pui8MACAddr[1] = (ui32User0 >> 8) & 0xff;
    pui8MACAddr[2] = (ui32User0 >> 16) & 0xff;
    pui8MACAddr[3] = ui32User1 & 0xff;
    pui8MACAddr[4] = (ui32User1 >> 8) & 0xff;
    pui8MACAddr[5] = (ui32User1 >> 16) & 0xff;

    //
    // Format this address into the string used by the relevant widget.
    //
    usnprintf(g_pcMACAddr, SIZE_MAC_ADDR_BUFFER,
              "MAC: %02X-%02X-%02X-%02X-%02X-%02X",
              pui8MACAddr[0], pui8MACAddr[1], pui8MACAddr[2], pui8MACAddr[3],
              pui8MACAddr[4], pui8MACAddr[5]);

    //
    // Remember that we don't have an IP address yet.
    //
    usnprintf(g_pcIPAddr, SIZE_IP_ADDR_BUFFER, "IP: Not assigned");

    //
    // Initialize the lwIP TCP/IP stack.
    //
    lwIPInit(g_ui32SysClock, pui8MACAddr, 0, 0, 0, IPADDR_USE_DHCP);

    //
    // Start the remote software update module.
    //
    SoftwareUpdateInit(SoftwareUpdateRequestCallback);
}

//*****************************************************************************
//
// Initialize UART0 and set the appropriate communication parameters.
//
//*****************************************************************************
void
SetupForUART(void)
{
    //
    // Enable UART0 peripheral.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure the UART for 115200, n, 8, 1
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
                            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                            UART_CONFIG_WLEN_8));

    //
    // Enable the UART operation.
    //
    ROM_UARTEnable(UART0_BASE);
}

//*****************************************************************************
//
// Enable the USB controller
//
//*****************************************************************************
void
SetupForUSB(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    ROM_USBClockEnable(USB0_BASE, 4, USB_CLOCK_INTERNAL);
}

//*****************************************************************************
//
// Demonstrate the use of the boot loader.
//
//*****************************************************************************
int
main(void)
{
    tContext sContext;
    uint32_t ui32IPAddr;

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    //
    // Configure the device pins.
    //
    PinoutSet();

    //
    // Initialize the display driver.
    //
    Kentec320x240x16_SSD2119Init(g_ui32SysClock);

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

    //
    // Draw the application frame.
    //
    FrameDraw(&sContext, "boot-demo-emac-flash");

    //
    // Initialize the peripherals that each of the boot loader flavors
    // supports.  Although this example is only intended for use with the
    // Ethernet boot loader, we initialize the other two peripherals too just
    // in case it is used with the USB or serial boot loaders.
    //
    SetupForUART();
    SetupForEthernet();
    SetupForUSB();

    //
    // Add the compile-time defined widgets to the widget tree.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sBackground);

    //
    // Paint the widget tree to make sure they all appear on the display.
    //
    WidgetPaint(WIDGET_ROOT);

    //
    // We don't have an IP address yet so clear the variable to tell us to
    // check until we are assigned one.
    //
    ui32IPAddr = 0;

    //
    // Loop forever, processing widget messages.
    //
    while(!g_bFirmwareUpdate)
    {
        //
        // Do we have an IP address yet? If not, check to see if we've been
        // assigned one since the last time we checked.
        //
        if(ui32IPAddr == 0 || ui32IPAddr == 0xFFFFFFFF)
        {
            //
            // What is our current IP address?
            //
            ui32IPAddr = lwIPLocalIPAddrGet();

            //
            // If it's non zero, update the display.
            //
            if(ui32IPAddr!= 0 && ui32IPAddr != 0xFFFFFFFF)
            {
                usprintf(g_pcIPAddr, "IP: %d.%d.%d.%d",
                         ui32IPAddr & 0xff, (ui32IPAddr >> 8) & 0xff,
                         (ui32IPAddr >> 16) & 0xff,  ui32IPAddr >> 24);
                WidgetPaint((tWidget *)&g_sIPAddr);
            }
        }

        //
        // Process any messages from or for the widgets.
        //
        WidgetMessageQueueProcess();
    }

    //
    // If we drop out, the user has sent "Magic Package" via LMFlash
    // so we tidy up and transfer control to the boot loader.
    //

    //
    // Tell the user that we got their instruction.
    //
    CanvasTextSet(&g_sStatus, "Updating...");
    WidgetPaint((tWidget *)&g_sStatus);

    //
    // Process all remaining messages on the queue (including the paint message
    // we just posted).
    //
    WidgetMessageQueueProcess();

    //
    // Transfer control to the bootloader.
    //
    SoftwareUpdateBegin(g_ui32SysClock);

    //
    // The previous function never returns but we need to stick in a return
    // code here to keep the compiler from generating a warning.
    //
    return(0);
}
