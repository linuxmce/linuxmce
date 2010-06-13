#ifndef __WIRBELSCAN_SCANFILTER_H_
#define __WIRBELSCAN_SCANFILTER_H_

#include <libsi/si.h>
#include <vdr/receiver.h>
#include <vdr/filter.h>
#include <vdr/pat.h>
#include <linux/dvb/frontend.h>
#include "caDescriptor.h"

/**********************************************************************************************************
*  table	pid		table_id	defined_by
*  PAT	0x00		0x00		MPEG-2		500msec		Program association section
*  CAT	0x01		0x01		MPEG-2		?		Conditional access section
*  PMT	0x10..0x1FFE	0x02		MPEG-2		500msec		Program Map Table
*  TSDT	0x02		0x03		MPEG-2		10sec		Transport Stream Description Section
***********************************************************************************************************
*  NIT	0x10		0x40		DVB		25msec..10sec	Network Information section(actual)				(mandatory)
*  SDT	0x11		0x42		DVB		25msec..2sec	Service description section(actual_transport_stream)		(mandatory)
*  EIT	0x12		0x4E		DVB		25msec..2sec	Event Information Section  (actual ts, present/following)	(mandatory)
*  TDT	0x14		0x70		DVB		25msec..30sec	Time and date section						(mandatory)
***********************************************************************************************************
*  NIT	0x10		0x41		DVB		25msec..10sec	Other Network Information section				(optional)
*  BAT	0x11		0x4A		DVB		25msec..10sec	Bouquet Association Table					(optional)
*  SDT	0x11		0x46		DVB		25msec..10sec	Other Service description section				(optional)
*  EIT	0x12		0x4F		DVB		25msec..2sec	Event Information Section  (other ts, present/following)	(optional)
*  EIT	0x12		0x50..0x5F	DVB		25msec..10sec	Event Information Section  (actual ts, schedule)		(optional)
*  EIT	0x12		0x60..0x6F	DVB		25msec..10sec	Event Information Section  (other ts,  schedule)    (optional)
*  RST	0x13		0x71		DVB				Running status section						(optional)
*  TOT	0x14		0x73		DVB		25msec..30sec	Time offset section						(optional)
*  MIP	0x15
*  ST	0x10..0x14	0x72		DVB		----		Stuffing section						(optional)
*  DIT	0x1E		0x7E		DVB				Discontinuity Information Section				(optional)
*  SIT	0x1F		0x7F		DVB				Selection Information Section					(optional)
*  UNT	0x10..0x1FFE	0x4B		DVB
*  INT	0x10..0x1FFE	0x4C    DVB				INT per PMT In PMT: data_broadcast_id = 0x000B
**********************************************************************************************************
*  AIT			0x74            AIT Used for MHP
*  DSM-CC	0x10..0x1FFE	0x39		DVB				DSM-CC addressable section per PMT
*  DSM-CC	0x10..0x1FFE	0x3A		DVB				DSM-CC, MPE per PMT
*  DSM-CC	0x10..0x1FFE	0x3B		DVB				DSM-CC, U-N messages, except DDM per PMT
*  DSM-CC	0x10..0x1FFE	0x3C		DVB				DSM-CC, DDM per PMT
*  DSM-CC	0x10..0x1FFE	0x3D		DVB				DSM-CC, stream descriptors per PMT
*  DSM-CC	0x10..0x1FFE	0x3E		DVB				DSM-CC, private data, IP-Datagram per PMT used for DVB-H
*  DSM-CC	0x10..0x1FFE	0x3F		DVB				DSM-CC addressable section per PMT
**********************************************************************************************************
*  0x10..0x1FFE	0x78						MPE-FEC Per PMT DVB-H, changed from 0x75
*     0x79						RNT
*  CA	0x01		0x80..0x8F	DVB				CA EMM, CA ECM  (0x80, 0x81 = ECM)
**********************************************************************************************************/

