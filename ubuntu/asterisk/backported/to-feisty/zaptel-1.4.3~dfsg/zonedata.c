/*
 * BSD Telephony Of Mexico "Tormenta" Tone Zone Support 2/22/01
 * 
 * Working with the "Tormenta ISA" Card 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under thet erms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 * Primary Author: Mark Spencer <markster@digium.com>
 * 
 * This information from ITU E.180 Supplement 2.
 * UK information from BT SIN 350 Issue 1.1
 * Helpful reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf
 */
#include "tonezone.h"

struct tone_zone builtin_zones[]  =
{
	{ 0, "us", "United States / North America", { 2000, 4000 }, 
	  {
		  { ZT_TONE_DIALTONE, "350+440" },
		  { ZT_TONE_BUSY, "480+620/500,0/500" },
		  { ZT_TONE_RINGTONE, "440+480/2000,0/4000" },
		  { ZT_TONE_CONGESTION, "480+620/250,0/250" },
		  { ZT_TONE_CALLWAIT, "440/300,0/10000" },
		  { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,0" },
		  { ZT_TONE_STUTTER, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
	  },
	},
	{ 1, "au", "Australia", {  400, 200, 400, 2000 },
	  {
		  { ZT_TONE_DIALTONE, "413+438" },		
		  { ZT_TONE_BUSY, "425/375,0/375" },
		  { ZT_TONE_RINGTONE, "413+438/400,0/200,413+438/400,0/2000" },
		  /* XXX Congestion: Should reduce by 10 db every other cadence XXX */
		  { ZT_TONE_CONGESTION, "425/375,0/375,420/375,0/375" }, 
		  { ZT_TONE_CALLWAIT, "425/100,0/200,425/200,0/4400" },
		  { ZT_TONE_DIALRECALL, "413+428" },
		  { ZT_TONE_RECORDTONE, "!425/1000,!0/15000,425/360,0/15000" },
		  { ZT_TONE_INFO, "425/2500,0/500" },
		  { ZT_TONE_STUTTER, "413+438/100,0/40" },
	  },
	},
	{ 2, "fr", "France", { 1500, 3500 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  /* Dialtone can also be 440+330 */
		  { ZT_TONE_DIALTONE, "440" },
		  { ZT_TONE_BUSY, "440/500,0/500" },
		  { ZT_TONE_RINGTONE, "440/1500,0/3500" },
		  /* CONGESTION - not specified */
		  { ZT_TONE_CONGESTION, "440/250,0/250" },
		  { ZT_TONE_CALLWAIT, "440/300,0/10000" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,0" },
		  { ZT_TONE_STUTTER, "!440/100,!0/100,!440/100,!0/100,!440/100,!0/100,!440/100,!0/100,!440/100,!0/100,!440/100,!0/100,440" },
	  },
	},
	{ 3, "nl", "Netherlands", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  /* Most of these 425's can also be 450's */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/250,0/250" },
		  { ZT_TONE_CALLWAIT, "425/500,0/9500" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "950/330,1400/330,1800/330,0/1000" },
		  { ZT_TONE_STUTTER, "425/500,0/50" },
	  },
	},
	{ 4, "uk", "United Kingdom", { 400, 200, 400, 2000 },
	  {
		  /* From British Telecom SIN350 v1.2 */
		  { ZT_TONE_DIALTONE, "350+440" },
		  { ZT_TONE_BUSY, "400/375,0/375" },
		  { ZT_TONE_RINGTONE, "400+450/400,0/200,400+450/400,0/2000" },
		  { ZT_TONE_CONGESTION, "400/400,0/350,400/225,0/525" },
		  { ZT_TONE_CALLWAIT, "400/100,0/4000" },
		  { ZT_TONE_DIALRECALL, "350+440" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/60000" },
		  { ZT_TONE_INFO, "950/330,0/15,1400/330,0/15,1800/330,0/1000" },
		  { ZT_TONE_STUTTER, "350+440/750,440/750" },
	  },
	},
	{ 5, "fi", "Finland", { 1000, 4000 },
	  {
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/300,0/300" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "425/150,0/150,425/150,0/8000" },
		  { ZT_TONE_DIALRECALL, "425/650,0/25" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "950/650,0/325,950/325,0/30,1400/1300,0/2600" },
		  { ZT_TONE_STUTTER, "425/650,0/25" },
	  },
	},
	{ 6,"es","Spain", { 1500, 3000},
	  {
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/200,0/200" },
		  { ZT_TONE_RINGTONE, "425/1500,0/3000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200,425/200,0/200,425/200,0/600" },
		  { ZT_TONE_CALLWAIT, "425/175,0/175,425/175,0/3500" },
		  { ZT_TONE_DIALRECALL, "!425/200,!0/200,!425/200,!0/200,!425/200,!0/200,425" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "950/330,0/1000" },
		  { ZT_TONE_STUTTER, "425/500,0/50" },
	  },
	},
	{ 7,"jp","Japan", { 1000, 2000 },
	  {
		  { ZT_TONE_DIALTONE, "400" },
		  { ZT_TONE_BUSY, "400/500,0/500" },
		  { ZT_TONE_RINGTONE, "400+15/1000,0/2000" },
		  { ZT_TONE_CONGESTION, "400/500,0/500" },
		  { ZT_TONE_CALLWAIT, "400+16/500,0/8000" },
		  { ZT_TONE_DIALRECALL, "!400/200,!0/200,!400/200,!0/200,!400/200,!0/200,400" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,0" },
		  { ZT_TONE_STUTTER, "!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,400" },
	  },
	},
	{ 8,"no","Norway", { 1000, 4000 },
	  {
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "425/200,0/600,425/200,0/10000" },
		  { ZT_TONE_DIALRECALL, "470/400,425/400" },
		  { ZT_TONE_RECORDTONE, "1400/400,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,0" },
		  { ZT_TONE_STUTTER, "470/400,425/400" },
	  },
	},
	{ 9, "at", "Austria", { 1000, 5000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "420" },
		  { ZT_TONE_BUSY, "420/400,0/400" },
		  { ZT_TONE_RINGTONE, "420/1000,0/5000" },
		  { ZT_TONE_CONGESTION, "420/200,0/200" },
		  { ZT_TONE_CALLWAIT, "420/40,0/1960" },
		  { ZT_TONE_DIALRECALL, "420" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/80,0/14920" },
		  { ZT_TONE_INFO, "950/330,1450/330,1850/330,0/1000" },
		  { ZT_TONE_STUTTER, "380+420" },
	  },
	},
	{ 10, "nz", "New Zealand", { 400, 200, 400, 2000 },
	  {
		  { ZT_TONE_DIALTONE, "400" },
		  { ZT_TONE_BUSY, "400/500,0/500" },
		  { ZT_TONE_RINGTONE, "400+450/400,0/200,400+450/400,0/2000" },
		  { ZT_TONE_CONGESTION, "400/250,0/250" },
		  { ZT_TONE_CALLWAIT, "400/250,0/250,400/250,0/3250" },
		  { ZT_TONE_DIALRECALL, "!400/100!0/100,!400/100,!0/100,!400/100,!0/100,400" },
		  { ZT_TONE_RECORDTONE, "1400/425,0/15000" },
		  { ZT_TONE_INFO, "400/750,0/100,400/750,0/100,400/750,0/100,400/750,0/400" },
		  { ZT_TONE_STUTTER, "!400/100!0/100,!400/100,!0/100,!400/100,!0/100,!400/100!0/100,!400/100,!0/100,!400/100,!0/100,400" },
	  },
	},
	{ 11,"it","Italy", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425/200,0/200,425/600,0/1000" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "425/400,0/100,425/250,0/100,425/150,0/14000" },
		  { ZT_TONE_DIALRECALL, "470/400,425/400" },
		  { ZT_TONE_RECORDTONE, "1400/400,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,0" },
		  { ZT_TONE_STUTTER, "470/400,425/400" },
	  },
	},
	{ 12, "us-old", "United States Circa 1950/ North America", { 2000, 4000 }, 
	  {
		  { ZT_TONE_DIALTONE, "600*120" },
		  { ZT_TONE_BUSY, "500*100/500,0/500" },
		  { ZT_TONE_RINGTONE, "420*40/2000,0/4000" },
		  { ZT_TONE_CONGESTION, "500*100/250,0/250" },
		  { ZT_TONE_CALLWAIT, "440/300,0/10000" },
		  { ZT_TONE_DIALRECALL, "!600*120/100,!0/100,!600*120/100,!0/100,!600*120/100,!0/100,600*120" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,0" },
		  { ZT_TONE_STUTTER, "!600*120/100,!0/100,!600*120/100,!0/100,!600*120/100,!0/100,!600*120/100,!0/100,!600*120/100,!0/100,!600*120/100,!0/100,600*120" },
	  },
	},
	{ 13, "gr", "Greece", { 1000, 4000 },
	  {
		  { ZT_TONE_DIALTONE, "425/200,0/300,425/700,0/800" },
		  { ZT_TONE_BUSY, "425/300,0/300" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "425/150,0/150,425/150,0/8000" },
		  { ZT_TONE_DIALRECALL, "425/650,0/25" },
		  { ZT_TONE_RECORDTONE, "1400/400,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,0" },
		  { ZT_TONE_STUTTER, "425/650,0/25" },
	  },
	},
	{ 14, "tw", "Taiwan", { 1000, 4000 },
	  {
		  { ZT_TONE_DIALTONE, "350+440" },
		  { ZT_TONE_BUSY, "480+620/500,0/500" },
		  { ZT_TONE_RINGTONE, "440+480/1000,0/2000" },
		  { ZT_TONE_CONGESTION, "480+620/250,0/250" },
		  { ZT_TONE_CALLWAIT, "350+440/250,0/250,350+440/250,0/3250" },
		  { ZT_TONE_DIALRECALL, "300/1500,0/500" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,0" },
		  { ZT_TONE_STUTTER, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
	  },
	},
	{ 15, "cl", "Chile", { 1000, 3000 }, 
	  {
		  { ZT_TONE_DIALTONE, "400" },
		  { ZT_TONE_BUSY, "400/500,0/500" },
		  { ZT_TONE_RINGTONE, "400/1000,0/3000" },
		  { ZT_TONE_CONGESTION, "400/200,0/200" },
		  { ZT_TONE_CALLWAIT, "400/250,0/8750" },
		  { ZT_TONE_DIALRECALL, "!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,400" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/333,!1400/333,!1800/333,0" },
		  { ZT_TONE_STUTTER, "!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,!400/100,!0/100,400" },
	  },
	},
	{ 16, "se", "Sweden", { 1000, 5000 }, 
	  {
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/250,0/250" },
		  { ZT_TONE_RINGTONE, "425/1000,0/5000" },
		  { ZT_TONE_CONGESTION, "425/250,0/750" },
		  { ZT_TONE_CALLWAIT, "425/200,0/500,425/200,0/9100" },
		  { ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/332,!0/24,!1400/332,!0/24,!1800/332,!0/2024,"
		    "!950/332,!0/24,!1400/332,!0/24,!1800/332,!0/2024,"
		    "!950/332,!0/24,!1400/332,!0/24,!1800/332,!0/2024,"
		    "!950/332,!0/24,!1400/332,!0/24,!1800/332,!0/2024,"
		    "!950/332,!0/24,!1400/332,!0/24,!1800/332,0" },
		  /*{ ZT_TONE_STUTTER, "425/320,0/20" },              Real swedish standard, not used for now */
		  { ZT_TONE_STUTTER, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
	  },
	},
	{ 17, "be", "Belgium", { 1000, 3000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/3000" },
		  { ZT_TONE_CONGESTION, "425/167,0/167" },
		  { ZT_TONE_CALLWAIT, "1400/175,0/175,1400/175,0/3500" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "900/330,1400/330,1800/330,0/1000" },
		  { ZT_TONE_STUTTER, "425/1000,0/250" },
	  },
	},
	{ 18, "sg", "Singapore", { 400, 200, 400, 2000 },
	  {
		  /* Reference: http://www.ida.gov.sg/idaweb/doc/download/I397/ida_ts_pstn1_i4r2.pdf */
		  { ZT_TONE_DIALTONE,   "425" },
		  { ZT_TONE_BUSY,       "425/750,0/750" },
		  { ZT_TONE_RINGTONE,   "425*24/400,0/200,425*24/400,0/2000" },
		  { ZT_TONE_CONGESTION, "425/250,0/250" },
		  { ZT_TONE_CALLWAIT,   "425*24/300,0/200,425*24/300,0/3200" },
		  /* DIALRECALL - not specified - use repeating Holding Tone A,B*/
		  { ZT_TONE_DIALRECALL, "425*24/500,0/500,425/500,0/2500" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO,       "950/330,1400/330,1800/330,0/1000" },
		  { ZT_TONE_STUTTER,    "!425/200,!0/200,!425/600,!0/200,!425/200,!0/200,!425/600,!0/200,!425/200,!0/200,!425/600,!0/200,!425/200,!0/200,!425/600,!0/200,425" },
	  },
	},
	{ 19, "il", "Israel", { 1000, 3000 },
	  {
		  { ZT_TONE_DIALTONE, "414" },
		  { ZT_TONE_BUSY, "414/500,0/500" },
		  { ZT_TONE_RINGTONE, "414/1000,0/3000" },
		  { ZT_TONE_CONGESTION, "414/250,0/250" },
		  { ZT_TONE_CALLWAIT, "414/100,0/100,414/100,0/100,414/600,0/3000" },
		  { ZT_TONE_DIALRECALL, "!414/100,!0/100,!414/100,!0/100,!414/100,!0/100,414" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "1000/330,1400/330,1800/330,0/1000" },
		  { ZT_TONE_STUTTER, "!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,!414/160,!0/160,414" },
	  },
	},
	{ 20, "br", "Brazil", { 1000, 4000 },
	  {
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/250,0/250" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/250,0/250,425/750,0/250" },
		  { ZT_TONE_CALLWAIT, "425/50,0/1000" },
		  { ZT_TONE_DIALRECALL, "350+440" },
		  { ZT_TONE_RECORDTONE, "425/250,0/250" },
		  { ZT_TONE_INFO, "950/330,1400/330,1800/330" },
		  { ZT_TONE_STUTTER, "350+440" } },
	},
	{ 21, "hu", "Hungary", { 1250, 3750 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/300,0/300" },
		  { ZT_TONE_RINGTONE, "425/1250,0/3750" },
		  { ZT_TONE_CONGESTION, "425/300,0/300" },
		  { ZT_TONE_CALLWAIT, "425/40,0/1960" },
		  { ZT_TONE_DIALRECALL, "425+450" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/400,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,0" },
		  { ZT_TONE_STUTTER, "350+375+400" },
	  },
	},
	{ 22, "lt", "Lithuania", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/350,0/350" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "425/150,0/150,425/150,0/4000" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,0" },
		  /* STUTTER not specified */
		  { ZT_TONE_STUTTER, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
	  },
	},
	{ 23, "pl", "Poland", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/500,0/500" },
		  { ZT_TONE_CALLWAIT, "425/150,0/150,425/150,0/4000" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,0" },
		  /* STUTTER not specified */
		  { ZT_TONE_STUTTER, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
	  },
	},
	{ 24, "za", "South Africa", { 400, 200, 400, 2000 },
	  {
		  { ZT_TONE_DIALTONE, "400*33" },
		  { ZT_TONE_BUSY, "400/500,0/500" },
		  { ZT_TONE_RINGTONE, "400*33/400,0/200,400*33/400,0/2000" },
		  { ZT_TONE_CONGESTION, "400/250,0/250" },
		  { ZT_TONE_CALLWAIT, "400*33/250,0/250,400*33/250,0/250,400*33/250,0/250,400*33/250,0/250" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "350+440" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,!950/330,!1400/330,!1800/330,!0/1000,0" },
		  /* STUTTER not specified */
		  { ZT_TONE_STUTTER, "!400*33/100,!0/100,!400*33/100,!0/100,!400*33/100,!0/100,!400*33/100,!0/100,!400*33/100,!0/100,!400*33/100,!0/100,400*33" },
	  },
	},
	{ 25, "pt", "Portugal", { 1000, 5000 },
	  {
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/5000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "425/200,425/200,425/200,0/5000" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "425/1000,0/200" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "950/330,1400/330,1800/330,0/1000" },
		  /* STUTTER not specified */
		  { ZT_TONE_STUTTER, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
	  },
	},
	{ 26, "ee", "Estonia", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" }, 
		  { ZT_TONE_BUSY, "425/300,0/300" }, 
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" }, 
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "950/650,0/325,950/325,0/30,1400/1300,0/2600" }, 
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "425/650,0/25" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "950/0,0/325,950/325,0/30,1400/1300,0/2600" },
		  /* STUTTER not specified */
		  { ZT_TONE_STUTTER, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
	  },
	},
	{ 27, "mx", "Mexico", { 2000, 4000 }, 
	  {
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/250,0/250" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/250,0/250" },
		  { ZT_TONE_CALLWAIT, "425/200,0/600,425/200,0/10000" },
		  { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  { ZT_TONE_INFO, "950/330,0/30,1400/330,0/30,1800/330,0/1000" },
		  { ZT_TONE_STUTTER, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
	  },
	},
	{ 28, "in", "India", {  400, 200, 400, 2000 }, 
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "400*25" },
		  { ZT_TONE_BUSY, "400/750,0/750" },
		  { ZT_TONE_RINGTONE, "400*25/400,0/200,400*25/400,0/2000" },
		  { ZT_TONE_CONGESTION, "400/250,0/250" },
		  { ZT_TONE_CALLWAIT, "400/200,0/100,400/200,0/7500" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		  /* INFO - not specified */
		  { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,0/1000" },
		  /* STUTTER - not specified */
		  { ZT_TONE_STUTTER, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
	  },
	},
	{ 29, "de", "Germany", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/480,0/480" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/240,0/240" },
		  { ZT_TONE_CALLWAIT, "!425/200,!0/200,!425/200,!0/5000,!425/200,!0/200,!425/200,!0/5000,!425/200,!0/200,!425/200,!0/5000,!425/200,!0/200,!425/200,!0/5000,!425/200,!0/200,!425/200,0" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/80,0/15000" },
		  { ZT_TONE_INFO, "950/330,1400/330,1800/330,0/1000" },
		  { ZT_TONE_STUTTER, "425+400" },
	  },
	},
	{ 30, "ch", "Switzerland", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "425/200,0/200,425/200,0/4000" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/80,0/15000" },
		  { ZT_TONE_INFO, "950/330,1400/330,1800/330,0/1000" },
		  { ZT_TONE_STUTTER, "425+340/1100,0/1100" },
	  },
	},
	{ 31, "dk", "Denmark", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425" },
		  { ZT_TONE_BUSY, "425/500,0/500" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/200,0/200" },
		  { ZT_TONE_CALLWAIT, "!425/200,!0/600,!425/200,!0/3000,!425/200,!0/200,!425/200,0" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/80,0/15000" },
		  { ZT_TONE_INFO, "950/330,1400/330,1800/330,0/1000" },
		  /* STUTTER - not specified */
		  { ZT_TONE_STUTTER, "425/450,0/50" },
	  },
	},
	{ 32, "cz", "Czech Republic", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "425/330,0/330,425/660,0/660" },
		  { ZT_TONE_BUSY, "425/330,0/330" },
		  { ZT_TONE_RINGTONE, "425/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "425/165,0/165" },
		  { ZT_TONE_CALLWAIT, "425/330,0/9000" },
		  /* DIALRECALL - not specified */
		  { ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425/330,0/330,425/660,0/660" },
		  /* RECORDTONE - not specified */
		  { ZT_TONE_RECORDTONE, "1400/500,0/14000" },
		  { ZT_TONE_INFO, "950/330,0/30,1400/330,0/30,1800/330,0/1000" },
		  /* STUTTER - not specified */
		  { ZT_TONE_STUTTER, "425/450,0/50" },
	  },
	},
	{ 33, "cn", "China", { 1000, 4000 },
	  {
		  /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		  { ZT_TONE_DIALTONE, "450" },
		  { ZT_TONE_BUSY, "450/350,0/350" },
		  { ZT_TONE_RINGTONE, "450/1000,0/4000" },
		  { ZT_TONE_CONGESTION, "450/700,0/700" },
		  { ZT_TONE_CALLWAIT, "450/400,0/4000" },
		  { ZT_TONE_DIALRECALL, "450" },
		  { ZT_TONE_RECORDTONE, "950/400,0/10000" },
		  { ZT_TONE_INFO, "450/100,0/100,450/100,0/100,450/100,0/100,450/400,0/400" },
		  /* STUTTER - not specified */
		  { ZT_TONE_STUTTER, "450+425" },
	  },
	},
	{ 34, "ar", "Argentina", { 1000, 4500 },
	  {
		{ ZT_TONE_DIALTONE, "425" },
		{ ZT_TONE_BUSY, "425/300,0/300" },
		{ ZT_TONE_RINGTONE, "425/1000,0/4500" },
		{ ZT_TONE_CONGESTION, "425/200,0/300" },
		{ ZT_TONE_CALLWAIT, "425/200,0/9000" },
		{ ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425/330,0/330,425/660,0/660" },
		{ ZT_TONE_RECORDTONE, "1400/500,0/14000" },
		{ ZT_TONE_INFO, "425/100,0/100" },
		{ ZT_TONE_STUTTER, "425/450,0/50" },
	  },
	},
	{ 35, "my", "Malaysia", { 400, 200, 400, 2000 },
  	  {
        	{ ZT_TONE_DIALTONE, "425" },
		{ ZT_TONE_BUSY, "425/500,0/500" },
		{ ZT_TONE_RINGTONE, "425/400,0/200,425/400,0/2000" },
		{ ZT_TONE_CONGESTION, "425/500,0/500" },
		{ ZT_TONE_CALLWAIT, "425/100,0/4000" },
		{ ZT_TONE_DIALRECALL, "350+440" },
		{ ZT_TONE_RECORDTONE, "1400/500,0/60000" },
		{ ZT_TONE_INFO, "950/330,0/15,1400/330,0/15,1800/330,0/1000" },
		{ ZT_TONE_STUTTER, "450+425" },
	  },
	},
	{ 36, "th", "Thailand", { 1000, 4000 },
          {
		/* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		{ ZT_TONE_DIALTONE,   "400*50" },
		{ ZT_TONE_BUSY,       "400/500,0/500" },
		{ ZT_TONE_RINGTONE,   "400/1000,0/4000" },
		{ ZT_TONE_CONGESTION, "400/300,0/300" },
		{ ZT_TONE_CALLWAIT,   "1000/400,10000/400,1000/400" },
		/* DIALRECALL - not specified - use special dial tone instead. */
		{ ZT_TONE_DIALRECALL, "400*50/400,0/100,400*50/400,0/100" },
		/* RECORDTONE - not specified */
		{ ZT_TONE_RECORDTONE, "1400/500,0/15000" },
		/* INFO - specified as an announcement - use tones instead. */
		{ ZT_TONE_INFO,       "950/330,1400/330,1800/330" },
		/* STUTTER - not specified */
		{ ZT_TONE_STUTTER,    "!400/200,!0/200,!400/600,!0/200,!400/200,!0/200,!400/600,!0/200,!400/200,!0/200,!400/600,!0/200,!400/200,!0/200,!400/600,!0/200,400" },
	  },
	},
	{ 37, "bg", "Bulgaria", { 1000, 4000 },
          {
		/* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
		{ ZT_TONE_DIALTONE,   "425" },
		{ ZT_TONE_BUSY,       "425/500,0/500" },
		{ ZT_TONE_RINGTONE,   "425/1000,0/4000" },
		{ ZT_TONE_CONGESTION, "425/250,0/250" },
		{ ZT_TONE_CALLWAIT,   "425/150,0/150,425/150,0/4000" },
		{ ZT_TONE_DIALRECALL, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
		{ ZT_TONE_RECORDTONE, "1400/425,0/15000" },
		{ ZT_TONE_INFO,       "950/330,1400/330,1800/330,0/1000" },
		{ ZT_TONE_STUTTER,    "425/1500,0/100" },
	  },
	},
        { 38, "ve", "Venezuela", { 1000, 4000 },
          {
            /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
            { ZT_TONE_DIALTONE, "425" },
            { ZT_TONE_BUSY, "425/500,0/500" },
            { ZT_TONE_RINGTONE, "425/1000,0/4000" },
            { ZT_TONE_CONGESTION, "425/250,0/250" },
            { ZT_TONE_CALLWAIT, "400+450/300,0/6000" },
            { ZT_TONE_DIALRECALL, "425" },
            { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
            { ZT_TONE_INFO, "!950/330,!1440/330,!1800/330,0/1000" },
            /* STUTTER - not specified */
            { ZT_TONE_STUTTER, "!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,!425/100,!0/100,425" },
          },
        },
        { 39, "ph", "Philippines", { 1000, 4000 },
          {
            /* Reference: http://www.itu.int/ITU-T/inr/forms/files/tones-0203.pdf */
            { ZT_TONE_DIALTONE, "425" },
            { ZT_TONE_BUSY, "480+620/500,0/500" },
            { ZT_TONE_RINGTONE, "425+480/1000,0/4000" },
            { ZT_TONE_CONGESTION, "480+620/250,0/250" },
            { ZT_TONE_CALLWAIT, "440/300,0/10000" },
            /* DIAL RECALL - not specified */
            { ZT_TONE_DIALRECALL, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
            /* RECORD TONE - not specified */
            { ZT_TONE_RECORDTONE, "1400/500,0/15000" },
            /* INFO TONE - not specified */
            { ZT_TONE_INFO, "!950/330,!1400/330,!1800/330,0" },
            /* STUTTER TONE - not specified */
            { ZT_TONE_STUTTER, "!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,!350+440/100,!0/100,350+440" },
          },
        },
	{ -1 }
};
