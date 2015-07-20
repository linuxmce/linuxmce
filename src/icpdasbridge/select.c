#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include "msw.h"


#define PORT "8888"   			// port we're listening on
#define MESSAGE_SIZE 512 		// max read buffer size for sockets
#define MAX_CLIENTS 10			// max number of TCP sockets/clients (never exceed 50)
#define MAX_PORT_NAME 100		// max length of port name
#define MAX_CMD 100 			// maximum length of command value
#define CFG_PATH "modules.txt"	// Path to config file
#define POLL_INTERVAL 30		// Interval (ms) to poll sensors

#define FMT_CHANGE_D "CHANGE:COM=%u:ADDR=%x:PORT=%u:VAL=%u:OLD_VAL=%u\n"
#define FMT_CHANGE_A "CHANGE:COM=%u:ADDR=%x:PORT=%u:VAL=%+.2f:OLD_VAL=%+.2f\n"
#define FMT_INFO_D   "INFO:COM=%u:ADDR=%x:PORT=%u:VAL=%u:NAME=%s\n"
#define FMT_INFO_A   "INFO:COM=%u:ADDR=%x:PORT=%u:VAL=%+.2f:NAME=%s\n"

// Definition of a single port (i.e single connection)
// inside the module
struct port_s {
	unsigned int number;			// Port number in the module
	char *name;						// Port name/description
	struct timeval last_changed;	// Timestamp of last port state change
	unsigned int i_value;			// Current port state (digit)
	unsigned int i_old_value;		// Previous port state before change (digit)
	float		 f_value;			// Current port state (analogue)
	float		 f_old_value;		// Previous port state before change (analogue)
	unsigned int fucked;			// Flag if port device is fucked 
									// and sends 1 when off and 0 when on.
	int changed;					// Flag if port was changed during last poll
	unsigned int min_poll;			// Minimum number of time (ms) between module polls
};
typedef struct port_s port_t;

// Definition of a module of ICPDAS (plugged or remote IO)
struct module_s {
	unsigned int address;			// Address of the module
	unsigned int model;				// Model of the module
	unsigned int count;				// Number of ports used in the module
	unsigned int com_port;			// COM port number from which this 
									// module can be reached
	struct timeval last_changed;	// Time of last port state change
	unsigned int value;				// Current Digital module state 
									// (sum of all ports state)
	unsigned int old_value;			// Previous Digital module state before change
	char		c_value[80];		// Current Analogue module state
	char		c_old_value[80];	// Previous Analogue module state before change
	int changed;					// Flag if port was changed during last poll
	unsigned int min_poll;			// Minimum number of time (ms) between port polls/reports
	port_t **ports;					// Array of ports in the module
};
typedef struct module_s module_t;

// Definition of a whole controller (ICPDAS)
struct controller_s {
	unsigned int count;				// Number of ports
	module_t **mods;				// Modules
};
typedef struct controller_s controller_t;

// Function prototypes:
void		trim(char *);
int			read_config(char *, controller_t *);
int			icpdas_com_d(int, char *,int *, char *);
int			send_command(controller_t *,char *,int);
int			send_command_slot(module_t *,unsigned int);
int			calc_module_value(module_t *, port_t *, int);
int			send_command_com(module_t *,unsigned int);
void		print_status(controller_t *,int);
module_t 	*find_module(controller_t *,unsigned int,unsigned int);
module_t	*add_module(controller_t *,unsigned int,unsigned int,unsigned int);
port_t		*find_port(module_t *,unsigned int);
int			get_module_state(module_t *);
int			get_controller_state(controller_t *);
int			get_module_state_d8(module_t *, unsigned int *);
int			get_module_state_d87(module_t *, unsigned int *, char *);

