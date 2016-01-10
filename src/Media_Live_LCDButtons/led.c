/* vfd demo application
 * Copyright (C) 2006, Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/* This is an example application that turns on the icons on the front panel
 * and also displays text on the VFD screen
 */
 
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/hiddev.h>
#include "libvfd.h"


//**************************************************************
//  HID devices exchange data with the host computer using data bundles called 
//  reports.  Each report is divided into "fields", each of which can have one
//  or more "usages".  In the hid-core each one of these usages has a single
//  signed 32 bit value.
//
//  read():
//  This is the event interface.  When the HID device's state changes, it
//  performs an interrupt transfer containing a report which contains the 
//  changed value.  The hid-core.c module parses the report, and returns to
//  hiddev.c the individual usages that have changed within the report.  In
//  its basic mode, the hiddev will make these individual usage changes
//  available to the reader using a struct hiddev_event:
//    struct hiddev_event { unsigned hid;
//  containing the HID usage identifier for the status that changed, and the
//  value that it was changed to.  Note that the structure is defined within
//  <linux/hiddev.h>, along with some other useful #defines and structures.  
//  The HID usage identifier is a composite of the HID usage page shifed to 
//  the 16 high order bits ORed with the usage code.  The behavior of the read()
//  function can be modified using the HIDIOCSFLAG ioctl described below.
//
//  ioctl():
//  Instructs the kernel to retrieve all input and feature report values from
//  the device.  At this point, all the usage structures will contain current
//  values for the device, and will maintain it as the device changes.  Note 
//  that the use of this ioctl is unnecessary in general, since later kernels
//  automatically initialize the reports from the device at attach time.
//**************************************************************

/* hack - fix improper signed char handling - it's seeing 0x80 as a negative value*/
#define VALUE_FILTER(_value)  (_value>0x7F)?(__s32)(0xFFFFFF00 | _value):(_value)

#define VFD_PACKET_SIZE(s) (s*8)

