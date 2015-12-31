#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <usb.h>
#include "hexfile.h"

static const char rcsid[] = "$Id: fpga_load.c 2477 2007-04-29 22:12:49Z tzafrir $";

#define	ERR(fmt, arg...) do { \
				if(verbose >= LOG_ERR) \
					fprintf(stderr, "%s: ERROR (%d): " fmt, \
						progname, __LINE__, ## arg); \
			} while(0);
#define	INFO(fmt, arg...) do { \
				if(verbose >= LOG_INFO) \
					fprintf(stderr, "%s: " fmt, \
						progname, ## arg); \
			} while(0);
#define	DBG(fmt, arg...) do { \
				if(verbose >= LOG_DEBUG) \
					fprintf(stderr, "%s: DBG: " fmt, \
						progname, ## arg); \
			} while(0);

static int	verbose = LOG_WARNING;
static char	*progname;

#define	MAX_HEX_LINES	2000
#define	PACKET_SIZE	512
#define	EEPROM_SIZE	16
#define	SERIAL_SIZE	8

enum fpga_load_packet_types {
	PT_STATUS_REPLY	= 0x01,
	PT_DATA_PACKET	= 0x01,
#ifdef	XORCOM_INTERNAL
	PT_EEPROM_SET	= 0x04,
#endif
	PT_EEPROM_GET	= 0x08,
	PT_RENUMERATE	= 0x10,
	PT_RESET	= 0x20,
	PT_BAD_COMMAND	= 0xAA
};

struct myeeprom {
	uint8_t		source;
	uint16_t	vendor;
	uint16_t	product;
	uint8_t		release_major;
	uint8_t		release_minor;
	uint8_t		reserved;
	uint8_t		serial[SERIAL_SIZE];
} PACKED;

struct fpga_packet_header {
	struct {
		uint8_t		op;
	} PACKED header;
	union {
		struct {
			uint16_t	seq;
			uint8_t		status;
		} PACKED status_reply;
		struct {
			uint16_t	seq;
			uint8_t		reserved;
			uint8_t		data[ZERO_SIZE];
		} PACKED data_packet;
		struct {
			struct myeeprom		data;
		} PACKED eeprom_set;
		struct {
			struct myeeprom		data;
		} PACKED eeprom_get;
	} d;
} PACKED;

enum fpga_load_status {
	FW_FAIL_RESET	= 1,
	FW_FAIL_TRANS	= 2,
	FW_TRANS_OK	= 4,
	FW_CONFIG_DONE	= 8
};

struct my_usb_device {
	struct usb_device	*dev;
	usb_dev_handle		*handle;
	char			iManufacturer[BUFSIZ];
	char			iProduct[BUFSIZ];
	char			iSerialNumber[BUFSIZ];
	int			is_usb2;
	struct myeeprom		eeprom;
};

const char *load_status2str(enum fpga_load_status s)
{
	switch(s) {
		case FW_FAIL_RESET: return "FW_FAIL_RESET";
		case FW_FAIL_TRANS: return "FW_FAIL_TRANS";
		case FW_TRANS_OK: return "FW_TRANS_OK";
		case FW_CONFIG_DONE: return "FW_CONFIG_DONE";
		default: return "UNKNOWN";
	}
}

struct usb_device *dev_of_path(const char *path)
{
	struct usb_bus		*bus;
	struct usb_device	*dev;
	char			dirname[PATH_MAX];
	char			filename[PATH_MAX];
	const char		*p;
	int			bnum;
	int			dnum;
	int			ret;