/**************  DESCRIPTORS  ****************************************************************************
 *
 ########################### ISO 13181-1 ########################################################################
 * value	PAT	CAT	PMT	TSDT	NIT	BAT	SDT	EIT	TOT	PMT	SIT	Descriptor_Tag
 **********************************************************************************************************
 * 0x00	Reserved
 * 0x01	Reserved
 **********************************************************************************************************
 * 0x02	*	*	*	*	-	-	-	-	-	-	-	video_stream_descriptor
 * 0x03	*	*	*	*	-	-	-	-	-	-	-	audio_stream_descriptor
 * 0x04	*	*	*	*	-	-	-	-	-	-	-	hierarchy_descriptor
 * 0x05	*	*	*	*	-	-	-	-	-	-	-	registration_descriptor
 * 0x06	*	*	*	*	-	-	-	-	-	-	-	data_stream_alignment_descriptor
 * 0x07	*	*	*	*	-	-	-	-	-	-	-	target_background_grid_descriptor
 * 0x08	*	*	*	*	-	-	-	-	-	-	-	Video_window_descriptor
 * 0x09	*	*	*	*	-	-	-	-	-	-	-	CA_descriptor
 * 0x0A	*	*	*	*	-	-	-	-	-	-	-	ISO_639_language_descriptor
 * 0x0B	*	*	*	*	-	-	-	-	-	-	-	System_clock_descriptor
 * 0x0C	*	*	*	*	-	-	-	-	-	-	-	Multiplex_buffer_utilization_descriptor
 * 0x0D	*	*	*	*	-	-	-	-	-	-	-	Copyright_descriptor
 * 0x0E	*	*	*	*	-	-	-	-	-	-	-	Maximum_bitrate_descriptor
 * 0x0F	*	*	*	*	-	-	-	-	-	-	-	Private_data_indicator_descriptor
 * 0x10	*	*	*	*	-	-	-	-	-	-	-	Smoothing_buffer_descriptor
 * 0x11	*	*	*	*	-	-	-	-	-	-	-	STD_descriptor
 * 0x12	*	*	*	*	-	-	-	-	-	-	-	IBP_descriptor
 **********************************************************************************************************
 * 0x13	-	-	-	-								CarouselIdentifierDescriptorTag
 * 0x14
 * ...
 * 0x1A	*	*	*	*	-	-	-	-	-	-	-	Defined	in ISO/IEC 13818-6 (DSM-CC)
 **********************************************************************************************************
 * 0x1B	*	*	*	*	-	-	-	-	-	-	-	MPEG-4_video_descriptor
 * 0x1C	*	*	*	*	-	-	-	-	-	-	-	MPEG-4_audio_descriptor
 * 0x1D	*	*	*	*	-	-	-	-	-	-	-	IOD_descriptor
 * 0x1E	*	*	*	*	-	-	-	-	-	-	-	SL_descriptor
 * 0x1F	*	*	*	*	-	-	-	-	-	-	-	FMC_descriptor
 * 0x20	*	*	*	*	-	-	-	-	-	-	-	External_ES_ID_descriptor
 * 0x21	*	*	*	*	-	-	-	-	-	-	-	MuxCode_descriptor
 * 0x22	*	*	*	*	-	-	-	-	-	-	-	FmxBufferSize_descriptor
 * 0x23	*	*	*	*	-	-	-	-	-	-	-	MultiplexBuffer_descriptor
 **********************************************************************************************************
 * 0x24
 * ...
 * 0x3F	ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
 **********************************************************************************************************
 ########################### EN 300 468 Version 1.8.1 ########################################################################
 * value	PAT	CAT	PMT	TSDT	NIT	BAT	SDT	EIT	TOT	PMT	SIT	Descriptor_Tag
 * 0x40	-	-	-	-	*	-	-	-	-	-	-	network_name_descriptor
 * 0x41	-	-	-	-	*	*	-	-	-	-	-	service_list_descriptor
 * 0x42	-	-	-	-	*	*	*	*	-	-	*	stuffing_descriptor
 * 0x43	-	-	-	-	*	-	-	-	-	-	-	satellite_delivery_system_descriptor
 * 0x44	-	-	-	-	*	-	-	-	-	-	-	cable_delivery_system_descriptor
 * 0x45	-	-	-	-	-	-	-	-	-	*	-	VBI_data_descriptor
 * 0x46	-	-	-	-	-	-	-	-	-	*	-	VBI_teletext_descriptor
 * 0x47	-	-	-	-	-	*	*	-	-	-	*	bouquet_name_descriptor
 * 0x48	-	-	-	-	-	-	*	-	-	-	*	service_descriptor
 * 0x49	-	-	-	-	-	*	*	-	-	-	*	country_availability_descriptor
 * 0x4A	-	-	-	-	*	*	*	*	-	-	*	linkage_descriptor
 * 0x4B	-	-	-	-	-	-	*	-	-	-	*	NVOD_reference_descriptor
 * 0x4C	-	-	-	-	-	-	*	-	-	-	*	time_shifted_service_descriptor
 * 0x4D	-	-	-	-	-	-	-	*	-	-	*	short_event_descriptor
 * 0x4E	-	-	-	-	-	-	-	*	-	-	*	extended_event_descriptor
 * 0x4F	-	-	-	-	-	-	-	*	-	-	*	time_shifted_event_descriptor
 * 0x50	-	-	-	-	-	-	*	*	-	-	*	component_descriptor
 * 0x51	-	-	-	-	-	-	*	-	-	*	*	mosaic_descriptor
 * 0x52	-	-	-	-	-	-	-	-	-	*	-	stream_identifier_descriptor
 * 0x53	-	-	-	-	-	*	*	*	-	-	*	CA_identifier_descriptor
 * 0x54	-	-	-	-	-	-	-	*	-	-	*	content_descriptor
 * 0x55	-	-	-	-	-	-	-	*	-	-	*	parental_rating_descriptor
 * 0x56	-	-	-	-	-	-	-	-	-	*	-	teletext_descriptor
 * 0x57	-	-	-	-	-	-	*	*	-	-	*	telephone_descriptor
 * 0x58	-	-	-	-	-	-	-	-	*	-	-	local_time_offset_descriptor
 * 0x59	-	-	-	-	-	-	-	-	-	*	-	subtitling_descriptor
 * 0x5A	-	-	-	-	*	-	-	-	-	-	-	terrestrial_delivery_system_descriptor
 * 0x5B	-	-	-	-	*	-	-	-	-	-	-	multilingual_network_name_descriptor
 * 0x5C	-	-	-	-	-	*	-	-	-	-	-	multilingual_bouquet_name_descriptor
 * 0x5D	-	-	-	-	-	-	*	-	-	-	*	multilingual_service_name_descriptor
 * 0x5E	-	-	-	-	-	-	-	*	-	-	*	multilingual_component_descriptor
 * 0x5F	-	-	-	-	*	*	*	*	-	*	*	private_data_specifier_descriptor
 * 0x60	-	-	-	-	-	-	-	-	-	*	-	service_move_descriptor
 * 0x61	-	-	-	-	-	-	-	*	-	-	*	short_smoothing_buffer_descriptor
 * 0x62	-	-	-	-	*	-	-	-	-	-	-	frequency_list_descriptor
 * 0x63	-	-	-	-	-	-	-	-	-	-	*	partial_transport_stream_descriptor
 * 0x64	-	-	-	-	-	-	*	*	-	-	*	data_broadcast_descriptor
 * 0x65	-	-	-	-	-	-	-	-	-	*	-	scrambling_descriptor
 * 0x66	-	-	-	-	-	-	-	-	-	*	-	data_broadcast_id_descriptor
 * 0x67	-	-	-	-	-	-	-	-	-	-	-	transport_stream_descriptor
 * 0x68	-	-	-	-	-	-	-	-	-	-	-	DSNG_descriptor
 * 0x69	-	-	-	-	-	-	-	*	-	-	-	PDC_descriptor
 * 0x6A	-	-	-	-	-	-	-	-	-	*	-	AC-3_descriptor
 * 0x6B	-	-	-	-	-	-	-	-	-	*	-	ancillary_data_descriptor
 * 0x6C	-	-	-	-	*	-	-	-	-	-	-	cell_list_descriptor
 * 0x6D	-	-	-	-	*	-	-	-	-	-	-	cell_frequency_link_descriptor
 * 0x6E	-	-	-	-	-	-	*	-	-	-	-	announcement_support_descriptor
 * 0x6F	-	-	-	-	-	-	-	-	-	*	-	application_signalling_descriptor
 * 0x70	-	-	-	-	-	-	-	-	-	*	-	adaptation_field_data_descriptor
 * 0x71	-	-	-	-	-	-	*	-	-	-	-	service_identifier_descriptor
 * 0x72	-	-	-	-	-	-	*	-	-	-	-	service_availability_descriptor
 * 0x73	-	-	-	-	*	*	*	-	-	-	-	default_authority_descriptor
 * 0x74	-	-	-	-	-	-	-	-	-	*	-	related_content_descriptor
 * 0x75	-	-	-	-	-	-	-	*	-	-	-	TVA_id_descriptor
 * 0x76	-	-	-	-	-	-	-	*	-	-	-	content_identifier_descriptor
 * 0x77	-	-	-	-	*	-	-	-	-	-	-	time_slice_fec_identifier_descriptor
 * 0x78	-	-	-	-	-	-	-	-	-	*	-	ECM_repetition_rate_descriptor
 * 0x79	-	-	-	-	*	-	-	-	-	-	-	S2_satellite_delivery_system_descriptor
 * 0x7A	-	-	-	-	-	-	-	-	-	*	-	enhanced_AC-3_descriptor
 * 0x7B	-	-	-	-	-	-	-	-	-	*	-	DTS_descriptor
 * 0x7C	-	-	-	-	-	-	-	-	-	*	-	AAC_descriptor
 * 0x7D	-	-	-	-	-	-	-	-	-	-	-	reserved
 * 0x7E	-	-	-	-	-	-	-	-	-	-	-	reserved
 * 0x7F	-	-	-	-	*	*	*	*	*	*	*	extension_descriptor
 **************************************************************************************************************************************
 * 0x80
 * ..
 * 0xFE   user_defined
 * 0xFF   forbidden
 *
 **********************************************************************************************************/

