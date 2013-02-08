/*

  MTX -- SCSI Tape Attached Medium Changer Control Program
  $Date: 2007/01/29 03:22:45 $
  $Revision: 1.12 $

  Copyright 1997-1998 by Leonard N. Zubkoff <lnz@dandelion.com>
  Now maintained by Eric Lee Green <eric@badtux.org>

  This program is free software; you may redistribute and/or modify it under
  the terms of the GNU General Public License Version 2 as published by the
  Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for complete details.

  The author respectfully requests that any modifications to this software be
  sent directly to him for evaluation and testing.

  Thanks to Philip A. Prindeville <philipp@enteka.com> of Enteka Enterprise
  Technology Service for porting MTX to Solaris/SPARC.

  Thanks to Carsten Koch <Carsten.Koch@icem.de> for porting MTX to SGI IRIX.

  Thanks to TECSys Development, Inc. for porting MTX to Digital Unix and
  OpenVMS.

  Near complete re-write Feb 2000 Eric Lee Green <eric@badtux.org> to add support for
  multi-drive tape changers, extract out library stuff into mtxl.c,
  and otherwise bring things up to date for dealing with LARGE tape jukeboxes
  and other such enterprise-class storage subsystems. 

*/

char *argv0;

#include "mtx.h"   /* various defines for bit order etc. */
#include "mtxl.h"

