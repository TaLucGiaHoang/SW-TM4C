//*****************************************************************************
//
// usbhmsc.h - Definitions for the USB MSC host driver.
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
// This is part of revision 2.1.4.178 of the Tiva USB Library.
//
//*****************************************************************************

#ifndef __USBHMSC_H__
#define __USBHMSC_H__

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
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

typedef struct tUSBHMSCInstance tUSBHMSCInstance;

//*****************************************************************************
//
// These defines are the the events that will be passed in the \e ui32Event
// parameter of the callback from the driver.
//
//*****************************************************************************
#define MSC_EVENT_OPEN          1
#define MSC_EVENT_CLOSE         2

//*****************************************************************************
//
// The prototype for the USB MSC host driver callback function.
//
//*****************************************************************************
typedef void (*tUSBHMSCCallback)(tUSBHMSCInstance *psMSCInstance,
                                 uint32_t ui32Event,
                                 void *pvEventData);

//*****************************************************************************
//
// Prototypes for the USB MSC host driver APIs.
//
//*****************************************************************************
extern tUSBHMSCInstance * USBHMSCDriveOpen(uint32_t ui32Drive,
                                           tUSBHMSCCallback pfnCallback);
extern void USBHMSCDriveClose(tUSBHMSCInstance *psMSCInstance);
extern int32_t USBHMSCDriveReady(tUSBHMSCInstance *psMSCInstance);
extern int32_t USBHMSCBlockRead(tUSBHMSCInstance *psMSCInstance,
                                uint32_t ui32LBA, uint8_t *pui8Data,
                                uint32_t ui32NumBlocks);
extern int32_t USBHMSCBlockWrite(tUSBHMSCInstance *psMSCInstance,
                                 uint32_t ui32LBA, uint8_t *pui8Data,
                                 uint32_t ui32NumBlocks);
extern uint32_t USBHMSCLPMSleep(tUSBHMSCInstance *psMSCInstance);
extern uint32_t USBHMSCLPMStatus(tUSBHMSCInstance *psMSCInstance);

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBHMSC_H__
