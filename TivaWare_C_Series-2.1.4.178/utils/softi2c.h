//*****************************************************************************
//
// softi2c.h - Defines and macros for the SoftI2C.
//
// Copyright (c) 2010-2017 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 2.1.4.178 of the Tiva Utility Library.
//
//*****************************************************************************

#ifndef __SOFTI2C_H__
#define __SOFTI2C_H__

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
//! \addtogroup softi2c_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! This structure contains the state of a single instance of a SoftI2C module.
//
//*****************************************************************************
typedef struct
{
    //
    //! The address of the callback function that is called to simulate the
    //! interrupts that would be produced by a hardware I2C implementation.
    //! This address can be set via a direct structure access or using the
    //! SoftI2CCallbackSet function.
    //
    void (*pfnIntCallback)(void);

    //
    //! The address of the GPIO pin to be used for the SCL signal.  This member
    //! can be set via a direct structure access or using the SoftI2CSCLGPIOSet
    //! function.
    //
    uint32_t ui32SCLGPIO;

    //
    //! The address of the GPIO pin to be used for the SDA signal.  This member
    //! can be set via a direct structure access or using the SoftI2CSDAGPIOSet
    //! function.
    ///
    uint32_t ui32SDAGPIO;

    //
    //! The flags that control the operation of the SoftI2C module.  This
    //! member should not be accessed or modified by the application.
    //
    uint8_t ui8Flags;

    //
    //! The slave address that is currently being accessed.  This member should
    //! not be accessed or modified by the application.
    //
    uint8_t ui8SlaveAddr;

    //
    //! The data that is currently being transmitted or received.  This member
    //! should not be accessed or modified by the application.
    //
    uint8_t ui8Data;

    //
    //! The current state of the SoftI2C state machine.  This member should not
    //! be accessed or modified by the application.
    //
    uint8_t ui8State;

    //
    //! The number of bits that have been transmitted and received in the
    //! current frame.  This member should not be accessed or modified by the
    //! application.
    //
    uint8_t ui8CurrentBit;

    //
    //! The set of virtual interrupts that should be sent to the callback
    //! function.  This member should not be accessed or modified by the
    //! application.
    //
    uint8_t ui8IntMask;

    //
    //! The set of virtual interrupts that are currently asserted.  This member
    //! should not be accessed or modified by the application.
    //
    uint8_t ui8IntStatus;
}
tSoftI2C;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// SoftI2C commands.
//
//*****************************************************************************
#define SOFTI2C_CMD_SINGLE_SEND 0x00000007
#define SOFTI2C_CMD_SINGLE_RECEIVE                                            \
                                0x00000007
#define SOFTI2C_CMD_BURST_SEND_START                                          \
                                0x00000003
#define SOFTI2C_CMD_BURST_SEND_CONT                                           \
                                0x00000001
#define SOFTI2C_CMD_BURST_SEND_FINISH                                         \
                                0x00000005
#define SOFTI2C_CMD_BURST_SEND_ERROR_STOP                                     \
                                0x00000004
#define SOFTI2C_CMD_BURST_RECEIVE_START                                       \
                                0x0000000b
#define SOFTI2C_CMD_BURST_RECEIVE_CONT                                        \
                                0x00000009
#define SOFTI2C_CMD_BURST_RECEIVE_FINISH                                      \
                                0x00000005
#define SOFTI2C_CMD_BURST_RECEIVE_ERROR_STOP                                  \
                                0x00000004

//*****************************************************************************
//
// SoftI2C error status.
//
//*****************************************************************************
#define SOFTI2C_ERR_NONE        0x00000000
#define SOFTI2C_ERR_ADDR_ACK    0x00000004
#define SOFTI2C_ERR_DATA_ACK    0x00000008

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern bool SoftI2CBusy(tSoftI2C *psI2C);
extern void SoftI2CCallbackSet(tSoftI2C *psI2C, void (*pfnCallback)(void));
extern void SoftI2CControl(tSoftI2C *psI2C, uint32_t ui32Cmd);
extern uint32_t SoftI2CDataGet(tSoftI2C *psI2C);
extern void SoftI2CDataPut(tSoftI2C *psI2C, uint8_t ui8Data);
extern uint32_t SoftI2CErr(tSoftI2C *psI2C);
extern void SoftI2CInit(tSoftI2C *psI2C);
extern void SoftI2CIntClear(tSoftI2C *psI2C);
extern void SoftI2CIntDisable(tSoftI2C *psI2C);
extern void SoftI2CIntEnable(tSoftI2C *psI2C);
extern bool SoftI2CIntStatus(tSoftI2C *psI2C, bool bMasked);
extern void SoftI2CSCLGPIOSet(tSoftI2C *psI2C, uint32_t ui32Base,
                              uint8_t ui8Pin);
extern void SoftI2CSDAGPIOSet(tSoftI2C *psI2C, uint32_t ui32Base,
                              uint8_t ui8Pin);
extern void SoftI2CSlaveAddrSet(tSoftI2C *psI2C, uint8_t ui8SlaveAddr,
                                bool bReceive);
extern void SoftI2CTimerTick(tSoftI2C *psI2C);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __SOFTI2C_H__
