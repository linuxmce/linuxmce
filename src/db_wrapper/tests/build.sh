#!/bin/bash

g++ -DUSE_MYSQL_WRAPPER -ggdb -o test Main.cpp ../*.cpp -I../../ -lmysqlclient

