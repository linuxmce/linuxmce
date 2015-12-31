/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 * mythconfig.c
 *
 * Copyright © 2006 Silicondust Engineering Ltd. <www.silicondust.com>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "hdhomerun.h"
#ifdef WIN32
	#include <mysql.h>
	int channelscan_execute(struct hdhomerun_device_t *hd, channelscan_callback_t callback, ...) { return 0; }
	unsigned int hdhomerun_device_get_tuner(struct hdhomerun_device_t *hd) { return 0; }
	uint32_t hdhomerun_device_get_device_id(struct hdhomerun_device_t *hd) { return 0; }
	void hdhomerun_device_destroy(struct hdhomerun_device_t *hd) { return; }
	struct hdhomerun_device_t *hdhomerun_device_create_from_str(const char *device_str) { return NULL; }
	void hdhomerun_device_set_tuner(struct hdhomerun_device_t *hd, unsigned int tuner) { return; }
	int hdhomerun_device_set_tuner_from_str(struct hdhomerun_device_t *hd, const char *tuner_str) { return 0; }
#else
	#include <mysql/mysql.h>
	void Sleep(int miliseconds)
	{
		struct timeval t;
		t.tv_sec = (miliseconds / 1000);
		t.tv_usec = (miliseconds % 1000) * 1000;
		while (t.tv_sec > 0 || t.tv_usec > 0)
			select(0, NULL, NULL, NULL, &t);
	}
#endif

static const char appname[] = "mythconfig";
static MYSQL *g_pMySQL = NULL;
static uint32_t g_Frequency = 0;
static char g_szCurrentScan[1000];
static char g_szCallback_Lock[1000];
static int g_mplexid = 0;
static int g_scan_total_count = 200; // For keeping track of percent complete
static int g_scan_current_count = 1; // For keeping track of percent complete
static int g_PK_Device = 0;


#define PARM_PK_DEVICE	1
#define PARM_ID			2
#define PARM_TUNER		3
#define PARM_IP			4
#define PARM_USER		5
#define PARM_PASS		6
#define PARM_DB			7

#define MYSQLPORT	3306

static int help(void)
{
	printf("Usage:\n");
	printf("\t%s <PK_Device> <id> <tuner> <ip of mysql> <mysql username> <mysql password> <mysql database>\n", appname);
	printf("\tPK_Device is the pluto device id\n");
	printf("\tid can be the device ID or the IP address of the HDHomeRun\n");
	printf("\ttuner can be \"ALL\" to scan all tuners\n");
	printf("\t<ip of mysql> is often localhost\n");
	printf("\t<mysql password> can be \"\" if there is no password\n");
	printf("\t<mysql database> is normally \"mythconverg\"\n");
	return 1;
}

#if 0
int add_atsc_channel(int atsc_major_channel, int atsc_minor_channel)
{
	char sSQL[1000];
	sprintf(sSQL,
		"SELECT chanid FROM channel WHERE sourceid=%d AND atsc_major_chan=%d AND atsc_minor_chan=%d",
		g_sourceid, atsc_major_channel, atsc_minor_channel);

	int iresult;
	MYSQL_RES *pMYSQL_RES;
	MYSQL_ROW row;
	if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 || (pMYSQL_RES = mysql_store_result(g_pMySQL))==NULL )
	{
		printf("Error executing query: %s\n",sSQL);
		return 1;
	}

	if( pMYSQL_RES->row_count>0 )
	{
		printf("atsc_major_chan=%d atsc_minor_chan=%d already in the database\n",
			atsc_major_channel, atsc_minor_channel);
		return 1;
	}

	// Nick -- how do you assign channel id's??
	sprintf(sSQL,
		"INSERT INTO channel(sourceid,atsc_major_chan,atsc_minor_chan) VALUES(%d,%d,%d)",
			g_sourceid, atsc_major_channel, atsc_minor_channel);

	if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
	{
		printf("Cannot execute query %s\n", sSQL);
		return 1;
	}
	return 1;
}

int add_qam_channel(char *pFrequency)
{
	return 0;
}
#endif

// Will get called back with the frequency and the number of the channel
// Doesn't mean that it's matched, just means it's testing and we're 
// storing the frequency in g_Frequency
static int cmd_scan_callback_scanning(const char *str)
{
	struct tm *newtime;
	time_t aclock;
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form 
	printf("%s Scanning %s\n", asctime( newtime ),str);

	strcpy(g_szCurrentScan,str);
	unsigned long frequency;
	if (sscanf(str, "%lu", &frequency) != 1) {
		g_Frequency = 0;
		return 1;
	}

	g_Frequency = (uint32_t)frequency;
	return 1;
}

