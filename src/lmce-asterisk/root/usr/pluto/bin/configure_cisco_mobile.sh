#!/bin/bash
# Argument = -c <coreip> -n <phonenumber> -?

USAGE="Usage: `basename $0` -c <coreip> -n <phonenumber> "

while getopts c:n: OPT; do
    case "$OPT" in
         c)
            INTIP=$OPTARG
            ;;
         n)
            PHONE=$OPTARG
            ;;
        \?)
            # getopts issues an error message
            echo $USAGE >&2
            exit 1
            ;;
    esac
done

if [[ -n $INTIP ]] && [[ -n $PHONE ]]; then
	echo "Configuring TFTP script for phone $PHONE on core $INTIP"
	sed -r "s,%PHONE_NO%,$PHONE,g;s,%CORE_INT_IP%,$INTIP,g" /usr/pluto/templates/ciscomobile.cnf.xml.tmpl >/tftpboot/$PHONE.cnf.xml
else
	echo $USAGE >&2
	exit 1
fi
