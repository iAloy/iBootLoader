# iAloy Module Firmware - manage_fwu_eeprom.py
#
#  MIT License
#
#  @copyright
#  Copyright (c) 2020-2024 iAloy
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.


import smbus
import time
import argparse
import sys
import os
from intelhex import IntelHex

FORMAT_BYTE             = 0xFF
PAYLOAD_SIZE            = 16

FIRMWARE_1_SIZE         = 30720 # 30 Bytes
FIRMWARE_2_SIZE         = 30720 # 30 Bytes
CONFIG_SIZE             = 2048  #  2 Bytes
UNUSED_SIZE             = 2048  #  2 Bytes

OPTION_FORMAT           = "format"
OPTION_FIRMWARE         = "firmware"
OPTION_CONFIG           = "config"
OPTION_DUMP             = "dump"

REGION_OPTN_FIRMWARE_1 = "FW1"
REGION_OPTN_FIRMWARE_2 = "FW2"
REGION_OPTN_CONFIG     = "CNF"
REGION_OPTN_UNUSED     = "UNSD"

FWU_MODE_UNKNOWN        = "UNKNOWN"
FWU_MODE_ENABLE         = "ENABLE"
FWU_MODE_DISABLE        = "DISABLE"

FWU_SLOT_1              = '1'
FWU_SLOT_2              = '2'

HALF_LINE               = '1'
FULL_LINE               = '2'

CONFIG_OP_FWU           = 1
CONFIG_OP_SLOT          = 2
CONFIG_OP_BKUP          = 3

CONFIG_START_ADDRESS    = FIRMWARE_1_SIZE + FIRMWARE_2_SIZE
CONFIG_FWU_MODE_ADDRESS = CONFIG_START_ADDRESS + 0
CONFIG_FWU_SLOT_ADDRESS = CONFIG_START_ADDRESS + 1
CONFIG_FWU_BKUP_ADDRESS = CONFIG_START_ADDRESS + 2

CONFIG_OP_FWU_DEFAULT   = FWU_MODE_ENABLE
CONFIG_OP_SLOT_DEFAULT  = FWU_SLOT_1
CONFIG_OP_BKUP_DEFAULT  = FWU_MODE_ENABLE

ARG_SHORT               = 0
ARG_FULL                = 1

arg_opt = {
    "address"   : ["-a", "--address"],
    "length"    : ["-l", "--length"],
    "firmware"  : ["-f", "--firmware"],
    "slot"      : ["-s", "--slot"],
    "dump_file" : ["-d", "--dump_file"],
    "fwm_mode"  : ["-m", "--fwm_mode"],
    "backup"    : ["-b", "--backup"],
    "region"    : ["-r", "--region"],
    "default"   : ["-D", "--default"],
    "legacy"    : ["-L", "--legacy"],
}


bus=smbus.SMBus(1)

def dump_eeprom_to_intel_hex(file_path, eeprom_data, line_length):
    if line_length == HALF_LINE:
        byte_count = 16
    else:
        byte_count = 32

    ih = IntelHex()
    ih.frombytes(eeprom_data)
    ih.tofile(file_path, format='hex', byte_count=byte_count)


def hex_to_list(hex_filename):
    ih = IntelHex(hex_filename)
    data_list = ih.tobinarray()
    return data_list


