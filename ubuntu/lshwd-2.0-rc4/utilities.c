/*****************************************************************************
 *
 * lshwd utilities - procedures for cleaner code
 *
 * Copyright (C) 2004-2005 Elia Yehuda, z4ziggy@user-contributions.org
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utilities.h"

char *lookup_block;			/* allocated buffer for modules list (table files)	*/
unsigned int lookup_block_len;			/* length of allocated buffer (table files) 		*/

/*
 * generic modules to be changed. format = "old name", "new name"
 * change to suit whatever changes are needed in the table files (pci/usb/pcmcia)
 */

char *
updated_modules[][2] =
{
	//{ "nv", "nvidia"},
	{ "Mouse:USB|Wheel", "usbhid"},
	{ "Mouse:USB|Generic", "usbhid"},
	{ "Mouse:USB|Microsoft Explorer", "usbhid"},
	{ "via82cxxx_audio", "snd_via82xx"},
	{ "usb-uhci", "uhci_hcd"},						// usb-uhci=2.4 uhci-hcd=2.6 
	{ "usb-ohci", "ohci_hcd"}						// usb-uhci=2.4 uhci-hcd=2.6
};


/* quick compare strings procedure */
int
compare(char *string1,char *string2,int len)
{
	while(len && (*string1++ == *string2++)) len--;
	return(!len);
}

/* search in updated_modules for module name, and replace with updated one if found */
void
updated_module_name(char* module)
{
	uint i = 0;
	for (i=sizeof(updated_modules)/sizeof(updated_modules[0]); i--;)
	{
		if (compare(updated_modules[i][0], module, strlen(updated_modules[i][0])))
		{
			sprintf(module, updated_modules[i][1]);
		}
	}
}


/* 
 * search for module name and description according to vendorid and deviceid.
 * the search uses buffer (lookup_block) allocated by init_lookup_block which must to be called prior 
 */
int
lookup_module(int vendorid, int deviceid, int subvendorid, int subdeviceid, 
		char* module, int sizeofbuf, char* description, int sizeofdesc)
{
	char cmpbuf[34];

	//module = description = "unknown";
	/* formatting the string to search for */
	if (subvendorid == 0 && subdeviceid == 0) 
	{
	        sprintf(cmpbuf, "0x%04x\t0x%04x", vendorid, deviceid);
	}
	else 
	{
	        sprintf(cmpbuf, "0x%04x\t0x%04x\t0x%04x\t0x%04x", vendorid, deviceid, subvendorid, subdeviceid);
	}
	//memset(module, 0, sizeofbuf);
	/* default module name... */
	sprintf(module,"unknown");
	uint i = 0;
	char *buf = lookup_block;
	while (i<lookup_block_len)
	{
		if (*buf == cmpbuf[0])
		{
			if (compare(buf, cmpbuf, strlen(cmpbuf)))
			{
				for (; *buf++ != '\"'; );
				/*lets grab the pcitable module name*/
				for (i=0; (i < sizeofbuf) && (*buf != '\n') && (*buf != '\"') ; module[i++] = *buf++);
				module[i] = '\0';
				/* check if module name should be replaced according to updated_modules */
				updated_module_name(module);
				*buf++;
				for (; *buf++ != '\"'; );
				for (i=0; (i < sizeofdesc) && (*buf != '\n') && (*buf != '\"') ; description[i++] = *buf++);
				description[i] = '\0';
				return 1;
			}
		}
		for (; (i++ < lookup_block_len) && (*buf++ != '\n') ; );
	}
	/* if no success and we had subvendorid, lets try without sub-id's */
	if (subvendorid != 0 || subdeviceid != 0) 
		return lookup_module(vendorid, deviceid, 0, 0, module, sizeofbuf, description, sizeofdesc);
	return 0;
}

/* 
 * free lookup_block after calling init_lookup_block 
 */
void
cleanup_lookup_block(void)
{
	free(lookup_block);
	lookup_block = NULL;
}

/* 
 * allocating buffer (lookup_block) and reading table file to memory for quick searching 
 */
inline void
init_lookup_block(char *filename)
{
	lookup_block_len = read_file(filename, &lookup_block);
}