#define PID_PAT                               0x00
#define PID_CAT                               0x01
#define PID_PMT                               0x02
#define PID_TSDT                              0x03
#define PID_NIT                               0x10
#define PID_SDT                               0x11
#define PID_BAT                               0x11
#define PID_EIT                               0x12
#define PID_RST                               0x13
#define PID_TDT                               0x14
#define PID_DIT                               0x1E
#define PID_SIT                               0x1F

#define TABLE_ID_PAT                          0x00 //program allocation table						(mandatory)
#define TABLE_ID_CAT                          0x01 //conditional access section					(mandatory)
#define TABLE_ID_PMT                          0x02 //program map section						(mandatory)
#define TABLE_ID_TSDT                         0x03 //transport stream description section				(mandatory)
#define TABLE_ID_NIT_ACTUAL                   0x40 //network information section, *actual* network			(mandatory)
#define TABLE_ID_NIT_OTHER                    0x41 //network information section, *other*  network			(optional)
#define TABLE_ID_SDT_ACTUAL                   0x42 //service description section, *actual* transport stream		(mandatory)
#define TABLE_ID_SDT_OTHER                    0x46 //service description section, *other*  transport stream		(optional)
#define TABLE_ID_BAT                          0x4A //bouquet association section					(optional)
#define TABLE_ID_EIT_ACTUAL_PRESENT           0x4E //Event Information Section  (actual ts, present/following)	(mandatory)
#define TABLE_ID_EIT_OTHER_PRESENT            0x4F //Event Information Section  (other ts,  present/following)	(optional)
#define TABLE_ID_EIT_ACTUAL_SCHEDULE_START    0x50 //Event Information Section  (actual ts, schedule)			(optional)
#define TABLE_ID_EIT_ACTUAL_SCHEDULE_STOP     0x5F //Event Information Section  (actual ts, schedule)			(optional)
#define TABLE_ID_EIT_OTHER_SCHEDULE_START     0x60 //Event Information Section  (other ts,  schedule)			(optional)
#define TABLE_ID_EIT_OTHER_SCHEDULE_STOP      0x6F //Event Information Section  (other ts,  schedule)			(optional)
#define TABLE_ID_SIT                          0x7F //service information section					(optional)
#define TABLE_ID_CIT_PREM                     0xA0 //premiere content information section				(optional, undefined in 13818/300468)