def dump_firmware_region(EEPROM_ADDRESS, dump_regions, line_length, firmware_dump_file = "firmware_dump.hex"):
    global bus
    if dump_regions == None:
        data_size     = FIRMWARE_1_SIZE + FIRMWARE_2_SIZE + CONFIG_SIZE + UNUSED_SIZE
        start_address = 0
        dump_file     = firmware_dump_file
        print(f"\nDumping Full EEPROM into {dump_file} ...")
        if (dump_hardware(EEPROM_ADDRESS, data_size, start_address, line_length, dump_file)):
            print("Dumping Complete.")
    else:
        if REGION_OPTN_FIRMWARE_1 in dump_regions:
            data_size     = FIRMWARE_1_SIZE
            start_address = 0
            dump_file     = firmware_dump_file.replace(".hex", "_fw_1.hex")
            print(f"\nDumping EEPROM Firmware 1 Region into {dump_file} ...")
            if (dump_hardware(EEPROM_ADDRESS, data_size, start_address, line_length, dump_file)):
                print("Dumping Complete.")

        if REGION_OPTN_FIRMWARE_2 in dump_regions:
            data_size     = FIRMWARE_2_SIZE
            start_address = FIRMWARE_1_SIZE
            dump_file     = firmware_dump_file.replace(".hex", "_fw_2.hex")
            print(f"\nDumping EEPROM Firmware 2 Region into {dump_file} ...")
            if (dump_hardware(EEPROM_ADDRESS, data_size, start_address, line_length, dump_file)):
                print("Dumping Complete.")

        if REGION_OPTN_CONFIG in dump_regions:
            data_size     = CONFIG_SIZE
            start_address = FIRMWARE_1_SIZE + FIRMWARE_2_SIZE
            dump_file     = firmware_dump_file.replace(".hex", "_conf.hex")
            print(f"\nDumping EEPROM Config Region into {dump_file} ...")
            if (dump_hardware(EEPROM_ADDRESS, data_size, start_address, line_length, dump_file)):
                print("Dumping Complete.")

        if REGION_OPTN_UNUSED in dump_regions:
            data_size     = UNUSED_SIZE
            start_address = FIRMWARE_1_SIZE + FIRMWARE_2_SIZE + CONFIG_SIZE
            dump_file     = firmware_dump_file.replace(".hex", "_unusd.hex")
            print(f"\nDumping EEPROM Unused Region into {dump_file} ...")
            if (dump_hardware(EEPROM_ADDRESS, data_size, start_address, line_length, dump_file)):
                print("Dumping Complete.")


def dump_hardware(EEPROM_ADDRESS, data_size, start_address, line_length, firmware_dump_file):
    try:
        msb_address = start_address >> 8
        lsb_address = start_address & 0xFF
        bus.write_i2c_block_data(EEPROM_ADDRESS, msb_address, [lsb_address])
        time.sleep(0.01)
        total_size_str = str(data_size)
        data_buffer = []
        for index in range(data_size):
            value = bus.read_byte(EEPROM_ADDRESS)
            data_buffer.append(value)
            percent = ((index + 1) * 100)/data_size
            percent = "{:.2f}".format(round(percent, 2))
            print(f"Progress : [{index + 1}/{total_size_str}]  {percent}%", end="\r")
        print("\n")

        dump_eeprom_to_intel_hex(firmware_dump_file, data_buffer, line_length)
        return True
    except Exception as e:
        print("Error: ", e)
        return False


def update_eeprom(data_list, EEPROM_ADDRESS, start_address = 0, legacy_upload = False):
    global bus
    total_size = len(data_list)
    total_size_str = str(total_size)
    frame_index = 0
    payload = []
    if legacy_upload:
        print(f"Legacy Mode Activated.")
    for index, byte_data in enumerate(data_list):
        if not legacy_upload:
            if frame_index == PAYLOAD_SIZE:
                frame_index = 0
                payload = []
            payload.append(byte_data)
            frame_index += 1
            if frame_index != PAYLOAD_SIZE:
                continue
            address = start_address + index - (PAYLOAD_SIZE - 1)
        else:
            payload = [byte_data]
            address = start_address + index

        msb_address = address >> 8
        lsb_address = address & 0xFF
        percent = ((index + 1) * 100)/total_size
        percent = "{:.2f}".format(round(percent, 2))
        print(f"Progress : [{index + 1}/{total_size_str}]  {percent}%", end="\r")
        payload_with_lsb_address = [lsb_address]
        payload_with_lsb_address.extend(payload)
        bus.write_i2c_block_data(EEPROM_ADDRESS, msb_address, payload_with_lsb_address)
        time.sleep(0.005)

    print("\n")


