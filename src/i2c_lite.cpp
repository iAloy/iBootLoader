/**
  @file
  iBootLoader - i2c_lite.cpp

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

#include "ialoy_code.h"
#include "i2c_lite.h"


/**
  Initilize the i2c bus to read EEPROM.

**/
void i2c_lite_init()
{
    // Set prescaler for 100 kHz
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
    TWBR = ((F_CPU / 100000) - 16) / 2;
}


/**
  Start the i2c bus.

**/
void i2c_lite_start()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}


/**
  Stop the i2c bus.

**/
void i2c_lite_stop()
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while ((TWCR & (1 << TWSTO)));
}


/**
  Write one byte data to the i2c bus.

  @param[in]    data       Byte data to write.

**/
void i2c_lite_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}


/**
  Read one byte from the i2c bus.

  @param[in out]    data                  Buffer for byte data.
  @param[in]        clk                   Acknowledgment byte.

  @retval           RETURN_CODE_FAILURE   Failed to read data.
  @retval           RETURN_CODE_SUCCESS   Data read successfully.

**/
uint8_t i2c_lite_read(uint8_t *data, uint8_t ack)
{
    uint32_t retry = 0;
    uint32_t retry_max = 100000;

    TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA);
    while (!(TWCR & (1 << TWINT)))
    {
        if (retry++ > retry_max)
        {
            return RETURN_CODE_FAILURE;
        }
    }
    *data = TWDR;

    return RETURN_CODE_SUCCESS;
}