/* A table for printing out the peripheral device type as ASCII. */ 
static char *PeripheralDeviceType[32] = {
  "Disk Drive",  /* 0 */
  "Tape Drive",   /* 1 */
  "Printer",   /* 2 */
  "Processor",   /* 3 */
  "Write-once",   /* 4 */
  "CD-ROM",       /* 5 */
  "Scanner",      /* 6 */
  "Optical",      /* 7 */ 
  "Medium Changer",  /* 8 */
  "Communications",  /* 9 */
  "ASC IT8",           /* a */ 
  "ASC IT8",            /* b */
  "RAID Array",         /* c */
  "Enclosure Services",  /* d */
  "RBC Simplified Disk", /* e */
  "OCR/W",           /* f */
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
  
static int argc;
static char **argv;




static char *device=NULL; /* the device name passed as argument */

/* Unfortunately this must be true for SGI, because SGI does not
   use an int :-(. 
*/

static DEVICE_TYPE MediumChangerFD = (DEVICE_TYPE) -1;
static int device_opened = 0;  /* okay, replace check here. */

/* was: static int MediumChangerFD=-1; *//* open filehandle to that device */
static int arg1=-1;       /* first arg to command */
static int arg2=-1;       /* second arg to command */
static int arg3=-1;       /* third arg to command, if exchange. */

static SCSI_Flags_T SCSI_Flags = { 0, 0, 0,0,0,0,0,0,0,0 };
  
static Inquiry_T *inquiry_info;  /* needed by MoveMedium etc... */
static ElementStatus_T *ElementStatus = NULL;
void Position(int dest);


/* pre-defined commands: */
static void ReportInquiry(void);
static void Status(void);
static void Load(void);
static void Unload(void);
static void First(void);
static void Last(void);
static void Next(void);
/* static void Previous(void); */
static void InvertCommand(void);
static void Transfer(void);
static void Eepos(void);
static void NoAttach(void);
static void Version(void);
static void do_Inventory(void); 
static void do_Unload(void);
static void do_Erase(void);
static void NoBarCode(void);
static void do_Position(void);
static void Invert2(void);
static void Exchange(void);
static void AltReadElementStatus(void);

struct command_table_struct {
  int num_args;
  char *name;
  void (*command)(void);
  int need_device;
  int need_status;
} command_table[] = {
  { 0, "inquiry",ReportInquiry, 1,0},
  { 0, "status", Status, 1,1 },
  { 0, "invert", InvertCommand, 0,0},
  { 0, "noattach",NoAttach,0,0},
  { 1, "eepos", Eepos, 0,0},
  { 2, "load", Load, 1,1 },
  { 2, "unload", Unload, 1,1 },
  { 2, "transfer", Transfer, 1,1 },
  { 1, "first", First, 1,1 },
  { 1, "last", Last, 1,1 },
  { 1, "next", Next, 1,1 },
  { 0, "--version", Version, 0,0 },
  { 0, "inventory", do_Inventory, 1,0},
  { 0, "eject", do_Unload, 1, 0},
  { 0, "erase", do_Erase, 1, 0},
  { 0, "nobarcode", NoBarCode, 0,0},
  { 1, "position", do_Position, 1, 1},
  { 0, "invert2", Invert2, 0, 0}, 
  { 3, "exchange", Exchange, 1, 1 },
  { 0, "altres", AltReadElementStatus, 0,0},
  { 0, NULL, NULL }
};

static void Usage()
{
  fprintf(stderr, "Usage:\n\
  mtx --version\n\
  mtx [ -f <loader-dev> ] noattach <more commands>\n\
  mtx [ -f <loader-dev> ] inquiry | inventory \n\
  mtx [ -f <loader-dev> ] [altres] [nobarcode] status\n\
  mtx [ -f <loader-dev> ] [altres] [nobarcode] drivestatus [<drive#>]\n\
  mtx [ -f <loader-dev> ] [altres] first [<drive#>]\n\
  mtx [ -f <loader-dev> ] [altres] last [<drive#>]\n\
  mtx [ -f <loader-dev> ] [altres] next [<drive#>]\n\
  mtx [ -f <loader-dev> ] [altres] previous [<drive#>]\n\
  mtx [ -f <loader-dev> ] [altres] [invert] load <storage-element-number> [<drive#>]\n\
  mtx [ -f <loader-dev> ] [altres] [invert] unload [<storage-element-number>][<drive#>]\n\
  mtx [ -f <loader-dev> ] [altres] [eepos eepos-number] transfer <storage-element-number> <storage-element-number>\n\
  mtx [ -f <loader-dev> ] [altres] [eepos eepos-number][invert][invert2] exchange <storage-element-number> <storage-element-number>\n\
  mtx [ -f <loader-dev> ] [altres] position <storage-element-number>\n\
  mtx [ -f <loader-dev> ] eject\n");


#ifndef VMS
  exit(1);
#else
  sys$exit(VMS_ExitCode);
#endif
}



static void Version(void) {
  fprintf(stderr,"mtx version %s\n\n",VERSION);
  Usage(); 
}

static void NoAttach(void) {
  SCSI_Flags.no_attached=1;
}

static void InvertCommand(void) {
  SCSI_Flags.invert=1;
  /* invert_bit=1;*/
}

static void Invert2(void) {
  SCSI_Flags.invert2=1;
  /* invert2_bit=1;*/
}


static void NoBarCode(void) {
  SCSI_Flags.no_barcodes=1;  /* don't request barcodes, sigh! */
} 
static void do_Position(void) {
	int driveno,src;
	if(arg1 >= 0 && arg1 <= ElementStatus->StorageElementCount) {
		driveno = arg1-1;
	} else {
		driveno = 0;
	}
	src = ElementStatus->StorageElementAddress[driveno];
	Position(src);
}

static void AltReadElementStatus(void) {
  SCSI_Flags.querytype=MTX_ELEMENTSTATUS_READALL;  /* use alternative way to read element status from device - used to support XL1B2 */
}

/* First and Last are easy. Next is the bitch. */
static void First(void){
  int driveno;
  /* okay, first see if we have a drive#: */
  if (arg1 >= 0 && arg1 < ElementStatus->DataTransferElementCount) {
    driveno=arg1;
  } else {
    driveno = 0;
  }
  /* now see if there's anything *IN* that drive: */
  if (ElementStatus->DataTransferElementFull[driveno]) {
    /* if so, then unload it... */
    arg1=ElementStatus->DataTransferElementSourceStorageElementNumber[driveno]+1;
    if (arg1==1) {
      printf("loading...done.\n");  /* it already has tape #1 in it! */ 
      return;
    }
    arg2=driveno;
    Unload();
  }
  /* and now to actually do the Load(): */
  arg2=driveno;
  arg1=1; /* first! */
  Load(); /* and voila! */
  
}

static void Last(void) {
  int driveno;
  /* okay, first see if we have a drive#: */
  if (arg1 >= 0 && arg1 < ElementStatus->DataTransferElementCount) {
    driveno=arg1;
  } else {
    driveno = 0;
  }

  /* now see if there's anything *IN* that drive: */
  if (ElementStatus->DataTransferElementFull[driveno]) {
    /* if so, then unload it... */
    arg1=ElementStatus->DataTransferElementSourceStorageElementNumber[driveno]+1;
    if (arg1==ElementStatus->StorageElementCount) {
      printf("loading...done.\n");  /* it already has last tape in it! */ 
      return;
    }
    arg2=driveno;
    Unload();
  }

  arg1 = ElementStatus->StorageElementCount; /* the last slot... */
  arg2=driveno;
  Load(); /* and load it, sigh! */

}


static void Next(void) {
  int driveno;
  int current=0; 
  /* okay, first see if we have a drive#: */
  if (arg1 >= 0 && arg1 < ElementStatus->DataTransferElementCount) {
    driveno=arg1;
  } else {
    driveno = 0;
  }

  /* Now to see if there's anything in that drive! */
  if (ElementStatus->DataTransferElementFull[driveno]) {
    /* if so, unload it! */
    arg1=ElementStatus->DataTransferElementSourceStorageElementNumber[driveno]+1;
    current=arg1;
    arg2=driveno;
    Unload();
  }
  
  /* okay, now to load, if we can... */
  /* Thanks, Chris McCrory! */
  while ( ! ElementStatus->StorageElementFull[current++]) { /*cycle til found*/
    if ( current > ElementStatus->StorageElementCount ) {/*last slot...*/
      FatalError("No More Tapes\n");
    }
  }
  arg1=current;
  arg2=driveno;
  Load();
}

static void do_Inventory(void) 
{
  int i;
  i=Inventory(MediumChangerFD);
  if (i < 0) {
    fprintf(stderr,"mtx:inventory failed\n");
    fflush(stderr);
    exit(1); /*  exit with an error status. */
  }
  return;  /* if it failed, well, sigh.... */
}

/* For Linux, this allows us to do a short erase on a tape (sigh!).
 * Note that you'll need to do a 'mt status' on the tape afterwards in
 * order to get the tape driver in sync with the tape drive again. Also
 * note that on other OS's, this might do other evil things to the tape
 * driver. Note that to do an erase, you must first rewind using the OS's
 * native tools!
 */
static void do_Erase(void) {
  RequestSense_T *RequestSense;
  RequestSense=Erase(MediumChangerFD);
  if (RequestSense) {
    PrintRequestSense(RequestSense);
    exit(1);  /* exit with an error status. */
  }
  return;
}
  

/* This should eject a tape or magazine, depending upon the device sent
 * to.
 */
static void do_Unload(void)
{
  int i;
  i=Eject(MediumChangerFD);
  if (i<0) {
    fprintf(stderr,"mtx:eject failed\n");
    fflush(stderr);
  }
  return;  /* if it failed, well, sigh.... */
}

static void ReportInquiry(void)
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
    printf("Attached Changer API: Yes\n");
  } else {
    printf("Attached Changer API: No\n");
  }
  free(Inquiry);  /* well, we're about to exit, but ... */
}

