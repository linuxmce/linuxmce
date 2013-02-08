/* Copyright 2000 Enhanced Software Technologies Inc.
 *   Released under terms of the GNU General Public License as
 * required by the license on 'mtxl.c'.
 */

/* 
* $Date: 2001/06/05 17:10:21 $
* $Revision: 1.1.1.1 $
*/

/* What this does: Basically dumps out contents of:
 *  Mode Sense: Element Address Assignment Page (0x1d)
 *           1Eh (Transport Geometry Parameters) has a bit which indicates is 
 *               a robot is  capable of rotating the media. It`s the 
 *                `Rotate` bit, byte 2, bit 1.
 *          Device Capabilities page (0x1f)
 * Inquiry -- prints full inquiry info. 
 *   DeviceType:
 *    Manufacturer:
 *    ProdID:  
 *    ProdRevision:
 *   If there is a byte 55, we use the Exabyte extension to 
 * print out whether we have a bar code reader or not.  This is
 * bit 0 of byte 55. 
 *
 * Next, we request element status on the drives. We do not
 * request volume tags though. If Exabyte
 * extensions are supported, we report the following information for
 * each drive:
 *
 *  Drive number
 *  EXCEPT (with ASC and ASCQ), if there is a problem. 
 *  SCSI address and LUN
 *  Tape drive Serial number
 *   
 */

#include <stdio.h>
#include "mtx.h"
#include "mtxl.h"

DEVICE_TYPE MediumChangerFD;  /* historic purposes... */

char *argv0;

/* A table for printing out the peripheral device type as ASCII. */ 
static char *PeripheralDeviceType[32] = {
  "Disk Drive",
  "Tape Drive",
  "Printer",
  "Processor",
  "Write-once",
  "CD-ROM",
  "Scanner",
  "Optical",
  "Medium Changer",
  "Communications",
  "ASC IT8",
  "ASC IT8",
  "RAID Array",
  "Enclosure Services",
  "OCR/W",
  "Bridging Expander", /* 0x10 */
  "Reserved",  /* 0x11 */
  "Reserved", /* 0x12 */
  "Reserved",  /* 0x13 */
  "Reserved",  /* 0x14 */
  "Reserved",  /* 0x15 */
  "Reserved",  /* 0x16 */
  "Reserved",  /* 0x17 */
  "Reserved",  /* 0x18 */
  "Reserved",  /* 0x19 */
  "Reserved",  /* 0x1a */
  "Reserved",  /* 0x1b */
  "Reserved",  /* 0x1c */
  "Reserved",  /* 0x1d */
  "Reserved",  /* 0x1e */
  "Unknown"    /* 0x1f */
};


/* okay, now for the structure of an Element Address Assignment Page:  */

typedef struct EAAP {
  unsigned char Page_Code;
  unsigned char Parameter_Length;
  unsigned char MediumTransportElementAddress[2];
  unsigned char NumMediumTransportElements[2];
  unsigned char FirstStorageElementAdddress[2];
  unsigned char NumStorageElements[2];
  unsigned char FirstImportExportElementAddress[2];
  unsigned char NumImportExportElements[2];
  unsigned char FirstDataTransferElementAddress[2];
  unsigned char NumDataTransferElements[2];
  unsigned char Reserved[2];
} EAAP_Type;

/* okay, now for the structure of a transport geometry
 * descriptor page:
 */
typedef struct TGDP {
  unsigned char Page_Code;
  unsigned char ParameterLength;
  unsigned char Rotate;
  unsigned char ElementNumber;  /* we don't care about this... */
} TGDP_Type;


/* Structure of the Device Capabilities Page: */
typedef struct DCP {
  unsigned char Page_Code;
  unsigned char ParameterLength;
  unsigned char CanStore;  /* bits about whether elements can store carts */
  unsigned char Reserved; 
  unsigned char MT_Xfer;  /* bits about whether mt->xx xfers work. */
  unsigned char ST_Xfer;  /* bits about whether st->xx xfers work. */
  unsigned char IE_Xfer;  /* bits about whether id->xx xfers work. */
  unsigned char DT_Xfer; /* bits about whether DT->xx xfers work. */
  unsigned char Reserved2[12];  /* more reserved data */
} DCP_Type ; 

