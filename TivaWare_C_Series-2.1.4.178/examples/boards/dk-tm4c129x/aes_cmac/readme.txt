AES128 and AES256 CMAC Demo

Simple demo showing an authentication operation using the AES128 and
AES256 modules in CMAC mode.  A series of test vectors are authenticated.

This module is also capable of CBC-MAC mode, but this has been determined
to be insecure when using variable message lengths.  CMAC is now 
recommended instead by NIST.

Please note that the use of interrupts and uDMA is not required for the
operation of the module.  It is only done for demonstration purposes.

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

This is part of revision 2.1.4.178 of the DK-TM4C129X Firmware Package.