static void Status(void)
{
  int StorageElementNumber;
  int TransferElementNumber;
  printf("  Storage Changer %s:%d Drives, %d Slots ( %d Import/Export )\n",
	 device,
	 ElementStatus->DataTransferElementCount,
	 ElementStatus->StorageElementCount,
	 ElementStatus->ImportExportCount);


  for (TransferElementNumber=0;TransferElementNumber <
	 ElementStatus->DataTransferElementCount;TransferElementNumber++) {
    
    printf("Data Transfer Element %d:",TransferElementNumber);
    if (ElementStatus->DataTransferElementFull[TransferElementNumber])
      {
	if (ElementStatus->DataTransferElementSourceStorageElementNumber[TransferElementNumber] > -1) 
	  printf("Full (Storage Element %d Loaded)",
		 ElementStatus->DataTransferElementSourceStorageElementNumber[TransferElementNumber]+1);
	else printf("Full (Unknown Storage Element Loaded)");
	if
	  (ElementStatus->DataTransferPrimaryVolumeTag[TransferElementNumber][0])
	  printf(":VolumeTag = %s",ElementStatus->DataTransferPrimaryVolumeTag[TransferElementNumber]);
	if
	  (ElementStatus->DataTransferAlternateVolumeTag[TransferElementNumber][0])
	  
	  printf(":AlternateVolumeTag = %s",ElementStatus->DataTransferAlternateVolumeTag[TransferElementNumber]); 
	putchar('\n');
      }
    else printf("Empty\n");
  }

  for (StorageElementNumber = 0;
       StorageElementNumber < ElementStatus->StorageElementCount;
       StorageElementNumber++) {
    printf("      Storage Element %d%s:%s", StorageElementNumber+1, 
	   (ElementStatus->StorageElementIsImportExport[StorageElementNumber]) ? " IMPORT/EXPORT" : "",
	   (ElementStatus->StorageElementFull[StorageElementNumber] ? "Full " : "Empty"));
    if (ElementStatus->PrimaryVolumeTag[StorageElementNumber][0]) {
      printf(":VolumeTag=%s",ElementStatus->PrimaryVolumeTag[StorageElementNumber]);
    }
    if (ElementStatus->AlternateVolumeTag[StorageElementNumber][0]) {
            printf(":AlternateVolumeTag=%s",ElementStatus->AlternateVolumeTag[StorageElementNumber]);
    }
    putchar('\n');
  }
  
#ifdef VMS
  VMS_DefineStatusSymbols();
#endif
}

