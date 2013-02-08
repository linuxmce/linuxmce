#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <zlib.h>

extern "C"
{
#include <mysql/mysql.h>
}

#define Stations "/var/www/tribune/statrec.txt"
#define Headends "/var/www/tribune/headend.fsf"
#define Lineups "/var/www/tribune/lineup.fsf"
#define ProgramRecords "/var/www/tribune/progrec.txt"
#define Schedules "/var/www/tribune/skedrec.txt"

using namespace std;


struct actors_t {
  	string firstname;
  	string lastname;
	string role;
};

struct genre_t{
	string description;
	int level;
};

MYSQL *mysqltribune;
//multimap<string,pair<string,int> > multimapActorName_To_Key;
multimap<string,int> multimapRoleDesc_To_Key;
map<string,string> GenreKey;
map<string,string> ActorKey;
//map<string,string> RoleKey;
//map<string, int > multimapGenreDescription_To_Key;

char *FindDelim(const char *input)
{
	char* pIn = (char*) input;
	if( !pIn )
		return NULL;

	while( *pIn!='|' && *pIn!=0 )
		pIn++;

	if( *pIn=='|' )
            *pIn++=0;

	//return *pIn == 0 ? NULL : pIn;
	return pIn;

}

char *GetChannelID(const char *input)
{
	char line[5*1024];
	sprintf(line,"%s",input);
	const char *pChannelID = line;
	const char *pRest = FindDelim(pChannelID);

	return *pChannelID == 0 ? NULL : (char*) pChannelID;
}

char *GetScheduleID(const char *input)
{
	char line[5*1024], ScheduleID[5*1024];
	sprintf(line,"%s",input);
	const char *pStationID = line;
	const char *pProgramRecordID = FindDelim(pStationID);
	const char *pAirDate = FindDelim(pProgramRecordID);
	const char *pAirTime = FindDelim(pAirDate);
	const char *pRest = FindDelim(pAirTime);

	sprintf(ScheduleID, "%s%s%s", pStationID, pAirDate, pAirTime);

	return ScheduleID;
}

char *GetHeadendID(const char *input)
{
	char line[5*1024], HeadendID[5*1024];
	sprintf(line,"%s",input);
	const char *pHeadendID = line;
	const char *pComunityName = FindDelim(pHeadendID);
	const char *pCountyName = FindDelim(pComunityName);
	const char *pCountySize = FindDelim(pCountyName);
	const char *pStCountyCode = FindDelim(pCountySize);
	const char *pStateServed = FindDelim(pStCountyCode);
	const char *pZipCode = FindDelim(pStateServed);
	const char *pRest = FindDelim(pZipCode);

	sprintf(HeadendID, "%s%s", pHeadendID, pZipCode);

	return HeadendID;
}

const char *EscapeString(string m_headerSubject){

	char * S = new char[strlen(m_headerSubject.c_str())*3 +1];
	mysql_real_escape_string(mysqltribune, S, m_headerSubject.c_str(), m_headerSubject.length());
	m_headerSubject = m_headerSubject.assign(S);
	return m_headerSubject.c_str();
}

string StringToLower(string strToConvert)
{//change each element of the string to lower case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = tolower(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}

unsigned long GetChecksum(const char *line, int size)
{
	//cout <<  "L " << line <<  endl;
	unsigned long crc;
	crc = crc32(0, Z_NULL, 0);
	crc = crc32(crc, (const Bytef *) line, size);

	return crc;
}

void ImportChannel(bool bInsert, const char *pData, unsigned long checksum)
{
	const char *pChannelID = pData;

	const char *pTimeZone = FindDelim(pChannelID);

	const char *pStationName = FindDelim(pTimeZone);

	const char *pStationCallSign = FindDelim(pStationName);
	
	const char *pStationAffill = FindDelim(pStationCallSign);

	const char *pStationCity = FindDelim(pStationAffill);

	const char *pStationState = FindDelim(pStationCity);
	
	const char *pStationZipCode = FindDelim(pStationState);
	
	const char *pStationCountry = FindDelim(pStationZipCode); 

	const char *pDmaName = FindDelim(pStationCountry);

	const char *pDmaNum = FindDelim(pDmaName);

	const char *pChannelNum = FindDelim(pDmaNum);
	
	const char *pRest = FindDelim(pChannelNum);


	if( ! pChannelID || ! pTimeZone || ! pStationName || ! pStationCallSign ||
		! pStationAffill || ! pStationCity || ! pStationState || ! pStationZipCode || ! pStationCountry ) {
		cerr<<"Error parsing string"<<pData<<endl;
		exit(1);
	}

	else {
		char sSQL[10000];

		if( bInsert )
				sprintf(sSQL,"INSERT INTO Station(PK_Station, TimeZone, Name, CallSign, Affil, City, State, ZipCode, Country, DmaName, DmaNum, FccChannelNum, Checksum) VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%lu\")", pChannelID, pTimeZone, pStationName, pStationCallSign, pStationAffill, pStationCity, pStationState, pStationZipCode, pStationCountry, pDmaName, pDmaNum, pChannelNum, checksum);
		else{
				sprintf(sSQL,"UPDATE Station SET TimeZone=\"%s\", Name=\"%s\", CallSign=\"%s\", Affil=\"%s\", City=\"%s\", State=\"%s\", ZipCode=\"%s\", Country=\"%s\", DmaName=\"%s\", DmaNum=\"%s\", FccChannelNum=\"%s\", Checksum=\"%lu\" WHERE PK_Station='%s'",  pTimeZone, pStationName, pStationCallSign, pStationAffill, pStationCity, pStationState, pStationZipCode, pStationCountry, pDmaName, pDmaNum, pChannelNum, checksum, pChannelID);
		}

		if( mysql_query( mysqltribune, sSQL) ){
			cerr<<"query: "<<sSQL<<endl
					<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
			exit(1);
		}
	

	}

}

void ImportChannelTable()
{
	MYSQL_RES *mrestribune;
	MYSQL_ROW rowtribune;
	map<int,unsigned long> mapPrimaryKey_To_Checksum;
	map<int,unsigned long> mapPrimaryKey_To_ChecksumFile;

	string query="SELECT PK_Station,Checksum FROM Station";

	if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) {
		while( (rowtribune=mysql_fetch_row(mrestribune)) ) {
			mapPrimaryKey_To_Checksum[ atoi(rowtribune[0]) ] = atoll(rowtribune[1]);
		}
	}
		
            // Now we have a map of all existing channels in the database with the id and checksum
            // open the text file

	    ifstream stationfile(Stations);
            string line;
	
	if (!stationfile) {
		cerr<<"Error opening file "<<Stations<<endl;
		exit(1);
	}

	if (stationfile.is_open()) {
		while(! stationfile.eof()) {
			getline (stationfile,line);
			
			if (line=="")
				continue;

			unsigned long checksum = GetChecksum(line.c_str(), line.size());
			int chanid = atoi(GetChannelID(line.c_str()));
	
			mapPrimaryKey_To_ChecksumFile[chanid] = checksum;
	
			map<int,unsigned long>::iterator it = mapPrimaryKey_To_Checksum.find(chanid);
		
			if( it == mapPrimaryKey_To_Checksum.end() ) {
				ImportChannel(true,line.c_str(),checksum); // It’s a new row
			} else {
				if(it->second != checksum) {
					ImportChannel(false,line.c_str(),checksum); // It’s an existing row, but the checksum changed
				}
			}
		}
		map<int,unsigned long>::iterator iter;
		for( iter = mapPrimaryKey_To_Checksum.begin(); iter != mapPrimaryKey_To_Checksum.end(); iter++ ) {
			map<int,unsigned long>::iterator it=mapPrimaryKey_To_ChecksumFile.find(iter->first);
			if( it== mapPrimaryKey_To_ChecksumFile.end() ){
				
				char query[100000];
				sprintf(query,"DELETE FROM Station WHERE PK_Station='%s'",iter->first);

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}	
			}
		}
	}

}


