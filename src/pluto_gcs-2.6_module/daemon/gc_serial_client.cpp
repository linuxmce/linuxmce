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

// Global variables are the args and socket
std::string ip_addr;
std::string input_fifo_name, output_fifo_name;
int port;
int gc100_socket;
FILE *input_fifo;
FILE *output_fifo;
bool readonly;
std::string id;
int loop_no;

int open() {
   int return_value;
   struct hostent *hp;
   struct sockaddr_in sin;
   int res;

   return_value=0;
   printf("Opening comm to GC\n");

   hp = gethostbyname(ip_addr.c_str());
   if (!hp) {
       fprintf(stderr,"gc_seriald: Did not resolve hp\n");
       return_value=-1; // TODO: figure return value
   } else {
       gc100_socket=socket(PF_INET, SOCK_STREAM,0);
       if (gc100_socket < 0) {
           fprintf(stderr,"gc_seriald: Did not create gc100 socket\n");
           return_value=-1; // TODO: figure return value
       } else {
           memset(&sin,0,sizeof(sin));
           sin.sin_family=AF_INET;
           sin.sin_port=htons(port);

           memcpy(&sin.sin_addr, hp->h_addr, sizeof(sin.sin_addr));
           if (connect(gc100_socket, (sockaddr *) &sin, sizeof(sin))) {
               fprintf(stderr,"gc_seriald: Unable to connect to GC100 on socket.\n");
               close(gc100_socket);
	       return_value=-1; // TODO: figure return value
           } else {
               res = fcntl(gc100_socket, F_GETFL);
               if ((res < 0) || (fcntl(gc100_socket, F_SETFL, res | O_NONBLOCK) < 0)) {
                   fprintf(stderr,"gc_seriald: Unable to set flags on GC100 socket.\n");
                   close(gc100_socket);
	           return_value=-1; // TODO: figure return value
               } else {
                   printf("gc_seriald: GC100 socket connect OK\n");
               }
           } // end if connect
       } // end if gc socket > 0
   }

   return return_value;
}

int write(void *buffer, int size) {
   int return_value;
   return_value=0;

   return_value=write(gc100_socket,buffer,size);

   return return_value;
}

int read(void *buffer, int size) {
   int return_value;

   return_value=read(gc100_socket, buffer, size);

   return return_value;
}

int close() {
   int return_value;

   return_value=close(gc100_socket);

   return return_value;
}

int main(int argc, char **argv) {

    readonly=false;
    loop_no=0;

    if (argc<3) {
	printf("Usage: gc_serial_client <devicename> <id> [r] \n\n");
    } else {

	input_fifo_name=std::string(argv[1]);
	char input_buffer[512];
	char output_buffer[512];
	int retval;
	int iter;
	bool file_error;
	int read_size, write_size;

	id =std::string(argv[2]);
	printf("Args received are input is %s id=%s\n",input_fifo_name.c_str(),id.c_str());

	if (argc>3) {
	    if (strcasecmp(argv[3],"r")==0) {
	        printf("READ_ONLY\n");
		readonly=true;
	    }
	}
	file_error=false;

	if (readonly) {
	    input_fifo=fopen(input_fifo_name.c_str(),"r");
	} else {
	    input_fifo=fopen(input_fifo_name.c_str(),"r+");
	}

	if (input_fifo==NULL) {
	     printf("open() input device open returned NULL\n");
	     file_error=true;
	}

	iter=1;
	// Begin test driver loop

	if (!file_error) {
	  while (true) {
	    //sprintf(output_buffer,"Test output string iter=%d-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32-33-34-35-36-37-38-39-40-41-42-43-44-45-46-47-48-49-50-51-52-53-54-55-56-57-58-59-60-61-62-63-64-65-66-67-68-69-70-71-72-73-74-75-76-77-78-79-80-81-82-83-84-85-86-87-88-89-90-91-92-93-94-95-96-97-98-99-100",iter++);
	    sprintf(output_buffer,"%s/%d",id.c_str(),iter++);

	    read_size=fread((void*) input_buffer,1,1,input_fifo);
	    //printf("client: read from dev returned  %d\n",read_size);

	    if (read_size>0) {
		input_buffer[read_size]='\0';
	        //printf("\tread() returned %s\n",input_buffer);

		// Binary version
		printf("read() returned: ");
		for (int i=0; i<read_size; i++) {
		    printf("%02x ",(unsigned char) input_buffer[i]);
		}
		printf("\n");
	    }

	    sleep(1);

	    //if ((((int) (loop_no /50)) %2) ==0 ) { // Once in a while swtich back to read only
	    //if (loop_no < 10 ) { // Once in a while swtich back to read only
	    if (true) {
	     if (!readonly) {
	        retval=fwrite((void *) output_buffer,1,strlen(output_buffer),input_fifo);
	        //retval=fwrite((void *) output_buffer,1,1,input_fifo);
                //printf("client: write() back to dev was  %d\n",retval);
	     }
	    }
	    loop_no++;
	  }
	}
	
    }
}
