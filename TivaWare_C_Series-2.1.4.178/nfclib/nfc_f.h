//*****************************************************************************
//
// nfc_f.h - Type F (Felica) NFC Header
//
// Copyright (c) 2014-2017 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 2.1.4.178 of the Tiva Firmware Development Package.
//
//*****************************************************************************
#ifndef __NFC_F_H__
#define __NFC_F_H__

#include "types.h"

//*****************************************************************************
//
// List of Commands
//
//*****************************************************************************
#define	SENSF_REQ_CMD		0x00
#define	SENSF_RES_CMD		0x01

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************
void NFCTypeF_SendSENSF_REQ(void);
void NFCTypeF_SendSENSF_RES(void);
tStatus NFCTypeF_ProcessReceivedData(uint8_t * pui8RxBuffer);
uint8_t * NFCTypeF_GetNFCID2(void);
void NFCTypeF_SetBufferPtr(uint8_t * buffer_ptr);

#endif //__NFC_F_H__