void Position(int dest) {
	RequestSense_T *result;
	result = PositionElement(MediumChangerFD,dest,ElementStatus);
	if(result) /* We have an error */
	{
		FatalError("Could not position transport\n");
	}
}

void Move(int src, int dest) {
  RequestSense_T *result; /* from MoveMedium */
  
  result=MoveMedium(MediumChangerFD,src,dest,ElementStatus,inquiry_info,&SCSI_Flags);
  if (result)   /* we have an error! */
     {
      if (result->AdditionalSenseCode == 0x3B &&
	  result->AdditionalSenseCodeQualifier == 0x0E)
	FatalError("source Element Address %d is Empty\n", src);

      if (result->AdditionalSenseCode == 0x3A &&
          result->AdditionalSenseCodeQualifier == 0x00)
       FatalError("Drive needs offline before move\n");  

      if (result->AdditionalSenseCode == 0x3B &&
	  result->AdditionalSenseCodeQualifier == 0x0D)
	FatalError("destination Element Address %d is Already Full\n",
		   dest);
      if (result->AdditionalSenseCode == 0x30 &&
	  result->AdditionalSenseCodeQualifier == 0x03)
	FatalError("Cleaning Cartridge Installed and Ejected\n");
      PrintRequestSense(result);
      FatalError("MOVE MEDIUM from Element Address %d to %d Failed\n",
		 src,dest);
    }
}   



/* okay, now for the Load, Unload, etc. logic: */

static void Load(void) {
  int src, dest;
  /* okay, check src, dest: arg1=src, arg2=dest */
  if (arg1 < 1) {
    FatalError("No source specified\n");
  }
  if (arg2 < 0) {
    arg2 = 0; /* default to 1st drive :-( */
  }
  arg1--;  /* we use zero-based arrays, sigh, not 1 base like some lusers */
  /* check for filehandle: */
  if (!device_opened) {
    FatalError("No Media Changer Device Specified\n");
  } 
  /* okay, we should be there: */
  if (arg1 < 0 || arg1 >= ElementStatus->StorageElementCount) {
    	FatalError(
	  "illegal <storage-element-number> argument '%d' to 'load' command\n",
	  arg1+1);
  }
  if (arg2 < 0 || arg2 >= ElementStatus->DataTransferElementCount) {
    FatalError(
	       "illegal <drive-number> argument '%d' to 'load' command\n",
	       arg2);
  }
  if (ElementStatus->DataTransferElementFull[arg2])  {
    FatalError("Drive %d Full (Storage Element %d loaded)\n",arg2,
	       ElementStatus->DataTransferElementSourceStorageElementNumber[arg2]+1);
  }
  /* Now look up the actual devices: */
  dest=ElementStatus->DataTransferElementAddress[arg2];
  src=ElementStatus->StorageElementAddress[arg1];
  fprintf(stdout, "Loading media from Storage Element %d into drive %d...",arg1+1,arg2);
  fflush(stdout);
  Move(src,dest);  /* load it into the particular slot, if possible! */
  fprintf(stdout,"done\n");
  fflush(stdout);
  /* now set the status for further command son this line... */
  ElementStatus->StorageElementFull[arg1] = false;
  ElementStatus->DataTransferElementFull[arg2] = true;

  return; /* and done! */
}