unsigned char amd_logo[VFD_PAGE_SIZE] = {
  0x7f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x3f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x1f,0xfe,0x07,0x83,0xc0,0xe3,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0f,0xfe,0x07,0x83,0xe1,0xe3,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x07,0xfe,0x07,0xc3,0xe1,0xe3,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x10,0x1e,0x0f,0xc3,0xf3,0xe3,0x87,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x30,0x1e,0x0e,0xe3,0xb3,0x63,0x83,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x30,0x1e,0x1c,0xe3,0xb7,0x63,0x83,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0x1e,0x1c,0xe3,0x9e,0x63,0x83,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0x1e,0x1c,0x73,0x9e,0x63,0x83,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0x1e,0x3f,0xf3,0x9c,0x63,0x83,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xce,0x3f,0xf3,0x8c,0x63,0x8f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0x86,0x38,0x3b,0x80,0x63,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0x02,0x70,0x3b,0x80,0x63,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


static unsigned char icon_bits[] = 
{
  15, 14, 13, 12, 11, 10,  9,  8,
  18, 17, 16, 19, 23, 27, 31, 35,
  39, 43, 47, 51, 55, 59, 63
};
static unsigned int gIconMask;
static unsigned char gLayout      = VFD_STR_LAYOUT_1;
static unsigned char gScrollTime  = 1;
static unsigned char gFlags       = 0;
static int gDisplayMode           = VFD_MODE_NONE;
static unsigned char gPages[VFD_PAGE_SIZE][VFD_PAGE_COUNT];

//**************************************************************
// FUNCTION: Compare
//
// INPUT: 
//  const char *pszValue - string to compare
//  short sValue - numberic value to compare
//
// OUTPUT:
//   int - Boolean value, 0 on non match, 1 on success.
//
// DESCRIPTION: Compare a character value to a numeric value.
//**************************************************************
int Compare(const char *pszValue, short sValue)
{
  int iValue;
  // convert the pszValue to a number
  sscanf( pszValue, "%4x", &iValue);
  return( iValue == sValue );
}


//**************************************************************
// FUNCTION: OpenHID
//
// INPUT: 
//  const char *pszVendor - Vendor ID of device to open
//  const char *pszProduct - Product ID of device to open
//
// OUTPUT:
//   int fd - file descriptor to the opened HID device or -1 if err.
//
// DESCRIPTION: This function will open all the HID's on the
//  system until we find a match or we've exhausted our search.
//**************************************************************
static int OpenHID(const char *pszVendor, const char *pszProduct)
{
  int i, fd=-1;
  char port[32];
  char name[256];
  const char *hiddev_prefix = "/dev/usb/hiddev"; /* in devfs */
  int version;
  struct hiddev_devinfo device_info;

  //******************************************************
  // Loop through all the 16 HID ports/devices looking for 
  // one that matches our device.
  //******************************************************
  for(i=0; i<16;i++)
  {
    sprintf(port, "%s%d", hiddev_prefix, i);
    if((fd = open(port,O_WRONLY))>=0)
    {
      ioctl(fd, HIDIOCGDEVINFO, &device_info);
      ioctl(fd, HIDIOCGNAME(sizeof(name)), name);

      // If we've found our device, no need to look further, time to stop searching
      if( Compare(pszVendor, device_info.vendor) && Compare(pszProduct, device_info.product))
      {
        break; // stop the for loop
      }
    }
  }

  //******************************************************
  // If we've found our device, print out some information about it.
  //******************************************************
  if(fd != -1)
  {
    int appl; 
    printf("Found Device - Name is %s\n", name);
    printf("Vendor[0x%04hx] Product[0x%04hx] Version[0x%04hx]\n\t",
          device_info.vendor, device_info.product, device_info.version);
    switch(device_info.bustype)
    {
      default: 
    	  printf(" an unknown bus type: 0x%04hx ", device_info.bustype);
    	  printf("bus[%d], devnum[%d] ifnum[%d]\n", device_info.busnum,
    		  device_info.devnum, device_info.ifnum); 
	      break;
    }
    //******************************************************
    // Read the version - it's a packed 32 field, so 
    // unpack it in order to display
    //******************************************************
    ioctl(fd, HIDIOCGVERSION, &version);
    printf("HIDdev Driver Version is %d.%d.%d\n", 
    	  version >>16, (version>>8)&0xff,version &0xff);

    printf("There are %d applications for this device\n", device_info.num_applications);

    for(i=0;i<device_info.num_applications; i++)
    {
			appl = ioctl(fd, HIDIOCAPPLICATION,	i);
			printf("Application[%i]	is 0x%x	", i,	appl);
      //******************************************************
			// The magic values	come from	various	usage	table	specs	
      //******************************************************
			switch(appl	>> 16)
			{
				case 0x01: printf("(Generic	Desktop	Page)\n");break;
				case 0x02: printf("(Simulation Controls)\n");	break;
				case 0x03: printf("(VR Controls)\n");					break;
				case 0x04: printf("(Sport	Controls)\n");			break;
				case 0x05: printf("(Game Controls)\n");				break;
				case 0x06: printf("(Generic	Device Controls)\n");break;
				case 0x07: printf("(Keyboard/Keypad)\n");			break;
				case 0x08: printf("(LEDs)\n");								break;
				case 0x09: printf("(Button)\n");							break;
				case 0x0A: printf("(Ordinal)\n");							break;
				case 0x0B: printf("(Telphony)\n");						break;
				case 0x0C: printf("(Consumer Product Page)\n");	break;
				case 0x0D: printf("(Digitizer)\n");						break;
				case 0x0E: printf("(Reserved)\n");						break;
				case 0x0F: printf("(PID	Page)\n");						break;
				case 0x14: printf("(Alphanumeric Display)\n"); break;
				case 0x15:
				case 0x3f: printf("(Reserved)\n");						break;
				case 0x40: printf("(Medical	Instruments)\n");	break;
				case 0x80: printf("(USB	Monitor	Page)\n");		break;
				case 0x81: printf("(USB	Enumerated Values	Page)\n"); break;
				case 0x82: printf("(VESA Virtual Controls	Page)\n"); break;
				case 0x83: printf("(Reserved Monitor Page)\n");	break;
				case 0x84: printf("(Power	Device Page)\n");		break;
				case 0x85: printf("(Battery	System Page)\n");	break;
				case 0x86: 
				case 0x87: printf("(Reserved Power Device	Page)\n"); break;
				case 0x8C: printf("(Bar	Code Scanner Page)\n");	break;
				case 0x8D: printf("(Scale	Page)\n");					break;
				case 0x8E: printf("(Magnetic Stripe	Reading	Device)\n"); break;
				case 0x8F: printf("(Point	of Sale	pages)\n");	break;
				case 0x90: printf("(Camera Control Page)\n");	break;
				case 0x91: printf("(Arcade Page)\n");					break;
				default: 
				{
					unsigned int page	=	(appl	>> 16) & 0x0000FFFF;
					if((page >=	0xFF00)	&& (page <=	0xFFFF))
						printf("(Vendor	Defined	-	0x%04X)\n",page);	
					else
						printf("(Unknown page	-	needs	to be	added	0x%04X)\n",(appl>>16));	
					break;
				}
			}
		}
  }
  return fd;
}

//**************************************************************
// FUNCTION: FindReports
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//
// OUTPUT:
//  none
//
// DESCRIPTION: This function will print out the type of reports
//  this HID accepts.
//**************************************************************
static void FindReports(int fd)
{
  int iFields, iUsage;
  unsigned int iReportType;
  struct hiddev_report_info repInfo;
  struct hiddev_field_info fieldInfo;
  struct hiddev_usage_ref usageRef;

  //******************************************************
  // For each type of report (Input, Output, Feature)
  // find out which is accepted for this device.
  //******************************************************
  for(iReportType=HID_REPORT_TYPE_MIN; 
      iReportType<=HID_REPORT_TYPE_MAX; 
  	  iReportType ++)
  {
    repInfo.report_type = iReportType;
    repInfo.report_id = HID_REPORT_ID_FIRST;

    //******************************************************
    // HIDIOCGREPORTINFO - struct hiddev_report_info (read/write)
    // Obtain the usage information if it is found
    //******************************************************
    while(ioctl(fd, HIDIOCGREPORTINFO, &repInfo)>=0)
    {
      printf("  Report id: %d (%s) (%d fields)\n",
      	      repInfo.report_id, 
	            (iReportType == HID_REPORT_TYPE_INPUT) ? "Input" : 
	            (iReportType == HID_REPORT_TYPE_OUTPUT) ? "Output" : "Feature/Other",
	            repInfo.num_fields);
     
      //******************************************************
      // HIDIOCGFIELDINFO - struct hiddev_field_info (read/write)
      // Returns the field information associated with a report 
      // in a hiddev_field_info structure.  The user must fill 
      // in report_id and report_type in this structure.  The 
      // field_index should also be filled in, which should be 
      // a number between 0 and maxfield-1.
      //******************************************************
      for(iFields = 0; iFields<repInfo.num_fields; iFields++)
      {
        memset(&fieldInfo, 0, sizeof(fieldInfo));
	      fieldInfo.report_type = repInfo.report_type;
        fieldInfo.report_id   = repInfo.report_id;
        fieldInfo.field_index = iFields;
        ioctl(fd, HIDIOCGFIELDINFO, &fieldInfo);

        // Print out information about this field
        printf("    Field: %d(id=%d): app: %04x phys %04x flags %x "
	              "(%d usages) unit %x exp %d\n",
	        iFields, fieldInfo.report_id, fieldInfo.application, 
	        fieldInfo.physical, fieldInfo.flags, fieldInfo.maxusage, 
          fieldInfo.unit, fieldInfo.unit_exponent);

        //******************************************************
        // HIDIOCGUCODE - struct hiddev_usage_ref(read/write)
        // Fill in the structure with report_type, report_id,
        // field_index, and usage_index to obtain the usage_code.
        //
        // HIDIOCGUSAGE - struct hiddev_usage_ref(read/write)
        // Obtain the value for the usage_code.
        //******************************************************
        for(iUsage=0; iUsage<fieldInfo.maxusage; iUsage++)
        {
          memset(&usageRef, 0, sizeof(usageRef));
	        usageRef.report_type = fieldInfo.report_type;
	        usageRef.report_id = fieldInfo.report_id;
	        usageRef.field_index = iFields;
	        usageRef.usage_index = iUsage;
	        ioctl(fd, HIDIOCGUCODE, &usageRef);
	        ioctl(fd, HIDIOCGUSAGE, &usageRef);
	        printf("      Usage: %04x val %d idx %x\n", usageRef.usage_code,
	          usageRef.value, usageRef.usage_index);
        }
      }

      //******************************************************
      // Go to the next report if there is one.
      //******************************************************
      repInfo.report_id |= HID_REPORT_ID_NEXT;
    }
  }
}

//**************************************************************
// FUNCTION: SendReport
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//	const unsigned char *buf - Message to write
//	size_t size - size of buf.
//
// OUTPUT:
//  int err - result of the ioctl call (On success 0, On error -1)
//
// DESCRIPTION: This function will write the 'buf' to the opened
//	HID device.  Specifically, it updates the device's usage 
//	reference with the data and then sends a report to the HID.
//**************************************************************
static int SendReport(int fd, const unsigned char *buf, size_t size)
{
	struct hiddev_report_info rinfo;
	struct hiddev_usage_ref	uref;
	struct hiddev_usage_ref	uref2;
	int	i, err;
  extern int errno;


  //******************************************************
	// Initialize the usage Reference and mark it for OUTPUT
  //******************************************************
	memset(&uref, 0, sizeof(uref));
	uref.report_type = HID_REPORT_TYPE_OUTPUT;
	uref.report_id	 = 0;
	uref.field_index = 0;

	//**************************************************************
	// Fill in the information that we wish to set
	//**************************************************************
	uref.usage_code  = 0xffa10005; //	unused?
	for(i=0;i<size;i++)
	{
		uref.usage_index = i;
		uref.value	 = VALUE_FILTER(buf[i]);

		//**************************************************************
		// HIDIOCSUSAGE - struct hiddev_usage_ref (write)
		// Sets the value of a usage in an output report.  The user fills
		// in the hiddev_usage_ref structure as above, but additionally 
		// fills in the value field.
		//**************************************************************
		if((err	= ioctl(fd, HIDIOCSUSAGE, &uref)) < 0)
		{
			printf("Error with sending the USAGE ioctl %d\n", errno);
			return err;
		}
		uref.usage_code  = 0xffa10006; //	unused?
	}

	//**************************************************************
	// HIDIOCSREPORT - struct hiddev_report_info (write)
	// Instructs the kernel to SEND a report to the device.  This 
	// report can be filled in by the user through HIDIOCSUSAGE calls 
	// (below) to fill in individual usage values in the report before
	// sending the report in full to the device.
	//**************************************************************
	memset(&rinfo, 0,	sizeof(rinfo));
	rinfo.report_type	= HID_REPORT_TYPE_OUTPUT;
	rinfo.report_id		= 0;
	rinfo.num_fields	= 0;
	if((err	= ioctl(fd, HIDIOCSREPORT, &rinfo)) < 0)
	{
		printf("Error with sending the REPORT ioctl %d\n", errno);
	}

  //******************************************************
	// All done, let's return what we did.
  //******************************************************
	return err;
}


//**************************************************************
// FUNCTION: VFDShowIcons
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//	int mask - Icon mask
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will turn on/off various icons
//  depending upon what was selected.
//**************************************************************
int VFDShowIcons(int fd, unsigned int mask)
{
  int i, err;
  unsigned char bitmap[8];
  const unsigned char panelCmd[]  = {0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
  const unsigned char iconCmd[]   = {0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  memset(bitmap, 0, sizeof(bitmap));
  for(i=0; i< VFD_ICON_COUNT; i++)
  {
    if(!(mask & (1<<i)))
      continue;
    bitmap[icon_bits[i]/8] |= 1 << (icon_bits[i] % 8);
    //printf("Bitmap[%d]=%x\n",icon_bits[i]/8,bitmap[icon_bits[i]/8]);
  }

  err = SendReport(fd, panelCmd, sizeof(panelCmd));
  err = SendReport(fd, iconCmd, sizeof(iconCmd));
  err = SendReport(fd, bitmap, sizeof(bitmap));
  return err;
}

//**************************************************************
// FUNCTION: VFDIconSet
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//	int icon - Icon to manipulate
//	int state - 1 for on, 0 for off.
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will set up the icon mask so that
//  the selected icons are marked to be on/off as desired.
//**************************************************************
int VFDIconSet(int fd, int icon, int state)
{
  if(icon > VFD_ICON_DVD)
    return -1;

  if(state)
    gIconMask |= 1<<icon;
  else
    gIconMask &= ~(1<<icon);

  return VFDShowIcons(fd, gIconMask);
}

//**************************************************************
// FUNCTION: VFDIconOn
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//	int icon - icon to turn on
//
// OUTPUT:
//  int err - result of the VFDIconSet call (On success 0, On error -1)
//
// DESCRIPTION: This function will turn on one specific icon.
//**************************************************************
int VFDIconOn(int fd, int icon)
{
#ifdef DEBUG
	printf("VFDIconOn %d\n",icon);
#endif
  return VFDIconSet(fd, icon, 1);
}

//**************************************************************
// FUNCTION: VFDIconOff
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//	int icon - icon to turn off
//
// OUTPUT:
//  int err - result of the VFDIconSet call (On success 0, On error -1)
//
// DESCRIPTION: This function will turn off one specific icon.
//**************************************************************
int VFDIconOff(int fd, int icon)
{
#ifdef DEBUG
	printf("VFDIconOff %d\n",icon);
#endif
  return VFDIconSet(fd, icon, 0);
}

//**************************************************************
// FUNCTION: VFDTurnOffIcons
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will turn off all the icons
//**************************************************************
int VFDTurnOffIcons(int fd)
{
#ifdef DEBUG
	printf("VFDTurnOffIcons\n");
#endif
  int err;
  const unsigned char panelCmd[]  = {0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
  const unsigned char iconCmd[]   = {0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //icon command
  const unsigned char iconoff[]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //icon data

  err = SendReport(fd, panelCmd, sizeof(panelCmd));
  err = SendReport(fd, iconCmd, sizeof(iconCmd));
  err = SendReport(fd, iconoff, sizeof(iconoff));
  return err;
}

//**************************************************************
// FUNCTION: VFDSetVolume
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int level - volume level (between 0 and 12)
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will raise/lower the volume indicator.
//**************************************************************
int VFDSetVolume(int fd, int level)
{
  int i;

  if(level>12)
    return -1;

  // Clear all of the volume values
  gIconMask &= ~0xFFFFF800;

  for(i=0;i<level;i++)
    gIconMask |= 1 << (VFD_VOLUME_1 + i);

  return VFDShowIcons(fd, gIconMask);
}

//**************************************************************
// FUNCTION: VFDSetString
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int region - region to display the string
//  int offset - location to display the string
//  unsigned char *buffer - string to display
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will display a string in the 
//  specified region.
//**************************************************************
int VFDSetString(int fd, int region, int offset, unsigned char *buffer)
{
#ifdef DEBUG
	printf("VFDSetString region %d offset %d buffer %s\n",region,offset,buffer);
#endif

  int i,size;
  int len = strlen(buffer) + 1;// make sure we make room for the NULL
  unsigned char stringCmd[8];

  if( region > VFD_STR_REGION_4)
    return -1;

  if( offset > 111)
    offset = 111;

  if( len > 128) 
  {
    len = 128;
    buffer[127] = 0;
  }

  //******************************************************
  // Figure out how many 8 character lines we'll be sending
  //******************************************************
  size = ((len + 7) / 8) + 1;

  //******************************************************
  //  Setup the string command packet
  //******************************************************
  memset(stringCmd, 0, 8);
  stringCmd[VFD_CLC_OFFSET] = size;
  stringCmd[VFD_FID_OFFSET] = VFD_FID_SET_STRING;
  stringCmd[VFD_SET_STRING_RN] = region;
  stringCmd[VFD_SET_STRING_SL] = len;
  stringCmd[VFD_SET_STRING_XP] = offset;
  SendReport(fd, stringCmd, sizeof(stringCmd));

  //******************************************************
  // Now send the string for display
  //******************************************************
  for(i=0; i<len;i+=8)
  {
    // make sure we only send 8 bytes at a time
    size = (len-i);
    size = (size > 8) ? 8 : size; 
    SendReport(fd, &buffer[i], size);
  }
  VFDGlobalUpdateDisplay(fd);
}

//**************************************************************
// FUNCTION: VFDClearString
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int region - region to display the string based upon the chosen layout
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will clear a string in the 
//  specified region.
//**************************************************************
int VFDClearString(int fd, int region)
{
  unsigned char pszClearStr[8];

  if( region > VFD_STR_REGION_4)
    return -1;

  memset(pszClearStr, 0, 8);
  pszClearStr[VFD_CLC_OFFSET] = 1;
  pszClearStr[VFD_FID_OFFSET] = VFD_FID_STRING_CLEAR;
  pszClearStr[VFD_STRING_CLEAR_MD] = VFD_CLEAR_STR;
  pszClearStr[VFD_STRING_CLEAR_RN] = region;
  return SendReport(fd, pszClearStr, sizeof(pszClearStr));
}

//**************************************************************
// FUNCTION: VFDSetDisplay
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int mode - VFD_MODE_NONE, VFD_MODE_STR, VFD_MODE_GRAPHICS
//  unsigned char layout - VFD_STR_LAYOUT_1, VFD_STR_LAYOUT_2, VFD_STR_LAYOUT_3
//  unsigned char time - Scrolling Time in 50 ms units
//  unsigned char flags - String Scrolling Enable Flag 
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will turn on scrolling in the specified region.
//**************************************************************
int VFDSetDisplay(int fd, int mode, unsigned char layout, unsigned char time, unsigned char flags)
{
  unsigned char packet[VFD_PACKET_SIZE(1)];
  memset(packet, 0, 8);

  packet[VFD_CLC_OFFSET]     = 1;
  packet[VFD_FID_OFFSET]     = VFD_FID_SET_DISPLAY;
  packet[VFD_SET_DISPLAY_MD] = mode;
  packet[VFD_SET_DISPLAY_DM] = layout;
  packet[VFD_SET_DISPLAY_ST] = time;
  packet[VFD_SET_DISPLAY_SF] = flags;
  return SendReport(fd, packet, sizeof(packet));
}

//**************************************************************
// FUNCTION: VFDUpdateDisplay
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int mode - VFD_MODE_NONE, VFD_MODE_STR, VFD_MODE_GRAPHICS
//  unsigned char layout - VFD_STR_LAYOUT_1, VFD_STR_LAYOUT_2, VFD_STR_LAYOUT_3
//  unsigned char time - Scrolling Time in 50 ms units
//  unsigned char flags - String Scrolling Enable Flag 
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will update the display mode/layout/scrolltime.
//**************************************************************
int VFDUpdateDisplay(int fd, int mode, unsigned char layout, unsigned char time, unsigned char flags)
{
  if(mode != gDisplayMode)
    return 0;

  return VFDSetDisplay(fd, mode, layout, time, flags);
}

//**************************************************************
// FUNCTION: VFDGlobalUpdateDisplay
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: Update the display based upon the global configuration values.
//**************************************************************
int VFDGlobalUpdateDisplay(fd)
{
  return VFDUpdateDisplay(fd, gDisplayMode, gLayout, gScrollTime, gFlags);
}

//**************************************************************
// FUNCTION: VFDSetScrollRegion
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int region - region to scroll
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will turn on scrolling in the specified region.
//**************************************************************
int VFDSetScrollRegion(int fd, int region)
{
#ifdef DEBUG
	printf("VFDSetScrollRegion region %d\n",region);
#endif
  gFlags &= ~0x0F;
  gFlags |= (region & 0x0F);
  VFDUpdateDisplay(fd, VFD_MODE_STR, gLayout, gScrollTime, gFlags | VFD_SCROLL_ENABLE);
}

//**************************************************************
// FUNCTION: VFDSetScrollTime
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int time - time in ms to scroll
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will set the rate at which to scroll
//**************************************************************
int VFDSetScrollTime(int fd, int time)
{
  gScrollTime = time / 50;
  VFDUpdateDisplay(fd, VFD_MODE_STR, gLayout, gScrollTime, gFlags | VFD_SCROLL_ENABLE);
}

//**************************************************************
// FUNCTION: VFDEnableDisplay
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  int mode - VFD_MODE_NONE, VFD_MODE_STR, VFD_MODE_GRAPHICS
//  unsigned char layout - VFD_STR_LAYOUT_1, VFD_STR_LAYOUT_2, VFD_STR_LAYOUT_3
//  unsigned char time - Scrolling Time in 50 ms units
//  unsigned char flags - String Scrolling Enable Flag 
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function specifies how the VFD is segmented.
//**************************************************************
int VFDEnableDisplay(int fd, int mode, unsigned char layout, unsigned char time, unsigned char flags)
{
  gDisplayMode = mode;
  return VFDSetDisplay(fd, mode, layout, time, flags);
}

//**************************************************************
// FUNCTION: VFDDisableDisplay
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function will disable any updates to the display
//**************************************************************
int VFDDisableDisplay(int fd)
{
  gDisplayMode = VFD_MODE_NONE; 
  return VFDSetDisplay(fd, VFD_MODE_NONE, 0, 0, 0);
}


//**************************************************************
// FUNCTION: VFDEnableString
//
// INPUT: 
//	int fd - file descriptor to the opened HID device
//  unsigned char ucLayout - specify how the VFD is segmented
//
// OUTPUT:
//  int err - result of the SendReport call (On success 0, On error -1)
//
// DESCRIPTION: This function specifies how the VFD is laid out.
//         LAYOUT 1          LAYOUT 2             LAYOUT 3
//   /----------------\  /---------------\  /--------------------\
//   |                |  |   Region 1    |  |Region 1 | Region 2 |
//   |    Region 1    |  |---------------|  |---------+----------|
//   |                |  |   Region 3    |  |Region 3 | Region 4 |
//   \----------------/  \---------------/  \--------------------/
//**************************************************************
int VFDEnableString(int fd, unsigned char ucLayout)
{
  if(ucLayout < VFD_STR_LAYOUT_1 || ucLayout > VFD_STR_LAYOUT_3)
    return -1;

  gLayout = ucLayout;
  return VFDEnableDisplay(fd, VFD_MODE_STR, gLayout, gScrollTime, gFlags | VFD_SCROLL_ENABLE);
}


//**************************************************************
// FUNCTION: _set_pixel
//
// INPUT: 
//	int page - page to draw on(VFD_GR_PAGE_1 ... VFD_GR_PAGE_4)
//  int x - coordinate
//  int y - coordinate
//  int color - 0 or 1 - turn on/off the LED
//
// OUTPUT:
//  Nothing
//
// DESCRIPTION: This function turns on/off pixels in the graphic page.
//**************************************************************
static void _set_pixel(int page, int x, int y, int color)
{
  unsigned char dst = ((y/8) * 112) + x;
  unsigned char mask = (1 << (7 - (y%8)));

  // Turn the LED on/off based upon the color setting
  if(color) 
    gPages[page][dst] |= mask;
  else
    gPages[page][dst] &= ~mask;
}


//**************************************************************
// FUNCTION: VFDGraphicsClearBuffer
//
// INPUT: 
//	int page - page to clear (VFD_GR_PAGE_1 ... VFD_GR_PAGE_4)
//
// OUTPUT:
//  0 on success, -1 on error
//
// DESCRIPTION: This function clears the specified graphic page.
//**************************************************************
int VFDGraphicsClearBuffer(int page)
{
  if(page >= VFD_PAGE_COUNT)
    return -1;

  // Clear the page to all off
  memset(gPages[page], 0, VFD_PAGE_SIZE);
  return 0;
}

//**************************************************************
// FUNCTION: VFDGraphicsCopyPage
//
// INPUT: 
//  int page - Graphic page (dst) (VFD_GR_PAGE_1 ... VFD_GR_PAGE_4)
//  unsigned char *buffer - buffer worth of data (src)
//
// OUTPUT:
//  Nothing
//
// DESCRIPTION: This function turns on/off pixels in the graphic page.
//**************************************************************
int VFDGraphicsCopyPage(int page, unsigned char *buffer)
{
  int x, y;
  int shift;

  if(page >= VFD_PAGE_COUNT)
    return -1;

  for(y=0; y < VFD_HEIGHT; y++)
  {
    for(x=0; x < VFD_WIDTH; x++)
    {
      unsigned char src = (y*14)+(x/8);
      shift = 7 - (x%8);
      if(shift == 0)
        shift = 1;
      else
        shift = 1 << shift;
      _set_pixel(page, x, y, buffer[src] & shift);
    }
  }
  return 0;
}

//**************************************************************
// FUNCTION: VFDSetGraphics
//
// INPUT: 
//  int fd - file descriptor to the opened HID device
//  unsigned char region - Graphic Page index
//  unsigned char *buf - data to display
//
// OUTPUT:
//
// DESCRIPTION: 
//**************************************************************
int VFDSetGraphics(int fd, unsigned char region, unsigned char *buf)
{
  int i, size;
  unsigned char packet[8];

  // Send the Command to Set Graphics
  memset(packet, 0, 8);

  packet[VFD_CLC_OFFSET] = 29;
  packet[VFD_FID_OFFSET] = VFD_FID_SET_GRAPHICS;
  packet[VFD_SET_GRAPHICS_GP]=region;
  SendReport(fd, packet, sizeof(packet));

  // Send the actual graphics
  for(i=0; i<VFD_PAGE_SIZE;i+=8)
  {
    // make sure we only send 8 bytes at a time
    size = (VFD_PAGE_SIZE - i);
    size = (size > 8) ? 8 : size; 
    SendReport(fd, &buf[i], size);
  }
}

//**************************************************************
// FUNCTION: VFDGraphicsSendPage
//
// INPUT: 
//  int fd - file descriptor to the opened HID device
//  int page - page to display (VFD_GR_PAGE_1 ... VFD_GR_PAGE_4)
//
// OUTPUT:
//  -1 on error, or results of VFDSetGraphics
//
// DESCRIPTION:  Sends the graphics page to the VFD to be displayed.
//**************************************************************
int VFDGraphicsSendPage(int fd, int page)
{
  if(page >= VFD_PAGE_COUNT)
    return -1;
  return VFDSetGraphics(fd, page+1,gPages[page]);
}

//**************************************************************
// FUNCTION: VFDGraphicsShowPage
//
// INPUT: 
//  int fd - file descriptor to the opened HID device
//  int page - page to get ready to show (VFD_GR_PAGE_1 ... VFD_GR_PAGE_4)
//
// OUTPUT:
//  -1 on error
//
// DESCRIPTION: Sets the VFD into Graphic mode
//**************************************************************
int VFDGraphicsShowPage(int fd, int page)
{
  if(page >= VFD_PAGE_COUNT)
    return -1;
  return VFDEnableDisplay(fd, VFD_MODE_GRAPHICS, page+1, 0, 0);
}

//**************************************************************
// FUNCTION: VFDGraphicsRect
//
// INPUT: 
//  int page - page to get ready to show (VFD_GR_PAGE_1 ... VFD_GR_PAGE_4)
//  unsigned char color - 0 or 1
//  int srcx - starting X Coordinate
//  int srcy - starting Y Coordinate
//  int width - how wide to make the box
//  int height - how tall to make the box
//
// OUTPUT:
//  -1 on error
//
// DESCRIPTION: Draws a box at (srcx,srcy) - (srcx+width, srcy+height)
//**************************************************************
int VFDGraphicsRect(int page, unsigned char color, int srcx, int srcy, int width, int height)
{
  unsigned char *b;
  int h, w, x, y;

  if( page >= VFD_PAGE_COUNT)
    return -1;

  if (srcx > VFD_WIDTH || srcy > VFD_HEIGHT)
    return 0;

  h = (srcy + height > VFD_HEIGHT) ? VFD_HEIGHT - srcy : srcy + height;
  w = (srcx + width > VFD_WIDTH) ? VFD_WIDTH - srcx : srcx + width;
  b = gPages[page];

  for( y = srcy; y < h; y++ )
    for( x = srcx; x < w; x++)
      _set_pixel(page, x, y, color);

  return 0;
}

//**************************************************************
// FUNCTION: main
//
// INPUT: 
//	int argc
//  int argv
//
// OUTPUT:
//  int 
//
// DESCRIPTION: Main processing function.
//**************************************************************
int xmain(int argc, char **argv) 
{
  int fd=-1;
  int err,i;
  unsigned char buffer[64];
  const char *pszVendor = "040b"; // hex number
  const char *pszProduct = "7001"; // hex number

  // initialize the global icon mask
  gIconMask = 0;

  //******************************************************
  // Open the HID Device, exit if not found.
  //******************************************************
  if((fd = OpenHID(pszVendor, pszProduct))< 0)
  {
    printf("Device for Vendor[%s] Product[%s] was not found, exiting\n", pszVendor, pszProduct);
    exit(0);
  }

  //******************************************************
  // Initialize the internal report structures
  //******************************************************
  if(ioctl(fd, HIDIOCINITREPORT,0)<0) 
    exit(1);

  //******************************************************
  // Find out what type of reports this device accepts
  //******************************************************
  //FindReports(fd);

  //******************************************************
  // Set up the display to show graphics
  //******************************************************
  VFDTurnOffIcons(fd);
  VFDSetDisplay(fd, VFD_MODE_NONE, 0, 0, 0);
  VFDGraphicsClearBuffer(VFD_GR_PAGE_3);

  //******************************************************
  // This draws a nice little box to prove graphics works
  //******************************************************
  //VFDGraphicsRect(VFD_GR_PAGE_3, 1, 1, 0, 2, 2);

  //******************************************************
  // Draw the AMD Logo to show how to do graphics
  //******************************************************
  VFDGraphicsCopyPage(VFD_GR_PAGE_3, amd_logo);
  VFDGraphicsSendPage(fd, VFD_GR_PAGE_3);
  VFDGraphicsShowPage(fd, VFD_GR_PAGE_3);
  getchar();

  //******************************************************
  // Set up the display, scrolling region, scroll rate etc.
  //******************************************************
  VFDDisableDisplay(fd);
  VFDClearString(fd, VFD_STR_REGION_1);
  VFDClearString(fd, VFD_STR_REGION_3);
  VFDEnableString(fd, VFD_STR_LAYOUT_2);
  VFDSetScrollRegion(fd, VFD_SCROLL_REGION3);
  VFDSetScrollTime(fd, 500);

  //******************************************************
  // Tell the user what to do
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_3, 0, "Press Any Key To Turn On Next Icon");

  //******************************************************
  // Show them the DVD Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "DVD");
  VFDIconOn(fd, VFD_ICON_DVD);
  getchar();
  VFDIconOff(fd, VFD_ICON_DVD);

  //******************************************************
  // Show them the Video Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Video");
  VFDIconOn(fd, VFD_ICON_VIDEO);
  getchar();
  VFDIconOff(fd, VFD_ICON_VIDEO);

  //******************************************************
  // Show them the CD Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "CD");
  VFDIconOn(fd, VFD_ICON_CD);
  getchar();
  VFDIconOff(fd, VFD_ICON_CD);

  //******************************************************
  // Show them the Play Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Play");
  VFDIconOn(fd, VFD_ICON_PLAY);
  getchar();
  VFDIconOff(fd, VFD_ICON_PLAY);

  //******************************************************
  // Show them the Rewind Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Rewind");
  VFDIconOn(fd, VFD_ICON_RWND);
  getchar();
  VFDIconOff(fd, VFD_ICON_RWND);

  //******************************************************
  // Show them the Pause Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Pause");
  VFDIconOn(fd, VFD_ICON_PAUSE);
  getchar();
  VFDIconOff(fd, VFD_ICON_PAUSE);

  //******************************************************
  // Show them the Fast Forward Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Fast Forward");
  VFDIconOn(fd, VFD_ICON_FFWD);
  getchar();
  VFDIconOff(fd, VFD_ICON_FFWD);

  //******************************************************
  // Show them the Speaker Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Speaker/Mute");
  VFDIconOn(fd, VFD_ICON_SPEAKER);
  getchar();
  VFDIconOff(fd, VFD_ICON_SPEAKER);

  //******************************************************
  // Show them the Record Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Record");
  VFDIconOn(fd, VFD_ICON_REC);
  getchar();
  VFDIconOff(fd, VFD_ICON_REC);

  //******************************************************
  // Show them the Radio Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Radio");
  VFDIconOn(fd, VFD_ICON_RADIO);
  getchar();
  VFDIconOff(fd, VFD_ICON_RADIO);

  //******************************************************
  // Show them the Volume Icon
  //******************************************************
  VFDSetString(fd, VFD_STR_REGION_1, 0, "Volume");
  VFDIconOn(fd, VFD_ICON_VOLUME);
  getchar();

  //******************************************************
  // Stop scrolling the "Press any key to continue" message
  //******************************************************
  VFDClearString(fd, VFD_STR_REGION_3);

  //******************************************************
  // Show them how to adjust the volume levels
  //******************************************************
  for(i=0;i<13;i++)
  {
    sprintf(buffer,"Raising Volume %02d",i);
    VFDSetString(fd, VFD_STR_REGION_1, 0, buffer);
    VFDSetVolume(fd,i);
    sleep(1);
  }

  for(i=12;i>-1;i--)
  {
    sprintf(buffer,"Lowering Volume %02d",i);
    VFDSetString(fd, VFD_STR_REGION_1, 0, buffer);
    VFDSetVolume(fd,i);
    sleep(1);
  }

  //******************************************************
  // Tell them the demo is over, it's time to quit
  //******************************************************
  VFDTurnOffIcons(fd);

  VFDSetString(fd, VFD_STR_REGION_1, 0, "All Done - goodbye");
  VFDSetString(fd, VFD_STR_REGION_3, 0, "Press Any Key To Exit");
  getchar();

  //******************************************************
  // All done, turn everything off
  //******************************************************
  VFDClearString(fd, VFD_STR_REGION_1);
  VFDClearString(fd, VFD_STR_REGION_3);
  close(fd);

  return 0;
}

int StartLCD()
{
	int VfdHandle=-1;
	int err,i;
	unsigned char buffer[64];
	const char *pszVendor = "040b"; // hex number
	const char *pszProduct = "7001"; // hex number

	// initialize the global icon mask
	gIconMask = 0;

	//******************************************************
	// Open the HID Device, exit if not found.
	//******************************************************
	if((VfdHandle = OpenHID(pszVendor, pszProduct))< 0)
	{
		printf("Device for Vendor[%s] Product[%s] was not found, exiting", pszVendor, pszProduct);
		return 0;
	}

	//******************************************************
	// Initialize the internal report structures
	//******************************************************
	if(ioctl(VfdHandle, HIDIOCINITREPORT,0)<0) 
	{
		printf("Media_Live_LCDButtons::StartLCD ioctl failed");
		return 0;
	}

	//******************************************************
	// Find out what type of reports this device accepts
	//******************************************************
	//FindReports(VfdHandle);

	//******************************************************
	// Set up the display to show graphics
	//******************************************************
	VFDTurnOffIcons(VfdHandle);
	VFDSetDisplay(VfdHandle, VFD_MODE_NONE, 0, 0, 0);
	VFDGraphicsClearBuffer(VFD_GR_PAGE_3);

	//******************************************************
	// Set up the display, scrolling region, scroll rate etc.
	//******************************************************
	VFDDisableDisplay(VfdHandle);
	VFDClearString(VfdHandle, VFD_STR_REGION_1);
	VFDClearString(VfdHandle, VFD_STR_REGION_3);
	VFDEnableString(VfdHandle, VFD_STR_LAYOUT_2);
	VFDSetScrollRegion(VfdHandle, 0);
	VFDSetScrollTime(VfdHandle, 500);

	return VfdHandle;
}