#define _MAXNITS                              16
#define _MAXNETWORKNAME                       Utf8BufSize(256)
#define MAX_PMTS                              256 //max allowed PMTs per Transponder

#define SCANNED_CHANNEL                       0x8000
#define INVALID_CHANNEL                       0x4000
#define NO_LOCK                               0x2000

//#define DEBUGVERSION 1

#ifdef DEBUGVERSION
#define HEXDUMP(d, l)    for (int i = 0; i < l; i ++) {if (!(i % 16)) {printf("%s0x%.4x:    ", i ? "\n    " : "    ", (i / 16) *16);} printf("%.2x ", *(d + i));} printf("\n");
#else
#define HEXDUMP(d, l)
#endif

//--------------------------------------------------------------------------------------------

cChannel * GetByTransponder(const cChannel * Transponder);
int        ServicesOnTransponder(const cChannel * Transponder);
int        AddChannels();

//---------cTransponders---------------------------------------------------------------------
class cTransponders : public cChannels {
private:
protected:
public:
  bool       IsUniqueTransponder(const cChannel * NewTransponder);
  cChannel * NextTransponder(void);
  };

extern cTransponders NewTransponders;
extern cChannels     NewChannels;

//--------cPatScanner-------------------------------------------------------------------------
class cPmtScanner;

