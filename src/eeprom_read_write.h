/**
  @file
  iBootLoader - eeprom_read_write.h

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


#ifndef EEPROM_READ_WRITE_H
#define EEPROM_READ_WRITE_H


/**
  Initilize EEPROM with default 0x50 i2c address.

**/
void init_EEPROM_bus();


/**
  Count number of ones in the binary of an int.

  @param[in]    state       ENABLE / DISABLE EEPROM to expose or hide.

**/
void update_EEPROM_bus(uint8_t status);


/**
  Read a page data from the EEPROM from a specific address.

  @param[in out]    page_buffer           Buffer pointer to get the data.
  @param[in]        page_number           Page Number from where data need to be read.
  @param[in]        page_size             Page size, amount of data need to be read.

  @retval           RETURN_CODE_FAILURE   Failed to read data.
  @retval           RETURN_CODE_SUCCESS   Data read successfully.

**/
uint8_t read_from_EEPROM_page(uint8_t *page_buffer, uint16_t page_number, uint8_t page_size);


/**
  Write a page data to the specific address of EEPROM.

  @param[in out]    page_buffer           Buffer pointer of the data.
  @param[in]        page_number           Page Number where data need to be write.
  @param[in]        page_size             Page size, amount of data need to be write.

**/
void write_to_EEPROM_page(uint8_t *page_buffer, uint16_t page_number, uint8_t page_size);

#endif //EEPROM_READ_WRITE_H
