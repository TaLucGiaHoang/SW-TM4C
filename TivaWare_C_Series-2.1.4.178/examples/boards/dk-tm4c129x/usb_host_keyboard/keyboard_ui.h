//*****************************************************************************
//
// keyboard_ui.h - Header for the DK-TM4C129X USB keyboard application UI.
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

#ifndef __KEYBOARD_UI_H__
#define __KEYBOARD_UI_H__

//*****************************************************************************
//
// Status of the device.
//
//*****************************************************************************
typedef struct
{
    //
    // Holds if there is a device connected to this port.
    //
    bool bConnected;

    //
    // Move to a new line.
    //
    bool bNewLine;

    //
    // The instance data for the device if bConnected is true.
    //
    uint32_t ui32Instance;

    //
    // The keyboard modifier flags.
    //
    uint32_t ui32Modifiers;
}
tKeyboardStatus;

extern tKeyboardStatus g_sStatus;


//*****************************************************************************
//
// The ASCII code for a backspace character.
//
//*****************************************************************************
#define ASCII_BACKSPACE 0x08

void UIInit(uint32_t ui32SysClock);
void UIPrintChar(const char cChar);
void UIUpdateStatus(void);

#endif
