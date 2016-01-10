// GC100 Serial Driver user-space driver/daemon
// Created 19 April 2004 by Matthew Lawson
// Copyright (c) 2004 1Control
// Usage: 
// 	gc_seriald ip_addr port
// The program runs as a daemon and connects to the GC100 at ip_addr and port.
// It uses a /dev/xxx device so the serial ports on the GC100 can be used
// transparently by any other program

#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
//#include <list>
//#include <iostream>
//#include <sstream>
//using namespace::std;

// Global variables are the args and socket
std::string ip_addr;
std::string input_fifo_name, output_fifo_name;
int port;
int iter;
int gc100_socket;
FILE *input_fifo;
FILE *output_fifo;
char next_char_to_dev, next_char_to_gc;
bool buffer_to_dev_full, buffer_to_gc_full;
bool first_input, first_output;

void sleep_ms(int time)
{
	// Sleep for the specified no. of ms.
	// Be careful to use the pthread sleep function if thread are being used.
	// Using the system sleep() will cause all threads in the process to sleep

	struct timeval tmo;

    tmo.tv_sec = 0;
    tmo.tv_usec = time*1000;

    select(0, NULL, NULL, NULL, &tmo);
}  // end sleep_ms

// Prototype
int gc_close();

int gc_open()
{
	int return_value;
	struct hostent *hp;
	struct sockaddr_in sin;
	int res;

	return_value = 0;
	printf("Opening comm to GC\n");

	hp = gethostbyname(ip_addr.c_str());
	if (!hp)
	{
		fprintf(stderr,"gc_seriald: Did not resolve hp\n");
		return_value = -1; // TODO: figure return value
	}
	else
	{
		gc100_socket=socket(PF_INET, SOCK_STREAM, 0);
		if (gc100_socket < 0)
		{
			fprintf(stderr, "gc_seriald: Did not create gc100 socket\n");
			return_value = -1; // TODO: figure return value
		}
		else
		{
			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);

			memcpy(&sin.sin_addr, hp->h_addr, sizeof(sin.sin_addr));
			if (connect(gc100_socket, (sockaddr *) &sin, sizeof(sin)))
			{
				fprintf(stderr,"gc_seriald: Unable to connect to GC100 on socket.\n");
				close(gc100_socket);
				return_value = -1; // TODO: figure return value
			}
			else
			{
//				res = fcntl(gc100_socket, F_GETFL);
//				if ((res < 0) || (fcntl(gc100_socket, F_SETFL, res | O_NONBLOCK) < 0))
//				{
//					fprintf(stderr, "gc_seriald: Unable to set flags on GC100 socket.\n");
//					close(gc100_socket);
//					return_value = -1; // TODO: figure return value
//				}
//				else
//				{
					printf("gc_seriald: GC100 socket connect OK\n");
//				}
			} // end if connect
		} // end if gc socket > 0
	}

	return return_value;
}

int gc_write(void *buffer, int size)
{
	int return_value;
	return_value = 0;

	return_value = write(gc100_socket, buffer, size);

	if (return_value < 0)
	{
		int reopen_res;

		if (errno != EAGAIN)
		{  // Resource temp. unavail is OK
			printf("Err is %d\n",errno);
			printf("Err is ********************************** %s ***********************\n", strerror(errno));
			gc_close();

			reopen_res = -1;

			while (reopen_res != 0)
			{
				printf("Reopening socket...\n");
				reopen_res = gc_open();
			}
			printf("Reopened OK\n");
		}
	}
	return return_value;
}

int gc_read(void *buffer, int size)
{
	int return_value;

	return_value = read(gc100_socket, buffer, size);
	//printf("gc_read: after read\n");

	if (return_value < 0)
	{
		int reopen_res;

		if (errno != EAGAIN)
		{
			printf("Err is %d\n",errno);
			printf("Err is  *********************** %s ******************\n",strerror(errno));
			gc_close();

			reopen_res=-1;

			while (reopen_res != 0)
			{
				printf("Reopening socket...\n");
				reopen_res = gc_open();
			}
			printf("Reopened OK\n");
		}
	}
	return return_value;
}

int gc_close()
{
	return close(gc100_socket);
}

