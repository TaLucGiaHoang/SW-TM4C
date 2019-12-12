Boot Loader Demo 1

An example to demonstrate the use of a flash-based boot loader.  At
startup, the application will configure the UART and USB peripherals,
and then branch to the boot loader to await the start of an
update.  If using the serial boot loader (boot_serial), the UART will
always be configured at 115,200 baud and does not require the use of
auto-bauding.

This application is intended for use with any of the three flash-based boot
loader flavors (boot_serial or boot_usb) included in the software
release.  To accommodate the largest of these (boot_usb), the link address
is set to 0x2800.  If you are using serial, you may change this
address to a 1KB boundary higher than the last address occupied
by the boot loader binary as long as you also rebuild the boot
loader itself after modifying its bl_config.h file to set APP_START_ADDRESS
to the same value.

The boot_demo2 application can be used along with this application to
easily demonstrate that the boot loader is actually updating the on-chip
flash.

Note that the TM4C123G and other Blizzard-class devices also
support the serial and USB boot loaders in ROM.  To make use of this
function, link your application to run at address 0x0000 in flash and enter
the bootloader using the ROM_UpdateSerial and ROM_UpdateUSB functions 
(defined in rom.h).  This mechanism is used in the utils/swupdate.c 
module when built specifically targeting a suitable Blizzard-class device.

-------------------------------------------------------------------------------

Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
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

This is part of revision 2.1.4.178 of the DK-TM4C123G Firmware Package.
