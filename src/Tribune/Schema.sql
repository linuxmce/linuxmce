CREATE TABLE IF NOT EXISTS Headend(
 PK_Headend int(11) NOT NULL auto_increment,
 HeadendID varchar(7),
 CommunityName varchar(28),
 CountyName varchar(25),
 CountySize varchar(1),
 StCountyCode varchar(5),
 StateServed varchar(2),
 ZipCode varchar(5),
 DmaCode int(3),
 DmaName varchar(70),
 MsoCode varchar(5),
 DmaRank varchar(4),
 HeadendName varchar(42),
 HeadendLocation varchar(28),
 MsoName varchar(42),
 TimeZoneCode int(1),
 PRIMARY KEY(PK_Headend)
);

CREATE TABLE IF NOT EXISTS Lineup(
 PK_Lineup int(11) NOT NULL,
 HeadendID varchar(7),
 Device varchar(1),
 FK_Station int(11),
 TmsChan varchar(5),
 ServiceTier int(1),
 EffectiveDate varchar(8),
 ExpirationDate varchar(8),
 Checksum varchar(11) NOT NULL,
 TimestampF TIMESTAMP,
 PRIMARY KEY(PK_Lineup)
);

CREATE TABLE IF NOT EXISTS Station(
 PK_Station int(11) NOT NULL,
 TimeZone varchar(30),
 Name varchar(40),
 CallSign varchar(10),
 Affil varchar(25),
 City varchar(20),
 State varchar(15),
 ZipCode varchar(12),
 Country varchar(15),
 DmaName varchar(70),
 DmaNum int(10),
 FccChannelNum int(8),
 Checksum varchar(11) NOT NULL,
 TimestampF TIMESTAMP,
 PRIMARY KEY(PK_Station)
);

CREATE TABLE IF NOT EXISTS Schedule(
 PK_Schedule bigint(19) NOT NULL,
 FK_Station int(11),
 FK_ProgramRecord varchar(12),
 AirDate varchar(8),
 AirTime varchar(8),
 Duration varchar(8),
 PartNum int(3),
 NumOfParts int(3),
 CCaptioning varchar(1),
 Stereo varchar(1),
 Repeating varchar(1),
 LiveTapeDelay varchar(5),
 Subtitled varchar(1),
 PremiereFinale varchar(15),
 JoinedInProgress varchar(1),
 CableInTheClassroom varchar(1),
 TVRating varchar(4),
 Sap varchar(1),
 Blackout varchar(1),
 SexRating varchar(1),
 ViolenceRating varchar(1),
 LanguageRating varchar(1),
 DialogRating varchar(1),
 FvRating varchar(1),
 Enhanced varchar(1),
 ThreeD varchar(1),
 LetterBox varchar(1),
 Hdtv varchar(1),
 Dolby varchar(5),
 Dvs varchar(1),
 Checksum varchar(11) NOT NULL,
 TimestampF TIMESTAMP,
 PRIMARY KEY(PK_Schedule)
);

CREATE TABLE IF NOT EXISTS ProgramRecord(
 PK_ProgramRecord varchar(12),
 Title varchar(120),
 ReducedTitle1 varchar(70),
 ReducedTitle2 varchar(40),
 ReducedTitle3 varchar(20),
 ReducedTitle4 varchar(10),
 AltTitle varchar(120),
 ReducedDesc1 varchar(100),
 ReducedDesc2 varchar(60),
 ReducedDesc3 varchar(40),
 AdvisoryAdultContent varchar(30),
 AdvisoryExplicitLanguage varchar(30),
 AdvisoryNudity varchar(30),
 AdvisoryViolence varchar(30),
 AdvisorySexualContent varchar(30),
 AdvisoryRapeContent varchar(30),
 Description varchar(255),
 RealYear varchar(4),
 MpaaRating varchar(5),
 StarRating varchar(5),
 RunTime varchar(4),
 ColorCode varchar(20),
 ProgramLanguage varchar(10),
 OrgCountry varchar(15),
 MadeForTV varchar(1),
 SourceType varchar(10),
 ShowType varchar(30),
 Holiday varchar(30),
 SynEpiNum int(11),
 AltSynEpiNum int(20),
 EpiTitle varchar(150),
 NetSynSource varchar(10),
 NetSynType varchar(21),
 DescWithActors varchar(255),
 ReducedDescWithActors varchar(100),
 OrgStudio varchar(25),
 Gamedate varchar(8),
 GameTime varchar(4),
 GameTimeZone varchar(30),
 OrgAirDate varchar(8),
 UniqueID varchar(8), 
 Checksum varchar(11) NOT NULL,
 TimestampF TIMESTAMP,
 PRIMARY KEY(PK_ProgramRecord)
);

CREATE TABLE IF NOT EXISTS ProgramRecord_Genre(
 FK_ProgramRecord varchar(12),
 FK_Genre varchar(32)
);

CREATE TABLE IF NOT EXISTS Genre(
 PK_Genre varchar(32),
 Description varchar(30),
 Level int(1),
 PRIMARY KEY(PK_Genre)
);

CREATE TABLE IF NOT EXISTS Actor_ProgramRecord(
 FK_Actor varchar(50),
 FK_ProgramRecord varchar(12)
);

CREATE TABLE IF NOT EXISTS Actor(
 PK_Actor varchar(50),
 FirstName varchar(30),
 LastName varchar(30),
 PRIMARY KEY(PK_Actor)
);

CREATE TABLE IF NOT EXISTS Actor_Role(
 PK_ActorRole varchar(60),
 FK_Actor varchar(50),
 FK_Role int(11)
);

CREATE TABLE IF NOT EXISTS Role(
 PK_Role int(11) NOT NULL auto_increment,
 Description varchar(30),
 PRIMARY KEY(PK_Role)
);

CREATE TABLE IF NOT EXISTS BlackListChannels(
ChannelName varchar(50)
);

CREATE TABLE IF NOT EXISTS ClientLineup(
 PK_ClientLineup int(11),
 LineupName varchar(30)
);

