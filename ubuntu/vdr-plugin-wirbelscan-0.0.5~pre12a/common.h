/*
 * common.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __WIRBELSCAN_COMMON_H_
#define __WIRBELSCAN_COMMON_H_

#include <linux/types.h>
#include <sys/ioctl.h>

typedef enum scantype {
  DVB_TERR    = 0,
  DVB_CABLE   = 1,
  DVB_SAT     = 2,
  PVRINPUT    = 3,
  PVRINPUT_FM = 4,
  DVB_ATSC    = 5,
} scantype_t;



/* generic functions */
void  dlog(const int level, const char *fmt, ...);
int   IOCTL(int fd, int cmd, void * data);

#define closeIfOpen(aFile)      if (aFile    >= 0)    { close(aFile);      aFile = -1;    }
#define DeleteAndNull(aPointer) if (aPointer != NULL) { delete(aPointer);  aPointer=NULL; }
#define verbose(x...) dlog(4, x)
#define fatal(x...)   dlog(0, x); return -1

const char * vdr_inversion_name(int inversion);
const char * vdr_fec_name(int fec);
const char * vdr17_modulation_name(int modulation);
const char * vdr_modulation_name(int modulation);
const char * vdr_bandwidth_name(int bandwidth);
const char * vdr_transmission_mode_name(int transmission_mode);
const char * vdr_guard_name(int guard_interval);
const char * vdr_hierarchy_name(int hierarchy);
const char * vdr_name_to_short_name(const char * satname);
const char * xine_transmission_mode_name (int transmission_mode);
const char * xine_modulation_name(int modulation);
const char * atsc_mod_to_txt ( int id );
int dvbc_modulation(int index);
int dvbc_symbolrate(int index);

#endif