	assert(path != NULL);
	if(access(path, F_OK) < 0) {
		perror(path);
		return NULL;
	}
	/* Find last '/' */
	if((p = memrchr(path, '/', strlen(path))) == NULL) {
		ERR("Missing a '/' in %s\n", path);
		return NULL;
	}
	/* Get the device number */
	ret = sscanf(p + 1, "%d", &dnum);
	if(ret != 1) {
		ERR("Path tail is not a device number: '%s'\n", p);
		return NULL;
	}
	/* Search for a '/' before that */
	p = memrchr(path, '/', p - path);
	if(p == NULL)
		p = path;		/* Relative path */
	else
		p++;			/* skip '/' */
	/* Get the bus number */
	ret = sscanf(p, "%d", &bnum);
	if(ret != 1) {
		ERR("Path tail is not a bus number: '%s'\n", p);
		return NULL;
	}
	sprintf(dirname, "%03d", bnum);
	sprintf(filename, "%03d", dnum);
	for (bus = usb_busses; bus; bus = bus->next) {
		if(strcmp(bus->dirname, dirname) != 0)
			continue;
		for (dev = bus->devices; dev; dev = dev->next) {
			if(strcmp(dev->filename, filename) == 0)
				return dev;
		}
	}
	ERR("no usb device match '%s'\n", path);
	return NULL;
}

int get_usb_string(char *buf, unsigned int len, uint16_t item, usb_dev_handle *handle)
{
	char	tmp[BUFSIZ];
	int	ret;

	if (!item)
		return 0;
	ret = usb_get_string_simple(handle, item, tmp, BUFSIZ);
	if (ret <= 0)
		return ret;
	return snprintf(buf, len, "%s", tmp);
}

/* My device parameters */
#define	MY_CONFIG	1
#define	MY_INTERFACE	0
#define	MY_ENDPOINTS	4

#define	MY_EP_OUT	0x04
#define	MY_EP_IN	0x88

#define	FPGA_EP_OUT	0x02
#define	FPGA_EP_IN	0x86

#define	TIMEOUT		5000

static const int my_endpoints[MY_ENDPOINTS] = {
	FPGA_EP_OUT,
	MY_EP_OUT,
	FPGA_EP_IN,
	MY_EP_IN
};

void my_usb_device_cleanup(struct my_usb_device *mydev)
{
	assert(mydev != NULL);
	if(!mydev->handle) {
		return;	/* Nothing to do */
	}
	if(usb_release_interface(mydev->handle, MY_INTERFACE) != 0) {
		ERR("Releasing interface: usb: %s\n", usb_strerror());
	}
	if(usb_close(mydev->handle) != 0) {
		ERR("Closing device: usb: %s\n", usb_strerror());
	}
}

static void show_device_info(const struct my_usb_device *mydev)
{
	const struct myeeprom	*eeprom;
	const uint8_t		*data = eeprom->serial;

	assert(mydev != NULL);
	eeprom = &mydev->eeprom;
	data = eeprom->serial;
	printf("USB    iManufacturer: [%s]\n", mydev->iManufacturer);
	printf("USB    iProduct:      [%s]\n", mydev->iProduct);
	printf("USB    iSerialNumber: [%s]\n", mydev->iSerialNumber);
	printf("EEPROM Source:        0x%02X\n", eeprom->source);
	printf("EEPROM Vendor:        0x%04X\n", eeprom->vendor);
	printf("EEPROM Product:       0x%04X\n", eeprom->product);
	printf("EEPROM Release:       %d.%03d\n", eeprom->release_major, eeprom->release_minor);
	printf("EEPROM Serial:        0x[%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X]\n",
			data[0], data[1], data[2], data[3],
			data[4], data[5], data[6], data[7]); 
}

void dump_packet(const char *msg, const char *buf, int len)
{
	int	i;

	for(i = 0; i < len; i++)
		INFO("%s: %2d> 0x%02X\n", msg, i, (uint8_t)buf[i]);
}

int send_usb(const char *msg, struct my_usb_device *mydev, int endpoint, struct fpga_packet_header *phead, int len, int timeout)
{
	char	*p = (char *)phead;
	int	ret;

	if(verbose >= LOG_DEBUG)
		dump_packet(msg, p, len);
	ret = usb_bulk_write(mydev->handle, endpoint, p, len, timeout);
	if(ret < 0) {
		ERR("bulk_write failed: %s\n", usb_strerror());
		dump_packet("send_usb[ERR]", p, len);
		return ret;
	} else if(ret != len) {
		ERR("bulk_write short write: %s\n", usb_strerror());
		dump_packet("send_usb[ERR]", p, len);
		return -EFAULT;
	}
	return ret;
}

