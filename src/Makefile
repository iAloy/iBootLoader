# Makefile for iAloy Module Firmware
#
# Copyright (c) 2020-2024 iAloy
#
# All rights reserved.
#
# Redistribution and use in binary form, without modification, are permitted
# provided that the following conditions are met:
#
# 1. Redistributions of the binary form must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


# for 512kb   0x7E00
# for 2048kb  0x7800

STARTING_ADDRESS = 0x7800

ifndef CONFIG_FILE
CONFIG_FILE = ../config.json
endif

OBJDIR        = build-iBootLoader

VERSION       = $(shell jq -r .version       ${CONFIG_FILE})
PLATFORM_TYPE = $(shell jq -r .platform_type ${CONFIG_FILE})
SERIAL_ENABLE = $(shell jq -r .serial_enable ${CONFIG_FILE})


LDFLAGS  += -mrelax -Wl,-section-start=.text=$(STARTING_ADDRESS)

CPPFLAGS += -DVERSION=\"${VERSION}\" \
			-DSERIAL_ENABLE=${SERIAL_ENABLE} \

LOCAL_INO_SRCS = iBootLoader.ino

LOCAL_CPP_SRCS = serial_lite.cpp \
				 i2c_lite.cpp \
				 flash_read_write.cpp \
				 eeprom_read_write.cpp \
				 watchdog_timer.cpp \

include /usr/share/arduino/Arduino.mk