int
read_file(char *filename, char **block)
{
	char sharedfile[1024];
	int block_len = 0;
	FILE *f;
	
	sprintf(sharedfile, "%s/%s", SHARED_PATH, filename);
	block_len = 0;
	if ((f=fopen(filename, "r")) || (f=fopen(sharedfile,"r")))
	{
		fseek(f, 0L, SEEK_END);
		block_len = ftell(f);
		rewind(f);
		*block = (char*)realloc(*block, block_len);
		fread(*block, sizeof(char), block_len, f);
		fclose(f);
	}
	return block_len;
}


#define USE_FORK_COMMAND
#ifdef USE_FORK_COMMAND
/*
 * executing command to /dev/null 
 */
int
execCommand(char **argv)
{
	int status, pid;

	if (!(pid = fork()))
	{
		close(0);
		close(1);
		close(2);
		open("/dev/null", 2);
		open("/dev/null", 1);
		open("/dev/null", 0);
		execv(argv[0], argv);
		exit(-1);
	}
	waitpid(pid,&status, 0); //WCONTINUED);
	//wait(&status);
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	return -1;
}
#else
int
execCommand(char **argv)
{
	char cmd[1024]={0};
	int n;
	for (n=0;argv[n];n++)
	{
		strcat(cmd, argv[n]);
		strcat(cmd, " ");
	}
	//strcat(cmd,"> /dev/null 2>&1");
	//strcat(cmd,"&>/dev/null");
	strcat(cmd,">&/dev/null");
	return system(cmd);
}
#endif

#define QM_INFO 5
struct module_info
{
	unsigned long addr;
	unsigned long size;
	unsigned long flags;
	long usecount;
};

//#define USE_SYSCALL_MODULE_CHECK
#ifdef USE_SYSCALL_MODULE_CHECK
int
isLoaded(char *module)
{
	struct module_info mi;
	int tmp;
	return !syscall(SYS_query_module,module,QM_INFO,&mi,sizeof(mi),&tmp);
}
#else
/*
 * using rmmod technique to search for mdoule, since syscall(SYS_query_module... 
 * doesnt work...
 */
/* If we can check usage without entering kernel, do so. */
int 
isLoaded(char *module)
{
	FILE *module_list;
	char line[10240], name[64];
	unsigned long size, refs;
	int scanned;
	
	if (!(module_list = fopen("/proc/modules", "r")))
	{
		DEBUG("can't open /proc/modules: %s\n", strerror(errno));
		return 0;

	}
	while (fgets(line, sizeof(line)-1, module_list) != NULL) 
	{
		if (strchr(line, '\n') == NULL) 
		{
			DEBUG("V. v. long line broke lshwd.\n");
			refs = 0;
			goto out;
		}

		scanned = sscanf(line, "%s %lu %lu", name, &size, &refs);
		DEBUG("%s %s\n",name, module);
		if (strcmp(name, module) == 0)
		{
			refs = 1;
			goto out;
		}
	}
	DEBUG("Module %s does not exist in /proc/modules\n", module);
	refs = 0;
out:
	fclose(module_list);
	return refs;
}
#endif

/*
 * loading module using modprobe 
 */
int
loadModule(char *module)
{
	char *args[] = { MODPROBE_PGM, NULL, "-q", NULL };
	if (isLoaded(module))
	{
		return -1;
	}
	else
	{
		args[1] = module;
		DEBUG("> modprobe %s\n", module);
		return execCommand(args);
	}
}

/*
 * removing module using modprobe 
 */
int
removeModule(char *module)
{
	char *args[] = { MODPROBE_PGM, "-r", NULL, NULL };
	if (isLoaded(module))
	{
		args[2] = module;
		DEBUG("> modprobe -r %s\n", module);
		return execCommand(args);
	}
	else
	{
		return -1;
	}
}

/*
 * checking if module has attached network device 
 */