//void ImportHeadend(bool bInsert, const char *pData, unsigned long checksum)
void ImportHeadend(const char *pData)
{
	//char HeadendID[5*1024];
	
	const char *pHeadendID = pData;

	const char *pComunityName = FindDelim(pHeadendID);

	const char *pCountyName = FindDelim(pComunityName);

	const char *pCountySize = FindDelim(pCountyName);
	
	const char *pStCountyCode = FindDelim(pCountySize);

	const char *pStateServed = FindDelim(pStCountyCode);

	const char *pZipCode = FindDelim(pStateServed);
	
	const char *pDmaCode = FindDelim(pZipCode);

	//sprintf(HeadendID, "%s%s", pHeadendID, pZipCode);
	
	const char *pDmaName = FindDelim(pDmaCode); 

	const char *pMsoCode = FindDelim(pDmaName);

	const char *pDmaRank = FindDelim(pMsoCode);

	const char *pHeadendName = FindDelim(pDmaRank);

	const char *pHeadendLocation = FindDelim(pHeadendName);

	const char *pMsoName = FindDelim(pHeadendLocation);

	const char *pTimeZoneCode = FindDelim(pMsoName);
	
	const char *pRest = FindDelim(pTimeZoneCode);

	if( ! pHeadendID || ! pComunityName || ! pCountyName || ! pDmaName || 
	    ! pMsoCode || ! pHeadendName || ! pHeadendLocation || ! pMsoName || ! pZipCode) {
		cerr<<"Error parsing string"<<pData<<endl;
		exit(1);
	}

	else {
		char sSQL[10000];

		sprintf(sSQL,"INSERT INTO Headend(HeadendID, CommunityName, CountyName, CountySize, StCountyCode, StateServed, ZipCode, DmaCode, DmaName, MsoCode, DmaRank, HeadendName, HeadendLocation, MsoName, TimeZoneCode) VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")",pHeadendID, pComunityName, pCountyName, pCountySize, pStCountyCode, pStateServed, pZipCode, pDmaCode, pDmaName, pMsoCode, pDmaRank, pHeadendName, pHeadendLocation, pMsoName, pTimeZoneCode);
// 		if (bInsert)
// 			sprintf(sSQL,"INSERT INTO Headend(PK_Headend, HeadendID, CommunityName, CountyName, CountySize, StCountyCode, StateServed, ZipCode, DmaCode, DmaName, MsoCode, DmaRank, HeadendName, HeadendLocation, MsoName, TimeZoneCode, Checksum) VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%lu\")", HeadendID, pHeadendID, pComunityName, pCountyName, pCountySize, pStCountyCode, pStateServed, pZipCode, pDmaCode, pDmaName, pMsoCode, pDmaRank, pHeadendName, pHeadendLocation, pMsoName, pTimeZoneCode, checksum);
// 		else
// 			sprintf(sSQL,"UPDATE Headend SET HeadendID=\"%s\", CommunityName=\"%s\", CountyName=\"%s\", CountySize=\"%s\", StCountyCode=\"%s\", StateServed=\"%s\", ZipCode=\"%s\", DmaCode=\"%s\", DmaName=\"%s\", MsoCode=\"%s\", DmaRank=\"%s\", HeadendName=\"%s\", HeadendLocation=\"%s\", MsoName=\"%s\", TimeZoneCode=\"%s\" Checksum=\"%lu\" WHERE PK_Headend='%s'", pHeadendID, pComunityName, pCountyName, pCountySize, pStCountyCode, pStateServed, pZipCode, pDmaCode, pDmaName, pMsoCode, pDmaRank, pHeadendName, pHeadendLocation, pMsoName, pTimeZoneCode, checksum, HeadendID);

		if( mysql_query( mysqltribune, sSQL) ){
			cerr<<"query: "<<sSQL<<endl
					<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
			exit(1);
		}
	

	}

}

