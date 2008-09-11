/*****************************************************************************
 *
 * lshwd - lists hardware devices and their approp modules. 
 * see the README for additional information.
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
//#include "usb.h"
#include "psaux.h"

#define LSHWD_VERSION "2.0-rc4"

/*****************************************************************************
 * lshwd device information
 *****************************************************************************/

enum bus_types {BUS_PCI, BUS_USB, BUS_PCMCIA, BUS_FIREWIRE, BUS_MOUSE};

typedef struct lshwd_device
{
	unsigned char bustype;		// pci/usb/pcmcia/firewire/mouse
	
	unsigned short bus;
	unsigned short device;
	unsigned short func; 		// pci function/usb protocol
	char *businfo;
	
	unsigned int classid;
	char *classname;
	
	unsigned short vendorid;
	unsigned short productid;
	unsigned short subvendorid;	// pci only
	unsigned short subproductid;	// pci only
	
	char *desc;
	char *module;
	char *devicenode;

	struct lshwd_device *prev;
	struct lshwd_device *next;

} *plshwd_device;


/*****************************************************************************
 * global defines and variables
 *****************************************************************************/

#define LSHWD_VERSION_TEXT      "lshwd " LSHWD_VERSION ", list hardware devices with approp modules\n" \
                                "Detects all types of PCI/PCMCIA/USB/FireWire cards/chips (ethernet,vga,sound,etc.).\n" \
                                "coded by <z4ziggy at user-contributions.org> for Arch linux, 2004-2005\n\n"

plshwd_device first_device = NULL;
char showids, autoload, disdefdesc,machinemode,plainmode,showlist,removeduplicates,outputxinfo, sysfsmode;
char usbmousefound = 0;

// ANSI COLORS (from Arch scheme)
#define COLOR_NORMAL	"\033[0m" 
#define COLOR_RED	"\033[1;31m" 
#define COLOR_GREEN	"\033[1;32m" 
#define COLOR_YELLOW	"\033[1;33m" 
#define COLOR_BLUE	"\033[1;34m" 
#define COLOR_WHITE	"\033[1;37m"

char *title_color = COLOR_BLUE;
char *modules_color = COLOR_GREEN;
char *normal_color = COLOR_NORMAL;


/*****************************************************************************
 * devices functions
 *****************************************************************************/

void 
free_devices(void) 
{
	plshwd_device tmp;
	
	while (first_device != NULL) {
		free(first_device->desc);
		free(first_device->module);
		free(first_device->classname);
		free(first_device->businfo);
		free(first_device->devicenode);
		tmp = first_device->next;
		free(first_device);
		first_device = tmp;
	}
}

/* ---------------------------------------------------------------------- */

/*
 * making sure we dont have duplicates (for pci_scan) 
 */

char
is_device_listed(plshwd_device newdev)
{
	plshwd_device dev = first_device;
	
	while (dev != NULL) {
		if ((dev->bustype == newdev->bustype) &&
			(dev->classid == newdev->classid) &&
			(dev->bus == newdev->bus) &&
			(dev->device == newdev->device) &&
			(dev->func == newdev->func)) 
			return 1;
		dev = dev->next;
	}
	return 0;
}

/* ---------------------------------------------------------------------- */

void
add_device(plshwd_device newdev)
{
	plshwd_device dev, lastdev;
	dev = lastdev = first_device;

#ifdef REVERSED_LSHWD_DEVICE_LIST
	dev = malloc(sizeof(*dev));
#else
	if (first_device == NULL)
	{
		first_device = malloc(sizeof(*first_device));
		dev = first_device;
	} else
	{
		dev = first_device;
		while (dev->next) dev = dev->next;
		lastdev = dev;
		dev->next = malloc(sizeof(*dev));
		dev = dev->next;
		
	}
#endif
	if (dev == NULL) return;
	dev->next = NULL;
	dev->prev = lastdev;
	dev->bustype = newdev->bustype;
	dev->classid = newdev->classid;
	dev->classname = strdup(newdev->classname);
	dev->bus = newdev->bus;
	dev->device = newdev->device;
	dev->func = newdev->func;
	dev->businfo = strdup(newdev->businfo);
	dev->vendorid = newdev->vendorid;
	dev->productid = newdev->productid;
	dev->subvendorid = newdev->subvendorid;
	dev->subproductid = newdev->subproductid;
	dev->desc = strdup(newdev->desc);
	dev->module = strdup(newdev->module);
	dev->devicenode = strdup(newdev->devicenode);
#ifdef REVERSED_LSHWD_DEVICE_LIST
	dev->next = first_device;
	if (first_device) first_device->prev = dev;
	first_device = dev;
#endif
}

