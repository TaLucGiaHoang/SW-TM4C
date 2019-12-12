//*****************************************************************************
//
// display_task.h - Prototypes for the display task.
//
// Copyright (c) 2009-2017 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __DISPLAY_TASK_H__
#define __DISPLAY_TASK_H__

//*****************************************************************************
//
// Prototypes for the display task.
//
//*****************************************************************************
extern void DisplayImage(uint32_t ui16X, uint32_t ui16Y,
                         const uint8_t *pui8Image);
extern void DisplayString(uint32_t ui16X, uint32_t ui16Y,
                          const char *pcString);
extern void DisplayMove(uint32_t ui16X, uint32_t ui16Y);
extern void DisplayDraw(uint32_t ui16X, uint32_t ui16Y);
extern uint32_t DisplayTaskInit(void);

#endif // __DISPLAY_TASK_H__