void ImportHeadendTable()
{
	
	string query = "DELETE FROM Headend";

	if( mysql_query( mysqltribune, query.c_str()) ){
		cerr<<"query: "<<query<<endl
			<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
		exit(1);
	}

	ifstream headendfile(Headends);
	string line;
	
	if (!headendfile) {
		cerr<<"Error opening file "<<headendfile<<endl;
		exit(1);
	}

	if (headendfile.is_open()) {
		while(! headendfile.eof()) {
			getline (headendfile,line);
		
			if (line=="")
				continue;

			ImportHeadend(line.c_str()); // It’s a new row
			
			}
	}
	

// 	MYSQL_RES *mrestribune;
// 	MYSQL_ROW rowtribune;
// 	map<string,unsigned long> mapPrimaryKey_To_Checksum;
// 
// 	string query="SELECT PK_Headend,Checksum FROM Headend";
// 
// 	if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) {
// 		while( (rowtribune=mysql_fetch_row(mrestribune)) ) {
// 			mapPrimaryKey_To_Checksum[ rowtribune[0] ] = atoll(rowtribune[1]);
// 		}
// 	}
// 		
//             // Now we have a map of all existing channels in the database with the id and checksum
//             // open the text file
// 
// 	ifstream headendfile(Headends);
// 	string line;
// 	
// 	if (!headendfile) {
// 		cerr<<"Error opening file "<<headendfile<<endl;
// 		exit(1);
// 	}
// 
// 	if (headendfile.is_open()) {
// 		while(! headendfile.eof()) {
// 			getline (headendfile,line);
// 			
// 			if (line=="")
// 				continue;
// 
// 			unsigned long checksum = GetChecksum(line.c_str(), line.size());
// 			string chanid = GetHeadendID(line.c_str());
// 			map<string,unsigned long>::iterator it = mapPrimaryKey_To_Checksum.find(chanid);
// 		
// 			if( it == mapPrimaryKey_To_Checksum.end() ) {
// 				ImportHeadend(true,line.c_str(),checksum); // It’s a new row
// 			} else {
// 				if(it->second != checksum) {
// 					ImportHeadend(false,line.c_str(),checksum); // It’s an existing row, but the checksum changed
// 				}
// 			}
// 		}
// 	}

}

void ImportSchedule(bool bInsert, const char *pData, unsigned long checksum)
{
	char ScheduleID[5*1024];

	const char *pStationID = pData;

	const char *pProgramRecordID = FindDelim(pStationID);

	const char *pAirDate = FindDelim(pProgramRecordID);

	const char *pAirTime = FindDelim(pAirDate);
	
	const char *pDuration = FindDelim(pAirTime);

	sprintf(ScheduleID, "%s%s%s", pStationID, pAirDate, pAirTime);

	const char *pPartNum = FindDelim(pDuration);

	const char *pNumOfParts = FindDelim(pPartNum);
	
	const char *pCCaptioning = FindDelim(pNumOfParts);
	
	const char *pStereo = FindDelim(pCCaptioning); 

	const char *pRepeating = FindDelim(pStereo);

	const char *pLiveTapeDelay = FindDelim(pRepeating);

	const char *pSubtitled = FindDelim(pLiveTapeDelay);

	const char *pPremiereFinale = FindDelim(pSubtitled);

	const char *pJoinedInProgress = FindDelim(pPremiereFinale);

	const char *pCableInTheClassroom = FindDelim(pJoinedInProgress);
	
	const char *pTVRating = FindDelim(pCableInTheClassroom);

	const char *pSap = FindDelim(pTVRating);

	const char *pBlackout = FindDelim(pSap);

	const char *pSexRating = FindDelim(pBlackout);

	const char *pViolenceRating = FindDelim(pSexRating);

	const char *pLanguageRating = FindDelim(pViolenceRating);

	const char *pDialogRating = FindDelim(pLanguageRating);

	const char *pFvRating = FindDelim(pDialogRating);

	const char *pEnhanced = FindDelim(pFvRating);

	const char *pThreeD = FindDelim(pEnhanced);

	const char *pLetterBox = FindDelim(pThreeD);

	const char *pHdtv = FindDelim(pLetterBox);

	const char *pDolby = FindDelim(pHdtv);

	const char *pDvs = FindDelim(pDolby);

	const char *pRest = FindDelim(pDvs);

	if( ! pStationID || ! pProgramRecordID ) {
		cerr<<"Error parsing string"<<pData<<endl;
		exit(1);
	}

	else {
		char sSQL[10000];
		
		if( bInsert )
			sprintf(sSQL,"INSERT INTO Schedule(PK_Schedule, FK_Station, FK_ProgramRecord, AirDate, AirTime, Duration, PartNum, NumOfParts, CCaptioning, Stereo, Repeating, LiveTapeDelay, Subtitled, PremiereFinale, JoinedInProgress, CableInTheClassroom, TVRating, Sap, Blackout, SexRating, ViolenceRating, LanguageRating, DialogRating, FvRating, Enhanced, ThreeD, LetterBox, Hdtv, Dolby, Dvs, Checksum) VALUES (\"%llu\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\", \"%lu\")", atoll(ScheduleID), pStationID, pProgramRecordID, pAirDate, pAirTime, pDuration, pPartNum, pNumOfParts, pCCaptioning, pStereo, pRepeating, pLiveTapeDelay, pSubtitled, pPremiereFinale, pJoinedInProgress, pCableInTheClassroom, pTVRating, pSap, pBlackout, pSexRating, pViolenceRating, pLanguageRating, pDialogRating, pFvRating, pEnhanced, pThreeD, pLetterBox, pHdtv, pDolby, pDvs, checksum);
		else
			sprintf(sSQL,"UPDATE Schedule SET FK_Station=\"%s\", FK_ProgramRecord=\"%s\", AirDate=\"%s\", AirTime=\"%s\", Duration=\"%s\", PartNum=\"%s\", NumOfParts=\"%s\", CCaptioning=\"%s\", Stereo=\"%s\", Repeating=\"%s\", LiveTapeDelay=\"%s\", Subtitled=\"%s\", PremiereFinale=\"%s\", JoinedInProgress=\"%s\", CableInTheClassroom=\"%s\", TVRating=\"%s\", Sap=\"%s\", Blackout=\"%s\", SexRating=\"%s\", ViolenceRating=\"%s\", LanguageRating=\"%s\", DialogRating=\"%s\", FvRating=\"%s\", Enhanced=\"%s\", ThreeD=\"%s\", LetterBox=\"%s\", Hdtv=\"%s\", Dolby=\"%s\", Dvs=\"%s\", Checksum=\"%lu\" WHERE PK_Schedule='%llu'", pStationID, pProgramRecordID, pAirDate, pAirTime, pDuration, pPartNum, pNumOfParts, pCCaptioning, pStereo, pRepeating, pLiveTapeDelay, pSubtitled, pPremiereFinale, pJoinedInProgress, pCableInTheClassroom, pTVRating, pSap, pBlackout, pSexRating, pViolenceRating, pLanguageRating, pDialogRating, pFvRating, pEnhanced, pThreeD, pLetterBox, pHdtv, pDolby, pDvs, checksum, atoll(ScheduleID));

		if( mysql_query( mysqltribune, sSQL) ){
			cerr<<"query: "<<sSQL<<endl
					<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
			exit(1);
		}
	

	}

}