/* ---------------------------------------------------------------------- */

/* qsort procedure for sorting pci devices per class type */
int
compare_lshwd_class(const void *A, const void *B)
{
	struct lshwd_device *a = (*(struct lshwd_device **)A);
	struct lshwd_device *b = (*(struct lshwd_device **)B);
	if (a->classid > b->classid) return 1;
	if (a->classid < b->classid) return -1;
	return 0;
}

/* ---------------------------------------------------------------------- */

/*
 * group devices according to classid
 */

void
sort_devices(void)
{
	struct lshwd_device **index, **h, **last_dev;
	int cnt;
	struct lshwd_device *d;
	
	/* counting number of total devices */
	for (cnt=0, d=first_device; d; d=d->next) cnt++;
	/* allocating memory for all pci devices */
	h = index = alloca(sizeof(struct lshwd_device *) * cnt);
	for (d=first_device; d; d=d->next) *h++ = d;
	qsort(index, cnt, sizeof(struct lshwd_device *), compare_lshwd_class);
	/* arranging result of qsort starting from first_dev */
	last_dev = &first_device;
	h = index;
	/* loop on all devices and copy contents from array to pci device list */
	while (cnt--)
	{
		*last_dev = *h;
		/* checking for duplicates only if removeduplicates is on */
		/* we compare the vendor_id and device_id of next item */
		if (!removeduplicates || ((!cnt) || //((!(*(h+1))) ||
			(!( ((*(h+1))->vendorid == (*last_dev)->vendorid) &&
			((*(h+1))->productid == (*last_dev)->productid) ))))
			last_dev = &(*last_dev)->next;
		h++;
	}
	*last_dev = NULL;
}

/* ---------------------------------------------------------------------- */

/*
 * display devices entries and load modules if needed
 */

void
dump_devices(void)
{
	plshwd_device dev = first_device;
	char lastclassname[128]={0};
	char idstring[40]={0};
	int count=0;
	
	while (dev != NULL) 
	{
		if (machinemode)
		{
			if (*dev->devicenode)
				printf("\"%s\" \"%s\" \"%s\" \"%s\"\n", 
				       dev->classname, dev->desc, dev->module, dev->devicenode);
			else
				printf("\"%s\" \"%s\" \"%s\"\n", 
				       dev->classname, dev->desc, dev->module);
		}
		else if (showlist)
		{
			sprintf(idstring, "%04x:%04x:%04x:%04x ", 
				dev->vendorid, dev->productid, dev->subvendorid, dev->subproductid);
			if (*dev->devicenode)
				printf("%s %s%s: %s [%s] (%s)\n",
				       dev->businfo, showids ? idstring : "", 
				       dev->classname, dev->desc, dev->devicenode, dev->module);
			else
				printf("%s %s%s: %s (%s)\n",
				       dev->businfo, showids ? idstring : "", dev->classname, dev->desc, dev->module);
		}
		else
		{
			if (strcmp(lastclassname, dev->classname)) 
				printf("%s%s%s\n",title_color, dev->classname, normal_color);
			strcpy(lastclassname,dev->classname);
			if (*dev->devicenode)
				printf("  %s%-16s:%s %s %s[%s]%s\n", 
				       modules_color, dev->module, normal_color, dev->desc,
				       modules_color,dev->devicenode,normal_color);
			else
				printf("  %s%-16s:%s %s\n", 
				       modules_color, dev->module, normal_color, dev->desc);
		}
		if (autoload) loadModule(dev->module);
		
		dev = dev->next;
		count++;
	}
	
	DEBUG("Found total of %d devices\n", count);
	
	if (!showlist) printf("\n");
}

/*****************************************************************************
 * scanners procedures
 *****************************************************************************/

