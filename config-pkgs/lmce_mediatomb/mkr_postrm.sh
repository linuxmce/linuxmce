#!/bin/bash
## Revove divertion for config.xml, as we have our own configuration file.

[[ -f /etc/mediatomb/config.xml ]] && rm -f /etc/mediatomb/config.xml || :
dpkg-divert --remove --rename --package lmce-mediatomb --divert /etc/mediatomb/config.xml.wrapped /etc/mediatomb/config.xml