void ImportScheduleTable()
{

	    MYSQL_RES *mrestribune;
	    MYSQL_ROW rowtribune;
            map<unsigned long long,unsigned long> mapPrimaryKey_To_Checksum;
	    map<unsigned long long,unsigned long> mapPrimaryKey_To_ChecksumFile;

            string query="SELECT PK_Schedule,Checksum FROM Schedule";

	    if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) 

		while( (rowtribune=mysql_fetch_row(mrestribune)) ) 

			mapPrimaryKey_To_Checksum[ atoll(rowtribune[0]) ] = atoll(rowtribune[1]);
		

            // Now we have a map of all existing lineups in the database with the id and checksum
            // open the text file

	    ifstream schedulefile(Schedules);
            string line;
	
	    if (!schedulefile) {
		cerr<<"Error opening file "<<Schedules<<endl;
		exit(1);
	    }

	    if (schedulefile.is_open()) {

		while(! schedulefile.eof())
		{
			getline (schedulefile,line);
			
			if (line=="")
				continue;

			unsigned long checksum = GetChecksum(line.c_str(),line.size());

			unsigned long long chanid = atoll(GetScheduleID(line.c_str()));

			mapPrimaryKey_To_ChecksumFile[ chanid ] = checksum;
	
			map<unsigned long long,unsigned long>::iterator it=mapPrimaryKey_To_Checksum.find(chanid);
		
			if( it== mapPrimaryKey_To_Checksum.end() ){
				
				ImportSchedule(true,line.c_str(),checksum); // It’s a new row
			}
			else if( it->second != checksum )
		
				ImportSchedule(false,line.c_str(),checksum); // It’s an existing row, but the checksum changed
		
		}

		map<unsigned long long,unsigned long>::iterator iter;
		for( iter = mapPrimaryKey_To_Checksum.begin(); iter != mapPrimaryKey_To_Checksum.end(); iter++ ) {
			map<unsigned long long,unsigned long>::iterator it=mapPrimaryKey_To_ChecksumFile.find(iter->first);
			if( it== mapPrimaryKey_To_ChecksumFile.end() ){

				cout<<"Sterg " << iter->first << endl;
				
				char query[100000];
				sprintf(query,"DELETE FROM Schedule WHERE PK_Schedule='%llu'",iter->first);

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}	
			}
		}
	    }

}

void ImportLineup(bool bInsert, const char *pData, unsigned long checksum)
{
	const char *pLineupID = pData;

	const char *pHeadendID = FindDelim(pLineupID);

	const char *pDevice = FindDelim(pHeadendID);

	const char *pStationID = FindDelim(pDevice);
	
	const char *pTmsChan = FindDelim(pStationID);

	const char *pServiceTier = FindDelim(pTmsChan);

	const char *pEffectiveDate = FindDelim(pServiceTier);
	
	const char *pExpirationDate = FindDelim(pEffectiveDate);
	
	const char *pRest = FindDelim(pExpirationDate);

	if( ! pLineupID || ! pHeadendID || ! pStationID || ! pTmsChan ||
		! pServiceTier || ! pEffectiveDate) {
		cerr<<"Error parsing string"<<pData<<endl;
		exit(1);
	}

	else {
		char sSQL[10000];

		if( bInsert )
				sprintf(sSQL,"INSERT INTO Lineup(PK_Lineup, HeadendID, Device, FK_Station,TmsChan, ServiceTier, EffectiveDate, ExpirationDate, Checksum) VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%lu\")", pLineupID, pHeadendID, pDevice, pStationID, pTmsChan, pServiceTier, pEffectiveDate, pExpirationDate, checksum);
		else
				sprintf(sSQL,"UPDATE Lineup SET HeadendID=\"%s\", Device=\"%s\", FK_Station=\"%s\", TmsChan=\"%s\", ServiceTier=\"%s\", EffectiveDate=\"%s\", ExpirationDate=\"%s\", Checksum=\"%lu\" WHERE PK_Lineup='%s'", pHeadendID, pDevice, pStationID, pTmsChan, pServiceTier, pEffectiveDate, pExpirationDate, checksum, pLineupID);

		if( mysql_query( mysqltribune, sSQL) ){
			cerr<<"query: "<<sSQL<<endl
					<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
			exit(1);
		}
	

	}

}