#define USB_BASE_CLASS_NETWORK                    0x020600
#define USB_BASE_CLASS_HID_MOUSE                  0x030003
#define USB_BASE_CLASS_HID_BOOT_MOUSE             0x030102
#define USB_SUBSYSTEM_VENDOR_ID                   0x08
#define USB_SUBSYSTEM_PRODUCT_ID                  0x0a
#define USB_SUBSYSTEM_CLASS_ID                    0x20
#define USB_CLASS_ID                              0x04
#define USB_SUBSYSTEM_DESC_OFFSET                 0x0f

/*
 * list usb devices according to /proc/bus/usb
 */

/* ---------------------------------------------------------------------- */

#define PCI_BASE_CLASS_NETWORK                    0x02
#define PCI_BASE_CLASS_DISPLAY                    0x03
#define PCI_SUBSYSTEM_VENDOR_ID                   0x00
#define PCI_SUBSYSTEM_PRODUCT_ID                  0x02
#define PCI_SUBSYSTEM_SUBVENDOR_ID                0x2c
#define PCI_SUBSYSTEM_SUBPRODUCT_ID               0x2e
#define PCI_SUBSYSTEM_CLASS_ID                    0x0a
#define PCI_SUBSYSTEM_CLASS_PROG_IF_ID            0x09

/*
 * list pci and pcmcia devices according to /proc/bus/pci
 */

void
scan_procfs_pci(void)
{
	unsigned char buf[256];
	char desc[128],module[128],businfo[8]={0}, filename[256]={0};
	int bus_cnt, dev_cnt, fd;
	struct dirent **buslist, **devlist;
	char found_yenta_socket = 0;
	static const char *procbuspci = "/proc/bus/pci";
	struct lshwd_device dev;
	
	dev.module = module;
	dev.desc = desc;
	dev.devicenode = "";
	dev.businfo = businfo;
	dev.bustype = BUS_PCI;
	dev.func = dev.classid = dev.subvendorid = dev.subproductid = 0;

	init_lookup_block("pcitable");

begin_procfs_pci_scan:

	bus_cnt = scandir(procbuspci, &buslist, 0, NULL);
	if (bus_cnt == -1) bus_cnt = 0;
	while (bus_cnt--)
	{
		if ((buslist[bus_cnt]->d_name[0] < '0') || (buslist[bus_cnt]->d_name[0] > '9')) { free(buslist[bus_cnt]); continue; }

		snprintf(filename, sizeof(filename), "%s/%s/", procbuspci, buslist[bus_cnt]->d_name);
		
		dev_cnt = scandir(filename, &devlist, 0, NULL);
		if (dev_cnt == -1) dev_cnt = 0;
		while (dev_cnt--)
		{
			if (devlist[dev_cnt]->d_name[0] == '.') { free(devlist[dev_cnt]); continue; }

			snprintf(filename, sizeof(filename), "%s/%s/%s", 
				 procbuspci, buslist[bus_cnt]->d_name, devlist[dev_cnt]->d_name);
			if ((fd = open(filename, O_RDONLY)) == -1)
			{
				DEBUG( "cannot open %s, %s (%d)\n", filename, strerror(errno), errno);
				free(devlist[dev_cnt]);
				continue;
			}
			if (read(fd, buf, 64) != 64)
			{
				DEBUG( "cannot read device descriptor %s (%d)\n", strerror(errno), errno );
				free(devlist[dev_cnt]);
				continue;
			}
			dev.devicenode = "";

			dev.bus = strtoul(buslist[bus_cnt]->d_name, NULL, 0);
			sscanf(devlist[dev_cnt]->d_name, "%2x.%hd", &dev.device, &dev.func);
			sprintf(dev.businfo, "%s:%s", buslist[bus_cnt]->d_name, devlist[dev_cnt]->d_name);

			dev.vendorid = buf[PCI_SUBSYSTEM_VENDOR_ID] | (buf[PCI_SUBSYSTEM_VENDOR_ID+1] << 8);
			dev.productid = buf[PCI_SUBSYSTEM_PRODUCT_ID] | (buf[PCI_SUBSYSTEM_PRODUCT_ID+1] << 8);
			dev.subvendorid = buf[PCI_SUBSYSTEM_SUBVENDOR_ID] | (buf[PCI_SUBSYSTEM_SUBVENDOR_ID+1] << 8);
			dev.subproductid = buf[PCI_SUBSYSTEM_SUBPRODUCT_ID] | (buf[PCI_SUBSYSTEM_SUBPRODUCT_ID+1] << 8);

			dev.classid = buf[PCI_SUBSYSTEM_CLASS_PROG_IF_ID] | (buf[PCI_SUBSYSTEM_CLASS_ID] << 8) | (buf[PCI_SUBSYSTEM_CLASS_ID+1] << 16) ;
			dev.classname = get_pci_classname(dev.classid);

			lookup_module(dev.vendorid, dev.productid, dev.subvendorid, dev.subproductid,
				      module, sizeof(module),
				      desc, sizeof(desc));
			if (strcmp(module, "yenta_socket") == 0) found_yenta_socket++;
			if ((dev.classid >> 16) == PCI_BASE_CLASS_NETWORK)
				dev.devicenode = find_network_devices(dev.module);
			if ((dev.classid >> 16) == PCI_BASE_CLASS_DISPLAY)
			{
				/* getxinfo will retrieve x info and create /tmp/xinfo if needed */
				struct xinfo *x = getxinfo(desc, module, outputxinfo);
				strcpy(module, x->xmodule);
				if (dev.vendorid == 0x10de) //nVidia
					snprintf(module, sizeof(module) - 1, "nv");
				else if (dev.vendorid == 0x1002) //ATI
					snprintf(module, sizeof(module) - 1, "radeon");
				DEBUG("%s\n%s\n%s\n%s\n", x->xserver, x->xmodule, x->xdesc, x->xopts);
			}

			if (!is_device_listed(&dev)) add_device(&dev);
			*desc = *module = *businfo = 0;
			
			close(fd);
			free(devlist[dev_cnt]);
		}
		free(devlist);
		free(buslist[bus_cnt]);
	}
	free(buslist);

	/* check for yenta_socket module (ONLY ONCE) */
	if ( autoload && (found_yenta_socket == 1) && !isLoaded("yenta_socket") )	
	{
		loadModule("yenta_socket");
		sleep(1);
		goto begin_procfs_pci_scan;
	}

	cleanup_lookup_block();     /* free table file   */
	return;
}