#define MT_BIT 0x01
#define ST_BIT 0x02
#define IE_BIT 0x04
#define DT_BIT 0x08

/* Okay, now for the inquiry information: */

static void ReportInquiry(DEVICE_TYPE MediumChangerFD)
{
  RequestSense_T RequestSense;
  Inquiry_T *Inquiry;
  int i;

  Inquiry = RequestInquiry(MediumChangerFD,&RequestSense);
  if (Inquiry == NULL) 
    {
      PrintRequestSense(&RequestSense);
      FatalError("INQUIRY Command Failed\n");
    }
  
  printf("Product Type: %s\n",PeripheralDeviceType[Inquiry->PeripheralDeviceType]);
  printf("Vendor ID: '");
  for (i = 0; i < sizeof(Inquiry->VendorIdentification); i++)
    printf("%c", Inquiry->VendorIdentification[i]);
  printf("'\nProduct ID: '");
  for (i = 0; i < sizeof(Inquiry->ProductIdentification); i++)
    printf("%c", Inquiry->ProductIdentification[i]);
  printf("'\nRevision: '");
  for (i = 0; i < sizeof(Inquiry->ProductRevisionLevel); i++)
    printf("%c", Inquiry->ProductRevisionLevel[i]);
  printf("'\n");\
  if (Inquiry->MChngr) {  /* check the attached-media-changer bit... */
    printf("Attached Changer: Yes\n");
  } else {
    printf("Attached Changer: No\n");
  }
  /* Now see if we have a bar code flag: */
  if (Inquiry->AdditionalLength > 50) {  /* see if we have 56 bytes: */
    if (Inquiry->VendorFlags & 1) {
      printf("Bar Code Reader: Yes\n");
    } else {
      printf("Bar Code Reader: No\n");
    }
  }
  
  free(Inquiry);  /* well, we're about to exit, but ... */
  return; /* done! */
}

/*********** MODE SENSE *******************/
/* We need 3 different mode sense pages. This is a generic
 * routine for obtaining mode sense pages. 
 */

static unsigned char
*mode_sense(DEVICE_TYPE fd, int pagenum, int alloc_len,  RequestSense_T *RequestSense) {
  CDB_T CDB;
  unsigned char *input_buffer; /*the input buffer -- has junk prepended to
				* actual sense page. 
				*/
  unsigned char *tmp;
  unsigned char *retval;  /* the return value. */
  int i,pagelen;

  if (alloc_len > 255) {
    FatalError("mode_sense(6) can only read up to 255 characters!\n");
  }

  input_buffer=(unsigned char *)xzmalloc(256); /* overdo it, eh? */

  /* clear the sense buffer: */
  slow_bzero((char *)RequestSense,sizeof(RequestSense_T));
  

  /* returns an array of bytes in the page, or, if not possible, NULL. */
  CDB[0]=0x1a; /* Mode Sense(6) */
  CDB[1]=0; 
  CDB[2]=pagenum; /* the page to read. */
  CDB[3]=0;
  CDB[4]=255; /* allocation length. This does max of 256 bytes! */
  CDB[5]=0;
  
  if (SCSI_ExecuteCommand(fd,Input,&CDB,6,
			  input_buffer,255,RequestSense) != 0) {
#ifdef DEBUG_MODE_SENSE
    fprintf(stderr,"Could not execute mode sense...\n");
    fflush(stderr);
#endif
    return NULL; /* sorry, couldn't do it. */
  }
  
  /* First skip past any header.... */
  tmp=input_buffer+4+input_buffer[3];
  /* now find out real length of page... */
  pagelen=tmp[1]+2;
  retval=xmalloc(pagelen);
  /* and copy our data to the new page. */
  for (i=0;i<pagelen;i++) {
    retval[i]=tmp[i];
  }
  /* okay, free our input buffer: */
  free(input_buffer);
  return retval;
}