void ImportLineupTable()
{
	    MYSQL_RES *mrestribune;
	    MYSQL_ROW rowtribune;
            map<int,unsigned long> mapPrimaryKey_To_Checksum;
	    map<int,unsigned long> mapPrimaryKey_To_ChecksumFile;

            string query="SELECT PK_Lineup,Checksum FROM Lineup";

	    if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) 

		while( (rowtribune=mysql_fetch_row(mrestribune)) ) 

			mapPrimaryKey_To_Checksum[ atoi(rowtribune[0]) ] = atoll(rowtribune[1]);
		

            // Now we have a map of all existing lineups in the database with the id and checksum
            // open the text file

	    ifstream lineupfile(Lineups);
            string line;
	
	    if (!lineupfile) {
		cerr<<"Error opening file "<<Stations<<endl;
		exit(1);
	    }

	    if (lineupfile.is_open()) {

		while(! lineupfile.eof())
		{
			getline (lineupfile,line);
			
			if (line=="")
				continue;

			unsigned long checksum = GetChecksum(line.c_str(),line.size());

			int chanid = atoi(GetChannelID(line.c_str()));

			mapPrimaryKey_To_ChecksumFile[chanid] = checksum;
	
			map<int,unsigned long>::iterator it=mapPrimaryKey_To_Checksum.find(chanid);
		
			if( it== mapPrimaryKey_To_Checksum.end() ){
				
				ImportLineup(true,line.c_str(),checksum); // It’s a new row
			}
			else if( it->second != checksum )
		
				ImportLineup(false,line.c_str(),checksum); // It’s an existing row, but the checksum changed
		
		}

		map<int,unsigned long>::iterator iter;
		for( iter = mapPrimaryKey_To_Checksum.begin(); iter != mapPrimaryKey_To_Checksum.end(); iter++ ) {
			map<int,unsigned long>::iterator it=mapPrimaryKey_To_ChecksumFile.find(iter->first);
			if( it== mapPrimaryKey_To_ChecksumFile.end() ){
				
				char query[100000];
				sprintf(query,"DELETE FROM Lineup WHERE PK_Lineup='%s'",iter->first);

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}	
			}
		}
	    }

}

void ImportGenreTable(const char *pProgramRecordID, genre_t genres[]){
	
	map<string,string>::iterator iter;
	char query[100000];
	char l[1000];

	for(int i = 0; i < 6; i++){
		if (genres[i].description!=""){
			sprintf(l, "%i", genres[i].level);
			string id = genres[i].description+(string)l;
			iter=GenreKey.find(id);
			if (iter != GenreKey.end() ){
				sprintf(query,"INSERT INTO ProgramRecord_Genre(FK_ProgramRecord, FK_Genre) VALUES(\"%s\",\"%s\")", pProgramRecordID, (iter->first).c_str());
				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}
			} else{
				sprintf(query,"INSERT INTO Genre(PK_Genre, Description, Level) VALUES(\"%s\",\"%s\",\"%d\")", id.c_str(), (genres[i].description).c_str(), genres[i].level);
				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}
			
				GenreKey[id]="";

				sprintf(query,"INSERT INTO ProgramRecord_Genre(FK_ProgramRecord, FK_Genre) VALUES(\"%s\",\"%s\")", pProgramRecordID, id.c_str());
				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}
			}
		}
	}
}

void ImportActorTable(const char *pProgramRecordID, actors_t actors[]){

	char actorSQL[100000];
	//set<string>::iterator iter;
	multimap<string,int>::iterator roleiter;
	bool flag = false;
	char query[100000];

	for(int i = 0; i < 40; i++){
		if (actors[i].firstname!=""){
			MYSQL_RES *mrestribune;
			MYSQL_ROW rowtribune;
			string id = actors[i].firstname + actors[i].lastname;
			id = StringToLower(id);

			map<string,string>::iterator iter=ActorKey.find(id);

			if (iter != ActorKey.end() ){


				sprintf(query,"SELECT PK_Role, Role.Description FROM Actor_Role JOIN Role ON FK_Role=PK_Role WHERE FK_Actor=%s",(iter->first).c_str());

				MYSQL_RES *mresrole;
				MYSQL_ROW rowrole;
				bool roleflag = false;
				string str = (string) query;
				if(mysql_real_query(mysqltribune,str.c_str(),(unsigned int) str.length())==0&&(mresrole=mysql_store_result(mysqltribune))){
					
					while(rowrole=mysql_fetch_row(mresrole))
						if (rowrole[1]==actors[i].role){
							roleflag = true;
							break;
						}
					mysql_free_result(mresrole);
				}

				if(roleflag){
					sprintf(actorSQL,"INSERT INTO Actor_ProgramRecord(FK_Actor, FK_ProgramRecord) VALUES(\"%s\",\"%s\")", (iter->first).c_str(), pProgramRecordID);
					if( mysql_query( mysqltribune, actorSQL) ){
						cerr<<"query: "<<actorSQL<<endl
							<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
						exit(1);
					}
					
				} else {
					roleiter=multimapRoleDesc_To_Key.find(actors[i].role);
					if( roleiter!= multimapRoleDesc_To_Key.end() ){
						char rolekey[20];
						sprintf(rolekey,"%d",roleiter->second);
						string ar_key = iter->first + (string)rolekey;
						sprintf(actorSQL,"INSERT INTO Actor_Role( PK_ActorRole, FK_Actor, FK_Role) VALUES(\"%s\",\"%s\",\"%d\")", ar_key.c_str(), (iter->first).c_str(), roleiter->second);

						if( mysql_query( mysqltribune, actorSQL) ){
							cerr<<"query: "<<actorSQL<<endl
								<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
							exit(1);
						}
					} else{
						sprintf(actorSQL,"INSERT INTO Role(Description) VALUES(\"%s\")", (actors[i].role).c_str());
						if( mysql_query( mysqltribune, actorSQL) ){
							cerr<<"query: "<<actorSQL<<endl
								<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
							exit(1);
						}
						int retrievedD = mysql_insert_id(mysqltribune);
						multimapRoleDesc_To_Key.insert(pair<string,int>(actors[i].role, retrievedD) );
						
						char rolekey[20];
						sprintf(rolekey,"%d",retrievedD);
						string ar_key = iter->first + (string)rolekey;
						sprintf(actorSQL,"INSERT INTO Actor_Role( PK_ActorRole, FK_Actor, FK_Role) VALUES(\"%s\",\"%s\",\"%d\")", ar_key.c_str(), (iter->first).c_str(), retrievedD);

						if( mysql_query( mysqltribune, actorSQL) ){
							cerr<<"query: "<<actorSQL<<endl
								<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
							exit(1);
						}
						
					}
				}
			} else {
				//string pk_actor = actors[i].firstname + actors[i].lastname;
				sprintf(actorSQL,"INSERT INTO Actor(PK_Actor, FirstName, LastName) VALUES(\"%s\",\"%s\",\"%s\")", id.c_str(), (actors[i].firstname).c_str(), (actors[i].lastname).c_str());
				if( mysql_query( mysqltribune, actorSQL) ){
					cerr<<"query: "<<actorSQL<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}

				//int lastID = (int) mysql_insert_id(mysqltribune);

				ActorKey[id]="";
			
				sprintf(actorSQL,"INSERT INTO Actor_ProgramRecord( FK_Actor, FK_ProgramRecord) VALUES(\"%s\",\"%s\")", id.c_str(), pProgramRecordID);
				if( mysql_query( mysqltribune, actorSQL) ){
					cerr<<"query: "<<actorSQL<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}

				roleiter=multimapRoleDesc_To_Key.find(actors[i].role);

				if( roleiter!= multimapRoleDesc_To_Key.end() ){
					char rolekey[20];
					sprintf(rolekey,"%d",roleiter->second);
					string ar_key = id + (string)rolekey;
					sprintf(actorSQL,"INSERT INTO Actor_Role( PK_ActorRole, FK_Actor, FK_Role) VALUES(\"%s\",\"%s\",\"%d\")", ar_key.c_str(), id.c_str(), roleiter->second);
					if( mysql_query( mysqltribune, actorSQL) ){
						cerr<<"query: "<<actorSQL<<endl
							<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
						exit(1);
					}
					
				} else{
					sprintf(actorSQL,"INSERT INTO Role(Description) VALUES(\"%s\")", (actors[i].role).c_str());
					if( mysql_query( mysqltribune, actorSQL) ){
						cerr<<"query: "<<actorSQL<<endl
							<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
						exit(1);
					}

					int retrievedID = (int) mysql_insert_id(mysqltribune);
					multimapRoleDesc_To_Key.insert(pair<string,int>(actors[i].role, retrievedID) );

					char rolekey[20];
					sprintf(rolekey,"%d",retrievedID);
					string ar_key = id + (string)rolekey;
					sprintf(actorSQL,"INSERT INTO Actor_Role( PK_ActorRole, FK_Actor, FK_Role) VALUES(\"%s\",\"%s\",\"%d\")", ar_key.c_str(), id.c_str(), retrievedID);

					if( mysql_query( mysqltribune, actorSQL) ){
						cerr<<"query: "<<actorSQL<<endl
							<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
						exit(1);
					}
				}
			}
		}
	}
	
}

