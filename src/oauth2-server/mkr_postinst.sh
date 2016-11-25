#!/bin/bash


mysql lmce_oauth2_db -e "show tables;" >/dev/null 2>/dev/null
RET="$?"

if [[ "$RET" != "0" ]]; then
	mysql -e "create database lmce_oauth2_db"
	mysql lmce_oauth2_db < /usr/pluto/database/lmce_oauth2_db.sql
fi
