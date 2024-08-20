BOOTLOADER = iBootLoader
OBJDIR     = src/build-${BOOTLOADER}


all: build copy

build:
	cd src && make

copy:
	cp ${OBJDIR}/src_.hex ${BOOTLOADER}.hex

clean:
	rm -rf ${OBJDIR}
	rm -rf ${BOOTLOADER}.hex