void ImportProgramRecord(bool bInsert, const char *pData, unsigned long checksum)
{
	const char *pProgramRecordID = pData;

	const char *pTitle = FindDelim(pProgramRecordID);

	const char *pReducedTitle1 = FindDelim(pTitle);

	const char *pReducedTitle2 = FindDelim(pReducedTitle1);

	const char *pReducedTitle3 = FindDelim(pReducedTitle2);

	const char *pReducedTitle4 = FindDelim(pReducedTitle3);

	const char *pAltTitle = FindDelim(pReducedTitle4);
	
	const char *pReducedDesc1 = FindDelim(pAltTitle);

	const char *pReducedDesc2 = FindDelim(pReducedDesc1);

	const char *pReducedDesc3 = FindDelim(pReducedDesc2);

	const char *pAdvisoryAdultContent = FindDelim(pReducedDesc3);

	const char *pAdvisoryExplicitLanguage = FindDelim(pAdvisoryAdultContent);
	
	const char *pAdvisoryNudity = FindDelim(pAdvisoryExplicitLanguage);

	const char *pAdvisoryViolence = FindDelim(pAdvisoryNudity);

	const char *pAdvisorySexualContent = FindDelim(pAdvisoryViolence);

	const char *pAdvisoryRapeContent = FindDelim(pAdvisorySexualContent);

	char *pInitial = (char*) FindDelim(pAdvisoryRapeContent);

	char *pFirstName, *pLastName, *pRole;

	actors_t actors[40];

	for(int i = 0; i < 40; i++){
		
		pFirstName = pInitial;

		pLastName = FindDelim(pFirstName);

		pRole = FindDelim(pLastName);

		pInitial = FindDelim(pRole);
		
		string firstname = (string)pFirstName;	

		if (firstname!=""){
			actors[i].firstname = firstname;
			
			string lastname = (string)pLastName;	
			actors[i].lastname = lastname;
	
			string role = (string)pRole;	
			actors[i].role = role;
		}

	}
	
	genre_t genres[6];
	
	char *pGenreDesc;
	int level = 1;

	for(int i = 0; i < 6; i++){
			
		pGenreDesc = pInitial;
		pInitial = FindDelim(pGenreDesc);

		string description = (string)pGenreDesc;

		if (description!=""){
			genres[i].description = description;
			genres[i].level = level;
			level++;
		}
		
	}
	
	const char *pDescription = pInitial;//FindDelim(pGenreDesc);

	const char *pRealYear = FindDelim(pDescription);

	const char *pMpaaRating = FindDelim(pRealYear);

	const char *pStarRating = FindDelim(pMpaaRating);

	const char *pRunTime = FindDelim(pStarRating);

	const char *pColorCode = FindDelim(pRunTime);

	const char *pProgramLanguage = FindDelim(pColorCode);

	const char *pOrgCountry = FindDelim(pProgramLanguage);

	const char *pMadeForTV = FindDelim(pOrgCountry);

	const char *pSourceType = FindDelim(pMadeForTV);

	const char *pShowType = FindDelim(pSourceType);

	const char *pHoliday = FindDelim(pShowType);

	const char *pSynEpiNum = FindDelim(pHoliday);

	const char *pAltSynEpiNum = FindDelim(pSynEpiNum);

	const char *pEpiTitle = FindDelim(pAltSynEpiNum);

	const char *pNetSynSource = FindDelim(pEpiTitle);

	const char *pNetSynType = FindDelim(pNetSynSource);

	const char *pDescWithActors = FindDelim(pNetSynType);

	const char *pReducedDescWithActors = FindDelim(pDescWithActors);

	const char *pOrgStudio = FindDelim(pReducedDescWithActors);

	const char *pGamedate = FindDelim(pOrgStudio);

	const char *pGameTime = FindDelim(pGamedate);

	const char *pGameTimeZone = FindDelim(pGameTime);

	const char *pOrgAirDate = FindDelim(pGameTimeZone);

	const char *pUniqueID = FindDelim(pOrgAirDate);

	const char *pRest = FindDelim(pUniqueID);

	if( !pProgramRecordID) {
		cerr<<"Error parsing string"<<pData<<endl;
		exit(1);
	}
	else {
		char sSQL[100000];
		string str;

		if( bInsert ){
				sprintf(sSQL,"INSERT INTO ProgramRecord(PK_ProgramRecord, Title, ReducedTitle1, ReducedTitle2, ReducedTitle3, ReducedTitle4, AltTitle, ReducedDesc1, ReducedDesc2, ReducedDesc3, AdvisoryAdultContent, AdvisoryExplicitLanguage, AdvisoryNudity, AdvisoryViolence, AdvisorySexualContent, AdvisoryRapeContent, Description, RealYear, MpaaRating, StarRating, RunTime, ColorCode, ProgramLanguage, OrgCountry, MadeForTV, SourceType, ShowType, Holiday, SynEpiNum, AltSynEpiNum, EpiTitle, NetSynSource, NetSynType, DescWithActors, ReducedDescWithActors, OrgStudio, Gamedate, GameTime, GameTimeZone, OrgAirDate, UniqueID, Checksum) VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%lu\")", pProgramRecordID, EscapeString((string)pTitle), EscapeString((string)pReducedTitle1), EscapeString((string)pReducedTitle2), EscapeString((string)pReducedTitle3), EscapeString((string)pReducedTitle4), EscapeString((string)pAltTitle), EscapeString((string)pReducedDesc1), EscapeString((string)pReducedDesc2), EscapeString((string)pReducedDesc3), pAdvisoryAdultContent, pAdvisoryExplicitLanguage, pAdvisoryNudity, pAdvisoryViolence, pAdvisorySexualContent, pAdvisoryRapeContent, EscapeString((string)pDescription), pRealYear, EscapeString((string)pMpaaRating), EscapeString((string)pStarRating), EscapeString((string)pRunTime), EscapeString((string)pColorCode), EscapeString((string)pProgramLanguage), EscapeString((string)pOrgCountry), EscapeString((string)pMadeForTV), EscapeString((string)pSourceType), EscapeString((string)pShowType), EscapeString((string)pHoliday), EscapeString((string)pSynEpiNum), EscapeString((string)pAltSynEpiNum), EscapeString((string)pEpiTitle), EscapeString((string)pNetSynSource), EscapeString((string)pNetSynType), EscapeString((string)pDescWithActors), EscapeString((string)pReducedDescWithActors), EscapeString((string)pOrgStudio), EscapeString((string)pGamedate), EscapeString((string)pGameTime), EscapeString((string)pGameTimeZone), EscapeString((string)pOrgAirDate), pUniqueID, checksum);

				ImportActorTable(pProgramRecordID, actors);

				ImportGenreTable(pProgramRecordID, genres);
				


		}
		else{
				sprintf(sSQL,"UPDATE ProgramRecord SET Title=\"%s\", ReducedTitle1=\"%s\", ReducedTitle2=\"%s\", ReducedTitle3=\"%s\", ReducedTitle4=\"%s\", AltTitle=\"%s\", ReducedDesc1=\"%s\", ReducedDesc2=\"%s\", ReducedDesc3=\"%s\", AdvisoryAdultContent=\"%s\", AdvisoryExplicitLanguage=\"%s\", AdvisoryNudity=\"%s\", AdvisoryViolence=\"%s\", AdvisorySexualContent=\"%s\", AdvisoryRapeContent=\"%s\", Description=\"%s\", RealYear=\"%s\", MpaaRating=\"%s\", StarRating=\"%s\", RunTime=\"%s\", ColorCode=\"%s\", ProgramLanguage=\"%s\", OrgCountry=\"%s\", MadeForTV=\"%s\", SourceType=\"%s\", ShowType=\"%s\", Holiday=\"%s\", SynEpiNum=\"%s\", AltSynEpiNum=\"%s\", EpiTitle=\"%s\", NetSynSource=\"%s\", NetSynType=\"%s\", DescWithActors=\"%s\", ReducedDescWithActors=\"%s\", OrgStudio=\"%s\", Gamedate=\"%s\", GameTime=\"%s\", GameTimeZone=\"%s\", OrgAirDate=\"%s\", UniqueID=\"%s\", Checksum=\"%lu\" WHERE PK_ProgramRecord=\"%s\"", EscapeString((string)pTitle), EscapeString((string)pReducedTitle1), EscapeString((string)pReducedTitle2), EscapeString((string)pReducedTitle3), EscapeString((string)pReducedTitle4), EscapeString((string)pAltTitle), EscapeString((string)pReducedDesc1), EscapeString((string)pReducedDesc2), EscapeString((string)pReducedDesc3), pAdvisoryAdultContent, pAdvisoryExplicitLanguage, pAdvisoryNudity, pAdvisoryViolence, pAdvisorySexualContent, pAdvisoryRapeContent, EscapeString((string)pDescription), pRealYear, EscapeString((string)pMpaaRating), EscapeString((string)pStarRating), EscapeString((string)pRunTime), EscapeString((string)pColorCode), EscapeString((string)pProgramLanguage), EscapeString((string)pOrgCountry), EscapeString((string)pMadeForTV), EscapeString((string)pSourceType), EscapeString((string)pShowType), EscapeString((string)pHoliday), EscapeString((string)pSynEpiNum), EscapeString((string)pAltSynEpiNum), EscapeString((string)pEpiTitle), EscapeString((string)pNetSynSource), EscapeString((string)pNetSynType), EscapeString((string)pDescWithActors), EscapeString((string)pReducedDescWithActors), EscapeString((string)pOrgStudio), EscapeString((string)pGamedate), EscapeString((string)pGameTime), EscapeString((string)pGameTimeZone), EscapeString((string)pOrgAirDate), pUniqueID, checksum, pProgramRecordID);

				char query[100000];
				sprintf(query,"DELETE FROM ProgramRecord_Genre WHERE FK_ProgramRecord='%s'",pProgramRecordID);

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}
					
				sprintf(query,"DELETE FROM Actor_ProgramRecord WHERE FK_ProgramRecord='%s'",pProgramRecordID);

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}

				ImportActorTable(pProgramRecordID, actors);

				ImportGenreTable(pProgramRecordID, genres);

		}

		if( mysql_query( mysqltribune, sSQL) ){
			cerr<<"query: "<<sSQL<<endl
					<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
			exit(1);
		}
	

	}

}