static void Transfer(void) {
  int src,dest;
  if (arg1 < 1 ) {
    FatalError("No source specified\n");
  } 
  if (arg2 < 1) {
    FatalError("No destination specified\n");
  }
  if (arg1 > ElementStatus->StorageElementCount) {
    FatalError("Invalid source\n");
  }
  if (arg2 > ElementStatus->StorageElementCount) {
    FatalError("Invalid destination\n");
  }
  /* okay, that's done... */
  src=ElementStatus->StorageElementAddress[arg1-1];
  dest=ElementStatus->StorageElementAddress[arg2-1];
  Move(src,dest);
}

/****************************************************************
 * Exchange() -- exchange medium in two slots, if so
 * supported by the jukebox in question.
 ***************************************************************/

static void Exchange(void) {
  RequestSense_T *result; /* from ExchangeMedium */
  int src,dest,dest2;
  if (arg1 < 1 ) {
    FatalError("No source specified\n");
  } 
  if (arg2 < 1) {
    FatalError("No destination specified\n");
  }
  if (arg1 > ElementStatus->StorageElementCount) {
    FatalError("Invalid source\n");
  }
  if (arg2 > ElementStatus->StorageElementCount) {
    FatalError("Invalid destination\n");
  }
  if (arg3==-1) {
    arg3=arg1;  /* true exchange of medium */
  }
  /* okay, that's done... */
  src=ElementStatus->StorageElementAddress[arg1-1];
  dest=ElementStatus->StorageElementAddress[arg2-1];
  dest2=ElementStatus->StorageElementAddress[arg3-1];
  
  result=ExchangeMedium(MediumChangerFD,src,dest,dest2,ElementStatus,inquiry_info,&SCSI_Flags);
  if (result) {  /* we have an error! */
    if (result->AdditionalSenseCode == 0x3B &&
	result->AdditionalSenseCodeQualifier == 0x0E)
      FatalError("source Element Address %d is Empty\n", src);
    
    if (result->AdditionalSenseCode == 0x3A &&
	result->AdditionalSenseCodeQualifier == 0x00)
      FatalError("Drive needs offline before move\n");  
    
    if (result->AdditionalSenseCode == 0x3B &&
	result->AdditionalSenseCodeQualifier == 0x0D)
      FatalError("destination Element Address %d is Already Full\n",
		 dest);
    if (result->AdditionalSenseCode == 0x30 &&
	result->AdditionalSenseCodeQualifier == 0x03)
      FatalError("Cleaning Cartridge Installed and Ejected\n");
    PrintRequestSense(result);
    FatalError("EXCHANGE MEDIUM from Element Address %d to %d Failed\n",
	       src,dest);
  }
}   

static void Eepos(void) {
  if (arg1 < 0 || arg1 > 3) {
    FatalError("eepos equires argument between 0 and 3.\n");
  }
  SCSI_Flags.eepos=arg1;
}


static void Unload(void) {
  int src,dest;  /* the actual SCSI-level numbers */
  if (arg2 < 0) {
    arg2 = 0; /* default to 1st drive :-( */
  }
  /* check for filehandle: */
  if (!device_opened) {
    FatalError("No Media Changer Device Specified\n");
  } 
  /* okay, we should be there: */
  if (arg1 < 0) {
    arg1 = ElementStatus->DataTransferElementSourceStorageElementNumber[arg2];
    if (arg1 < 0) {
      FatalError("No Source for tape in drive %d!\n",arg2);
    }
  } else {
    arg1--; /* go from bogus 1-base to zero-base */
  }

  if (arg1 >= ElementStatus->StorageElementCount) {
    	FatalError(
	  "illegal <storage-element-number> argument '%d' to 'unload' command\n",
	  arg1+1);
  }
    
  if (arg2 < 0 || arg2 >= ElementStatus->DataTransferElementCount) {
    FatalError(
	       "illegal <drive-number> argument '%d' to 'unload' command\n",
	       arg2);
  }
  if (!ElementStatus->DataTransferElementFull[arg2]) {
    FatalError("Data Transfer Element %d is Empty\n", arg2);
  }    
  /* Now see if something already lives where  we wanna go... */
  if (ElementStatus->StorageElementFull[arg1]) {
    FatalError("Storage Element %d is Already Full\n", arg1+1);
  }
  /* okay, now to get src, dest: */
  src=ElementStatus->DataTransferElementAddress[arg2];
  if (arg1 >=0) {
    dest=ElementStatus->StorageElementAddress[arg1];
  } else {
    dest=ElementStatus->DataTransferElementSourceStorageElementNumber[arg2];
  }
  if (dest < 0) { /* we STILL don't know, sigh... */
    FatalError("Do not know which slot to unload tape into!\n");
  }
  fprintf(stdout, "Unloading drive %d into Storage Element %d...",arg2, arg1+1);
  fflush(stdout); /* make it real-time :-( */ 
  Move(src,dest);
  fprintf(stdout, "done\n");
  fflush(stdout);
  ElementStatus->StorageElementFull[arg1] = true;
  ElementStatus->DataTransferElementFull[arg2] = false;
}