int main (void) {
	controller_t *ctrl = NULL;	// our controller data struct
	module_t *module = NULL;
	port_t *port = NULL;

	fd_set master;	// master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;		// maximum file descriptor number

	int listener;	 // listening socket descriptor
	int newfd;		// newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[MESSAGE_SIZE];	// buffer for client data
	int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

	int yes=1;		// for setsockopt() SO_REUSEADDR, below
	int i, j, k, rv, rs;

	struct timeval tv;

	struct addrinfo hints, *ai, *p;

	// initialize the struct if not yet
	ctrl = malloc(sizeof(controller_t));
	ctrl->count = 0;

	if (read_config(CFG_PATH,ctrl) == 0) {
		print_status(ctrl,0);
	} else {
		printf("Failed to read config\n");
		return -1;
	}

	for (i = 0; i < 7; i++) {
		Open_Slot(i);
	}

	rs = Open_Com(COM3,115200, Data8Bit, NonParity, OneStopBit);
	if (rs > 0) {
		printf("Failed to open COM3 port\n");
		return -1;
	}

	FD_ZERO(&master);	// clear the master and temp sets
	FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "ERROR: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for(p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "ERROR: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, MAX_CLIENTS) == -1) {
		perror("ERROR: failed to listen");
		exit(3);
	}

	// add the listener to the master set
	FD_SET(listener, &master);

	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	// main loop
	for(;;) {

		tv.tv_sec = 0;
		tv.tv_usec = POLL_INTERVAL * 1000;

		read_fds = master; // copy it
		
		rs = select(fdmax+1, &read_fds, NULL, NULL, &tv);

		if (rs == -1) {
			perror("ERROR: Failed to select from sockets");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				if (i == listener) {
					// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(
								listener,
								(struct sockaddr *)&remoteaddr,
								&addrlen
					);

					if (newfd == -1) {
						perror("ERROR: Failed to accept new connection");
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {	// keep track of the max
							fdmax = newfd;
						}
						sprintf(buf,"INFO:CONNECTED:SID=%d:IP=%s\n",
							newfd,
							inet_ntop(
								 AF_INET,
								 &(((struct sockaddr_in *)&remoteaddr)->sin_addr),
								 remoteIP, INET6_ADDRSTRLEN
							)
						);
						printf("%s",buf);
						send(newfd,buf,strlen(buf),0);
					}
				} else {
					// handle data from a client
					memset(buf,0,sizeof(buf));
					if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							printf("INFO:DISCONNECTED:SID=%d\n", i);
						} else {
							perror("ERROR:Failed to recieve data from socket, closing socket");
						}
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} else {

						// we got some data from a client
						// so send it to controller

						// remove end of lines
						trim(buf);

						if (send_command(ctrl,buf,i) == 0) {
						
							printf("INFO:SID=%u:%s\n",i,buf);

							// confirm to client the command was sent
							sprintf(buf,"OK\n");
							if (send(i,buf,strlen(buf),0) < 0) {
								perror("WARNING:Failed to confirm command receival to client");
							}
						} else {
							printf("ERROR:FAILED TO SEND COMMAND %s\n",buf);
						}
					}
				}
			}
		}
		// update clients with data from sensors
		if (get_controller_state(ctrl) > 0) {

			// update everyone about changes in states
			for (i = 0; i < ctrl->count; i++) {
				module = ctrl->mods[i];

				if (module->changed) {
					for (j = 0; j < module->count; j++) {
						port = module->ports[j];

						if (port->changed) {
							switch (module->model) {
								case 0x87017:
									sprintf(buf,FMT_CHANGE_A,
										module->com_port,
										module->address,
										port->number,
										port->f_value,
										port->f_old_value
									);
									break;
								default:
									sprintf(buf,FMT_CHANGE_D,
										module->com_port,
										module->address,
										port->number,
										port->i_value,
										port->i_old_value
									);
									break;
							}
							printf("%s",buf);
						
							for (k = 0; k <= fdmax; k++) {
								if (k != listener) {
									// update whoever can be updated
									send(k,buf,strlen(buf),0);
								}
							}
							
						}
					}
				}
			}
		}
	}

	Close_Com(COM3);

	for (i = 0; i < 7; i++) {
		Close_Slot(i);
	}

	return 0;
}


