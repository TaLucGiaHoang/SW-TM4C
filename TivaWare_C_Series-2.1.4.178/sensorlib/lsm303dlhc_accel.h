//*****************************************************************************
//
// lsm303dlhc.c - Driver for the ST L3GD20H gyrometer
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
// This is part of revision 2.1.4.178 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#ifndef __SENSORLIB_LSM303DLHC_ACCEL_H__
#define __SENSORLIB_LSM303DLHC_ACCEL_H__

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
// The structure that defines the internal state of the LSM303DLHC driver.
//
//*****************************************************************************
typedef struct
{
    //
    // The pointer to the I2C master interface instance used to communicate
    // with the LSM303DLHC.
    //
    tI2CMInstance *psI2CInst;

    //
    // The I2C address of the LSM303DLHC.
    //
    uint8_t ui8Addr;

    //
    // The state of the state machine used while accessing the LSM303DLHC.
    //
    uint8_t ui8State;

    //
    // The current accelerometer afs_sel setting.
    //
    uint8_t ui8AccelAfsSel;

    //
    // The new accelerometer afs_sel setting, which is used when a register
    // write succeeds.
    //
    uint8_t ui8NewAccelAfsSel;

    //
    // The data buffers used for sending/receiving data to/from the LSM303DLHC.
    //
    uint8_t pui8Data[8];

    //
    // The function that is called when the current request has completed
    // processing.
    //
    tSensorCallback *pfnCallback;

    //
    // The callback data provided to the callback function.
    //
    void *pvCallbackData;

    //
    // A union of structures that are used for read, write and
    // read-modify-write operations.  Since only one operation can be active at
    // a time, it is safe to re-use the memory in this manner.
    //
    union
    {
        //
        // A buffer used to store the write portion of a register read.
        //
        uint8_t pui8Buffer[2];

        //
        // The write state used to write register values.
        //
        tI2CMWrite8 sWriteState;

        //
        // The read-modify-write state used to modify register values.
        //
        tI2CMReadModifyWrite8 sReadModifyWriteState;
    }
    uCommand;
}
tLSM303DLHCAccel;

//*****************************************************************************
//
// Function prototypes.
//
//*****************************************************************************
extern uint_fast8_t LSM303DLHCAccelInit(tLSM303DLHCAccel *psInst,
                                        tI2CMInstance *psI2CInst,
                                        uint_fast8_t ui8I2CAddr,
                                        tSensorCallback *pfnCallback,
                                        void *pvCallbackData);
extern uint_fast8_t LSM303DLHCAccelRead(tLSM303DLHCAccel *psInst,
                                        uint_fast8_t ui8Reg,
                                        uint8_t *pui8Data,
                                        uint_fast16_t ui16Count,
                                        tSensorCallback *pfnCallback,
                                        void *pvCallbackData);
extern uint_fast8_t LSM303DLHCAccelWrite(tLSM303DLHCAccel *psInst,
                                         uint_fast8_t ui8Reg,
                                         const uint8_t *pui8Data,
                                         uint_fast16_t ui16Count,
                                         tSensorCallback *pfnCallback,
                                         void *pvCallbackData);
extern uint_fast8_t LSM303DLHCAccelReadModifyWrite(tLSM303DLHCAccel *psInst,
                                                   uint_fast8_t ui8Reg,
                                                   uint_fast8_t ui8Mask,
                                                   uint_fast8_t ui8Value,
                                                   tSensorCallback *pfnCallbak,
                                                   void *pvCallbackData);
extern uint_fast8_t LSM303DLHCAccelDataRead(tLSM303DLHCAccel *psInst,
                                            tSensorCallback *pfnCallback,
                                            void *pvCallbackData);
extern void LSM303DLHCAccelDataAccelGetRaw(tLSM303DLHCAccel *psInst,
                                           uint_fast16_t *pui16AccelX,
                                           uint_fast16_t *pui16AccelY,
                                           uint_fast16_t *pui16AccelZ);
extern void LSM303DLHCAccelDataAccelGetFloat(tLSM303DLHCAccel *psInst,
                                             float *pfAccelX, float *pfAccelY,
                                             float *pfAccelZ);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __SENSORLIB_LSM303DLHC_ACCEL_H__
