//*****************************************************************************
//
// commands.h - Header file for commandline functions in qs-cloud
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
// This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Forward declarations for command-line operations.
//
//*****************************************************************************
extern int Cmd_help(int argc, char *argv[]);
extern int Cmd_stats(int argc, char *argv[]);
extern int Cmd_activate(int argc, char *argv[]);
extern int Cmd_read(int argc, char *argv[]);
extern int Cmd_clear(int argc, char *argv[]);
extern int Cmd_sync(int argc, char *argv[]);
extern int Cmd_led(int argc, char *argv[]);
extern int Cmd_connect(int argc, char *argv[]);
extern int Cmd_getmac(int argc, char *argv[]);
extern int Cmd_setlocation(int argc, char *argv[]);
extern int Cmd_setproxy(int argc, char *argv[]);
extern int Cmd_setemail(int argc, char *argv[]);
extern int Cmd_alert(int argc, char *argv[]);
extern int Cmd_tictactoe(int argc, char *argv[]);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __COMMANDS_H__
