#!/bin/bash

curl -H "Content-Type: text/xml" -d "<dvdXml><authentication><user>tschak909</user><password>hal9000</password></authentication><requests><request type='information'><dvdId>3B509BE3243449DA</dvdId></request></requests></dvdXml>" http://api.dvdxml.com/