int recv_usb(const char *msg, struct my_usb_device *mydev, int endpoint, char *buf, size_t len, int timeout)
{
	int	ret;

	if(verbose >= LOG_DEBUG)
		dump_packet(msg, buf, len);
	ret = usb_bulk_read(mydev->handle, endpoint, buf, len, timeout);
	if(ret < 0) {
		ERR("bulk_read failed: %s\n", usb_strerror());
		dump_packet("recv_usb[ERR]", buf, len);
		return ret;
	}
	return ret;
}

#ifdef	XORCOM_INTERNAL
int eeprom_set(struct my_usb_device *mydev, const struct myeeprom *eeprom)
{
	int				ret;
	int				len;
	char				buf[PACKET_SIZE];
	struct fpga_packet_header	*phead = (struct fpga_packet_header *)buf;

	DBG("%s Start...\n", __FUNCTION__);
	assert(mydev != NULL);
	phead->header.op = PT_EEPROM_SET;
	memcpy(&phead->d.eeprom_set.data, eeprom, EEPROM_SIZE);
	len = sizeof(phead->d.eeprom_set) + sizeof(phead->header.op);
	ret = send_usb("eeprom_set[W]", mydev, MY_EP_OUT, phead, len, TIMEOUT);
	if(ret < 0)
		return ret;
	ret = recv_usb("eeprom_set[R]", mydev, MY_EP_IN, buf, sizeof(buf), TIMEOUT);
	if(ret <= 0)
		return ret;
	phead = (struct fpga_packet_header *)buf;
	if(phead->header.op == PT_BAD_COMMAND) {
		ERR("Firmware rejected PT_EEPROM_SET command\n");
		return -EINVAL;
	} else if(phead->header.op != PT_EEPROM_SET) {
		ERR("Got unexpected reply op=%d\n", phead->header.op);
		return -EINVAL;
	}
	return 0;
}
#endif

int eeprom_get(struct my_usb_device *mydev)
{
	int				ret;
	int				len;
	char				buf[PACKET_SIZE];
	struct fpga_packet_header	*phead = (struct fpga_packet_header *)buf;
	struct myeeprom			*eeprom;

	assert(mydev != NULL);
	eeprom = &mydev->eeprom;
	DBG("%s Start...\n", __FUNCTION__);
	phead->header.op = PT_EEPROM_GET;
	len = sizeof(phead->header.op);		/* warning: sending small packet */
	ret = send_usb("eeprom_get[W]", mydev, MY_EP_OUT, phead, len, TIMEOUT);
	if(ret < 0)
		return ret;
	ret = recv_usb("eeprom_get[R]", mydev, MY_EP_IN, buf, sizeof(buf), TIMEOUT);
	if(ret <= 0)
		return ret;
	phead = (struct fpga_packet_header *)buf;
	if(phead->header.op == PT_BAD_COMMAND) {
		ERR("PT_BAD_COMMAND\n");
		return -EINVAL;
	} else if(phead->header.op != PT_EEPROM_GET) {
		ERR("Got unexpected reply op=%d\n", phead->header.op);
		return -EINVAL;
	}
	memcpy(eeprom, &phead->d.eeprom_get.data, EEPROM_SIZE);
	return 0;
}