// Find module in contrller struct by module com port
// and address. Return pointer to module to NULL
module_t * find_module(controller_t *ctrl,unsigned int com_port,unsigned int address) {
	int i;

	for (i = 0; i < ctrl->count; i++) {
		if (ctrl->mods[i]->com_port == com_port && ctrl->mods[i]->address == address) {
			return ctrl->mods[i];
		}
	}

	return NULL;
}


// Add module to the controller struct and assign
// defaults and provided data. Return pointer to
// the module
module_t * add_module (controller_t *ctrl,unsigned int com_port,unsigned int address,unsigned int model) {
	module_t *module = NULL;

	// allocate or increase the size of the mods
	if (ctrl->count == 0) {
		ctrl->mods = malloc(sizeof(module_t *));
	} else {
		ctrl->mods = realloc(
			ctrl->mods, 
			(ctrl->count + 1) * sizeof(module_t *)
		);
	}

	// allocate and put data into mod struct
	module = malloc(sizeof(module_t));
	module->address = address;
	module->model = model;
	module->com_port = com_port;
	module->changed = 0;
	module->count = 0;
	gettimeofday(&module->last_changed,NULL);
	module->value = 0;
	module->old_value = 0;
	strcpy(module->c_value,"");
	strcpy(module->c_old_value,"");
	module->min_poll = 30000;

	ctrl->mods[ctrl->count] = module;
	ctrl->count++;

	return module;
}

// Find port by port number in module. Return pointer
// to port or NULL
port_t * find_port (module_t *module,unsigned int port_number) {
	int i;

	for (i = 0; i < module->count; i++) {
		if (module->ports[i]->number == port_number) {
			return module->ports[i];
		}
	}

	return NULL;
}

// Add port to module struct. Set default and provided values
// to the port. Return pointer to the port
port_t * add_port (module_t *module,unsigned int port_number,unsigned int fucked, unsigned int min_poll,char *name) {
	port_t *port = NULL;

	// allocate or increase the size of the ports for this module
	if (module->count == 0) {
		module->ports = malloc(sizeof(port_t *));
	} else {
		module->ports = realloc(
			module->ports, 
			(module->count + 1) * sizeof(port_t *)
		);
	}

	// allocate and put data into the port struct
	port = malloc(sizeof(port_t));
	port->number = port_number;
	port->name = strdup(name);
	port->i_value = 0;
	port->i_old_value = 0;
	port->f_value = 0;
	port->f_old_value = 0;
	port->fucked = fucked;
	port->min_poll = min_poll;
	gettimeofday(&port->last_changed,NULL);

	module->ports[module->count] = port;
	module->count++;

	if (module->min_poll > port->min_poll) {
		module->min_poll = port->min_poll;
	}

	return port;
}

int get_analogue_values_from_string (char * string, float * f_val) {
	char indi;
	int i = 0;

	if (sscanf(string,"%c%f%f%f%f%f%f%f%f",&indi,&f_val[0],&f_val[1],&f_val[2],&f_val[3],&f_val[4],&f_val[5],&f_val[6],&f_val[7]) == 9) {
		return 0;
	}

	return 1;
}

int parse_analogue_string (char * string) {
	float f_val[8];

	if (get_analogue_values_from_string(string,f_val)) {
		return 1;
	}
	sprintf(string,">%+2.2f%+2.2f%+2.2f%+2.2f%+2.2f%+2.2f%+2.2f%+2.2f",
		f_val[0],
		f_val[1],
		f_val[2],
		f_val[3],
		f_val[4],
		f_val[5],
		f_val[6],
		f_val[7]
	);

	return 0;
}

