#!/usr/bin/python

###################################################################################
#
#  MYTH STRESS TEST SCRIPT
#
###################################################################################

import sys
from socket import *
from time import sleep

###################################################################################
#
# SERVER IP AND PORT
#
###################################################################################

def send_command(socket_client, data):
	socket_client.send(data)
	print '>> ' + data

	sleep(1)
	data_res = socket_client.recv(4096)
	print '<< ' + data_res
	
	return


serverHost = 'localhost'
serverPort = 10001                 

socket_client = socket(AF_INET, SOCK_STREAM)    
socket_client.connect((serverHost, serverPort)) 

sleep(10)

#receive header info
data_res = socket_client.recv(4096)

send_command(socket_client, 'query location\n')
send_command(socket_client, 'key enter\n')
 
for n in range(1, 1000):
	print '\n\n----------------------- Step ' + str(n) + ' ---------------------------\n'
	send_command(socket_client, 'play chanid 4\n')
	send_command(socket_client, 'play chanid 6\n')
	send_command(socket_client, 'query location\n')
	send_command(socket_client, 'key enter\n')


