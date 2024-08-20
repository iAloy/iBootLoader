/**
  @file
  iBootLoader - ialoy_code.cpp

  MIT License

  @copyright
  Copyright (c) 2020-2024 iAloy

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
**/


#ifndef IALOY_CODE_H
#define IALOY_CODE_H


#define ENABLE               true
#define DISABLE              false

#define EEPROM_ENABLE_BIT    7

#define BAUDE_RATE_9600      103
#define BAUDE_RATE_115200    8

#define EEPROM_I2C_ADDRESS   0x50

#define RETURN_CODE_SUCCESS  0
#define RETURN_CODE_FAILURE  1

#define EEPROM_WRITE_DELAY   5

#define LED_DDR              DDRD
#define LED_PORT             PORTD
#define BUILD_IN_LED_PIN     PIND2


#define SIPO_DATA_DDR        DDRB
#define SIPO_CLK_DDR         DDRB
#define SIPO_LATCH_DDR       DDRD

#define SIPO_DATA_PORT       PORTB
#define SIPO_CLK_PORT        PORTB
#define SIPO_LATCH_PORT      PORTD

#define SIPO_DATA_PIN        PINB1 // 9
#define SIPO_CLK_PIN         PINB0 // 8
#define SIPO_LATCH_PIN       PIND7 // 7

#endif  //IALOY_CODE_H
