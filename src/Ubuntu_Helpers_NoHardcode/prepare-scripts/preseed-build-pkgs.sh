#!/bin/bash

#set -e

. /usr/share/debconf/confmodule

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"


# lirc-modules-source
db_fset lirc-modules-source/do-build seen true
db_set lirc-modules-source/drivers 'atiusb, bt829, cmdir, gpio, i2c, igorplugusb, imon, it87, mceusb, mceusb2, sasem, serial, sir, streamzap'
db_fset lirc-modules-source/drivers seen true
db_set lirc-modules-source/it87_type 'Standard'
db_fset lirc-modules-source/it87_type seen true
db_set lirc-modules-source/parallel_irq '7'
db_fset lirc-modules-source/parallel_irq seen true
db_set lirc-modules-source/parallel_port '0x378'
db_fset lirc-modules-source/parallel_port seen true
db_set lirc-modules-source/parallel_timer '65536'
db_fset lirc-modules-source/parallel_timer seen true
db_set lirc-modules-source/serial_irq '3'
db_fset lirc-modules-source/serial_irq seen true
db_set lirc-modules-source/serial_port '0x2f8'
db_fset lirc-modules-source/serial_port seen true
db_set lirc-modules-source/serial_softcarrier 'false'
db_fset lirc-modules-source/serial_softcarrier seen true
db_set lirc-modules-source/serial_transmitter 'true'
db_fset lirc-modules-source/serial_transmitter seen true
db_set lirc-modules-source/serial_type 'Other'
db_fset lirc-modules-source/serial_type seen true
db_set lirc-modules-source/sir_irq '3'
db_fset lirc-modules-source/sir_irq seen true
db_set lirc-modules-source/sir_port '0x2f8'
db_fset lirc-modules-source/sir_port seen true
db_set lirc-modules-source/sir_type 'Other'
db_fset lirc-modules-source/sir_type seen true
db_set lirc-modules-source/use_lirc_hints ''
db_fset lirc-modules-source/use_lirc_hints seen true
db_fset lirc-modules-source/what_next seen true

echo "*** Done: $0"