/* ---------------------------------------------------------------------- */

/*
 * list FireWire devices according to /proc/bus/ieee1394/devices
 */

void 
scan_procfs_firewire(void)
{
	//int loaded_driver = 0;
	unsigned long specid, version;
	char businfo[10], desc[128];
	
	char *next, *buf = NULL, *tmp;
	int bytes =0;
	struct lshwd_device dev;
	
	dev.module = "sbp2"; // yes, lame, but thats all firewire devices use... no probe here.
	dev.desc = desc;
	dev.devicenode = "";
	dev.businfo = businfo;
	dev.bustype = BUS_FIREWIRE;
	dev.classname = "FireWire device";
	dev.func = dev.classid = dev.subvendorid = dev.subproductid = 0;
		
	/* Do NOT attempt to load firewire module (may crash), rather check for existing one. -KK */
#if 0
	if (!loadModule("ohci1394"))
		loaded_driver = 1;
#endif
	if (!(bytes = read_file("/proc/bus/ieee1394/devices", &buf))) goto out;
	
	while (buf) 
	{
		specid = version = dev.bus = dev.device = dev.func = dev.vendorid = dev.productid = 0;
		*businfo = *desc = 0;
		
		next = strstr(buf+1,"Node[");
		if (next) 
		{
			*(next-1) = '\0';
			char *end;
			end = strstr(next,"]");
			if (end) *end = '\0';
			strcpy(businfo, next);
			if (sscanf(businfo,"%hd-%hd:%hd", &dev.bus, &dev.device, &dev.func) != 3)
				sscanf(businfo, "%hd:%hd", &dev.device, &dev.func);
		}

		if ((tmp=strstr(buf,"Software Specifier ID: "))) {
			specid = strtoul(tmp+23,NULL,16);
			tmp = strstr(buf,"Software Version: ");
			if (tmp)
				version = strtoul(tmp+18,NULL,16);
			if (version == 0x010483 && specid == 0x00609e) 
			{
				tmp = strstr(buf,"Vendor ID:");
				if (tmp) 
				{
					char *end;
					end = strstr(tmp,"\n");
					int pos = end - tmp - 9;
					char vendor_id_str[10];
					strncpy(vendor_id_str, tmp+pos, 8);
					dev.vendorid = strtoul(vendor_id_str,NULL,16);
				}
				
				tmp = strstr(buf,"Vendor/Model ID:");
				if (tmp) 
				{
					char *end;
					end = strstr(tmp,"\n");
					if (end) *end = '\0';
					strcpy(desc, tmp+17);
					
					int pos = end - tmp - 7;
					char product_id_str[10];
					strncpy(product_id_str, tmp+pos, 6);
					dev.productid = strtoul(product_id_str,NULL,16);
				} 
				else 
				{
					strcpy(desc, "Generic Firewire Storage Controller");
				}
				
				add_device(&dev);
			}
		}
		buf = next;
	}
	
out:
	free(buf);
/* Do NOT attempt to unload firewire module (may crash), rather check for existing one. -KK */
#if 0
	if (loaded_driver == 1)
		removeModule("ohci1394");
#endif
}