def write_firmware(firmware_file, slot, EEPROM_ADDRESS, legacy_write):
    firmware_data = hex_to_list(firmware_file)
    print(f"Writting {firmware_file} in slot {slot} ...")
    if slot == FWU_SLOT_1:
        start_address = 0
    if slot == FWU_SLOT_2:
        start_address = FIRMWARE_1_SIZE
    try:
        update_eeprom(firmware_data, EEPROM_ADDRESS, start_address, legacy_write)
        print("Writting Complete.")
    except Exception as e:
        print("Error: ", e)


def format_eeprom(format_regions, EEPROM_ADDRESS, legacy_write):
    if format_regions == None:
        over_write_data = [FORMAT_BYTE] * (FIRMWARE_1_SIZE + FIRMWARE_2_SIZE + CONFIG_SIZE + UNUSED_SIZE)
        print("Formating Full EEPROM ...")
        try:
            starting_address = 0
            update_eeprom(over_write_data, EEPROM_ADDRESS, starting_address, legacy_write)
            print("Formating Complete.")
        except Exception as e:
            print("Error: ", e)
    else:
        if REGION_OPTN_FIRMWARE_1 in format_regions:
            over_write_data = [FORMAT_BYTE] * (FIRMWARE_1_SIZE)
            print("Formating EEPROM Firmware Slot 1 region ...")
            try:
                starting_address = 0
                update_eeprom(over_write_data, EEPROM_ADDRESS, starting_address, legacy_write)
            except Exception as e:
                print("Error: ", e)
                return

        if REGION_OPTN_FIRMWARE_2 in format_regions:
            over_write_data = [FORMAT_BYTE] * (FIRMWARE_2_SIZE)
            print("Formating EEPROM Firmware Slot 2 region ...")
            try:
                starting_address = FIRMWARE_1_SIZE
                update_eeprom(over_write_data, EEPROM_ADDRESS, starting_address, legacy_write)
            except Exception as e:
                print("Error: ", e)
                return

        if REGION_OPTN_CONFIG in format_regions:
            over_write_data = [FORMAT_BYTE] * (CONFIG_SIZE)
            print("Formating EEPROM Config region ...")
            try:
                starting_address = FIRMWARE_1_SIZE + FIRMWARE_2_SIZE
                update_eeprom(over_write_data, EEPROM_ADDRESS, starting_address, legacy_write)
            except Exception as e:
                print("Error: ", e)
                return

        if REGION_OPTN_UNUSED in format_regions:
            over_write_data = [FORMAT_BYTE] * (UNUSED_SIZE)
            print("Formating EEPROM Unused region ...")
            try:
                starting_address = FIRMWARE_1_SIZE + FIRMWARE_2_SIZE + CONFIG_SIZE
                update_eeprom(over_write_data, EEPROM_ADDRESS, starting_address, legacy_write)
            except Exception as e:
                print("Error: ", e)
                return

        print("Formating Complete.")


def update_config(config_option, value, EEPROM_ADDRESS):
    address   = None
    byte_data = None

    if config_option == CONFIG_OP_FWU:
        address = CONFIG_FWU_MODE_ADDRESS
        print(">> Config: FWU ", end="")
        if value == FWU_MODE_UNKNOWN:
            byte_data = 0x00
            print("Unknown.")
        if value == FWU_MODE_ENABLE:
            byte_data = 0xEE
            print("Enabled.")
        if value == FWU_MODE_DISABLE:
            byte_data = 0xDD
            print("Disabled.")

    elif config_option == CONFIG_OP_SLOT:
        address = CONFIG_FWU_SLOT_ADDRESS
        if value == FWU_SLOT_1:
            byte_data = 0x01
        if value == FWU_SLOT_2:
            byte_data = 0x02
        print(f">> Config: Slot {byte_data}")

    elif config_option == CONFIG_OP_BKUP:
        address = CONFIG_FWU_BKUP_ADDRESS
        print(">> Config: Backup ", end="")
        if value == FWU_MODE_UNKNOWN:
            byte_data = 0x00
            print("Unknown.")
        if value == FWU_MODE_ENABLE:
            byte_data = 0xEE
            print("Enabled.")
        if value == FWU_MODE_DISABLE:
            byte_data = 0xDD
            print("Disabled.")

    if address != None or byte_data != None:
        print("Updating Config ...")
        try:
            msb_address = address >> 8
            lsb_address = address & 0xFF
            bus.write_i2c_block_data(EEPROM_ADDRESS, msb_address, [lsb_address, byte_data])
            time.sleep(0.005)
            print("Updating Complete.")
        except Exception as e:
            print("Error: ", e)