int send_hexline(struct my_usb_device *mydev, struct hexline *hexline, int seq)
{
	int				ret;
	int				len;
	uint8_t				*data;
	char				buf[PACKET_SIZE];
	struct fpga_packet_header	*phead = (struct fpga_packet_header *)buf;
	enum fpga_load_status		status;

	assert(mydev != NULL);
	assert(hexline != NULL);
	len = hexline->d.content.header.ll;	/* don't send checksum */
	data = hexline->d.content.tt_data.data;
	if(hexline->d.content.header.tt != TT_DATA) {
		ERR("Bad record %d type = %d\n", seq, hexline->d.content.header.tt);
		return -EINVAL;
	}
	phead->header.op = PT_DATA_PACKET;
	phead->d.data_packet.seq = seq;
	phead->d.data_packet.reserved = 0x00;
	memcpy(phead->d.data_packet.data, data, len);
	len += sizeof(hexline->d.content.header);
	DBG("%04d+\r", seq);
	ret = send_usb("hexline[W]", mydev, MY_EP_OUT, phead, len, TIMEOUT);
	if(ret < 0)
		return ret;
	ret = recv_usb("hexline[R]", mydev, MY_EP_IN, buf, sizeof(buf), TIMEOUT);
	if(ret <= 0)
		return ret;
	DBG("%04d-\r", seq);
	phead = (struct fpga_packet_header *)buf;
	if(phead->header.op != PT_STATUS_REPLY) {
		ERR("Got unexpected reply op=%d\n", phead->header.op);
		dump_packet("hexline[ERR]", buf, ret);
		return -EINVAL;
	}
	status = (enum fpga_load_status)phead->d.status_reply.status;
	switch(status) {
		case FW_TRANS_OK:
		case FW_CONFIG_DONE:
			break;
		case FW_FAIL_RESET:
		case FW_FAIL_TRANS:
			ERR("status reply %s (%d)\n", load_status2str(status), status);
			dump_packet("hexline[ERR]", buf, ret);
			return -EPROTO;
		default:
			ERR("Unknown status reply %d\n", status);
			dump_packet("hexline[ERR]", buf, ret);
			return -EPROTO;
	}
	return 0;
}

//. returns > 0 - ok, the number of lines sent
//. returns < 0 - error number
int send_splited_hexline(struct my_usb_device *mydev, struct hexline *hexline, int seq, uint8_t maxwidth)
{
	struct hexline *extraline;
	int linessent = 0;
	int allocsize;
	int extra_offset = 0;
	unsigned int this_line = 0;
	uint8_t bytesleft = 0;
	
	assert(mydev != NULL);
	if(!hexline) {
		ERR("Bad record %d type = %d\n", seq, hexline->d.content.header.tt);
		return -EINVAL;
	}
	bytesleft = hexline->d.content.header.ll;
	// split the line into several lines
	while (bytesleft > 0) {
		int status;
		this_line = (bytesleft >= maxwidth) ? maxwidth : bytesleft;
		allocsize = sizeof(struct hexline) + this_line + 1;
		// generate the new line
		if((extraline = (struct hexline *)malloc(allocsize)) == NULL) {
			ERR("Not enough memory for spliting the lines\n" );
			return -EINVAL;
		}
		memset( extraline, 0, allocsize );
		extraline->d.content.header.ll		= this_line;
		extraline->d.content.header.offset	= hexline->d.content.header.offset + extra_offset;
		extraline->d.content.header.tt		= hexline->d.content.header.tt;
		memcpy( extraline->d.content.tt_data.data, hexline->d.content.tt_data.data+extra_offset, this_line);
		status = send_hexline(mydev, extraline, seq+linessent );
		// cleanups
		free(extraline);
		extra_offset += this_line;
		bytesleft -= this_line;
		if (status)
			return status;
		linessent++;
	}
	return linessent;
}

