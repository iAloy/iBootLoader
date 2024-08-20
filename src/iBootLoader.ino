/**
  @file
  iBootLoader - iBootLoader.ino
  Entry point file iBootLoader

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
#include <avr/wdt.h>

#include "watchdog_timer.h"
#include "ialoy_code.h"
#include "serial_lite.h"
#include "i2c_lite.h"
#include "flash_read_write.h"
#include "eeprom_read_write.h"

#define APP_START_ADDRESS           0x0000
#define EEPROM_FIRMWARE_SIZE        30720  // 30 kilobytes
#define FIRMWARE_MAX_PAGE           (EEPROM_FIRMWARE_SIZE / SPM_PAGESIZE)

#define EEPROM_CONFIG_START_ADDRESS 0xF000

#define FWU_MODE_UNKNOWN            0x00
#define FWU_MODE_ENABLED            0xEE
#define FWU_MODE_DISABLED           0xDD

#define FIRMWARE_SLOT_1             1
#define FIRMWARE_SLOT_2             2

#define FIRMWARE_SLOT_1_PAGE_START  0
#define FIRMWARE_SLOT_1_PAGE_END    FIRMWARE_MAX_PAGE

#define FIRMWARE_SLOT_2_PAGE_START  FIRMWARE_MAX_PAGE
#define FIRMWARE_SLOT_2_PAGE_END    (FIRMWARE_MAX_PAGE + FIRMWARE_MAX_PAGE)

#define CONFIG_PAGE_SIZE            8
#define CONFIG_PAGE_NUMBER          (FIRMWARE_MAX_PAGE * 2)

#define FWU_MODE_ADDRESS            0
#define FWU_SLOT_ADDRESS            1
#define FWU_BKUP_MODE_ADDRESS       2
#define FWU_RECOVERY_MODE_ADDRESS   3


#ifndef VERSION
#define VERSION "0.0.0.0000"
#endif


/**
  Initlize the Application entry point pointer and call the pointer
  to jump to the application.

**/
void jump_to_application()
{
    void (*start_app)(void) = (void (*)(void))APP_START_ADDRESS;
    cli();
    start_app();
}


/**
  Main function of the iBootLoader. This is the entry point for the bootloader.

**/
int main()
{
    uint16_t eeprom_page_counter;
    uint8_t eeprom_page_offset;
    uint8_t flash_page_counter;
    uint8_t page_buffer[SPM_PAGESIZE];
    uint8_t config_buffer[CONFIG_PAGE_SIZE];

    disable_watchdog_timer();
    serial_setup();
    LED_DDR |= _BV(BUILD_IN_LED_PIN);
    init_EEPROM_bus();

    print_string("\n~~~~~~~~:  iBootloader ");
    print_string(VERSION);
    print_string("  :~~~~~~~~\n");

    update_EEPROM_bus(ENABLE);

    if (read_from_EEPROM_page(config_buffer, CONFIG_PAGE_NUMBER, CONFIG_PAGE_SIZE) == RETURN_CODE_SUCCESS)
    {

        if(config_buffer[FWU_MODE_ADDRESS] != FWU_MODE_ENABLED && \
        config_buffer[FWU_MODE_ADDRESS] != FWU_MODE_DISABLED)
        {
            print_string("FWU Mode Unknown; Turn it False.\n");
            config_buffer[FWU_MODE_ADDRESS] = FWU_MODE_DISABLED;
            write_to_EEPROM_page(config_buffer, CONFIG_PAGE_NUMBER, CONFIG_PAGE_SIZE);
            _delay_ms(EEPROM_WRITE_DELAY);
        }

        read_from_EEPROM_page(config_buffer, CONFIG_PAGE_NUMBER, CONFIG_PAGE_SIZE);

        if(config_buffer[FWU_MODE_ADDRESS] == FWU_MODE_ENABLED)
        {
            if(config_buffer[FWU_BKUP_MODE_ADDRESS] != FWU_MODE_DISABLED)
            {
                print_string("Old Firmware Backing up ...\n");

                for(flash_page_counter = 0, eeprom_page_counter = FIRMWARE_MAX_PAGE;
                    flash_page_counter < FIRMWARE_MAX_PAGE;
                    flash_page_counter++, eeprom_page_counter++)
                {
                    read_from_flash_memory_page(page_buffer, flash_page_counter);

                    write_to_EEPROM_page(page_buffer, eeprom_page_counter, SPM_PAGESIZE);
                    _delay_ms(EEPROM_WRITE_DELAY);

                    LED_PORT ^= _BV(BUILD_IN_LED_PIN);
                }
            }

            if (config_buffer[FWU_SLOT_ADDRESS] == FIRMWARE_SLOT_2)
            {
                print_string("Recovering Old Firmware from Slot 2 ...\n");
                eeprom_page_offset = FIRMWARE_MAX_PAGE;

                config_buffer[FWU_MODE_ADDRESS]          = FWU_MODE_DISABLED;
                config_buffer[FWU_SLOT_ADDRESS]          = FIRMWARE_SLOT_1;
                config_buffer[FWU_BKUP_MODE_ADDRESS]     = FWU_MODE_ENABLED;
                config_buffer[FWU_RECOVERY_MODE_ADDRESS] = FWU_MODE_DISABLED;
            }
            else
            {
                print_string("New Firmware Updating from Slot 1 ...\n");
                eeprom_page_offset = 0;

                /*
                BootLoader will keep the fwu_enable_mode ENABLE as well as It sets
                the firmware Slot as 2 and DISABLE the fwu_buckup_mode. Bootloader also
                turn-on the WatchDogTimer as 1 Second.
                The Application need to turn-off the WatchDogTimer and need to Reset all
                the configs as defult.
                If Application failed to do so, WatchDogTimer reset the system and
                BootLoader will overwrite the new firmware with old backup one.
                */

                config_buffer[FWU_MODE_ADDRESS]          = FWU_MODE_ENABLED; // Not Needed; only for code readability
                config_buffer[FWU_SLOT_ADDRESS]          = FIRMWARE_SLOT_2;
                config_buffer[FWU_BKUP_MODE_ADDRESS]     = FWU_MODE_DISABLED;
                config_buffer[FWU_RECOVERY_MODE_ADDRESS] = FWU_MODE_ENABLED;
            }

            for(flash_page_counter = 0, eeprom_page_counter = eeprom_page_offset;
                eeprom_page_counter < (uint16_t)(FIRMWARE_MAX_PAGE + eeprom_page_offset);
                flash_page_counter++, eeprom_page_counter++)
            {
                read_from_EEPROM_page(page_buffer, eeprom_page_counter, SPM_PAGESIZE);

                write_to_flash_memory_page(page_buffer, flash_page_counter);
                LED_PORT ^= _BV(BUILD_IN_LED_PIN);
            }

            write_to_EEPROM_page(config_buffer, CONFIG_PAGE_NUMBER, CONFIG_PAGE_SIZE);
            _delay_ms(EEPROM_WRITE_DELAY);

            print_string("WDT Activated.\n");
            enable_watchdog_timer(WATCHDOG_1S);
            print_string("Firmware update completed.\n");
        }
        else
        {
            print_string("No Firmware Update Available.\n");
        }
    }
    else
    {
        print_string("Not able to read FWU EEPROM\n");
    }
    update_EEPROM_bus(DISABLE);

    print_string("Jumping to the application ...\n\n");
    jump_to_application();

    return 0;
}