/* ---------------------------------------------------------------------- */

void 
scan_sysfs_pci()
{
	int dev_cnt;
	char desc[128],module[128];
	char *sysbuspci = "/sys/bus/pci/devices";
	char found_yenta_socket = 0;
	struct dirent **devlist;
	struct lshwd_device dev;
	
	dev.module = module;
	dev.desc = desc;
	dev.devicenode = "";
	dev.bustype = BUS_PCI;
	dev.func = dev.classid = dev.subvendorid = dev.subproductid = 0;
	
	init_lookup_block("pcitable");

begin_sysfs_pci_scan:

	dev_cnt = scandir(sysbuspci, &devlist, 0, NULL);
	if (dev_cnt == -1) dev_cnt = 0;
	while (dev_cnt--)
	{
		if (devlist[dev_cnt]->d_name[0] < '0' || devlist[dev_cnt]->d_name[0] > '9') { free(devlist[dev_cnt]); continue; }

		*desc = *module = 0;
		dev.devicenode = "";

		sscanf(devlist[dev_cnt]->d_name, "%*d:%2x:%2x.%x", &dev.bus, &dev.device, &dev.func);
		dev.businfo = devlist[dev_cnt]->d_name;
		dev.vendorid = get_hexvalue_from_file(sysbuspci, devlist[dev_cnt]->d_name,"vendor");
		dev.productid = get_hexvalue_from_file(sysbuspci, devlist[dev_cnt]->d_name,"device");
		dev.subvendorid = get_hexvalue_from_file(sysbuspci, devlist[dev_cnt]->d_name,"subsystem_vendor");
		dev.subproductid = get_hexvalue_from_file(sysbuspci, devlist[dev_cnt]->d_name,"subsystem_device");
		dev.classid = get_hexvalue_from_file(sysbuspci, devlist[dev_cnt]->d_name,"class");
		dev.classname = get_pci_classname(dev.classid);

		lookup_module(dev.vendorid, dev.productid, dev.subvendorid, dev.subproductid,
				module, sizeof(module),
				desc, sizeof(desc));

		if (strcmp(module, "yenta_socket") == 0) found_yenta_socket++;
		if ((dev.classid >> 16) == PCI_BASE_CLASS_NETWORK)
			dev.devicenode = find_network_devices(dev.module);
		if ((dev.classid >> 16) == PCI_BASE_CLASS_DISPLAY)
		{
			/* getxinfo will retrieve x info and create /tmp/xinfo if needed */
			struct xinfo *x = getxinfo(desc, module, outputxinfo);
			strcpy(module, x->xmodule);
			if (dev.vendorid == 0x10de) //nVidia
				snprintf(module, sizeof(module) - 1, "nv");
			else if (dev.vendorid == 0x1002) //ATI
				snprintf(module, sizeof(module) - 1, "radeon");
			DEBUG("%s\n%s\n%s\n%s\n", x->xserver, x->xmodule, x->xdesc, x->xopts);
		}
		if (!is_device_listed(&dev)) add_device(&dev);

		free(devlist[dev_cnt]);
	}
	free(devlist);

	/* check for yenta_socket module (ONLY ONCE) */
	if ( autoload && (found_yenta_socket == 1) && !isLoaded("yenta_socket") )	
	{
		loadModule("yenta_socket");
		sleep(1);
		goto begin_sysfs_pci_scan;
	}

	cleanup_lookup_block();     /* free table file   */
	return;
}

