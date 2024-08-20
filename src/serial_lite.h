/**
  @file
  iBootLoader - serial_lite.h

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


#ifndef SERIAL_LITE_H
#define SERIAL_LITE_H

#ifndef SERIAL_ENABLE
#define SERIAL_ENABLE 0
#endif

#if SERIAL_ENABLE

/**
  Setup the Serial port with 115200 baude rate.

**/
void serial_setup(void);


/**
  Print a string to the serial port.

  @param[in]      message       const char pointer of the string.

**/
void print_string(const char *message);


/**
  Print a integer number to the serial port.

  @param[in]      value       Integer value.

**/
void print_number(int value);

#else

/**
  Setup the Serial port with 115200 baude rate.

**/
#define serial_setup()


/**
  Print a string to the serial port.

  @param[in]      message       const char pointer of the string.

**/
#define print_string(...)


/**
  Print a integer number to the serial port.

  @param[in]      value       Integer value.

**/
#define print_number(...)

#endif

#endif // SERIAL_LITE_H
