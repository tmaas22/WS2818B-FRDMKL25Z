/*
 *  spi_io.h
 *  Author: Nelson Lombardo (C) 2015
 *  e-mail: nelson.lombardo@gmail.com
 *  License at the end of file.
 */
 // Modified 2017 by Alex Dean (agdean@ncsu.edu) for teaching FSMs

#ifndef _SPI_IO_H_
#define _SPI_IO_H_

#include "integer.h"        /* Type redefinition for portability */


// Intensity and Chase Controls
#define PATTERN_FREQ_MS	180

		// Used for "White"
#define WHITE_INTENSITY_PERCENT 0
#define BLUE_INTENSITY_PERCENT 0
#define GREEN_INTENSITY_PERCENT 12

		// Used for the Red Bits
#define OTHR_GREEN_INTENSITY 25
#define OTHR_BLUE_INTENSITY 25
#define RED_INTENSITY_PERCENT 2

#define USE_POLL 0

#define SLAVE 0
#define LED_COUNT (51)
#define BITS_PER_LED (24)
#define PATTERN_SIZE (3)
#define PATTERN_BITS (PATTERN_SIZE * BITS_PER_LED - 1)
#define ARRAY_SIZE (BITS_PER_LED * LED_COUNT)

/******************************************************************************
 Public methods
 *****************************************************************************/
extern volatile BYTE my_array[ARRAY_SIZE];
extern volatile BYTE pattern_2[ARRAY_SIZE];
extern volatile BYTE pattern_3[ARRAY_SIZE];
extern volatile int i;

extern volatile BYTE * whichArrayPtr;
		
/**
    \brief Initialize SPI hardware
 */
void SPI_Init (void);

/**
    \brief Read/Write a single byte.
    \param d Byte to send.
    \return Byte that arrived.
 */
void SPI_Write (BYTE d);

/**
    \brief Flush of SPI buffer.
 */
void SPI_Release (void);

/**
    \brief Selecting function in SPI terms, associated with SPI module.
 */
void SPI_CS_Low (void);

/**
    \brief Deselecting function in SPI terms, associated with SPI module.
 */
void SPI_CS_High (void);


/**
    \brief Start a non-blocking timer.
    \param ms Milliseconds.
 */
void SPI_Timer_On (WORD ms);

/**
    \brief Check the status of non-blocking timer.
    \return Status, TRUE if timeout is not reach yet.
 */
BOOL SPI_Timer_Status (void);

/**
    \brief Stop of non-blocking timer. Mandatory.
 */
void SPI_Timer_Off (void);

#endif

/*
The MIT License (MIT)

Copyright (c) 2015 Nelson Lombardo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