char *
find_network_devices(char *module)
{
	DIR *dirp;
	struct dirent *direntry;
	char dirbuf[127], result[127], *res = result;

	memset(dirbuf, 0, sizeof(dirbuf));
	memset(result, 0, sizeof(result));
	sprintf(dirbuf, "/proc/net/%s", module);
	/* check if directory with module name exists */
	if ((dirp = opendir(dirbuf)) != NULL)
	{
		/* we found a matching module dir, lets enum */
		while ((direntry = readdir(dirp)) != NULL)
		{
			if (direntry->d_type == DT_REG)
			{
				memset(dirbuf, 0, sizeof(dirbuf));
				sprintf(dirbuf,"%s" ,direntry->d_name);
				if (strlen(result)) strcat(result, ",");
				strcat(result, dirbuf);
			}
		}
		closedir(dirp);
	}
	return res;
}

/* ---------------------------------------------------------------------- */

int get_string_from_file(char *prefix, char *path, char *filename, char *buf, short buf_len)
{
	FILE *f;
	int len=0;
	char fullname[256]={0};
	
	sprintf(fullname, "%s/%s/%s", prefix, path, filename);
	if ((f = fopen(fullname, "r")))
	{
		fgets(buf, buf_len, f);
		len = strlen(buf)-1;
		do (buf[len]='\0'); while (buf[--len]==' ');
		fclose(f);
	}
	return len;
}


unsigned int get_hexvalue_from_file(char *prefix, char *path, char *filename)
{
	FILE *fd;
	unsigned int value = 0;
	char fullname[256]={0};
	
	sprintf(fullname, "%s/%s/%s", prefix, path, filename);
	if ((fd = fopen(fullname, "r")))
	{
		fscanf(fd, "%x", &value);
		fclose(fd);
	}
	return value;
}

/*
 * copied getxinfo from hwsetup, by  Klaus Knopper <knopper@knopper.net> 
 */

int exists(char *filename)
{
	struct stat s;
	return !stat(filename,&s);
}

/*
 * searching in /usr/share/hwdata/Cards for x module name and parameters
 * if outputxinfo is on, /tmp/xinfo will be created containing gfx card section for X config 
 */