void ImportProgramRecordTable()
{
	    MYSQL_RES *mrestribune;
	    MYSQL_ROW rowtribune;
            map<string,unsigned long> mapPrimaryKey_To_Checksum;
	    map<string,unsigned long> mapPrimaryKey_To_ChecksumFile;

            string query="SELECT PK_ProgramRecord,Checksum FROM ProgramRecord";

	    if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) 

		while( (rowtribune=mysql_fetch_row(mrestribune)) ) 

			mapPrimaryKey_To_Checksum[ rowtribune[0] ] = atoll(rowtribune[1]);
		

	    //add a genre map

            query="SELECT PK_Genre FROM Genre";

	    if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) {

	    	while( (rowtribune=mysql_fetch_row(mrestribune)) ){ 
			GenreKey[rowtribune[0]]="";
		}
	    }

	    query="SELECT PK_Actor FROM Actor";

	    if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) {

	    	while( (rowtribune=mysql_fetch_row(mrestribune)) ){ 
			ActorKey[rowtribune[0]]="";
		}
	    }
		
	    query="SELECT Description,PK_Role FROM Role";

	    if(mysql_real_query(mysqltribune,query.c_str(),(unsigned int) query.length())==0&&(mrestribune=mysql_store_result(mysqltribune))) {

	    	while( (rowtribune=mysql_fetch_row(mrestribune)) ){ 
			multimapRoleDesc_To_Key.insert(pair<string,int>(rowtribune[0], atoi(rowtribune[1])) );
		}
	    }
            // Now we have a map of all existing lineups in the database with the id and checksum
            // open the text file

	    ifstream programrecordfile(ProgramRecords);
            string line;
	
	    if (!programrecordfile) {
		cerr<<"Error opening file "<<Stations<<endl;
		exit(1);
	    }

	    if (programrecordfile.is_open()) {

		while(! programrecordfile.eof())
		{
			getline (programrecordfile,line);
			
			if (line=="")
				continue;

			unsigned long checksum = GetChecksum(line.c_str(),line.size());

			string chanid = GetChannelID(line.c_str());

			mapPrimaryKey_To_ChecksumFile[chanid] = checksum;
	
			map<string,unsigned long>::iterator it=mapPrimaryKey_To_Checksum.find(chanid);
			
		
			if( it== mapPrimaryKey_To_Checksum.end() ){
				
				ImportProgramRecord(true,line.c_str(),checksum); // It’s a new row
			}
			else if( it->second != checksum )
		
				ImportProgramRecord(false,line.c_str(),checksum); // It’s an existing row, but the checksum changed
		
		}

		map<string,unsigned long>::iterator iter;
		for( iter = mapPrimaryKey_To_Checksum.begin(); iter != mapPrimaryKey_To_Checksum.end(); iter++ ) {
			map<string,unsigned long>::iterator it=mapPrimaryKey_To_ChecksumFile.find(iter->first);
			if( it== mapPrimaryKey_To_ChecksumFile.end() ){
				
				char query[100000];
				sprintf(query,"DELETE FROM ProgramRecord WHERE PK_ProgramRecord='%s'",(iter->first).c_str());

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}

				sprintf(query,"DELETE FROM ProgramRecord_Genre WHERE FK_ProgramRecord='%s'",(iter->first).c_str());

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}	

				sprintf(query,"DELETE FROM Actor_ProgramRecord WHERE FK_ProgramRecord='%s'",(iter->first).c_str());

				if( mysql_query( mysqltribune, query) ){
					cerr<<"query: "<<query<<endl
						<<"Error executing query: "<<mysql_error(mysqltribune)<<endl;
					exit(1);
				}
			}
		}
	    }

}

int main(int argc, char *argv[]){

	string hosttribune="localhost",usertribune="root",passwdtribune="",dbtribune="pluto_tribune";
	my_bool reconnect = true;

	mysqltribune=mysql_init(NULL);
	mysql_options(mysqltribune, MYSQL_OPT_RECONNECT, &reconnect);
	if(!mysql_real_connect(mysqltribune,hosttribune.c_str(),usertribune.c_str(),passwdtribune.c_str(),dbtribune.c_str(),0,NULL,0)){
		cerr<<"Error connecting to db:"<<mysql_error(mysqltribune)<<endl;
		return false;
	}

	string command = "touch /var/Tribune/flagfile";
	system(command.c_str());
		
	ImportChannelTable();

	ImportHeadendTable();

	ImportLineupTable();

	ImportProgramRecordTable();

	ImportScheduleTable();

	command = "rm /var/Tribune/flagfile";
	system(command.c_str());
	
}