// This means it found something, either QAM or ATSC
static int cmd_scan_callback_lock(uint32_t DeviceID, unsigned int Tuner, int cardid, int sourceid, int cardinputid, const char *str)
{
	if (g_Frequency == 0) {
		return 1;
	}

	g_scan_current_count++;

	strcpy(g_szCallback_Lock,str);

	char lock[32];
	if (sscanf(g_szCallback_Lock, "%32s", lock) != 1) {
		return 1;
	}

	char Message[1000];

	if (strcmp(lock, "none") == 0) {
		/* No signal */
		sprintf(Message,
			"/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 %d 20 0 30 %d 13 \"%lu: no signal\"",
			g_PK_Device,g_scan_current_count * 100 / g_scan_total_count,(unsigned long)g_Frequency);
printf("%s\n",Message);
		system(Message);
		printf("%lu: no signal\n", (unsigned long)g_Frequency);
		return 1;
	}

	if (lock[0] == '(') {
		/* Unsupported signal */
		sprintf(Message,
			"/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 %d 20 0 30 %d 13 \"%lu: unsupported signal\"",
			g_PK_Device,g_scan_current_count * 100 / g_scan_total_count,(unsigned long)g_Frequency);
printf("%s\n",Message);
		system(Message);
		printf("%lu: unsupported signal\n", (unsigned long)g_Frequency);
		return 1;
	}

	/* Valid signal - put it into the database in the dtv_multiplex table */
	char sSQL[1000];
	sprintf(sSQL, "SELECT mplexid FROM dtv_multiplex WHERE frequency=%d AND sourceid=%d",g_Frequency,sourceid);
printf("sql123: %s\n",sSQL);
	MYSQL_RES *pMYSQL_RES;
	MYSQL_ROW row;
	int iresult;
	if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 || (pMYSQL_RES = mysql_store_result(g_pMySQL))==NULL )
	{
		printf("Error executing query: %s\n",sSQL);
		return 1;
	}
printf("123: %d %d %d\n",sSQL[0],sSQL[1],sSQL[2]);
printf("%d recs %s\n",(int) pMYSQL_RES->row_count,sSQL);
	if( pMYSQL_RES->row_count>0 )
	{
		row = mysql_fetch_row( pMYSQL_RES );
		g_mplexid = atoi(row[0]);
	}
	else
	{
		sprintf(sSQL, "INSERT INTO dtv_multiplex(frequency,sourceid) VALUES(%d,%d)", g_Frequency,sourceid);
		if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
		{
			printf("Cannot execute query %s\n", sSQL);
			return 1;
		}
		g_mplexid = (int) mysql_insert_id(g_pMySQL);
	}

	char *pModulation = NULL, *pSisStandard=NULL;
	if( strstr(g_szCallback_Lock,"8vsb") )
	{
		pModulation = "8vsb";
		pSisStandard = "atsc";
	}
	else if( strstr(g_szCallback_Lock,"qam256") )
	{
		pModulation = "qam_256";
		pSisStandard = "atsc";
	}

	if( pModulation==NULL )
	{
		printf("Warning.  Unknown modulation using 8vsb\n");
		pModulation = "8vsb";
	}

	if( pSisStandard==NULL )
	{
		printf("Warning.  Unknown SisStadard using atsc\n");
		pSisStandard = "atsc";
	}

	sprintf(sSQL, "UPDATE dtv_multiplex SET inversion=NULL,fec=NULL,modulation='%s',bandwidth=NULL,lp_code_rate=NULL,"
		"transmission_mode=NULL,guard_interval=NULL,constellation=NULL,hierarchy=NULL,hp_code_rate=NULL,"
		"sistandard='%s' WHERE mplexid=%d",pModulation,pSisStandard,g_mplexid);
	if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
	{
		printf("Error executing query: %s\n",sSQL);
		return 1;
	}

	printf("Setting standard: %s\n",sSQL);

	sprintf(Message,
		"/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 %d 20 0 30 %d 13 \"%s:%lu signal found\n\"",
		g_PK_Device,g_scan_current_count * 100 / g_scan_total_count,lock, (unsigned long)g_Frequency);
printf("%s\n",Message);
	printf("%s:%lu signal found: sql: %s\n", lock, (unsigned long)g_Frequency, sSQL);
	return 1;
}

