#define ERR_OPEN                        1
#define ERR_TIMEOUT                     4
#define ERR_FLOCK                       22
#define ERR_STTY                        23


int OpenSerialPortEx (const char Pname[],int *port);
int ReadSerialStringEx (int dev,char pnt[],int len,long timeout);
int WriteSerialStringEx (int dev,char pnt[],int len);
