CC = avr-gcc
CFLAGS = -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP 
LDLIBS = 
LDFLAGS = -mmcu=atmega328p


ctrl_Inmoov_Braco.hex: ctrl_Inmoov_Braco
	avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex ctrl_Inmoov_Braco  "ctrl_Inmoov_Braco.hex"

ctrl_Inmoov_Braco: ctrl_Inmoov_Braco.o ATmega328.o

ATmega328.o: ATmega328.c

ctrl_Inmoov_Braco.o: ctrl_Inmoov_Braco.c

basp: ctrl_Inmoov_Braco.hex
	/usr/bin/avrdude -pm328p -cusbasp -Uflash:w:ctrl_Inmoov_Braco.hex:a 

arduino: ctrl_Inmoov_Braco.hex
	/usr/bin/avrdude -pm328p -carduino -P/dev/ttyACM0 -Uflash:w:ctrl_Inmoov_Braco.hex:a 

clean:
	rm -f *.o *.hex ctrl_Inmoov_Braco