static int cmd_scan_callback_program(uint32_t DeviceID, unsigned int Tuner, int cardid, int sourceid, int cardinputid, const char *str)
{
	if (g_Frequency == 0) {
		return 1;
	}

	if( strstr(str,"encrypted")!=NULL )
	{
		printf("Skipping encrypted channel: %s\n", str);
		return 1;
	}
	else if( strstr(str,"no data")!=NULL )
	{
		printf("Skipping empty channel: %s\n", str);
		return 1;
	}

	char str_copy[1000],CallSign[100];
	strcpy(str_copy,str);

	int serviceid = atoi(str_copy);
	char *pChanId = strchr(str_copy,':');
	if( !pChanId )
	{
		printf("Can't find the channel id in %s\n", str);
		return 1;
	}
	*pChanId=0; // Null terminate it
	pChanId+=2;
	char *pCallSign = strchr(pChanId,' ');
	if( !pCallSign )
	{
		printf("Can't find the call sign in %s\n", str);
		sprintf(CallSign,"UNKNOWN");
		pCallSign=CallSign;
	}
	else
		*pCallSign++=0; // Null terminate it

	printf("current scan: %s\n%s\nservice id: %d chanid: %s callsign %s\n",g_szCurrentScan,g_szCallback_Lock,serviceid,pChanId,pCallSign);
	printf("PROGRAM: %s\n", str);

	char sSQL[1000];
	int iresult;
	MYSQL_RES *pMYSQL_RES;
	if( strstr(g_szCallback_Lock,"8vsb") )
	{
		char *pDot = strchr(pChanId,'.');
		if( !pDot )
		{
			printf("chanid %s malformed\n",pChanId);
			return 1;
		}

		char szChanName[10];
		strcpy(szChanName,pChanId);
		*pDot++ = 0;

		char szMythChanId[10];
		sprintf(szMythChanId,"%d%s%s", sourceid, pChanId, pDot);

		sprintf(sSQL,
			"SELECT chanid FROM channel WHERE chanid='%s'",
			szMythChanId);

		if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 || (pMYSQL_RES = mysql_store_result(g_pMySQL))==NULL )
		{
			printf("Error executing query: %s\n",sSQL);
			return 1;
		}

		char *pFreqId = strstr(g_szCurrentScan,"us-bcast:");
		if( !pFreqId )
		{
			printf("Can't find channel in: %s\n",g_szCallback_Lock);
			return 1;
		}

		pFreqId += 9;

		// Be sure the record is in the database
		if( pMYSQL_RES->row_count==0 )
		{
			sprintf(sSQL,
				"INSERT INTO channel(chanid) VALUES('%s')",
				szMythChanId);
			if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
			{
				printf("Cannot execute query %s\n", sSQL);
				return 1;
			}
		}

		sprintf(sSQL,
			"UPDATE channel SET channum='%s',freqid=%d,sourceid=%d,callsign='%s',name='%s',tvformat='ATSC',mplexid=%d,serviceid=%d,"
			"atsc_major_chan=%s,atsc_minor_chan=%s"
			" WHERE chanid='%s'"
			,szChanName,atoi(pFreqId),sourceid,pCallSign,pCallSign,g_mplexid,serviceid
			,pChanId,pDot,szMythChanId);
		if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
		{
			printf("Cannot execute query %s\n", sSQL);
			return 1;
		}
printf("Updated: %s\n",sSQL);
	}
	else if( strstr(g_szCallback_Lock,"qam256") )
	{
		char *pFreqId;
		int szLen;

		pFreqId = strstr(g_szCurrentScan,"us-cable:");
		szLen = 9;

		if( !pFreqId )
		{
			// If we can't find the channel by parsing us-cable: lets look for hrc, since its on its own output
			pFreqId = strstr(g_szCurrentScan,"us-hrc:");
			szLen = 7;
		}

		if( !pFreqId )
		{
			// No channel id found 
			printf("Can't find channel in: %s\n",g_szCallback_Lock);
			return 1;
		}
		else
			pFreqId += szLen;

		char szMythChanId[40];
		sprintf(szMythChanId,"%d%d%d", sourceid, atoi(pFreqId), serviceid);

		sprintf(sSQL,
			"SELECT chanid FROM channel WHERE chanid='%s'",
			szMythChanId);

		if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 || (pMYSQL_RES = mysql_store_result(g_pMySQL))==NULL )
		{
			printf("Error executing query: %s\n",sSQL);
			return 1;
		}

		char szChanName[100];
		sprintf(szChanName,"%d.%d", atoi(pFreqId), serviceid);

		// Be sure the record is in the database
		if( pMYSQL_RES->row_count==0 )
		{
			sprintf(sSQL,
				"INSERT INTO channel(chanid) VALUES('%s')",
				szMythChanId);
			if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
			{
				printf("Cannot execute query %s\n", sSQL);
				return 1;
			}
		}

		// This is just an unknown.  Make it clearer now
		if( pCallSign==CallSign )
			sprintf(CallSign,"UNKNOWN_%d.%d", atoi(pFreqId), serviceid);

		sprintf(sSQL,
			"UPDATE channel SET channum='%s',freqid=%d,sourceid=%d,callsign='%s',name='%s',mplexid=%d,serviceid=%d"
			" WHERE chanid='%s'"
			,szChanName,atoi(pFreqId),sourceid,pCallSign,pCallSign,g_mplexid,serviceid
			,szMythChanId);
		if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
		{
			printf("Cannot execute query %s\n", sSQL);
			return 1;
		}