int icpdas_com_d(int com,char *cmd, int *value, char * c_value) {
	int wRetVal;
	char szReceive[80],temp[80],indi;
	float f_val[8];
	WORD wT;

	wRetVal = Send_Receive_Cmd(com,cmd,szReceive, 20, 0, &wT);
	if (wRetVal) {
		printf("!PLAIN:S=%s:E=%d\n",cmd,wRetVal);
		return wRetVal;
	}
	if (sscanf(szReceive,"%c%f%f%f%f%f%f%f%f",
							&indi,
							&f_val[0],
							&f_val[1],
							&f_val[2],
							&f_val[3],
							&f_val[4],
							&f_val[5],
							&f_val[6],
							&f_val[7]) == 9) {
		if (parse_analogue_string(szReceive)) {
			return 666;
		}
		strcpy(c_value,szReceive);
		*value = 0;
	} else {
		sscanf(szReceive,"%c%s",&indi,temp);
		*value = (int)strtol(temp,NULL,16);
		strcpy(c_value,"");
	}
	return 0;
}

int get_module_state_d8 (module_t *module,unsigned int *value) {
	switch (module->model) {
		case 0x8040:  //DI on 8k
			*value = DI_32(module->address);
			break;
		case 0x8064:  //DO on 8k
			*value = DO_8_RB(module->address);
			break;
		default:
			printf("ERROR:Unknown module model '%x' on COM%u with address %x\n",
				module->model,
				module->com_port,
				module->address
			);
			return -1;
	}
	return 0;
}

int get_module_state_d87 (module_t *module, unsigned int *value, char * c_value) {
	int retVal;
	char szSent[80];

	switch (module->model) {
		case 0x87064: //DO
			sprintf(szSent,"@%X",module->address);
			retVal = icpdas_com_d(module->com_port,szSent,value,c_value);
			if (retVal == 0) {
				*value >>= 8;
				return 0;
			}
			break;
		case 0x87053: //DI
			sprintf(szSent,"@%X",module->address);
			retVal = icpdas_com_d(module->com_port,szSent,value,c_value);
			if (retVal == 0) {
				return 0;
			}
			break;
		case 0x7065: //DIO
			sprintf(szSent,"$%X6",module->address);
			retVal = icpdas_com_d(module->com_port,szSent,value,c_value);
			if (retVal == 0) {
				*value >>= 16;
				return 0;
			}
			break;
		case 0x87017: //AI
			sprintf(szSent,"#%X",module->address);
			retVal = icpdas_com_d(module->com_port,szSent,value,c_value);
			if (retVal == 0) {
				return 0;
			}
			break;
		default:
			printf("ERROR:Unknown module model '%x' on COM%u with address %x\n",
				module->model,
				module->com_port,
				module->address
			);
			return -1;
	}

	printf("Failed to poll module (%x,%x): %d\n",module->address,module->model,retVal);
	return retVal;
}

