/**
  @file
  iBootLoader - eeprom_read_write.cpp

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


#include <util/twi.h>

#include "eeprom_read_write.h"
#include "i2c_lite.h"
#include "ialoy_code.h"


/**
  Initilize EEPROM with default 0x50 i2c address.

**/
void init_EEPROM_bus()
{
    SIPO_DATA_DDR   |= _BV(SIPO_DATA_PIN);
    SIPO_CLK_DDR    |= _BV(SIPO_CLK_PIN);
    SIPO_LATCH_DDR  |= _BV(SIPO_LATCH_PIN);

    SIPO_DATA_PORT  &= ~(_BV(SIPO_DATA_PIN));
    SIPO_CLK_PORT   &= ~(_BV(SIPO_CLK_PIN));
    SIPO_LATCH_PORT |= _BV(SIPO_LATCH_PIN);
}


/**
  Count number of ones in the binary of an int.

  @param[in]    state       ENABLE / DISABLE EEPROM to expose or hide.

**/
void update_EEPROM_bus(uint8_t state)
{
    for(uint8_t index = 0; index <= 23; index++)
    {
        if((index == EEPROM_ENABLE_BIT) && state)
            SIPO_DATA_PORT |= _BV(SIPO_DATA_PIN);
        else
            SIPO_DATA_PORT &= ~(_BV(SIPO_DATA_PIN));

        SIPO_CLK_PORT ^= _BV(SIPO_CLK_PIN);
        SIPO_CLK_PORT ^= _BV(SIPO_CLK_PIN);
    }
    SIPO_LATCH_PORT ^= _BV(SIPO_LATCH_PIN);
    SIPO_LATCH_PORT ^= _BV(SIPO_LATCH_PIN);
}


/**
  Read SPM_PAGESIZE data from the EEPROM from a specific address.

  @param[in out]    page_buffer           Buffer pointer to get the data.
  @param[in]        page_number           Page Number from where data need to be read.
  @param[in]        page_size             Page size, amount of data need to be read.

  @retval           RETURN_CODE_FAILURE   Failed to read data.
  @retval           RETURN_CODE_SUCCESS   Data read successfully.

**/
uint8_t read_from_EEPROM_page(uint8_t *page_buffer, uint16_t page_number, uint8_t page_size)
{
    uint8_t status = RETURN_CODE_FAILURE;
    i2c_lite_init();

    i2c_lite_start();
    i2c_lite_write((EEPROM_I2C_ADDRESS << 1) | TW_WRITE);
    i2c_lite_write((uint8_t)((page_number * SPM_PAGESIZE) >> 8));    // MSB of memory address
    i2c_lite_write((uint8_t)((page_number * SPM_PAGESIZE) & 0xFF));  // LSB of memory address
    i2c_lite_stop();

    i2c_lite_start();
    i2c_lite_write((EEPROM_I2C_ADDRESS << 1) | TW_READ);

    for(uint8_t i = 0; i < page_size; i++)
    {
        status = i2c_lite_read(&page_buffer[i], i < page_size - 1);
        if (status == RETURN_CODE_FAILURE)
            return RETURN_CODE_FAILURE;
    }

    i2c_lite_stop();
    return RETURN_CODE_SUCCESS;
}


/**
  Write a page data to the specific address of EEPROM.

  @param[in out]    page_buffer           Buffer pointer of the data.
  @param[in]        page_number           Page Number where data need to be write.
  @param[in]        page_size             Page size, amount of data need to be write.

**/
void write_to_EEPROM_page(uint8_t *page_buffer, uint16_t page_number, uint8_t page_size)
{
    i2c_lite_init();

    i2c_lite_start();
    i2c_lite_write((EEPROM_I2C_ADDRESS << 1) | TW_WRITE);
    i2c_lite_write((uint8_t)((page_number * SPM_PAGESIZE) >> 8));    // MSB of memory address
    i2c_lite_write((uint8_t)((page_number * SPM_PAGESIZE) & 0xFF));  // LSB of memory address

    for(uint8_t i = 0; i < page_size; i++)
    {
        i2c_lite_write(page_buffer[i]);
    }

    i2c_lite_stop();
}
