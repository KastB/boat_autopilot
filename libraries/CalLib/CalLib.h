////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib-Arduino
//
//  Copyright (c) 2014-2015, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of 
//  this software and associated documentation files (the "Software"), to deal in 
//  the Software without restriction, including without limitation the rights to use, 
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
//  Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all 
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _CALLIB_H_
#define _CALLIB_H_

#include <Arduino.h>

#define CALLIB_DATA_VALID_LOW     0xfc // pattern to detect valid config - low byte
#define CALLIB_DATA_VALID_HIGH    0x15 // pattern to detect valid config - high byte

#ifdef __SAM3X8E__
#define CALLIB_START  ((uint32_t *)(IFLASH1_ADDR + IFLASH1_SIZE - IFLASH1_PAGE_SIZE))
#endif

typedef struct
{
  unsigned char validL;                 // should contain the valid pattern if a good config
  unsigned char validH;                 // should contain the valid pattern if a good config
  unsigned char magValid;               // true if data valid
  unsigned char pad;
  float magMin[3];                      // min values
  float magMax[3];                      // max values
  float rotRef[4];						//quaternion, that normalizes to north
} CALLIB_DATA;

//  erases any current data in the EEPROM
void calLibErase(byte device);

//  writes new data to the EEPROM
void calLibWrite(byte device, CALLIB_DATA * calData);

//  reads existing data and returns true if valid else false in not.
boolean calLibRead(byte device, CALLIB_DATA * calData);



#endif // _CALLIB_H_
