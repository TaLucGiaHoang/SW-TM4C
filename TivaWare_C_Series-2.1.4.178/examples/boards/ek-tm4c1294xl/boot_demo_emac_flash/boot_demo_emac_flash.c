//*****************************************************************************
//
// boot_demo_emac_flash.c - Boot Loader ethernet Demo.
//
// Copyright (c) 2008-2017 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/flash.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "utils/lwiplib.h"
#include "utils/swupdate.h"
#include "utils/ustdlib.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Boot Loader ethernet Demo (boot_demo_emac_flash)</h1>
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
//! loader (boot_emac).
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
//
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
// A global we use to update the system clock frequency
//
//*****************************************************************************
volatile uint32_t g_ui32SysClockFreq;

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
SysTickIntHandler(void)
{
    //
    // Call the lwIP timer.
    //
    lwIPTimer(1000 / TICKS_PER_SECOND);
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
    // We need to make sure that UART0 and its associated GPIO port are
    // enabled before we pass control to the boot loader.  The serial boot
    // loader does not enable or configure these peripherals for us if we
    // enter it via its SVC vector.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Set GPIO PA0 and PA1 as UART.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115200, n, 8, 1
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, g_ui32SysClockFreq, 115200,
                            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_WLEN_8));

    //
    // Enable the UART operation.
    //
    ROM_UARTEnable(UART0_BASE);
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
    ROM_SysTickPeriodSet(g_ui32SysClockFreq / TICKS_PER_SECOND);
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
    lwIPInit(g_ui32SysClockFreq, pui8MACAddr, 0, 0, 0, IPADDR_USE_DHCP);

    //
    // Start the remote software update module.
    //
    SoftwareUpdateInit(SoftwareUpdateRequestCallback);
}

//*****************************************************************************
//
// Enable the USB controller
//
//*****************************************************************************
void
SetupForUSB(void)
{
    //
    // The USB boot loader takes care of all required USB initialization so,
    // if the application itself doesn't need to use the USB controller, we
    // don't actually need to enable it here.  The only requirement imposed by
    // the USB boot loader is that the system clock is running from the PLL
    // when the boot loader is entered.
    //
}

//*****************************************************************************
//
// A simple application demonstrating use of the boot loader,
//
//*****************************************************************************
int
main(void)
{

    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPULazyStackingEnable();

    //
    // Set the system clock to run at 120MHz from the PLL
    //
    g_ui32SysClockFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    //
    // Initialize the peripherals that each of the boot loader flavours
    // supports.  Since this example is intended for use with any of the
    // boot loaders and we don't know which is actually in use, we cover all
    // bases and initialize for serial, Ethernet and USB use here.
    //
    SetupForUART();
    SetupForEthernet();
    SetupForUSB();

    //
    // Configure Port N pin 1 as output.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)));
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    //
    // If the Switch SW1 is not pressed then blink the LED D1 at 1 Hz rate
    // On switch SW press detection exit the blinking program and jump to
    // the flash boot loader.
    //
    while(!g_bFirmwareUpdate)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
        SysCtlDelay(g_ui32SysClockFreq / 6);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        SysCtlDelay(g_ui32SysClockFreq / 6);
    }

    //
    // Before passing control make sure that the LED is turned OFF.
    //
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
    
    //
    // Pass control to whichever flavour of boot loader the board is configured
    // with.
    //
    SoftwareUpdateBegin(g_ui32SysClockFreq);

    //
    // The previous function never returns but we need to stick in a return
    // code here to keep the compiler from generating a warning.
    //
    return(0);
}