int main(int argc, char **argv)
{
    buffer_to_dev_full = false;
    buffer_to_gc_full = false;
    first_input = true;
    first_output = true;

    if (argc < 4)
	{
		printf("Usage: gc_seriald <ip_addr> <port> <pipe/FIFO>\n\n");
	}
	else
	{
		ip_addr = std::string(argv[1]);
		sscanf(argv[2], "%d", &port);
		input_fifo_name = std::string(argv[3]);
		//output_fifo_name = std::string(argv[4]);
		char input_buffer[512];
		char output_buffer[512];
		char out_to_gc_buffer[512];
		std::string out_to_gc_string, out_to_dev_string;
		int retval;
		bool file_error;
		int read_size, write_size;
		int device_fd;
		int to_dev_length;
		//int last_dev_size;

		printf("Args received are %s and port %d, input is %s\n", ip_addr.c_str(), port, input_fifo_name.c_str());

		out_to_gc_string = "";
		out_to_dev_string = "";

		file_error = false;
		retval = gc_open();
		printf("open() GC100 returned  %d\n",retval);

		if (retval < 0)
		{
			file_error = true;
		}
		/*input_fifo = fopen(input_fifo_name.c_str(), "r+");
		if (input_fifo == NULL)
		{
			printf("open() input FIFO returned NULL\n");
			file_error = true;
		}*/

		device_fd = open(input_fifo_name.c_str(), O_RDWR /*| O_NONBLOCK*/);
		if (device_fd < 1)
		{
			printf("open() input FIFO returned NULL\n");
			file_error=true;
		}

		/*output_fifo=fopen(output_fifo_name.c_str(),"w");
		if (output_fifo==NULL)
		{
			printf("open() output FIFO returned NULL\n");
			file_error=true;
		}*/

		iter = 1;

		// Begin test driver loop

		if (!file_error)
		{
			int outstring_length;
			fd_set fdset;
			while (true)
			{
				FD_ZERO(&fdset);
				FD_SET(device_fd, &fdset);
				FD_SET(gc100_socket, &fdset);
				// READ from gcsd device and store it
	      
				select(FD_SETSIZE, &fdset, NULL, NULL, NULL);
				
				if (FD_ISSET(device_fd, &fdset))
				{
					read_size = read(device_fd, (void*) input_buffer, 500);
					//printf("read from dev returned %d\n",read_size);

					if (read_size > 0)
					{
						input_buffer[read_size] = '\0';
						//printf("\tRead: %s\n",input_buffer);
						out_to_gc_string += input_buffer;
					}

					// Write from buffer to GC
					outstring_length = out_to_gc_string.length();

					if (outstring_length > 0)
					{
						//printf("string size is %d\n",outstring_length);
		 
						retval = gc_write((void*) out_to_gc_string.c_str(), outstring_length);
						//printf("\twrite() back to gc was %d (%s)\n",retval,out_to_gc_string.substr(0,retval).c_str());

						if (retval>0)
						{
							out_to_gc_string = out_to_gc_string.substr(retval);
						}
					}
				}

				if (FD_ISSET(gc100_socket, &fdset))
				{
					// READ from GC100
					write_size = gc_read((void*) output_buffer, 500);

					if (write_size > 0)
					{
						output_buffer[write_size] = '\0';
						//printf("\t\t\t\t\tRead from GC: %s size=%d\n", output_buffer, write_size);
						out_to_dev_string += output_buffer;
					}

					// Write from buffer back to /dev/gcs device

					to_dev_length = out_to_dev_string.length();

					if (to_dev_length > 0)
					{
						/* if ((int) to_dev_length / 100 != (int) last_dev_size / 100)
						{
							printf("Stream to dev size is %d\n", to_dev_length);
							last_dev_size = to_dev_length;
						}*/
						retval = write(device_fd, (void *) out_to_dev_string.c_str(), to_dev_length);

						if (retval > 0)
						{
							//printf("\t\t\t\t\twrite() back to output dev was  %d (%s)\n",retval,out_to_dev_string.substr(0,retval).c_str());
							out_to_dev_string = out_to_dev_string.substr(retval);
							//printf("\t\t\t\t\tremaining to output dev is (%s)\n",out_to_dev_string.c_str());
						}
					}
				}
				sleep_ms(250);
				iter++;
			} // end loop
		}
		else
		{
			printf("There must have been a file error\n");
		} // if file err ok
	}  // if args ok
	printf("Exiting program\n");
}