/* Report the Element Address Assignment Page */
static EAAP_Type *ReportEAAP(DEVICE_TYPE MediumChangerFD) {
  EAAP_Type *EAAP; 
  RequestSense_T RequestSense;

  EAAP=(EAAP_Type *)mode_sense(MediumChangerFD,0x1d,sizeof(EAAP_Type),&RequestSense);
  
  if (EAAP==NULL) {
    printf("EAAP: No\n");
    return NULL;
  }

  /* we did get an EAAP, so do our thing: */
  printf("EAAP: Yes\n");
  printf("Number of Medium Transport Elements: %d\n", ( ((unsigned int)EAAP->NumMediumTransportElements[0]<<8) + (unsigned int)EAAP->NumMediumTransportElements[1]));
  printf("Number of Storage Elements: %d\n", ( ((unsigned int)EAAP->NumStorageElements[0]<<8) + (unsigned int)EAAP->NumStorageElements[1]));
  printf("Number of Import/Export Element Elements: %d\n", ( ((unsigned int)EAAP->NumImportExportElements[0]<<8) + (unsigned int)EAAP->NumImportExportElements[1]));
  printf("Number of Data Transfer Elements: %d\n", ( ((unsigned int)EAAP->NumDataTransferElements[0]<<8) + (unsigned int)EAAP->NumDataTransferElements[1]));
  
  return EAAP;
}

/* See if we can get some invert information: */

static void Report_TGDP(DEVICE_TYPE MediumChangerFD) {
  TGDP_Type *result;
  
  RequestSense_T RequestSense;

  result=(TGDP_Type *)mode_sense(MediumChangerFD,0x1e,255,&RequestSense);
  
  if (!result) {
    printf("Transport Geometry Descriptor Page: No\n");
    return;
  }

  printf("Transport Geometry Descriptor Page: Yes\n");
  
  /* Now print out the invert bit: */
  if ( result->Rotate & 1 ) {
    printf("Invertable: Yes\n");
  } else {
    printf("Invertable: No\n");
  }

  return;  /* done. */
}

/* Okay, let's get the Device Capabilities Page. We don't care
 * about much here, just whether 'mtx transfer' will work (i.e., 
 * ST->ST).
 */

static void Report_DCP(DEVICE_TYPE MediumChangerFD) {
  DCP_Type *result;
  RequestSense_T RequestSense;

  /* Get the page. */
  result=(DCP_Type *)mode_sense(MediumChangerFD,0x1f,sizeof(DCP_Type),&RequestSense);
  if (!result) {
    printf("Device Configuration Page: No\n");
    return;
  }
  
  printf("Device Configuration Page: Yes\n");

  /* okay, now see if we can do xfers: */
  if (result->ST_Xfer & ST_BIT) {
    printf("Can Transfer: Yes\n");
  } else {
    printf("Can Transfer: No\n");
  }
  /* We don't care about anything else at the moment, eventually we
   * do want to add the inport/export stuff here too...
   */

  return;
}

void usage(void) {
  FatalError("Usage: loaderinfo -f <generic-device>\n");
}


/* we only have one argument: "-f <device>". */
int main(int argc, char **argv) {
  DEVICE_TYPE fd;
  char *filename;

  argv0=argv[0];
  if (argc != 3) {
    fprintf(stderr,"argc=%d",argc);
    usage();
  }

  if (strcmp(argv[1],"-f")!=0) {
    usage();
  }
  filename=argv[2];
  
  fd=SCSI_OpenDevice(filename);
  
  /* Now to call the various routines: */
  ReportInquiry(fd);
  ReportEAAP(fd);
  Report_TGDP(fd);
  Report_DCP(fd);
  exit(0);
}