struct xinfo *
getxinfo ( char* devdesc, char* devdriver, char outputxinfo )
{
	const char *xfree4 = "XFree86", *xvesa4 = "vesa";
	const char *xpath = XPATH;
	static struct xinfo xi;
	int rescanned = 0;
	char *xinfo_opt = malloc(0);
	int xinfo_opt_len = 0;
	memset ( &xi, 0, sizeof ( struct xinfo ) );
	if ( devdesc )
		strncpy ( xi.xdesc, devdesc, sizeof ( xi.xdesc ) );
	if ( devdriver )
	{
		const char * driver[] =
		{ 
			"3DLabs", "Mach64", "Mach32", "Mach8", "AGX",
			"P9000", "S3 ViRGE", "S3V", "S3", "W32",
			"8514", "I128", "SVGA", xfree4, NULL
		};
		const char *server[] =
		{
			driver[ 0 ], driver[ 1 ], driver[ 2 ], driver[ 3 ],
			driver[ 4 ],
			driver[ 5 ], "S3V", driver[ 7 ], driver[ 8 ], driver[ 9 ],
			driver[ 10 ], driver[ 11 ], driver[ 12 ], xfree4, NULL
		};
		if ( !strncasecmp ( devdriver, "Card:", 5 ) ) 	/* RedHat Cards-DB */
		{	/* Kudzu "Cards" format */
			FILE * cardsdb;
			char xfree3server[ 128 ];
			memset ( xfree3server, 0, sizeof ( xfree3server ) );
			if ( ( cardsdb = fopen ( CARDSDB, "r" ) ) != NULL )
			{	/* Try to find Server and Module in /usr/share/kudzu/Cards */
				char buffer[ 1024 ];
				char searchfor[ 128 ];
				int found = 0;
				memset ( searchfor, 0, sizeof ( searchfor ) );
				sscanf ( &devdriver[ 5 ], "%127[^\r\n]", searchfor );
				while ( !found && !feof ( cardsdb ) && fgets ( buffer, 1024, cardsdb ) )
				{
					char sfound[ 128 ];
					memset ( sfound, 0, sizeof ( sfound ) );
					if ( strncasecmp ( buffer, "NAME ", 5 )
						|| ( sscanf( &buffer[ 5 ], "%127[^\r\n]", sfound ) != 1 )
						|| strcasecmp ( sfound, searchfor ) )
						continue;
					while ( !feof ( cardsdb ) && fgets ( buffer, 1024, cardsdb ) )
					{
						if ( buffer[ 0 ] < 32 )
							break;	/* End-of-line */
						if ( !strncasecmp( buffer, "SERVER ", 7 ) )
						{
							char x[ 20 ] = "";
							if ( sscanf( &buffer[ 7 ], "%19s", x ) == 1 )
							{
								char xserver[ 32 ];
								char fullpath[ 128 ];
								char *xf[ 2 ] =  { "", "XF86_" };
								int i;
								for ( i = 0; i < 2; i++ )
								{
									sprintf ( xserver, "%s%.24s", xf[ i ], x );
									sprintf ( fullpath, "%.90s%.32s", xpath, xserver );
									if ( exists ( fullpath ) )
									{
										strncpy ( xfree3server, xserver, sizeof ( xfree3server ) );
										break;	/* for */
									}
								}
							}
						}
						else if ( !strncasecmp( buffer, "DRIVER ", 7 ) )
						{
							char xmodule[ 32 ];
							char fullpath[ 128 ];
							char fullpath_xorg[ 128 ], fullpath_xorg7[ 128 ];
							sscanf ( &buffer[ 7 ], "%31s", xmodule );
							sprintf ( fullpath, XMODPATH "%.31s_drv.o", xmodule );
							sprintf ( fullpath_xorg, XMODPATH "%.31s_drv.so", xmodule );
							sprintf ( fullpath_xorg7, XMODPATH_XORG7 "%.31s_drv.so", xmodule );
							
							if ( exists ( fullpath ) || exists (fullpath_xorg) || exists (fullpath_xorg7) )
							{
								strncpy ( xi.xmodule, xmodule, sizeof( xi.xmodule ) );
							} 
						}
						else if ( !strncasecmp( buffer, "SEE ", 4 ) && rescanned < 10 )
						{	/* rescan Cards-DB for other server */
							fseek ( cardsdb, 0L, SEEK_SET );
							++rescanned;
							memset ( searchfor, 0, sizeof( searchfor ) );
							sscanf ( &buffer[ 4 ], "%127[^\r\n]", searchfor );
							break;	/* Continue with outer while() */
						}
						else if ( !strncasecmp( buffer, "LINE ", 5 ) && outputxinfo )
						{
							char xinfo_opt_line[128];
							sscanf ( &buffer[ 5 ], "%127[^\r\n]", xinfo_opt_line );
							xinfo_opt_len += strlen(xinfo_opt_line) + 1;
							xinfo_opt = (char*)realloc(xinfo_opt, xinfo_opt_len );
							strcat(xinfo_opt, xinfo_opt_line);
							strcat(xinfo_opt,"\n");
						}
						
					}
				}
				fclose ( cardsdb );
			}
			if ( *xi.xmodule || *xi.xserver || *xfree3server ) 	/* (Partial) Success */
			{
				if ( !*xi.xserver )
				{
					if ( *xfree3server && !*xi.xmodule )
						strncpy ( xi.xserver, xfree3server, sizeof ( xi.xserver ) );
					else
						strncpy ( xi.xserver, xfree4, sizeof ( xi.xserver ) );
				}
				if ( !*xi.xmodule )
					strcpy ( xi.xmodule, xvesa4 );
				goto bailout;
			}
		}
		/* Card not found in Cards database -> Try to guess from description */
		{
			int i;
			for ( i = 0; driver[ i ] != NULL; i++ )
			{
				if ( strstr ( devdriver, driver[ i ] ) )
				{
					char * xpos;
					if ( ( xpos = strstr ( devdriver, xfree4 ) ) != NULL ) 	/* Check for XFree 4 */
					{
						char xm[ 32 ] = "";
						strcpy ( xi.xserver, xfree4 );
						if ( sscanf ( xpos, "XFree86(%30[^)])", xm ) == 1 )
							strcpy ( xi.xmodule, xm );
						else
							strcpy ( xi.xmodule, xvesa4 );
					} else
					{
						char xserver[ 32 ];
						char fullpath[ 128 ];
						char *xf[ 2 ] = { "", "XF86_" };
						int j;
						for ( j = 0; j < 2; j++ )
						{
							sprintf ( xserver, "%s%.24s", xf[ j ], server[ i ] );
							sprintf ( fullpath, "%.90s%.32s", xpath, xserver );
							if ( exists ( fullpath ) )
							{
								strncpy ( xi.xserver, xserver, sizeof( xi.xserver ) );
								break;	/* for */
							}
						}
					}
				}
			}
		}
	}
	/* TODO: include xopts as Option in X config instead of startx parameters */
	/* Special options required? */
	if ( devdesc )
	{
		strncpy ( xi.xdesc, devdesc, sizeof ( xi.xdesc ) - 1 );
		/* Handle special cards that require special options */
		if ( strstr ( devdesc, "Trident" ) || strstr ( devdesc, "TGUI" )
		        || strstr ( devdesc, "Cirrus" ) || strstr ( devdesc, "clgd" ) )
		{
			if ( !strcmp ( xi.xserver, xfree4 ) )
				strncpy ( xi.xopts, "-depth 16", sizeof ( xi.xopts ) - 1 );
			else
				strncpy ( xi.xopts, "-bpp 16", sizeof ( xi.xopts ) - 1 );
		}
		else if ( strstr ( devdesc, "Savage 4" ) ) 	/* S3 Inc.|Savage 4 */
		{
			if ( !strcmp ( xi.xserver, xfree4 ) )
				strncpy ( xi.xopts, "-depth 32", sizeof ( xi.xopts ) - 1 );
			else
				strncpy ( xi.xopts, "-bpp 32", sizeof ( xi.xopts ) - 1 );
		}
	}
	/* Fallback values */
	if ( !*xi.xserver )
		strcpy ( xi.xserver, xfree4 );
	if ( !*xi.xmodule )
		strcpy ( xi.xmodule, xvesa4 );

bailout:
	if (outputxinfo)
	{
		/* dump gfx X config section to /tmp/xinfo */
		FILE *f;
		if ((f=fopen("/tmp/xinfo", "w")) != NULL)
		{
			*xinfo_opt='\0';	/* marking end of string */
			fprintf(f,"Section \"Device\"\n");
			fprintf(f,"\tIdentifier  \"Card0\"\n");
			fprintf(f,"\tDriver      \"%s\"\n",xi.xmodule);
			fprintf(f,"\tVendorName  \"All\"\n");
			fprintf(f,"\tBoardName   \"All\"\n");
			fprintf(f,"%s", xinfo_opt);
			fprintf(f,"EndSection\n");
			fclose(f);
			free(xinfo_opt);
		}
	}
	
	return &xi;
}