// Poll ICPDAS to get update on module state.
// Return number of ports changed or -1 on error
int get_module_state (module_t *module) {
	int retVal,i,j,changed,early,o_time,n_time;
	unsigned int val,state;
	char c_val[80];
	float f_val[8];
	port_t *port = NULL;
	struct timeval c_time;

	gettimeofday(&c_time,NULL);

	o_time = (module->last_changed.tv_sec * 1000) + (module->last_changed.tv_usec / 1000);
	n_time = (c_time.tv_sec * 1000) + (c_time.tv_usec / 1000);

	if (n_time < ( o_time + module->min_poll)) {
		for (j = 0; j < module->count; j++) {
			module->ports[j]->changed = 0;
		}
		return 0;
	}

	// act according to the module model
	switch (module->model) {
		case 0x87064: 	//DO on 87k
		case 0x87053: 	//DI on 87k
		case 0x7065:	//DO on 7k
		case 0x87017:	//AI on 87k
			retVal = get_module_state_d87(module,&val,c_val);
			break;
		case 0x8040:  //DI, DO on 8k
		case 0x8064:
			retVal = get_module_state_d8(module,&val);
			break;
		default:
			printf("ERROR:Unknown module model '%x' on COM%u with address %x\n",
				module->model,
				module->com_port,
				module->address
			);
			return -1;
	}
	if (retVal != 0) {
		return -1;
	}

	changed = 0;
	if (module->value != val) {
		module->old_value = module->value;
		module->changed = 1;
		module->value = val;
		gettimeofday(&module->last_changed,NULL);
		for (j = 0; j < module->count; j++) {
			port = module->ports[j];
			if (((port->last_changed.tv_sec * 1000) + (port->last_changed.tv_usec / 1000)) < n_time) {
				state = (((val >> port->number) & 1) ^ port->fucked);
				if (port->i_value != state) {
					port->i_old_value = port->i_value;
					port->i_value = state;
					port->changed = 1;
					gettimeofday(&port->last_changed,NULL);
					changed++;
				} else {
					port->changed = 0;
				}
			} else {
				port->changed = 0;
			}
		}
	} 
	if (strcmp(module->c_value,c_val) != 0) {
		strcpy(module->c_old_value,module->c_value);
		module->changed = 1;
		strcpy(module->c_value,c_val);
		gettimeofday(&module->last_changed,NULL);
		get_analogue_values_from_string(c_val,f_val);	
		for (j = 0; j < module->count; j++) {
			port = module->ports[j];
			if (((port->last_changed.tv_sec * 1000) + (port->last_changed.tv_usec / 1000)) < n_time) {
				if (port->f_value != f_val[j]) {
					port->f_old_value = port->f_value;	
					port->f_value = f_val[j];
					port->changed = 1;
					gettimeofday(&port->last_changed,NULL);
					changed++;
				} else {
					port->changed = 0;
				}
			} else {
				port->changed = 0;
			}
		}	
	}
	
	if (changed) {
		return changed;
	}

	module->changed = 0;
	for (j = 0; j < module->count; j++) {
		module->ports[j]->changed = 0;
	}
	
	return 0;
}

int get_controller_state (controller_t *ctrl) {
	int i,total = 0;

	for (i = 0; i < ctrl->count; i++) {
		total += get_module_state(ctrl->mods[i]);
	}

	return total;
}

// remove all \n & \r from end of line
void trim (char *txt) {
	int i = strlen(txt) - 1;
	
	while (i > 0 && (txt[i] == 10 || txt[i] == 13)) {
		txt[i] = '\0';
		i--;
	}
}

// Read a configuration file with all modules and ports
// from cfg_path file and place all info into ctrl structure
// recursivly
int read_config (char *cfg_path, controller_t *ctrl) {
	unsigned int address,model,port_number,com_port,fucked,min_poll; // temp vars for reading config
	char name[MAX_PORT_NAME],line[MESSAGE_SIZE];
	int line_count = 0;	// counters
	FILE *fp;
	module_t *module = NULL;
	port_t *port = NULL;

	// check we can open and read a file or return with error
	if ((fp = fopen(cfg_path,"r+")) == NULL) {
		printf("No modules.txt file found\n");
		return -1;
	}

	// read file line by line
	while (fgets(line,sizeof(line),fp)) {
		line_count++;

		// if line matches the format - parse it out
		if (sscanf(line,"%x:%x:%x:%x:%x:%u:%[^\n]\n",&com_port,&address,&model,&port_number,&fucked,&min_poll,name) == 7) {

			// find or create a module
			if ((module = find_module(ctrl,com_port,address)) == NULL) {
				module = add_module(ctrl,com_port,address,model);
			} else if (module->model != model) {
				printf("WARNING: config line %d module model doesn't match previous versions %x != %x, assuming %x\n",
					line_count,
					model,
					module->model,
					module->model
				);
			}

			port = add_port(module,port_number,fucked,min_poll,name);
		}
	}

	// done reading
	fclose(fp);

	return 0;
}

