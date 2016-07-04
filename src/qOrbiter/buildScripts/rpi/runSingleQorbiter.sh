#!/bin/bash
OMX_SESSION=$(cat "/tmp/omxplayerdbus.pi")
OMX_SESSION_PID=$(cat "/tmp/omxplayerdbus.pi.pid")
export DBUS_SESSION_BUS_ADDRESS=$OMX_SESSION
export DBUS_SESSION_BUS_PID=$OMX_SESSION_PID
/usr/pluto/bin/qorbiter-core-gl
