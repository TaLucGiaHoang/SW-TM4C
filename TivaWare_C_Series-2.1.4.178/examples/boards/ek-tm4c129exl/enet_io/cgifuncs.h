//*****************************************************************************
//
// cgifuncs.c - Helper functions related to CGI script parameter parsing.
//
// Copyright (c) 2008-2017 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 2.1.4.178 of the EK-TM4C129EXL Firmware Package.
//
//*****************************************************************************

#ifndef __CGIFUNCS_H__
#define __CGIFUNCS_H__

//*****************************************************************************
//
// Prototypes of functions exported by this module.
//
//*****************************************************************************
int32_t FindCGIParameter(const char *pcToFind, char *pcParam[],
                         int32_t i32NumParams);
bool IsValidHexDigit(const char cDigit);
unsigned char HexDigit(const char cDigit);
bool DecodeHexEscape(const char *pcEncoded, char *pcDecoded);
uint32_t EncodeFormString(const char *pcDecoded, char *pcEncoded,
                               uint32_t ui32Len);
uint32_t DecodeFormString(const  char *pcEncoded, char *pcDecoded,
                               uint32_t ui32Len);
bool CheckDecimalParam(const char *pcValue, int32_t *pi32Value);
int32_t GetCGIParam(const char *pcName, char *pcParams[], char *pcValue[],
                 int32_t i32NumParams, bool *pbError);

#endif // __CGIFUNCS_H__