class cPatScanner : public cFilter, public cThread {
private:
  bool          active;
  cPmtScanner * cPmtScanners[MAX_PMTS];
  cDevice *     parent;
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char * Data, int Length);
  virtual void Action(void);
public:
  cPatScanner(cDevice * Parent);
  ~cPatScanner();
  bool Active(void) {
    return (active);
    };
  };

//--------cNitScanner-------------------------------------------------------------------------

class cNitScanner : public cFilter, public cThread {
private:

  class cNit {
public:
    u_short networkId;
    char    name[MAXNETWORKNAME];
    bool    hasTransponder;
    };
  cSectionSyncer syncNit;
  cNit           nits[_MAXNITS];
  u_short        networkId;
  int            numNits;
  int            tableId;
  bool           active;
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char * Data, int Length);
  virtual void Action(void);
public:
  cNitScanner(int TableId);
  ~cNitScanner();
  bool Active(void) {
    return (active);
    };
  };

//--------cPmtScanner-------------------------------------------------------------------------

class cPmtScanner : public cFilter {
private:
  bool       active;
  int        pmtPid;
  int        pmtSid;
  cChannel * Channel;
  int        numPmtEntries;
  bool PmtVersionChanged(int PmtPid, int Sid, int Version);
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char * Data, int Length);
public:
  bool Active(void) {
    return (active);
    };
  cPmtScanner(cChannel * channel, u_short Sid, u_short PmtPid);
  };

//--------cSdtScanner-------------------------------------------------------------------------

class cSdtScanner : public cFilter, public cThread {
private:
  bool           active;
  int            tableId;
  cSectionSyncer sectionSyncer;
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char * Data, int Length);
  virtual void Action(void);
public:
  bool Active(void) {
    return (active);
    };
  cSdtScanner(int TableId);
  ~cSdtScanner();
  };

#endif
