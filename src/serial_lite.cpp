/**
  @file
  iBootLoader - serial_lite.cpp

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


#include <avr/io.h>
#include <stdio.h>

#include "ialoy_code.h"
#include "serial_lite.h"

#if SERIAL_ENABLE


/**
  Int to Ascii convertion library function. This code is copied from actual
  library. To minimize the binary footprint entire library is not used.

  @param[in]      num       Integer value of the char.
  @param[in out]  str       String buffer pointer to return the value.
  @param[in]      base      Base of the Integer value, Default 10.

  @retval         char*     Return same String buffer pointer [Redundant].

**/
char* itoa(int num, char* str, int base = 10) {
    int i = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    str[i] = '\0'; // Add null terminator

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    return str;
}

/**
  Setup the Serial port with 115200 baude rate.

**/
void serial_setup(void)
{
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit data format
    UBRR0L = BAUDE_RATE_115200;
}


/**
  Print a string to the serial port.

  @param[in]      message       const char pointer of the string.

**/
void print_string(const char *message)
{
    for(uint8_t i = 0; message[i] != '\0'; ++i)
    {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = message[i];
    }
}


/**
  Print a integer number to the serial port.

  @param[in]      value       Integer value.

**/
void print_number(int value)
{
    char buffer[10];
    itoa(value, buffer);
    print_string(buffer);
}

#endif
