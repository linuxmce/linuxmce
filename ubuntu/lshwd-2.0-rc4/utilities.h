#ifndef LSHWD_UTILITIES
#define LSHWD_UTILITIES

#define CARDSDB "/usr/share/hwdata/Cards"
#define XPATH "/usr/X11R6/bin/"
#define XMODPATH "/usr/X11R6/lib/modules/drivers/"
#define XMODPATH_XORG7 "/usr/lib/xorg/modules/drivers/"

#define SHARED_PATH 		"/usr/share/hwdata"		/* shared path which contains \
								   pcitable/usbtable/pcmciatable*/
#define MODPROBE_PGM		"/sbin/modprobe"		/* the secret of life  		*/

#ifdef DEBUG
#undef DEBUG
#define DEBUG(s...) fprintf(stderr, s)
#else
#define DEBUG(s...) ;
#endif

struct xinfo {
	char xserver[16];
	char xmodule[16];
	char xdesc[128];
	char xopts[128];
};

int get_string_from_file(char *prefix, char *path, char *filename, char *buf, short buf_len);
unsigned int get_hexvalue_from_file(char *prefix, char *path, char *filename);
int compare(char *string1,char *string2,int len);
inline void init_lookup_block(char *filename);
void cleanup_lookup_block(void);
int read_file(char *filename, char **block);
int lookup_module(int vendorid, int deviceid, int subvendorid, int subdeviceid, 
		char* module, int sizeofbuf, char* description, int sizeofdesc);
char *find_network_devices(char *module);
int execCommand(char **argv);
int isLoaded(char *module);
int loadModule(char *module);
int removeModule(char *module);
int exists(char *filename);
struct xinfo *getxinfo ( char* devdesc, char* devdriver, char outputxinfo );
char *get_pci_classname(unsigned int classid);
char *get_usb_classname(unsigned int classid);


#endif /* LSHWD_UTILITIES */
