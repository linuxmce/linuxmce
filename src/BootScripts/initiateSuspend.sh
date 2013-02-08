#!/bin/bash

if /usr/bin/pm-is-supported --suspend
then
    /usr/sbin/pm-suspend
else
    /sbin/halt
fi
