//*****************************************************************************
//
// uart_handler.c
//
// Copyright (c) 2006-2017 Texas Instruments Incorporated.  All rights reserved.
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

#include <stdint.h>
#ifdef __WIN32
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

//*****************************************************************************
//
//! \addtogroup uart_handler UART Handler API
//! This section describes the functions that are responsible for handling the
//! transfer of data over the UART port.  These functions use Microsoft Windows
//! APIs to communicate with the UART.
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! This variable holds the open handle to the UART in use by this
//! application.
//
//*****************************************************************************
#ifdef __WIN32
static HANDLE g_hComPort;
#else
static int32_t g_i32ComPort = -1;
#endif

//*****************************************************************************
//
//! OpenUART() opens the UART port.
//!
//! \param pcComPort is the text representation of the COM port to be
//!     opened. "COM1" is the valid string to use to open COM port 1 on the
//!     host.
//! \param ui32BaudRate is the baud rate to configure the UART to use.
//!
//! This function is used to open the host UART with the given baud rate.  The
//! rest of the settings are fixed at No Parity, 8 data bits and 1 stop bit.
//!
//! \return The function returns zero to indicated success while any non-zero
//!     value indicates a failure.
//
//*****************************************************************************
int32_t
OpenUART(char *pcComPort, uint32_t ui32BaudRate)
{
#ifdef __WIN32
    DCB sDCB;
    COMMTIMEOUTS sCommTimeouts;

    g_hComPort = CreateFile(pcComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			    OPEN_EXISTING, 0, NULL);

    if(g_hComPort == INVALID_HANDLE_VALUE)
    {
        return(-1);
    }

    if(GetCommState(g_hComPort, &sDCB) == 0)
    {
        return(-1);
    }

    sDCB.BaudRate = ui32BaudRate;
    sDCB.ByteSize = 8;
    sDCB.Parity = NOPARITY;
    sDCB.StopBits = ONESTOPBIT;
    sDCB.fAbortOnError = TRUE;
    sDCB.fOutxCtsFlow = FALSE;
    sDCB.fOutxDsrFlow = FALSE;
    sDCB.fDtrControl = DTR_CONTROL_ENABLE;
    if(SetCommState(g_hComPort, &sDCB) == 0)
    {
        return(-1);
    }

    if(GetCommTimeouts(g_hComPort, &sCommTimeouts) == 0)
    {
        return(-1);
    }

    sCommTimeouts.ReadIntervalTimeout = 8000;
    sCommTimeouts.ReadTotalTimeoutConstant = 8000;
    sCommTimeouts.ReadTotalTimeoutMultiplier = 8000;
    sCommTimeouts.WriteTotalTimeoutConstant = 8000;
    sCommTimeouts.WriteTotalTimeoutMultiplier = 8000;

    if(SetCommTimeouts(g_hComPort, &sCommTimeouts) == 0)
    {
        return(-1);
    }
    return(0);
#else
    struct termios sOptions;

    g_i32ComPort = open(pcComPort, O_RDWR | O_NOCTTY | O_NDELAY);
    if(g_i32ComPort == -1)
    {
        return(-1);
    }

    fcntl(g_i32ComPort, F_SETFL, 0);

    tcgetattr(g_i32ComPort, &sOptions);

    if(ui32BaudRate == 9600)
    {
        cfsetispeed(&sOptions, B9600);
        cfsetospeed(&sOptions, B9600);
    }
    else if(ui32BaudRate == 19200)
    {
        cfsetispeed(&sOptions, B19200);
        cfsetospeed(&sOptions, B19200);
    }
    else if(ui32BaudRate == 38400)
    {
        cfsetispeed(&sOptions, B38400);
        cfsetospeed(&sOptions, B38400);
    }
    else if(ui32BaudRate == 57600)
    {
        cfsetispeed(&sOptions, B57600);
        cfsetospeed(&sOptions, B57600);
    }
    else if(ui32BaudRate == 115200)
    {
        cfsetispeed(&sOptions, B115200);
        cfsetospeed(&sOptions, B115200);
    }
    else
    {
        cfsetispeed(&sOptions, B230400);
        cfsetospeed(&sOptions, B230400);
    }

    sOptions.c_cflag |= (CLOCAL | CREAD);

    sOptions.c_cflag &= ~(CSIZE);
    sOptions.c_cflag |= CS8;

    sOptions.c_cflag &= ~(PARENB);
    sOptions.c_cflag &= ~(CSTOPB);

    sOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    sOptions.c_oflag &= ~(OPOST);

    tcsetattr(g_i32ComPort, TCSANOW, &sOptions);

    return(0);
#endif
}

//*****************************************************************************
//
//! CloseUART() closes the UART port.
//!
//! This function closes the UART port that was opened by a call to OpenUART().
//!
//! \returns This function returns zero to indicate success while any non-zero
//! value indicates a failure.
//
//*****************************************************************************
int32_t
CloseUART(void)
{
#ifdef __WIN32
    return(CloseHandle(g_hComPort));
#else
    if(g_i32ComPort != -1)
    {
        close(g_i32ComPort);
    }

    return(0);
#endif
}

//*****************************************************************************
//
//! UARTSendData() sends data over a UART port.
//!
//! \param pui8Data
//!     The buffer to write out to the UART port.
//! \param ui8Size
//!     The number of bytes provided in pui8Data buffer that should be written
//!     out to the port.
//!
//! This function sends ui8Size bytes of data from the buffer pointed to by
//! pui8Data via the UART port that was opened by a call to OpenUART().
//!
//! \return This function returns zero to indicate success while any non-zero
//! value indicates a failure.
//
//*****************************************************************************
int32_t
UARTSendData(uint8_t const *pui8Data, uint8_t ui8Size)
{
#ifdef __WIN32
    unsigned long ulNumBytes;

    //
    // Send the Ack back to the device.
    //
    if(WriteFile(g_hComPort, pui8Data, ui8Size, &ulNumBytes, NULL) == 0)
    {
        return(-1);
    }
    if(ulNumBytes != ui8Size)
    {
        return(-1);
    }
    return(0);
#else
    if(write(g_i32ComPort, pui8Data, ui8Size) != ui8Size)
    {
        return(-1);
    }

    return(0);
#endif
}

//*****************************************************************************
//
//! UARTReceiveData() receives data over a UART port.
//!
//! \param pui8Data is the buffer to read data into from the UART port.
//! \param ui8Size is the number of bytes provided in pui8Data buffer that should
//!     be written with data from the UART port.
//!
//! This function reads back ui8Size bytes of data from the UART port, that was
//! opened by a call to OpenUART(), into the buffer that is pointed to by
//! pui8Data.
//!
//! \return This function returns zero to indicate success while any non-zero
//!     value indicates a failure.
//
//*****************************************************************************
int32_t
UARTReceiveData(uint8_t *pui8Data, uint8_t ui8Size)
{
#ifdef __WIN32
    unsigned long ulNumBytes;

    if(ReadFile(g_hComPort, pui8Data, ui8Size, &ulNumBytes, NULL) == 0)
    {
        return(-1);
    }
    if(ulNumBytes != ui8Size)
    {
        return(-1);
    }
    return(0);
#else
    if(read(g_i32ComPort, pui8Data, ui8Size) != ui8Size)
    {
        return(-1);
    }

    return(0);
#endif
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
