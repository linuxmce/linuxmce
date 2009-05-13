before you start compiling that, you need:


-get a working eibd:
http://www.auto.tuwien.ac.at/~mkoegler/index.php/eibdeb


- give your EIB Template the following attributes:
"bus type"
"bus address"

both must be string. type must be one of:
- ft12
- bcu1
- tpuart24
- tpuart
- ip
- ipt
- bcu1s
- tpuarts
- usb

address is required for ip, ipt and usb

see eibd --help for details.

if you want to modify the values, see EIB.cpp, function void EIB::getAway(). you need EIB sets a good string::sgateway into that fonction.