/*****************************************************************
 ** ARGUMENT PARSING SUBROUTINES: Parse arguments, dispatch. 
 *****************************************************************/

/* ***
 * int get_arg(idx):
 *
 * If we have an actual argument at the index position indicated (i.e. we
 * have not gone off the edge of the world), we return
 * its number. If we don't, or it's not a numeric argument, 
 * we return -1. Note that 'get_arg' is kind of misleading, we only accept 
 * numeric arguments, not any other kind. 
 */
int get_arg(int idx) {
  char *arg;
  int retval=-1;

  if (idx >= argc) return -1;  /* sorry! */
  arg=argv[idx];
  if (*arg < '0' || *arg > '9') {
    return -1;  /* sorry! */
  }

  retval=atoi(arg);
  return retval;
}

/* open_device() -- set the 'fh' variable.... */
void open_device(void) {


  if (device_opened) {
    SCSI_CloseDevice("Unknown",MediumChangerFD);  /* close it, sigh...  new device now! */
  }

  MediumChangerFD = SCSI_OpenDevice(device);
  device_opened=1; /* SCSI_OpenDevice does an exit() if not. */
}
  

/* we see if we've got a file open. If not, we open one :-(. Then
 * we execute the actual command. Or not :-(. 
 */ 
void execute_command(struct command_table_struct *command) {
  RequestSense_T RequestSense;


  if ((device==NULL) && command->need_device) {
   /* try to get it from TAPE environment variable... */
    device=getenv("CHANGER");
    if (device==NULL) {
      device=getenv("TAPE");
      if (device==NULL) {
	device="/dev/changer"; /* Usage(); */
      }
    }
    open_device();
  }
  if (!ElementStatus && command->need_status) {
    inquiry_info=RequestInquiry(MediumChangerFD,&RequestSense);
    if (!inquiry_info) {
      PrintRequestSense(&RequestSense);
      FatalError("INQUIRY command Failed\n");
    }
    ElementStatus = ReadElementStatus(MediumChangerFD,&RequestSense,inquiry_info,&SCSI_Flags);
    if (!ElementStatus) {
      PrintRequestSense(&RequestSense);
      FatalError("READ ELEMENT STATUS Command Failed\n"); 
    }
  }

  /* okay, now to execute the command... */
  command->command();
}

/* parse_args():
 *   Basically, we are parsing argv/argc. We can have multiple commands
 * on a line now, such as "unload 3 0 load 4 0" to unload one tape and
 * load in another tape into drive 0, and we execute these commands one
 * at a time as we come to them. If we don't have a -f at the start, we
 * barf. If we leave out a drive #, we default to drive 0 (the first drive
 * in the cabinet). 
 */ 

int parse_args(void) {
  int i,cmd_tbl_idx;
  struct command_table_struct *command;

  i=1;
  while (i<argc) {
    if (strcmp(argv[i],"-f") == 0) {
      i++;
      if (i>=argc) {
	Usage();
      }
      device=argv[i++];
      open_device(); /* open the device and do a status scan on it... */
    } else {
      cmd_tbl_idx=0;
      command=&command_table[0]; /* default to the first command... */
      command=&command_table[cmd_tbl_idx];
      while (command->name) {
	if (!strcmp(command->name,argv[i])) {
	  /* we have a match... */
	  break;
	}
	/* otherwise we don't have a match... */
	cmd_tbl_idx++;
	command=&command_table[cmd_tbl_idx];
      }
      /* if it's not a command, exit.... */
      if (!command->name) {
	Usage();
      }
      i++;  /* go to the next argument, if possible... */
      /* see if we need to gather arguments, though! */
      if (command->num_args == 0) {
	execute_command(command);  /* execute_command handles 'stuff' */
      }
      else {
	arg1=get_arg(i);  /* checks i... */
	if (arg1 != -1) {
	  i++;  /* next! */
	}
	if (command->num_args>=2 && arg1 != -1) {
	  arg2=get_arg(i); 
	  if (arg2 != -1) {
	    i++;
	  }
	  if (command->num_args==3 && arg2 != -1) {
	    arg3=get_arg(i);
	    if (arg3 != -1) {
	      i++;
	    }
	  }
	}
	execute_command(command);
      }
      arg1=-1;
      arg2=-1;
      arg3=-1;
    }
  }
  return 0; /* should never get here. */
}



