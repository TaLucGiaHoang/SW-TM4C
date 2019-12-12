//*****************************************************************************
//
// soft_spi_master.c - Example demonstrating how to configure SoftSSI.
//
// Copyright (c) 2010-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 2.1.4.178 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/softssi.h"
#include "utils/uartstdio.h"

//*****************************************************************************
//
//! \addtogroup ssi_examples_list
//! <h1>SoftSSI Master (soft_spi_master)</h1>
//!
//! This example shows how to configure the SoftSSI module.  The code will send
//! three characters on the master Tx then polls the receive FIFO until 3
//! characters are received on the master Rx.
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - GPIO Port A peripheral (for SoftSSI pins)
//! - SoftSSIClk - PA2
//! - SoftSSIFss - PA3
//! - SoftSSIRx  - PA4
//! - SoftSSITx  - PA5
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of
//! SoftSSI.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - SysTickIntHandler
//!
//! \note This example provide the same functionality using the same pins as
//! the spi_master example.  As such, it can be used as a guide for how to
//! convert code which uses hardware SSI to the SoftSSI module.
//
//*****************************************************************************

//*****************************************************************************
//
// Number of bytes to send and receive.
//
//*****************************************************************************
#define NUM_SSI_DATA            3

//*****************************************************************************
//
// The persistent state of the SoftSSI peripheral.
//
//*****************************************************************************
tSoftSSI g_sSoftSSI;

//*****************************************************************************
//
// The data buffer that is used as the transmit FIFO.  The size of this buffer
// can be increased or decreased as required to match the transmit buffering
// requirements of your application.
//
//*****************************************************************************
uint16_t g_pui16TxBuffer[16];

//*****************************************************************************
//
// The data buffer that is used as the receive FIFO.  The size of this buffer
// can be increased or decreased as required to match the receive buffering
// requirements of your application.
//
//*****************************************************************************
uint16_t g_pui16RxBuffer[16];

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Call the SoftSSI timer tick.
    //
    SoftSSITimerTick(&g_sSoftSSI);
}

//*****************************************************************************
//
// Configure SoftSSI in SPI mode 0.  This example will send out 3 bytes of
// data, then wait for 3 bytes of data to come in.  This will all be done using
// the polling method.
//
//*****************************************************************************
int
main(void)
{
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
    uint32_t ui32SysClock;
#endif

    uint32_t pui32DataTx[NUM_SSI_DATA];
    uint32_t pui32DataRx[NUM_SSI_DATA];
    uint32_t ui32Index;

    //
    // Set the clocking to run directly from the external crystal/oscillator.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_OSC), 25000000);
#else
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
#endif


    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for SSI operation.
    //
    InitConsole();

    //
    // Display the setup on the console.
    //
    UARTprintf("SoftSSI ->\n");
    UARTprintf("  Data: 8-bit\n\n");

    //
    // For this example SoftSSI is used with PortA[5:2].  The actual port and
    // pins used may be different on your design based on the set of GPIO pins
    // available.  GPIO port A needs to be enabled so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the SoftSSI module.  The size of the FIFO buffers can be
    // changed to accommodate the requirements of your application.  The GPIO
    // pins utilized can also be changed.
    // The pins are assigned as follows:
    //      PA2 - SoftSSICLK
    //      PA3 - SoftSSIFss
    //      PA4 - SoftSSIRx
    //      PA5 - SoftSSITx
    // TODO: change this to select the port/pin you are using.
    // TODO: change the buffer sizes to match your requirements.
    //
    memset(&g_sSoftSSI, 0, sizeof(g_sSoftSSI));
    SoftSSIClkGPIOSet(&g_sSoftSSI, GPIO_PORTA_BASE, GPIO_PIN_2);
    SoftSSIFssGPIOSet(&g_sSoftSSI, GPIO_PORTA_BASE, GPIO_PIN_3);
    SoftSSIRxGPIOSet(&g_sSoftSSI, GPIO_PORTA_BASE, GPIO_PIN_4);
    SoftSSITxGPIOSet(&g_sSoftSSI, GPIO_PORTA_BASE, GPIO_PIN_5);
    SoftSSIRxBufferSet(&g_sSoftSSI, g_pui16RxBuffer,
                       sizeof(g_pui16RxBuffer) / sizeof(g_pui16RxBuffer[0]));
    SoftSSITxBufferSet(&g_sSoftSSI, g_pui16TxBuffer,
                       sizeof(g_pui16TxBuffer) / sizeof(g_pui16TxBuffer[0]));

    //
    // Configure the SoftSSI module.  Use idle clock level low and active low
    // clock (mode 0) and 8-bit data.  You can set the polarity of the SoftSSI
    // clock when the SoftSSI module is idle.  You can also configure what
    // clock edge you want to capture data on.  Please reference the datasheet
    // for more information on the different SPI modes.
    //
    SoftSSIConfigSet(&g_sSoftSSI, SOFTSSI_FRF_MOTO_MODE_0, 8);

    //
    // Enable the SoftSSI module.
    //
    SoftSSIEnable(&g_sSoftSSI);

    //
    // Configure SysTick to provide an interrupt at a 10 KHz rate.  This is
    // used to control the clock rate of the SoftSSI module; the clock rate of
    // the SoftSSI Clk signal will be 1/2 the interrupt rate.
    // TODO: change this to a different timer if SysTick is not available.
    // TODO: change the interrupt rate to adjust the SoftSSI clock rate.
    //
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
    SysTickPeriodSet(ui32SysClock / 10000);
#else
    SysTickPeriodSet(SysCtlClockGet() / 10000);
#endif
    SysTickIntEnable();
    SysTickEnable();

    //
    // Initialize the data to send.
    //
    pui32DataTx[0] = 's';
    pui32DataTx[1] = 'p';
    pui32DataTx[2] = 'i';

    //
    // Display indication that the SoftSSI is transmitting data.
    //
    UARTprintf("Sent:\n  ");

    //
    // Send 3 bytes of data.
    //
    for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
    {
        //
        // Display the data that SSI is transferring.
        //
        UARTprintf("'%c' ", pui32DataTx[ui32Index]);

        //
        // Send the data using the "blocking" put function.  This function
        // will wait until there is room in the send FIFO before returning.
        // This allows you to assure that all the data you send makes it into
        // the send FIFO.
        //
        SoftSSIDataPut(&g_sSoftSSI, pui32DataTx[ui32Index]);
    }

    //
    // Wait until SoftSSI is done transferring all the data in the transmit
    // FIFO.
    //
    while(SoftSSIBusy(&g_sSoftSSI))
    {
    }

    //
    // Display indication that the SoftSSI is receiving data.
    //
    UARTprintf("\nReceived:\n  ");

    //
    // Receive 3 bytes of data.
    //
    for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
    {
        //
        // Receive the data using the "blocking" Get function. This function
        // will wait until there is data in the receive FIFO before returning.
        //
        SoftSSIDataGet(&g_sSoftSSI, &pui32DataRx[ui32Index]);

        //
        // Since we are using 8-bit data, mask off the MSB.
        //
        pui32DataRx[ui32Index] &= 0x00FF;

        //
        // Display the data that SoftSSI received.
        //
        UARTprintf("'%c' ", pui32DataRx[ui32Index]);
    }


    //
    // Return no errors
    //
    return(0);
}
