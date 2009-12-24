
# $Id: Makefile 1904 2008-03-08 05:27:18Z isely $

all: decode_log decode_eeprom fix_comma_bullshit

clean:
	rm -f decode_log decode_eeprom regdump fix_comma_bullshit

fix_comma_bullshit: fix_comma_bullshit.c
	gcc -O2 -o fix_comma_bullshit fix_comma_bullshit.c

decode_log: decode_log.cpp
	g++ -g -o decode_log decode_log.cpp

decode_eeprom: decode_eeprom.cpp
	g++ -g -o decode_eeprom decode_eeprom.cpp

regdump: regdump.c
	gcc -o regdump -I$(KDIR)/include regdump.c