int my_usb_device_init(const char devpath[], struct my_usb_device *mydev)
{
	struct usb_device_descriptor	*dev_desc;
	struct usb_config_descriptor	*config_desc;
	struct usb_interface		*interface;
	struct usb_interface_descriptor	*iface_desc;
	struct usb_endpoint_descriptor	*endpoint;
	int				ret;
	int				i;

	assert(mydev != NULL);
	usb_init();
	usb_find_busses();
	usb_find_devices();
	mydev->dev = dev_of_path(devpath);
	if(!mydev->dev) {
		ERR("Bailing out\n");
		return 0;
	}
	mydev->handle = usb_open(mydev->dev);
	if(!mydev->handle) {
		ERR("Failed to open usb device '%s/%s': %s\n", mydev->dev->bus->dirname, mydev->dev->filename, usb_strerror());
		return 0;
	}
	if(usb_set_configuration(mydev->handle, MY_CONFIG) != 0) {
		ERR("usb: %s\n", usb_strerror());
		return 0;
	}
	if(usb_claim_interface(mydev->handle, MY_INTERFACE) != 0) {
		ERR("usb: %s\n", usb_strerror());
		return 0;
	}
	if(usb_reset(mydev->handle) != 0) {
		ERR("Reseting device: usb: %s\n", usb_strerror());
		return 0;
	}
	dev_desc = &mydev->dev->descriptor;
	config_desc = mydev->dev->config;
	interface = &config_desc->interface[MY_INTERFACE];
	iface_desc = interface->altsetting;
	INFO("Vendor:Product=%04X:%04X Class=%d (endpoints=%d)\n",
			dev_desc->idVendor,
			dev_desc->idProduct,
			dev_desc->bDeviceClass,
			iface_desc->bNumEndpoints);
	if(iface_desc->bInterfaceClass != 0xFF) {
		ERR("Wrong Interface class %d\n", iface_desc->bInterfaceClass);
		return 0;
	}
	if(iface_desc->bInterfaceNumber != MY_INTERFACE) {
		ERR("Wrong Interface number %d\n", iface_desc->bInterfaceNumber);
		return 0;
	}
	if(iface_desc->bNumEndpoints != MY_ENDPOINTS) {
		ERR("Wrong number of endpoints: %d\n", iface_desc->bNumEndpoints);
		return 0;
	}
	endpoint = iface_desc->endpoint;
	mydev->is_usb2 = (endpoint->wMaxPacketSize == 512);
	for(i = 0; i < iface_desc->bNumEndpoints; i++, endpoint++) {
		if(endpoint->bEndpointAddress != my_endpoints[i]) {
			ERR("Wrong endpoint %d: address = 0x%X\n", i, endpoint->bEndpointAddress);
			return 0;
		}
		if(endpoint->bEndpointAddress == MY_EP_OUT || endpoint->bEndpointAddress == MY_EP_IN) {
			if(endpoint->wMaxPacketSize > PACKET_SIZE) {
				ERR("Endpoint #%d wMaxPacketSize too large (%d)\n", i, endpoint->wMaxPacketSize);
				return 0;
			}
		}
	}
	if(usb_resetep(mydev->handle, MY_EP_OUT) != 0) {
		ERR("Failed to reset usb output endpoint: %s\n", usb_strerror());
		return 0;
	}
	if(usb_resetep(mydev->handle, MY_EP_IN) != 0) {
		ERR("Failed to reset usb input endpoint: %s\n", usb_strerror());
		return 0;
	}
	ret = get_usb_string(mydev->iManufacturer, BUFSIZ, dev_desc->iManufacturer, mydev->handle);
	ret = get_usb_string(mydev->iProduct, BUFSIZ, dev_desc->iProduct, mydev->handle);
	ret = get_usb_string(mydev->iSerialNumber, BUFSIZ, dev_desc->iSerialNumber, mydev->handle);
	return 1;
}

int renumerate_device(struct my_usb_device *mydev, enum fpga_load_packet_types pt)
{
	char				buf[PACKET_SIZE];
	struct fpga_packet_header	*phead = (struct fpga_packet_header *)buf;
	int				ret;

	assert(mydev != NULL);
	DBG("Renumerating with 0x%X\n", pt);
	phead->header.op = pt;
	ret = send_usb("renumerate[W]", mydev, MY_EP_OUT, phead, 1, TIMEOUT);
	if(ret < 0)
		return ret;
	return 0;
}

/*
 * Returns: true on success, false on failure
 */