void print_status (controller_t *ctrl, int socket) {
	int i,j;
	char buf[MAX_CMD];
	char * format;
	module_t *module = NULL;
	port_t *port = NULL;

	for (i = 0; i < ctrl->count; i++) {

		module = ctrl->mods[i];
		for (j = 0; j < module->count; j++) {

			port = module->ports[j];
			switch (module->model) {
				case 0x87017:
					sprintf(buf,FMT_INFO_A,
							module->com_port,
							module->address,
							port->number,
							port->f_value,
							port->name
					);
					break;
				default:
					sprintf(buf,FMT_INFO_D,
							module->com_port,
							module->address,
							port->number,
							port->i_value,
							port->name
					);
					break;
			}
			if (!socket) {
				printf("%s",buf);
			} else {
				send(socket,buf,strlen(buf),0);
			}
		}
	}
}

int send_command_com(module_t *module, unsigned int value) {
	WORD retVal;
	char szSent[80];
	char c_value[80];

	switch (module->model) {
		case 0x7065:  // CMD: #AA00(DATA 00-FF)
			sprintf(szSent,"#%02X00%02X",module->address,value);
			break;
		case 0x87064: // CMD: @AA(DATA 00-FF)
			sprintf(szSent,"@%02X%02X",module->address,value);
			break;
		default:
			return -1;
	}

	return icpdas_com_d(module->com_port,szSent,&value,c_value);
}

int send_command_slot(module_t *module, unsigned int value) {
	switch (module->model) {
		case 0x8064:
			DO_16(module->address,value);
			break;
		default:
			return -1;
	}
	return 0;
}

int calc_module_value (module_t *module, port_t *port, int value) {
	return module->value 
			^ (-(value ^ port->fucked) ^ module->value) 
			& (1 << port->number);
}

int send_command (controller_t *ctrl,char *cmd, int socket) {
	unsigned int address,com_port,port_number,i_value;
	char c_cmd[MAX_CMD],szReceive[80],temp[80],indi,c_value[80];
	float fBuf[12];
	WORD wT;
	int i,j,wRetVal;
	module_t *module = NULL;
	port_t *port = NULL;

	if (sscanf(cmd,"SET:%x:%x:%x:%[^\n]",&com_port,&address,&port_number,c_cmd) == 4) {
		if ((module = find_module(ctrl,com_port,address)) == NULL) {
			printf("FAILED:MOD_404:%s\n",cmd);
			return -1;
		}

		if ((port = find_port(module,port_number)) == NULL) {
			printf("FAILED:PORT_404:%s\n",cmd);
			return -1;
		}

		switch (module->model) {
			case 0x87064:
			case 0x7065:
				sscanf(c_cmd,"%u",&i_value);
				if (port->i_value == i_value) {	
					printf("INFO:NO_CHANGE:%s\n",cmd);
				} else {
					return send_command_com(
						module,
						calc_module_value(module,port,i_value)
					);
				}
				break;
			case 0x8064:
				sscanf(c_cmd,"%u",&i_value);
				if (port->i_value == i_value) {
					printf("INFO:NO_CHANGE:%s\n",cmd);
				} else {
					return send_command_slot(
						module,
						calc_module_value(module,port,i_value)
					);
				}
				break;
			default:
				printf("FAILED:MOD_UNSUPPORTED:%s\n",cmd);
				return -1;
		}
		return 0;
	} else if (strcmp(cmd,"STATUS") == 0) {
		print_status(ctrl,socket);
		return 0;

	} else if (sscanf(cmd,"RAW:%s[^\n]",c_cmd) == 1) {
		wRetVal = icpdas_com_d(3,c_cmd,&i_value,c_value);
		if (wRetVal) {
			printf("PLAIN:S=%s:E=%d\n",c_cmd,wRetVal);
			return -1;
		}
		if (strlen(c_value) > 0) {
			printf("PLAIN(A):S=%s,R=%s\n",c_cmd,c_value);
		} else {
			printf("PLAIN(D):S=%s,R=%u\n",c_cmd,i_value);
		}
		return 0;
	}
	printf("FAILED:BAD_FORMAT:'%s'\n",cmd);
	return -1;
}