/*
 * List of known device classes, subclasses and programming interfaces
 * (copied from pci.ids)
 */

static struct {
	unsigned int classid;
	char *name;
} pci_classes[] =
{
	{ 0x000000, "Non-VGA unclassified device"},
	{ 0x000100, "VGA compatible unclassified device"},
	{ 0x010000, "SCSI storage controller"},
	{ 0x010100, "IDE interface"},
	{ 0x010200, "Floppy disk controller"},
	{ 0x010300, "IPI bus controller"},
	{ 0x010400, "RAID bus controller"},
	{ 0x018000, "Unknown mass storage controller"},
	{ 0x020000, "Ethernet controller"},
	{ 0x020100, "Token ring network controller"},
	{ 0x020200, "FDDI network controller"},
	{ 0x020300, "ATM network controller"},
	{ 0x020400, "ISDN controller"},
	{ 0x028000, "Network controller"},
	{ 0x030000, "VGA compatible controller"},
	{ 0x030001, "VGA compatible controller|8514"},
	{ 0x030100, "XGA compatible controller"},
	{ 0x030200, "3D controller"},
	{ 0x038000, "Display controller"},
	{ 0x040000, "Multimedia video controller"},
	{ 0x040100, "Multimedia audio controller"},
	{ 0x040200, "Computer telephony device"},
	{ 0x048000, "Multimedia controller"},
	{ 0x050000, "RAM memory"},
	{ 0x050100, "FLASH memory"},
	{ 0x058000, "Memory controller"},
	{ 0x060000, "Host bridge"},
	{ 0x060100, "ISA bridge"},
	{ 0x060200, "EISA bridge"},
	{ 0x060300, "MicroChannel bridge"},
	{ 0x060400, "PCI bridge"},
	{ 0x060401, "PCI bridge|Subtractive decode"},
	{ 0x060500, "PCMCIA bridge"},
	{ 0x060600, "NuBus bridge"},
	{ 0x060700, "CardBus bridge"},
	{ 0x060800, "RACEway bridge|Transparent mode"},
	{ 0x060801, "RACEway bridge|Endpoint mode"},
	{ 0x060900, "Semi-transparent PCI-to-PCI bridge"},
	{ 0x060940, "Semi-transparent PCI-to-PCI bridge|Primary bus towards host CPU"},
	{ 0x060980, "Semi-transparent PCI-to-PCI bridge|Secondary bus towards host CPU"},
	{ 0x060a00, "InfiniBand to PCI host bridge"},
	{ 0x068000, "Bridge"},
	{ 0x070000, "Serial controller|8250"},
	{ 0x070001, "Serial controller|16450"},
	{ 0x070002, "Serial controller|16550"},
	{ 0x070003, "Serial controller|16650"},
	{ 0x070004, "Serial controller|16750"},
	{ 0x070005, "Serial controller|16850"},
	{ 0x070006, "Serial controller|16950"},
	{ 0x070100, "Parallel controller|SPP"},
	{ 0x070101, "Parallel controller|BiDir"},
	{ 0x070102, "Parallel controller|ECP"},
	{ 0x070103, "Parallel controller|IEEE1284"},
	{ 0x0701fe, "Parallel controller|IEEE1284 Target"},
	{ 0x070200, "Multiport serial controller"},
	{ 0x070300, "Modem|Generic"},
	{ 0x070301, "Modem|Hayes/16450"},
	{ 0x070302, "Modem|Hayes/16550"},
	{ 0x070303, "Modem|Hayes/16650"},
	{ 0x070304, "Modem|Hayes/16750"},
	{ 0x078000, "Communication controller"},
	{ 0x080000, "PIC|8259"},
	{ 0x080001, "ISA PIC"},
	{ 0x080002, "EISA PIC"},
	{ 0x080010, "IO-APIC"},
	{ 0x080020, "IO(X)-APIC"},
	{ 0x080100, "DMA controller|8237"},
	{ 0x080101, "DMA controller|ISA DMA"},
	{ 0x080102, "DMA controller|EISA DMA"},
	{ 0x080200, "Timer|8254"},
	{ 0x080201, "ISA Timer"},
	{ 0x080202, "EISA Timers"},
	{ 0x080300, "RTC|Generic"},
	{ 0x080301, "RTC|ISA RTC"},
	{ 0x080400, "PCI Hot-plug controller"},
	{ 0x088000, "System peripheral"},
	{ 0x090000, "Keyboard controller"},
	{ 0x090100, "Digitizer Pen"},
	{ 0x090200, "Mouse controller"},
	{ 0x090300, "Scanner controller"},
	{ 0x090400, "Gameport controller"},
	{ 0x090410, "Gameport controller|Extended"},
	{ 0x098000, "Input device controller"},
	{ 0x0a0000, "Generic Docking Station"},
	{ 0x0a8000, "Docking Station"},
	{ 0x0b0000, "386"},
	{ 0x0b0100, "486"},
	{ 0x0b0200, "Pentium"},
	{ 0x0b1000, "Alpha"},
	{ 0x0b2000, "Power PC"},
	{ 0x0b3000, "MIPS"},
	{ 0x0b4000, "Co-processor"},
	{ 0x0c0000, "FireWire (IEEE 1394)"},
	{ 0x0c0010, "FireWire (IEEE 1394)|OHCI"},
	{ 0x0c0100, "ACCESS Bus"},
	{ 0x0c0200, "SSA"},
	{ 0x0c0300, "USB Controller|UHCI"},
	{ 0x0c0310, "USB Controller|OHCI"},
	{ 0x0c0320, "USB Controller|EHCI"},
	{ 0x0c0380, "USB Controller|Unspecified"},
	{ 0x0c03fe, "USB Controller|USB Device"},
	{ 0x0c0400, "Fibre Channel"},
	{ 0x0c0500, "SMBus"},
	{ 0x0c0600, "InfiniBand"},
	{ 0x0d0000, "IRDA controller"},
	{ 0x0d0100, "Consumer IR controller"},
	{ 0x0d1000, "RF controller"},
	{ 0x0d8000, "Wireless controller"},
	{ 0x0e0000, "I2O"},
	{ 0x0f0000, "Satellite TV controller"},
	{ 0x0f0100, "Satellite audio communication controller"},
	{ 0x0f0300, "Satellite voice communication controller"},
	{ 0x0f0400, "Satellite data communication controller"},
	{ 0x100000, "Network and computing encryption device"},
	{ 0x101000, "Entertainment encryption device"},
	{ 0x108000, "Encryption controller"},
	{ 0x110000, "DPIO module"},
	{ 0x110100, "Performance counters"},
	{ 0x111000, "Communication synchronizer"},
	{ 0x118000, "Signal processing controller"},
};

