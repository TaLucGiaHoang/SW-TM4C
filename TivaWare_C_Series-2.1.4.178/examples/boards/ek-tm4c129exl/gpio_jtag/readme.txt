GPIO JTAG Recovery

This example demonstrates changing the JTAG pins into GPIOs, a with a
mechanism to revert them to JTAG pins.  When first run, the pins remain in
JTAG mode.  Pressing the USR_SW1 button will toggle the pins between JTAG
mode and GPIO mode.  Because there is no debouncing of the push button
(either in hardware or software), a button press will occasionally result
in more than one mode change.

In this example, four pins (PC0, PC1, PC2, and PC3) are switched.

UART0, connected to the ICDI virtual COM port and running at 115,200,
8-N-1, is used to display messages from this application.

-------------------------------------------------------------------------------

Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
Software License Agreement

Texas Instruments (TI) is supplying this software for use solely and
exclusively on TI's microcontroller products. The software is owned by
TI and/or its suppliers, and is protected under applicable copyright
laws. You may not combine this software with "viral" open-source
software in order to form a larger program.

THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, FOR ANY REASON WHATSOEVER.

This is part of revision 2.1.4.178 of the EK-TM4C129EXL Firmware Package.