def main():
    parser = argparse.ArgumentParser(description = 'Remote Firmware Update')

    parser.add_argument(
        "Mode",
        choices = [OPTION_FORMAT, OPTION_FIRMWARE, OPTION_CONFIG, OPTION_DUMP],
        help    = f"Use '{OPTION_FORMAT}' or '{OPTION_FIRMWARE}' or '{OPTION_CONFIG}' or '{OPTION_DUMP}' for select the mode"
    )

    parser.add_argument(
        arg_opt["address"][ARG_SHORT],
        arg_opt["address"][ARG_FULL],
        choices = ['0x50', '0x51', '0x52', '0x53', '0x54', '0x55', '0x56', '0x57'],
        default = '0x50',
        help    = "EEPROM address. (Default 0x50)"
    )

    # Firmware Flashing Options
    if OPTION_FIRMWARE in sys.argv:
        parser.add_argument(
            arg_opt["firmware"][ARG_SHORT],
            arg_opt["firmware"][ARG_FULL],
            required = True,
            help     = "Firmware hex file"
        )

        parser.add_argument(
            arg_opt["slot"][ARG_SHORT],
            arg_opt["slot"][ARG_FULL],
            choices = [FWU_SLOT_1, FWU_SLOT_2],
            default = FWU_SLOT_1,
            help    = "Firmware slot"
        )

        parser.add_argument(
            arg_opt["legacy"][ARG_SHORT],
            arg_opt["legacy"][ARG_FULL],
            action ='store_true',
            help   = "Byte by Byte write. Slow but steady process."
        )

    # Firmware Dumping Options
    if OPTION_DUMP in sys.argv:
        parser.add_argument(
            arg_opt["dump_file"][ARG_SHORT],
            arg_opt["dump_file"][ARG_FULL],
            default = "firmware_dump.hex",
            help    = "Dump file name"
        )

        parser.add_argument(
            arg_opt["region"][ARG_SHORT],
            arg_opt["region"][ARG_FULL],
            nargs    = "+",
            choices  = [REGION_OPTN_FIRMWARE_1, REGION_OPTN_FIRMWARE_2, REGION_OPTN_CONFIG, REGION_OPTN_UNUSED],
            help     = "Set EEPROM region to format"
        )

        parser.add_argument(
            arg_opt["length"][ARG_SHORT],
            arg_opt["length"][ARG_FULL],
            choices = [HALF_LINE, FULL_LINE],
            default = FULL_LINE,
            help    = "Line size=> 16 Bytes: 1, 32 Bytes: 2 (Default 32 Bytes)"
        )

    # Firmware Configuration Options
    if OPTION_CONFIG in sys.argv:
        parser.add_argument(
            arg_opt["default"][ARG_SHORT],
            arg_opt["default"][ARG_FULL],
            action ='store_true',
            help   = "Set Default Config"
        )

        parser.add_argument(
            arg_opt["fwm_mode"][ARG_SHORT],
            arg_opt["fwm_mode"][ARG_FULL],
            choices  = [FWU_MODE_ENABLE, FWU_MODE_DISABLE, FWU_MODE_UNKNOWN],
            help     = "Set firmware update mode"
        )

        parser.add_argument(
            arg_opt["slot"][ARG_SHORT],
            arg_opt["slot"][ARG_FULL],
            choices = [FWU_SLOT_1, FWU_SLOT_2],
            default = FWU_SLOT_1,
            help    = "Firmware slot"
        )

        parser.add_argument(
            arg_opt["backup"][ARG_SHORT],
            arg_opt["backup"][ARG_FULL],
            choices = [FWU_MODE_ENABLE, FWU_MODE_DISABLE, FWU_MODE_UNKNOWN],
            default = FWU_MODE_ENABLE,
            help    = "Firmware backup enable disable"
        )

    # Firmware Formating Options
    if OPTION_FORMAT in sys.argv:
        parser.add_argument(
            arg_opt["region"][ARG_SHORT],
            arg_opt["region"][ARG_FULL],
            nargs    = "+",
            choices  = [REGION_OPTN_FIRMWARE_1, REGION_OPTN_FIRMWARE_2, REGION_OPTN_CONFIG, REGION_OPTN_UNUSED],
            help     = "Set EEPROM region to format"
        )

        parser.add_argument(
            arg_opt["legacy"][ARG_SHORT],
            arg_opt["legacy"][ARG_FULL],
            action ='store_true',
            help   = "Byte by Byte write. Slow but steady process."
        )

    args = parser.parse_args()

    start_time = time.time()

    EEPROM_ADDRESS = int(args.address, 16)
    print(f'EEPROM address: {hex(EEPROM_ADDRESS)}')

    if OPTION_FIRMWARE in sys.argv:
        firmware_file = args.firmware
        firmware_slot = args.slot
        legacy_write   = args.legacy

        if os.path.exists(firmware_file):
            write_firmware(firmware_file, firmware_slot, EEPROM_ADDRESS, legacy_write)
        else:
            print(f"ERROR: Firmware file {firmware_file} Not Found!!!")


    if OPTION_FORMAT in sys.argv:
        format_regions = args.region
        legacy_write   = args.legacy
        format_eeprom(format_regions, EEPROM_ADDRESS, legacy_write)


    if OPTION_DUMP in sys.argv:
        dump_file    = args.dump_file
        dump_regions = args.region
        line_length  = args.length
        dump_firmware_region(EEPROM_ADDRESS, dump_regions, line_length, dump_file)


    if OPTION_CONFIG in sys.argv:
        default  = args.default
        fwm_mode = args.fwm_mode
        fwm_slot = args.slot
        fwm_bkup = args.backup
        if default:
            print("\nApplying Default Config values.")
            update_config(CONFIG_OP_FWU,  CONFIG_OP_FWU_DEFAULT,  EEPROM_ADDRESS)
            update_config(CONFIG_OP_SLOT, CONFIG_OP_SLOT_DEFAULT, EEPROM_ADDRESS)
            update_config(CONFIG_OP_BKUP, CONFIG_OP_BKUP_DEFAULT, EEPROM_ADDRESS)
            print("Default Config done.\n\n")

        if arg_opt["fwm_mode"][ARG_SHORT] in sys.argv or arg_opt["fwm_mode"][ARG_FULL] in sys.argv:
            update_config(CONFIG_OP_FWU, fwm_mode, EEPROM_ADDRESS)

        if arg_opt["slot"][ARG_SHORT] in sys.argv or arg_opt["slot"][ARG_FULL] in sys.argv:
            update_config(CONFIG_OP_SLOT, fwm_slot, EEPROM_ADDRESS)

        if arg_opt["backup"][ARG_SHORT] in sys.argv or arg_opt["backup"][ARG_FULL] in sys.argv:
            update_config(CONFIG_OP_BKUP, fwm_bkup, EEPROM_ADDRESS)

    execution_time_seconds = time.time() - start_time
    minutes = int(execution_time_seconds // 60)
    seconds = execution_time_seconds % 60

    if minutes == 0:
        print(f"\nExecution time: {seconds:.2f} seconds")
    else:
        print(f"\nExecution time: {minutes} minutes and {seconds:.2f} seconds")

if __name__ == "__main__":
    main()
