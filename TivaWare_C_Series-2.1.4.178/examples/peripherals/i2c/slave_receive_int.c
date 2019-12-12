//*****************************************************************************
//
// slave_receive_int.c - Example demonstrating a simple I2C master message
//                       transmission using a slave interrupt when data is
//                       received.
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
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

//*****************************************************************************
//
//! \addtogroup i2c_examples_list
//! <h1>Slave Receive Interrupt (slave_receive_int)</h1>
//!
//! This example shows how to configure a receive interrupt on the slave
//! module.  This includes setting up the I2C0 module for loopback mode as well
//! as configuring the master and slave modules.  Loopback mode internally
//! connects the master and slave data and clock lines together.  The address
//! of the slave module is set to a value so it can receive data from the
//! master.
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - I2C0 peripheral
//! - GPIO Port B peripheral (for I2C0 pins)
//! - I2C0SCL - PB2
//! - I2C0SDA - PB3
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of I2C.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - INT_I2C0 - I2C0SlaveIntHandler
//
//*****************************************************************************

//*****************************************************************************
//
// Set the address for slave module. This is a 7-bit address sent in the
// following format:
//                      [A6:A5:A4:A3:A2:A1:A0:RS]
//
// A zero in the R/S position of the first byte means that the master
// transmits (sends) data to the selected slave, and a one in this position
// means that the master receives data from the slave.
//
//*****************************************************************************
#define SLAVE_ADDRESS           0x3C

//*****************************************************************************
//
// Global variable to hold the I2C data that has been received.
//
//*****************************************************************************
static uint32_t g_ui32DataRx;

//*****************************************************************************
//
// This is a flag that gets set in the interrupt handler to indicate that an
// interrupt occurred.
//
//*****************************************************************************
static bool g_bIntFlag = false;

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
// The interrupt handler for the for I2C0 data slave interrupt.
//
//*****************************************************************************
void
I2C0SlaveIntHandler(void)
{
    //
    // Clear the I2C0 interrupt flag.
    //
    I2CSlaveIntClear(I2C0_BASE);

    //
    // Read the data from the slave.
    //
    g_ui32DataRx = I2CSlaveDataGet(I2C0_BASE);

    //
    // Set a flag to indicate that the interrupt occurred.
    //
    g_bIntFlag = true;
}

//*****************************************************************************
//
// Configure the I2C0 master and slave and connect them using loopback mode.
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
    uint32_t ui32DataTx;

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
                                       SYSCTL_USE_PLL), 25000000);
#else
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
#endif

    //
    // The I2C0 peripheral must be enabled before use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //
    // For this example I2C0 is used with PortB[3:2].  The actual port and
    // pins used may be different on your part, consult the data sheet for
    // more information.  GPIO port B needs to be enabled so these pins can
    // be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    //
    // Select the I2C function for these pins.  This function will also
    // configure the GPIO pins pins for I2C operation, setting them to
    // open-drain operation with weak pull-ups.  Consult the data sheet
    // to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    //
    // Enable loopback mode.  Loopback mode is a built in feature that helps
    // for debug the I2Cx module.  It internally connects the I2C master and
    // slave terminals, which effectively lets you send data as a master and
    // receive data as a slave.  NOTE: For external I2C operation you will need
    // to use external pull-ups that are faster than the internal pull-ups.
    // Refer to the datasheet for more information.
    //
    HWREG(I2C0_BASE + I2C_O_MCR) |= 0x01;

    //
    // Enable the I2C0 interrupt on the processor (NVIC).
    //
    IntEnable(INT_I2C0);

    //
    // Configure and turn on the I2C0 slave interrupt.  The I2CSlaveIntEnableEx()
    // gives you the ability to only enable specific interrupts.  For this case
    // we are only interrupting when the slave device receives data.
    //
    I2CSlaveIntEnableEx(I2C0_BASE, I2C_SLAVE_INT_DATA);

    //
    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    //
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
    I2CMasterInitExpClk(I2C0_BASE, ui32SysClock, false);
#else
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
#endif

    //
    // Enable the I2C0 slave module.
    //
    I2CSlaveEnable(I2C0_BASE);

    //
    // Set the slave address to SLAVE_ADDRESS.  In loopback mode, it's an
    // arbitrary 7-bit number (set in a macro above) that is sent to the
    // I2CMasterSlaveAddrSet function.
    //
    I2CSlaveInit(I2C0_BASE, SLAVE_ADDRESS);

    //
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.  Set the address to SLAVE_ADDRESS
    // (as set in the slave module).  The receive parameter is set to false
    // which indicates the I2C Master is initiating a writes to the slave.  If
    // true, that would indicate that the I2C Master is initiating reads from
    // the slave.
    //
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);

    //
    // Set up the serial console to use for displaying messages.  This is just
    // for this example program and is not needed for proper I2C operation.
    //
    InitConsole();

    //
    // Enable interrupts to the processor.
    //
    IntMasterEnable();

    //
    // Display the example setup on the console.
    //
    UARTprintf("I2C Slave Interrupt Example ->");
    UARTprintf("\n   Module = I2C0");
    UARTprintf("\n   Mode = Receive interrupt on the Slave module");
    UARTprintf("\n   Rate = 100kbps\n\n");

    //
    // Initialize the data to send.
    //
    ui32DataTx = 'I';

    //
    // Indicate the direction of the data.
    //
    UARTprintf("Transferring from: Master -> Slave\n");

    //
    // Display the data that I2C0 is transferring.
    //
    UARTprintf("  Sending: '%c'", ui32DataTx);

    //
    // Place the data to be sent in the data register.
    //
    I2CMasterDataPut(I2C0_BASE, ui32DataTx);

    //
    // Initiate send of single piece of data from the master.  Since the
    // loopback mode is enabled, the Master and Slave units are connected
    // allowing us to receive the same data that we sent out.
    //
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    //
    // Wait for interrupt to occur.
    //
    while(!g_bIntFlag)
    {
    }

    //
    // Display that interrupt was received.
    //
    UARTprintf("\n  Slave Interrupt Received!\n");

    //
    // Display the data that the slave has received.
    //
    UARTprintf("  Received: '%c'\n\n", g_ui32DataRx);

    //
    // Loop forever.
    //
    while(1)
    {
    }
}
