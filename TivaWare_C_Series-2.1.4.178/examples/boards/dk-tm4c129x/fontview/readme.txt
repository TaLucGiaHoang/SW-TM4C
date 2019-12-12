Font Viewer

This example displays the contents of a TivaWare graphics library font
on the DK board's LCD touchscreen.  By default, the application shows a
test font containing ASCII, the Japanese Hiragana and Katakana alphabets,
and a group of Korean Hangul characters.  If an SDCard is installed and
the root directory contains a file named <tt>font.bin</tt>, this file is
opened and used as the display font instead.  In this case, the graphics
library font wrapper feature is used to access the font from the file
system rather than from internal memory.

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