printf("Updated: %s\n",sSQL);
	}

	return 1;
}

static int cmd_scan_callback(va_list ap, const char *type, const char *str)
{
	uint32_t DeviceID = va_arg(ap, uint32_t);
	unsigned int Tuner = va_arg(ap, unsigned int);
	int cardid = va_arg(ap, int);
	int sourceid = va_arg(ap, int);
	int cardinputid = va_arg(ap, int);

	printf("cmd_scan_callback_lock DeviceID %d, Tuner %d, cardid %d, sourceid %d, cardinputid %d, type: %s str: %s\n", DeviceID, Tuner, cardid, sourceid, cardinputid, type, str);

	if (strcmp(type, "SCANNING") == 0) {
		return cmd_scan_callback_scanning(str);
	}
	if (strcmp(type, "LOCK") == 0) {
		return cmd_scan_callback_lock(DeviceID, Tuner, cardid, sourceid, cardinputid, str);
	}
	if (strcmp(type, "PROGRAM") == 0) {
		return cmd_scan_callback_program(DeviceID, Tuner, cardid, sourceid, cardinputid, str);
	}

	return 1;
}

int myth_scan_tuner_wrapper(struct hdhomerun_device_t *pHD, int cardid, int sourceid, int cardinputid )
{
	uint32_t DeviceID = hdhomerun_device_get_device_id(pHD);
	unsigned int Tuner = hdhomerun_device_get_tuner(pHD);

	printf("Scanning HDHomeRun %08lX Tuner %u cardid %d sourceid %d cardinputid %d\n",
		(unsigned long)DeviceID, Tuner, cardid, sourceid, cardinputid);

	return channelscan_execute(pHD, cmd_scan_callback, DeviceID, Tuner, cardid, sourceid, cardinputid);
}

// Replace pHD with the instance of the HD Home Run
static int myth_scan_tuner(struct hdhomerun_device_t *pHD)
{
	uint32_t DeviceID = hdhomerun_device_get_device_id(pHD);
	unsigned int Tuner = hdhomerun_device_get_tuner(pHD);

	// Find the tuner in myth's database
	char sSQL[1000];
	sprintf(sSQL,
		"SELECT capturecard.cardid,cardinput.cardinputid,cardinput.sourceid FROM capturecard "
		"LEFT JOIN cardinput ON capturecard.cardid=cardinput.cardid "
		"WHERE videodevice='%08lX-%u' AND cardtype='HDHOMERUN'",
		(unsigned long)DeviceID, Tuner);


	time_t timeout = time(NULL) + 60;  // Wait up to 60 seconds for the HD Home Run device to get into the database
	int iresult;
	MYSQL_RES *pMYSQL_RES=NULL;
	MYSQL_ROW row=NULL;

	while(time(NULL) < timeout)
	{
		if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 || (pMYSQL_RES = mysql_store_result(g_pMySQL))==NULL )
		{
			printf("Error executing query: %s\n",sSQL);
			return 1;
		}

		row = mysql_fetch_row( pMYSQL_RES );
		if( row==NULL )
		{
			struct tm *newtime;
			time_t aclock;
			time( &aclock );   // Get time in seconds
			newtime = localtime( &aclock );   // Convert time to struct tm form 

			printf("%s Card %08lX tuner %u not yet in the mythtv-setup database.  Wait 10 seconds\n", asctime( newtime ), (unsigned long)DeviceID, Tuner);
			Sleep(10000);
			continue;  // try again
		}
		else
			break;
	}
	if( row==NULL )
	{
		printf("Skipping card %08lX tuner %u because it's not in the mythtv-setup database\n", (unsigned long)DeviceID, Tuner);
		return 0;  // Not an error, since we may keep scanning
	}

	int cardid = atoi( row[0] );

	int sourceid=0;
	if( row[2] )
		sourceid = atoi(row[2]);
	if( sourceid==0 )
	{
		sprintf(sSQL, "SELECT sourceid FROM videosource WHERE name='UNKNOWN_%d'",cardid);
		MYSQL_RES *pMYSQL_RES;
		MYSQL_ROW row;
		if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 || (pMYSQL_RES = mysql_store_result(g_pMySQL))==NULL
			|| (row = mysql_fetch_row(pMYSQL_RES))==NULL )
		{
			printf("card %08lX tuner %u has no existing source %s\n", (unsigned long)DeviceID, Tuner, sSQL);
			sprintf(sSQL, "INSERT INTO videosource(name,xmltvgrabber) VALUES('UNKNOWN_%d','/bin/true')",cardid);
			if( (iresult=mysql_query(g_pMySQL,sSQL))!=0 )
			{
				printf("Cannot execute query %s\n", sSQL);
				return 1;
			}
			sourceid = (int) mysql_insert_id(g_pMySQL);
		}
		else
		{
			sourceid = atoi(row[0]);
			printf("card %08lX tuner %u has an existing unknown source %d\n", (unsigned long)DeviceID, Tuner, sourceid);
		}
	}

	int cardinputid=0;
	if( row[1] )
		cardinputid = atoi(row[2]);

	if( cardid==0 || sourceid==0 )
	{
		printf("Error getting card id, sourceid, and cardinput id\n");
		return 1;
	}

	int result = myth_scan_tuner_wrapper(pHD, cardid, sourceid, cardinputid );
	if( result <= 0) {
		printf("myth_scan_tuner failed\n");
	} else {
		printf("myth_scan_tuner SUCCESS!\n");
	}
	return result;
}

