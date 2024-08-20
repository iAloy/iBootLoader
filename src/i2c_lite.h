/**
  @file
  iBootLoader - i2c_lite.h

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


#ifndef I2C_LITE_H
#define I2C_LITE_H


/**
  Initilize the i2c bus to read EEPROM.

**/
void i2c_lite_init();


/**
  Start the i2c bus.

**/
void i2c_lite_start();


/**
  Stop the i2c bus.

**/
void i2c_lite_stop();


/**
  Write one byte data to the i2c bus.

  @param[in]    data       Byte data to write.

**/
void i2c_lite_write(uint8_t data);


/**
  Read one byte from the i2c bus.

  @param[in out]    data                  Buffer for byte data.
  @param[in]        clk                   Acknowledgment byte.

  @retval           RETURN_CODE_FAILURE   Failed to read data.
  @retval           RETURN_CODE_SUCCESS   Data read successfully.

**/
uint8_t i2c_lite_read(uint8_t *data, uint8_t ack);

#endif //I2C_LITE_H