int fpga_load(struct my_usb_device *mydev, const struct hexdata *hexdata)
{
	unsigned int	i;
	unsigned int	j = 0;
	int		ret;
	int		finished = 0;
	const char	*v = hexdata->version_info;
	
	v = (v[0]) ? v : "Unknown";
	assert(mydev != NULL);
	INFO("FPGA_LOAD (version %s)\n", v);
	/*
	 * i - is the line number
	 * j - is the sequence number, on USB 2, i=j, but on
	 *     USB 1 send_splited_hexline may increase the sequence
	 *     number, as it needs 
	 */
	for(i = 0; i < hexdata->maxlines; i++) {
		struct hexline	*hexline = hexdata->lines[i];

		if(!hexline)
			break;
		if(finished) {
			ERR("Extra data after End Of Data Record (line %d)\n", i);
			return 0;
		}
		if(hexline->d.content.header.tt == TT_EOF) {
			DBG("End of data\n");
			finished = 1;
			continue;
		}
		if(mydev->is_usb2) {
			if((ret = send_hexline(mydev, hexline, i)) != 0) {
				perror("Failed sending hexline");
				return 0;
			}
		} else {
			if((ret = send_splited_hexline(mydev, hexline, j, 60)) < 0) {
				perror("Failed sending hexline (splitting did not help)");
				return 0;
			}
			j += ret;
		}
	}
	DBG("Finished...\n");
	return 1;
}

#include <getopt.h>

void usage()
{
	fprintf(stderr, "Usage: %s -D {/proc/bus/usb|/dev/bus/usb}/<bus>/<dev> [options...]\n", progname);
	fprintf(stderr, "\tOptions:\n");
	fprintf(stderr, "\t\t[-r]		# Reset the device\n");
	fprintf(stderr, "\t\t[-b <binfile>]	# Output to <binfile>\n");
	fprintf(stderr, "\t\t[-I <hexfile>]	# Input from <hexfile>\n");
	fprintf(stderr, "\t\t[-i]		# Show hexfile information\n");
	fprintf(stderr, "\t\t[-g]		# Get eeprom from device\n");
#ifdef XORCOM_INTERNAL
	fprintf(stderr, "\t\t[-C srC byte]	# Set Address sourCe (default: C0)\n");
	fprintf(stderr, "\t\t[-V vendorid]	# Set Vendor id on device\n");
	fprintf(stderr, "\t\t[-P productid]	# Set Product id on device\n");
	fprintf(stderr, "\t\t[-R release]	# Set Release. 2 dot separated decimals\n");
	fprintf(stderr, "\t\t[-S serial]	# Set Serial. 8 comma separated numbers\n");
#endif
	exit(1);
}

static void parse_report_func(int level, const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	if(level <= verbose)
		vfprintf(stderr, msg, ap);
	va_end(ap);
}