/* ---------------------------------------------------------------------- */

void 
scan_sysfs_firewire()
{
}

/* ---------------------------------------------------------------------- */

/*
 * simple mouse probe to list mouse device
 */

void 
scan_mouse(void)
{
	char devicebuf[128], desc[128], module[128];
	struct lshwd_device dev;
	
	//if (usbmousefound) return;

	if (psauxProbe( devicebuf, module, desc))
	{
		if (usbmousefound) sprintf(desc, "PS/2 Mouse port");
		dev.module = module;
		dev.desc = desc;
		dev.devicenode = devicebuf;
		dev.businfo = "---:---";
		dev.classname = "Mouse";
		dev.bustype = BUS_MOUSE;
		dev.bus = dev.device = dev.func = dev.vendorid = dev.productid = dev.subvendorid = dev.subproductid = 0;
		add_device(&dev);
	}
}

/*****************************************************************************
 * mandatory routines...
 *****************************************************************************/

int
usage(char *option)
{
	printf(LSHWD_VERSION_TEXT);
	if (strcmp(option,"-?")) fprintf(stderr,"lshwd: invalid option '%s'\n",option);
	printf( "usage: lshwd [-a] [-c] [-cc] [-d] [-id] [-m] [-n] [-ox] [-s]\n"
			"\t-a   auto-modprobe\n"
			"\t-c   categorized output\n"
			"\t-cc  colorized & categorized output\n"
			"\t-d   display default device description\n"
			"\t-id  display hardware id\n"
			"\t-m   machine readable format\n"
			"\t-n   no duplicates\n"
			"\t-ox  output X info to /tmp/xinfo (gfx card section only)\n"
			"\t-s   probe using sysfs (EXPERIMENTAL)\n"
	      );
	return option? 1 : 0;
}

/* ---------------------------------------------------------------------- */

int
main(int argc, char **argv)
{
	int c;

	/* default settings */
	autoload = disdefdesc = removeduplicates = machinemode = outputxinfo = usbmousefound = showids = sysfsmode = 0;
	showlist = plainmode  = 1;

	for(c=1;c<argc;c++)
	{
		if(!strcasecmp(argv[c],"-a"))      autoload = 1;
		else if(!strcasecmp(argv[c],"-d")) disdefdesc = 1;
		else if(!strcasecmp(argv[c],"-c")) showlist = 0;
		else if(!strcasecmp(argv[c],"-m")) machinemode = 1;
		else if(!strcasecmp(argv[c],"-n")) removeduplicates = 1;
		else if(!strcasecmp(argv[c],"-s")) sysfsmode = 1;
		else if(!strcasecmp(argv[c],"-ox")) outputxinfo = 1;
		else if(!strcasecmp(argv[c],"-id")) showids = 1;
		else if(!strcasecmp(argv[c],"-cc")) plainmode = showlist = 0;
		else return usage(argv[c]);
	}

	if (plainmode || showlist || machinemode)
		normal_color = modules_color = title_color = "";
	else
		printf("\e[H\e[J");				/* clear screen with ansi chars */

/*
	`Is there any point to which you would wish to draw my attention?' 
	`To the curious incident of the dog in the night-time.' 
	`The dog did nothing in the night-time.' 
	`That was the curious incident,' remarked Sherlock Holmes.
*/

	if (sysfsmode)
	{
		scan_sysfs_pci();
		scan_sysfs_firewire();
	}
	else
	{
		scan_procfs_pci();
		scan_procfs_firewire();
	}

	scan_mouse();
	
	if (!showlist) sort_devices();
	dump_devices();
	free_devices();

	return 0;
}