static unsigned int pci_classes_count = sizeof(pci_classes) / sizeof(pci_classes[0]);

char*
get_pci_classname(unsigned int classid) 
{
	int i;
	for (i = 0; i < pci_classes_count; i++)
		if (pci_classes[i].classid == classid) return pci_classes[i].name;
	/* try without functionid */
	classid = classid & 0xffff00;
	for (i = 0; i < pci_classes_count; i++)
		if (pci_classes[i].classid == classid) return pci_classes[i].name;
	return pci_classes[0].name;
}


/*
 * List of known device classes, subclasses and protocols
 * (copied from usb.ids)
 */

static struct {
	unsigned int classid;
	char *name;
} usb_classes[] =
{
	{ 0x000000, "USB Unclassified device"}, //"(Defined at Interface level)"},
	{ 0x010000, "USB Audio"},
	{ 0x010100, "USB Audio|Control Device"},
	{ 0x010200, "USB Audio|Streaming"},
	{ 0x010300, "USB Audio|MIDI Streaming"},
	{ 0x020000, "USB Communications"},
	{ 0x020100, "USB Direct Line"},
	{ 0x020200, "USB Modem|Abstract"},
	{ 0x020201, "USB Modem|AT-commands (v.25ter)"},
	{ 0x020202, "USB Modem|AT-commands (PCCA101)"},
	{ 0x020203, "USB Modem|AT-commands (PCCA101 + wakeup)"},
	{ 0x020204, "USB Modem|AT-commands (GSM)"},
	{ 0x020205, "USB Modem|AT-commands (3G)"},
	{ 0x020206, "USB Modem|AT-commands (CDMA)"},
	{ 0x0202fe, "USB Modem|Defined by command set descriptor"},
	{ 0x0202ff, "USB Modem|Vendor Specific (MSFT RNDIS?)"},
	{ 0x020300, "USB Telephone"},
	{ 0x020400, "USB Multi-Channel"},
	{ 0x020500, "USB CAPI Control"},
	{ 0x020600, "USB Ethernet Networking"},
	{ 0x020700, "USB ATM Networking"},
	{ 0x020800, "USB Wireless Handset Control"},
	{ 0x020900, "USB Device Management"},
	{ 0x020a00, "USB Mobile Direct Line"},
	{ 0x020b00, "USB OBEX"},
	{ 0x030000, "USB Human Interface Devices"},
	{ 0x030002, "USB Keyboard"},
	{ 0x030003, "USB Mouse"},
	{ 0x030100, "USB Boot Interface Subclass"},
	{ 0x030101, "USB Keyboard"},
	{ 0x030102, "USB Mouse"},
	{ 0x060000, "USB Imaging"},
	{ 0x060100, "USB Still Image Capture"},
	{ 0x060101, "USB Still Image Capture|Picture Transfer Protocol (PIMA 15470)"},
	{ 0x070000, "USB Printer"},
	{ 0x070100, "USB Printer"},
	{ 0x070101, "USB Printer|Unidirectional"},
	{ 0x070102, "USB Printer|Bidirectional"},
	{ 0x070103, "USB Printer|IEEE 1284.4 compatible bidirectional"},
	{ 0x0701ff, "USB Printer|Vendor Specific"},
	{ 0x080000, "USB Mass Storage"},
	{ 0x080100, "USB Mass Storage|RBC (typically Flash)|Control/Bulk/Interrupt"},
 	{ 0x080101, "USB Mass Storage|RBC (typically Flash)|Control/Bulk"},
 	{ 0x080150, "USB Mass Storage|RBC (typically Flash)|Bulk (Zip)"},
	{ 0x080200, "USB Mass Storage|SFF-8020i, MMC-2 (ATAPI)"},
	{ 0x080300, "USB Mass Storage|QIC-157"},
	{ 0x080400, "USB Mass Storage|Floppy (UFI)|Control/Bulk/Interrupt"},
	{ 0x080401, "USB Mass Storage|Floppy (UFI)|Control/Bulk"},
	{ 0x080450, "USB Mass Storage|Floppy (UFI)|Bulk (Zip)"},
	{ 0x080500, "USB Mass Storage|SFF-8070i"},
	{ 0x080600, "USB Mass Storage|SCSI|Control/Bulk/Interrupt"},
	{ 0x080601, "USB Mass Storage|SCSI|Control/Bulk"},
	{ 0x080650, "USB Mass Storage|SCSI|Bulk (Zip)"},
	{ 0x090000, "USB Hub"},
	{ 0x090001, "USB Hub|Single TT"},
	{ 0x090002, "USB Hub|TT per port"},
	{ 0x0a0000, "USB Data"},
	{ 0x0a0030, "USB Data|I.430 ISDN BRI"},
	{ 0x0a0031, "USB Data|HDLC"},
	{ 0x0a0032, "USB Data|Transparent"},
	{ 0x0a0050, "USB Data|Q.921M"},
	{ 0x0a0051, "USB Data|Q.921"},
	{ 0x0a0052, "USB Data|Q.921TM"},
	{ 0x0a0090, "USB Data|V.42bis"},
	{ 0x0a0091, "USB Data|Q.932 EuroISDN"},
	{ 0x0a0092, "USB Data|V.120 V.24 rate ISDN"},
	{ 0x0a0093, "USB Data|CAPI 2.0"},
	{ 0x0a00fd, "USB Data|Host Based Driver"},
	{ 0x0a00fe, "USB Data|CDC PUF"},
	{ 0x0a00ff, "USB Data|Vendor specific"},
	{ 0x0b0000, "USB Chip/SmartCard"},
	{ 0x0d0000, "USB Content Security"},
	{ 0x0e0000, "USB Video"},
	{ 0x0e0100, "USB Video Control"},
	{ 0x0e0200, "USB Video Streaming"},
	{ 0x0e0300, "USB Video Interface Collection"},
	{ 0xdc0000, "USB Diagnostic"},
	{ 0xdc0100, "USB Reprogrammable Diagnostics"},
	{ 0xdc0101, "USB Reprogrammable Diagnostics|USB2 Compliance"},
	{ 0xe00000, "USB Wireless"},
	{ 0xe00100, "USB Radio Frequency"},
	{ 0xe00101, "USB Wireless|Bluetooth"},
	{ 0xef0000, "USB Miscellaneous Device"},
	{ 0xef0200, "USB Common Class"},
	{ 0xef0201, "USB Common Class|Interface Association"},
	{ 0xfe0000, "USB Application Specific Interface"},
	{ 0xfe0100, "USB Device Firmware Update"},
	{ 0xfe0200, "USB IRDA Bridge"},
	{ 0xfe0300, "USB Test and Measurement"},
	{ 0xff0000, "USB Vendor Specific Class"},
	{ 0xffff00, "USB Vendor Specific Subclass"},
	{ 0xffffff, "USB Vendor Specific Protocol"},
};

static unsigned int usb_classes_count = sizeof(usb_classes) / sizeof(usb_classes[0]);

char*
get_usb_classname(unsigned int classid) 
{
	int i;
	for (i = 0; i < usb_classes_count; i++)
		if (usb_classes[i].classid == classid) return usb_classes[i].name;
	/* try without functionid */
	classid = classid & 0xffff00;
	for (i = 0; i < usb_classes_count; i++)
		if (usb_classes[i].classid == classid) return usb_classes[i].name;
	return usb_classes[0].name;
}

