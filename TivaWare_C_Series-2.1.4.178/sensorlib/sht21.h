//*****************************************************************************
//
// sht21.h - Prototypes for the SHT21 accelerometer driver.
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

#ifndef __SENSORLIB_SHT21_H__
#define __SENSORLIB_SHT21_H__

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
// The structure that defines the internal state of the SHT21 driver.
//
//*****************************************************************************
typedef struct
{
    //
    // The pointer to the I2C master interface instance used to communicate
    // with the SHT21.
    //
    tI2CMInstance *psI2CInst;

    //
    // the I2C address of the SHT21.
    //
    uint8_t ui8Addr;

    //
    // the state of the state machine used while accessing the
    // SHT21.
    //
    uint8_t ui8State;

    //
    // the data buffer used for sending/receiving data to/from the
    // SHT21.
    //
    uint8_t pui8Data[5];

    //
    // the 16 bit raw temperature reading
    //
    uint16_t ui16Temperature;

    //
    // the 16 bit raw humidity reading
    //
    uint16_t ui16Humidity;

    //
    // the function that is called when the current request has
    // completed processing.
    //
    tSensorCallback *pfnCallback;

    //
    // the callback data provided to the callback function.
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
tSHT21;

//*****************************************************************************
//
// Function prototypes.
//
//*****************************************************************************
extern uint_fast8_t SHT21Init(tSHT21 *psInst, tI2CMInstance *psI2CInst,
                              uint_fast8_t ui8I2CAddr,
                              tSensorCallback *pfnCallback,
                              void *pvCallbackData);
extern uint_fast8_t SHT21Read(tSHT21 *psInst, uint_fast8_t ui8Reg,
                              uint8_t *pui8Data, uint_fast16_t ui16Count,
                              tSensorCallback *pfnCallback,
                              void *pvCallbackData);
extern uint_fast8_t SHT21Write(tSHT21 *psInst, uint_fast8_t ui8Reg,
                               const uint8_t *pui8Data,
                               uint_fast16_t ui16Count,
                               tSensorCallback *pfnCallback,
                               void *pvCallbackData);
extern uint_fast8_t SHT21ReadModifyWrite(tSHT21 *psInst, uint_fast8_t ui8Reg,
                                         uint_fast8_t ui8Mask,
                                         uint_fast8_t ui8Value,
                                         tSensorCallback *pfnCallback,
                                         void *pvCallbackData);
extern uint_fast8_t SHT21DataRead(tSHT21 *psInst, tSensorCallback *pfnCallback,
                                  void *pvCallbackData);
extern void SHT21DataTemperatureGetRaw(tSHT21 *psInst,
                                       uint16_t *pui16Temperature);
extern void SHT21DataTemperatureGetFloat(tSHT21 *psInst, float *pfTemperature);
extern void SHT21DataHumidityGetRaw(tSHT21 *psInst, uint16_t *pui16Humidity);
extern void SHT21DataHumidityGetFloat(tSHT21 *psInst, float *pfHumidity);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __SENSORLIB_SHT21_H__