int main(int ArgCount,
	 char *ArgVector[],
	 char *Environment[])
{


#ifdef VMS
  RequestSense_T RequestSense;
#endif
   
 /* save these where we can get at them elsewhere... */
  argc=ArgCount;
  argv=ArgVector;

  argv0=argv[0];

  parse_args();  /* also executes them as it sees them, sigh. */

#ifndef VMS
  if (device) 
    SCSI_CloseDevice(device, MediumChangerFD);
  return 0;
#else
  if (device) {
    ElementStatus = ReadElementStatus(MediumChangerFD,&RequestSense);
    if (!ElementStatus) {
      PrintRequestSense(&RequestSense);                   
      FatalError("READ ELEMENT STATUS Command Failed\n"); 
    }
    VMS_DefineStatusSymbols();
    SCSI_CloseDevice(device, MediumChangerFD);
  }
  return SS$_NORMAL;
#endif


}
/*
 *$Log: mtx.c,v $
 *Revision 1.12  2007/01/29 03:22:45  robertnelson
 *Add support for Windows.
 *
 *Add build support for Windows using MinGW native and Linux cross-compile.
 *
 *Add build support for Windows using Microsoft Visual Studio 2005.
 *
 *Add support for building on x86_64.
 *
 *Add more debugging information.
 *
 *Eliminate compiler warnings.
 *
 *Revision 1.11  2006/02/21 03:08:53  elgreen
 *mtx 1.3.9 checkin
 *
 *Revision 1.9  2003/06/26 15:59:21  elgreen
 *ci
 *
 *Revision 1.8  2003/03/12 23:45:52  elgreen
 *mtx 1.3.4
 *
 *Revision 1.7  2002/10/01 19:36:08  elgreen
 *mtx 1.3.1
 *
 *Revision 1.6  2002/09/27 21:30:16  elgreen
 *CVS 1.3
 *
 *Revision 1.5  2002/08/14 22:05:29  mahlonstacy
 *Added position command
 *
 *Revision 1.4  2002/02/05 16:33:18  elgreen
 *added Christopher McCrory's patches to mtx.c (see CHANGES)
 *
 *Revision 1.3  2001/11/06 21:21:31  elgreen
 *Hopefully fix for the from-crontab problem
 *
 *Revision 1.2.2.1  2001/11/06 21:20:40  elgreen
 *Hopefully a fix to the problem with the 0 return for open in crontabs
 *
 *Revision 1.2  2001/06/09 17:26:26  elgreen
 *Added 'nobarcode' command to mtx (to skip the initial request asking for
 *barcodes for mtx status purposes).
 *
 *Revision 1.1.1.1  2001/06/05 17:10:22  elgreen
 *Initial import into SourceForge
 *
 *Revision 1.15  2001/04/18 16:32:59  eric
 *Cleaned up all -Wall messages.
 *
 *Revision 1.14  2001/04/18 16:08:08  eric
 *after re-arranging & fixing bugs
 *
 *Revision 1.13  2001/03/06 01:37:05  eric
 *Solaris changes
 *
 *Revision 1.12  2001/01/24 22:04:08  eric
 *added /dev/changer as default file name.
 *
 *Revision 1.11  2001/01/15 22:21:06  eric
 *added autoconf stuff.
 *
 *Revision 1.10  2000/11/30 20:37:17  eric
 *Added quicky 'erase' command because I needed blank tapes and Linux
 *only does long erases. (Not documented on purpose, because of rather
 *bizarre interactions with the native tape driver if you don't know
 *what you are doing).
 *
 *Revision 1.9  2000/11/28 01:10:50  eric
 *Fixed syntax error in mtx.c...
 *
 *Revision 1.8  2000/10/28 00:10:35  eric
 *Fixed stupid typo
 *
 *Revision 1.7  2000/10/27 23:36:57  eric
 *make mtx inventory return an error code if the inventory fails, so that
 *we can wait for inventory to be completed at system startup
 *
 *
 */