int main(int argc, char *argv[])
{
	struct my_usb_device	mydev;
	const char		*devpath = NULL;
	const char		*binfile = NULL;
	const char		*hexfile = NULL;
	struct hexdata		*hexdata = NULL;
	int			opt_reset = 0;
	int			opt_info = 0;
	int			opt_read_eeprom = 0;
#ifdef	XORCOM_INTERNAL
	int			opt_write_eeprom = 0;
	char			*vendor = NULL;
	char			*source = NULL;
	int			is_source_given = 0;
	char			*product = NULL;
	char			*release = NULL;
	char			*serial = NULL;
	uint8_t			serial_buf[SERIAL_SIZE];
	const char		options[] = "rib:C:D:ghI:vV:P:R:S:";
#else
	const char		options[] = "rib:D:ghI:v";
#endif
	int			ret = 0;

	progname = argv[0];
	assert(sizeof(struct fpga_packet_header) <= PACKET_SIZE);
	assert(sizeof(struct myeeprom) == EEPROM_SIZE);
	while (1) {
		int	c;

		c = getopt (argc, argv, options);
		if (c == -1)
			break;

		switch (c) {
			case 'D':
				devpath = optarg;
				break;
			case 'r':
				opt_reset = 1;
				break;
			case 'i':
				opt_info = 1;
				break;
			case 'b':
				binfile = optarg;
				break;
			case 'g':
				opt_read_eeprom = 1;
				break;
			case 'I':
				hexfile = optarg;
				break;
#ifdef	XORCOM_INTERNAL
			case 'V':
				vendor = optarg;
				break;
			case 'C':
				source = optarg;
				is_source_given = 1;
				break;
			case 'P':
				product = optarg;
				break;
			case 'R':
				release = optarg;
				break;
			case 'S':
				serial = optarg;
				{
					int		i;
					char		*p;
					unsigned long	val;

					p = strtok(serial, ",");
					for(i = 0; i < SERIAL_SIZE && p; i++) {
						val = strtoul(p, NULL, 0);
						if(val > 0xFF) {
							ERR("Value #%d for -S option is too large (%lu)\n", i+1, val);
							usage();
						}
						serial_buf[i] = val;
						p = strtok(NULL, ",");
					}
					if(i < SERIAL_SIZE) {
						ERR("got only %d values for -S option. Need %d\n", i, SERIAL_SIZE);
						usage();
					}
				}

				break;
#endif
			case 'v':
				verbose++;
				break;
			case 'h':
			default:
				ERR("Unknown option '%c'\n", c);
				usage();
		}
	}

	if (optind != argc) {
		usage();
	}
	if(hexfile) {
		parse_hexfile_set_reporting(parse_report_func);
		hexdata = parse_hexfile(hexfile, MAX_HEX_LINES);
		if(!hexdata) {
			ERR("Bailing out\n");
			exit(1);
		}
		if(opt_info) {
			printf("%s: Version=%s Checksum=%d\n",
					hexfile, hexdata->version_info,
					calc_checksum(hexdata));
		}
		if(binfile)
			dump_binary(hexdata, binfile);
		if(!devpath)
			return 0;
	}
	if(!devpath) {
		ERR("Missing device path\n");
		usage();
	}
#ifdef	XORCOM_INTERNAL
	if(vendor || product || release || serial || source )
		opt_read_eeprom = opt_write_eeprom = 1;
#endif
	DBG("Startup %s\n", devpath);

	if(!my_usb_device_init(devpath, &mydev)) {
		ERR("Failed to initialize USB device '%s'\n", devpath);
		ret = -ENODEV;
		goto dev_err;
	}
	ret = eeprom_get(&mydev);
	if(ret < 0) {
		ERR("Failed reading eeprom\n");
		goto dev_err;
	}
	if(opt_read_eeprom) {
		show_device_info(&mydev);
	}
	if(hexdata) {
		if (!mydev.is_usb2)
			INFO("Warning: working on a low end USB1 backend\n");
		if(!fpga_load(&mydev, hexdata)) {
			ERR("FPGA loading failed\n");
			ret = -ENODEV;
			goto dev_err;
		}
		ret = renumerate_device(&mydev, PT_RENUMERATE);
		if(ret < 0) {
			ERR("Renumeration failed: errno=%d\n", ret);
			goto dev_err;
		}
	}
#ifdef XORCOM_INTERNAL
	if(opt_write_eeprom) {
		// FF: address source is from device. C0: from eeprom
		if (is_source_given)
			mydev.eeprom.source = strtoul(source, NULL, 0);
		else
			mydev.eeprom.source = 0xC0;
		if(vendor)
			mydev.eeprom.vendor = strtoul(vendor, NULL, 0);
		if(product)
			mydev.eeprom.product = strtoul(product, NULL, 0);
		if(release) {
			int	release_major = 0;
			int	release_minor = 0;

			sscanf(release, "%d.%d", &release_major, &release_minor);
			mydev.eeprom.release_major = release_major;
			mydev.eeprom.release_minor = release_minor;
		}
		if(serial) {
			memcpy(&mydev.eeprom.serial, serial_buf, SERIAL_SIZE);
		}
		ret = eeprom_set(&mydev, &mydev.eeprom);
		if(ret < 0) {
			ERR("Failed writing eeprom: %s\n", strerror(-ret));
			goto dev_err;
		}
		printf("------- RESULTS -------\n");
		show_device_info(&mydev);
	}
#endif
	if(opt_reset) {
		DBG("Reseting to default\n");
		ret = renumerate_device(&mydev, PT_RESET);
		if(ret < 0) {
			ERR("Renumeration to default failed: errno=%d\n", ret);
			goto dev_err;
		}
	}
	DBG("Exiting\n");
dev_err:
	my_usb_device_cleanup(&mydev);
	return ret;
}
