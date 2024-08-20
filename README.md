# iBootLoader: Arduino Custom Bootloader with Rollback Feature

This repository contains a custom bootloader for Arduino that adds a rollback feature for firmware updates using an external I2C EEPROM memory IC (24LC512). This bootloader ensures that if a new firmware update fails, the Arduino device can automatically revert to the previous working firmware.

## Features

- **Firmware Rollback**: Automatically revert to the previous firmware if the new firmware fails to execute properly.
- **I2C EEPROM (24LC512) Integration**: Stores two firmware slots, configuration data, and reserved space in the external EEPROM.
- **Watchdog Timer (WDT) Support**: Utilizes WDT to monitor the success of the new firmware execution and trigger a rollback if necessary.
- **Configurable via I2C**: Firmware and configuration data can be managed via the I2C bus.

## Memory Layout

The 64KB EEPROM (24LC512) is divided into the following sections:

- **Firmware Slot 1**: 30KB - Stores the new firmware during the update process.
- **Firmware Slot 2**: 30KB - Stores the previous working firmware as a backup.
- **Config Space**: 2KB - Holds configuration settings for the bootloader and firmware update process.
- **Reserved Space**: 2KB - Reserved for future use or additional configuration.

## Bootloader Workflow

1. **Firmware Upload**:
   - A script named `manage_fwu_eeprom.py` is used to upload the new firmware to Firmware Slot 1 in the EEPROM and update the configuration space via the I2C bus.
   - After uploading, the Arduino device needs to be reset.

2. **Firmware Update Process**:
   - On reset, the bootloader reads the configuration space and initiates the firmware update.
   - The current working firmware is copied from the Arduino's memory to Firmware Slot 2 in the EEPROM (backup).
   - The new firmware from Firmware Slot 1 overwrites the working firmware in the Arduino's memory.
   - The configuration space is updated again to reflect the new firmware.

3. **Execution and WDT**:
   - The bootloader sets the Watchdog Timer (WDT) and jumps to the starting address of the new firmware.
   - The firmware is responsible for turning off the WDT during its initialization.
   - If the firmware fails to turn off the WDT, the WDT will reset the Arduino.

4. **Rollback Mechanism**:
   - If the WDT reset occurs, the bootloader assumes the new firmware failed.
   - The bootloader will then restore the previous firmware from Firmware Slot 2 in the EEPROM back to the Arduino's memory.
   - The bootloader jumps to the restored firmware to resume normal operation.

## Prerequisites

- Arduino board with I2C capability.
- 24LC512 EEPROM (or equivalent) connected via I2C.
- Python environment for running the `manage_fwu_eeprom.py` script.

## Getting Started

1. **Connect the 24LC512 EEPROM** to your Arduino via the I2C bus.
2. **Upload the Bootloader** to your Arduino board.
3. **Use the `manage_fwu_eeprom.py` script** to send new firmware to the EEPROM and update the configuration.
4. **Reset the Arduino** to initiate the firmware update process.

## Script Details

- `manage_fwu_eeprom.py`: A Python script for managing firmware updates via I2C. It handles writing the new firmware to the EEPROM and updating the configuration space.

## Contributing

Feel free to contribute to this project by opening issues, submitting pull requests, or suggesting new features.

## License

This project is licensed under the MIT License.