static int myth_scan_hdhomerun(struct hdhomerun_device_t *pHD, const char *pTuner)
{
	struct tm *newtime;
	time_t aclock;
	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form 
	printf("%s Starting\n", asctime( newtime ));

	if (strcasecmp(pTuner, "ALL") != 0) {
		if (hdhomerun_device_set_tuner_from_str(pHD, pTuner) <= 0) {
			fprintf(stderr, "invalid tuner: %s\n", pTuner);
			return 1;
		}
		return myth_scan_tuner(pHD);
	}

	for (unsigned int tuner = 0; tuner < 2; tuner++) {
		hdhomerun_device_set_tuner(pHD, tuner);
		int result = myth_scan_tuner(pHD);
		if (result < 0) {
			return result;
		}
	}

	return 1;
}

static int myth_scan(const char *pID, const char *pTuner)
{
	struct hdhomerun_device_t *pHD = hdhomerun_device_create_from_str(pID);
	if (!pHD) {
		fprintf(stderr, "invalid device id: %s\n", pID);
		return 1;
	}

	int result = myth_scan_hdhomerun(pHD, pTuner);
	if (result <= 0) {
		hdhomerun_device_destroy(pHD);
		fprintf(stderr, "myth_scan_hdhomerun failed\n");
		return result;
	}

	hdhomerun_device_destroy(pHD);
	return 1;
}

static int MySqlConnect(char *ip,char *user,char *password,char *database)
{
	my_bool reconnect = true;
	if( !g_pMySQL )
		g_pMySQL = mysql_init(NULL);

	mysql_options(g_pMySQL, MYSQL_OPT_RECONNECT, &reconnect);
	if (mysql_real_connect(g_pMySQL, ip, user, password, database, MYSQL_PORT, NULL, 0) == NULL)
		return 1; // Failure
	return 1; // Ok
}

int main(int argc, char *argv[])
{
	if( argc!=8 )
	{
		help();
		return 1;
	}

	if( MySqlConnect( argv[PARM_IP], argv[PARM_USER], argv[PARM_PASS], argv[PARM_DB]) <= 0 )
	{
		fprintf(stderr, "Failed to connect to MySql\n");
		return 1;
	}

	g_PK_Device = atoi(argv[PARM_PK_DEVICE]);
/*
g_Frequency=633000000;
g_mplexid=68;
strcpy(g_szCurrentScan,"633000000 (us-cable:92, us-irc:92)");
strcpy(g_szCallback_Lock,"qam256 (ss=98 snq=90 seq=100)");
cmd_scan_callback_program(93,1,18,8,6, "2: 0.0");
*/
	int result = myth_scan( argv[PARM_ID], argv[PARM_TUNER] );

	if( g_pMySQL ) {
		mysql_close(g_pMySQL);
	}

	if( result <= 0 )
	{
		printf("myth_scan failed\n");
		return 1;
	}

	return 0;
}
