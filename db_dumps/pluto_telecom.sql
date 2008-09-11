-- MySQL dump 10.11
--
-- Host: localhost    Database: pluto_telecom
-- ------------------------------------------------------
-- Server version	5.0.45-Debian_1ubuntu3.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `CallersForMe`
--

DROP TABLE IF EXISTS `CallersForMe`;
CREATE TABLE `CallersForMe` (
  `PK_CallersForMe` int(11) NOT NULL auto_increment,
  `EK_Users` int(11) default NULL,
  `PhoneNumber` varchar(100) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_CallersForMe`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `CallersForMe`
--

LOCK TABLES `CallersForMe` WRITE;
/*!40000 ALTER TABLE `CallersForMe` DISABLE KEYS */;
/*!40000 ALTER TABLE `CallersForMe` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Contact`
--

DROP TABLE IF EXISTS `Contact`;
CREATE TABLE `Contact` (
  `PK_Contact` int(11) NOT NULL auto_increment,
  `Name` varchar(60) default NULL,
  `Company` varchar(60) default NULL,
  `JobDescription` varchar(60) default NULL,
  `Title` varchar(60) default NULL,
  `Email` varchar(60) default NULL,
  `EntryID` varchar(60) default NULL,
  `EK_Users` int(11) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_Contact`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=32 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Contact`
--

LOCK TABLES `Contact` WRITE;
/*!40000 ALTER TABLE `Contact` DISABLE KEYS */;
INSERT INTO `Contact` VALUES (1,'Agnes','','','','arc@aim-fr.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(2,'Mihaela VASCU','','','','mvascu@yahoo.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(3,'Support NWH','','','','support@northwindwebhost.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(4,'Sales NWH','','','','sales@northwindwebhost.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(5,'Office Boti','','','','office@avocat-boti.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(6,'Office NFS','','','','office@newflavorstudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(7,'Dan. NFS','','','','dan.g@newflavorstudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(8,'Office NS','','','','office@nipplestudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(9,'Adrian FETCU','','','','adi.f@newflavorstudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(10,'Mircea Slanina','','','','mircea_slanina@isratech.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(11,'SIMONA URECHE','','','','simonaureche@magazinudescule.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(12,'Dan Xnet','','','','dan-gheorghiu@xnet.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(13,'Dan. NS','','','','dan.g@nipplestudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(14,'Office Moldoleasing','','','','office@moldoleasing.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(15,'Office Schrotter','','','','office@irinaschrotter.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(16,'Redactia De Est','','','','redactia@jurnaluldeest.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(17,'Office Magnum','','','','office@magnumcorp.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(18,'Ionut Gavriluc','','','','ionutgavriluc@magazinudescule.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(19,'Andrei Crenganis','','','','andrei.c@newflavorstudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(20,'SERBAN DANUT','BISTRITA AURIE S.N.C','','Manager','bistrita.aurie@xnet.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(21,'Nick Arghirescu','','','','nick@arghirescu.net',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(22,'Cristina PERHINSCHI','','','','cristina@quartzmatrix.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(23,'Asit Grup','','','','asit@artelecom.net',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(24,' St. Nicholas Ecumenical Institute','','','','infoecumenic@mail.dntis.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(25,'Adrian COSTAN','','','','adrian@hoteltraian.ro',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(27,'Radu Cristescu','','','','radu.c@newflavorstudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(28,'Cristian Miron','','','','cristian.m@newflavorstudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(29,'Dragos G.','','','','dgrosu@gmx.net',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(30,'Dragos Grosu','','','','dragos.g@newflavorstudio.com',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:07',NULL),(31,'Aaron Baalbergen','','','','aaron@plutohome.com\r\n','000000008149F06AD26A3A4A8F396F618315AEECC4002000',NULL,NULL,NULL,NULL,0,'2006-11-13 13:44:54',NULL);
/*!40000 ALTER TABLE `Contact` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Contact_Users_Sync`
--

DROP TABLE IF EXISTS `Contact_Users_Sync`;
CREATE TABLE `Contact_Users_Sync` (
  `FK_Contact` int(11) NOT NULL default '0',
  `EK_Users` int(11) NOT NULL default '0',
  `SyncID` varchar(30) default NULL,
  `LastModPluto` datetime default NULL,
  `LastModSync` datetime default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`FK_Contact`,`EK_Users`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Contact_Users_Sync`
--

LOCK TABLES `Contact_Users_Sync` WRITE;
/*!40000 ALTER TABLE `Contact_Users_Sync` DISABLE KEYS */;
/*!40000 ALTER TABLE `Contact_Users_Sync` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `IncomingLog`
--

DROP TABLE IF EXISTS `IncomingLog`;
CREATE TABLE `IncomingLog` (
  `PK_IncomingLog` int(11) NOT NULL auto_increment,
  `DateTime` datetime default NULL,
  `FK_PhoneNumber` int(11) default NULL,
  `PhoneNumber` varchar(30) default NULL,
  `Duration` int(11) default NULL,
  `Answered` tinyint(1) default '0',
  `Voicemail` tinyint(1) default '0',
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_IncomingLog`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `IncomingLog`
--

LOCK TABLES `IncomingLog` WRITE;
/*!40000 ALTER TABLE `IncomingLog` DISABLE KEYS */;
/*!40000 ALTER TABLE `IncomingLog` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Line_HouseMode`
--

DROP TABLE IF EXISTS `Line_HouseMode`;
CREATE TABLE `Line_HouseMode` (
  `PK_Line_HouseMode` int(11) NOT NULL auto_increment,
  `ID` int(11) default NULL,
  `Type` varchar(50) default NULL,
  `EK_HouseMode` int(11) default NULL,
  `Routing` text,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_Line_HouseMode`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Line_HouseMode`
--

LOCK TABLES `Line_HouseMode` WRITE;
/*!40000 ALTER TABLE `Line_HouseMode` DISABLE KEYS */;
INSERT INTO `Line_HouseMode` VALUES (1,99992,'SIP',3,'prompt',1,NULL,NULL,0,'2005-12-05 09:11:14',NULL),(2,99992,'SIP',2,'ring,5232',2,NULL,NULL,0,'2005-12-05 10:41:56',NULL),(3,99992,'SIP',6,'voicemail,33129',3,NULL,NULL,0,'2005-12-05 10:41:56',NULL),(4,99992,'SIP',5,'ring,',4,NULL,NULL,0,'2005-12-05 09:11:14',NULL),(5,99992,'SIP',4,'ring,5232',5,NULL,NULL,0,'2005-12-05 10:41:56',NULL),(6,99992,'SIP',1,'transfer,5553344',6,NULL,NULL,0,'2005-12-04 19:58:09',NULL);
/*!40000 ALTER TABLE `Line_HouseMode` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `OutgoingLog`
--

DROP TABLE IF EXISTS `OutgoingLog`;
CREATE TABLE `OutgoingLog` (
  `PK_OutgoingLog` int(11) NOT NULL auto_increment,
  `EK_Users` int(11) default NULL,
  `FK_PhoneNumber` int(11) default NULL,
  `PhoneNumber` varchar(30) default NULL,
  `Duration` int(11) default NULL,
  `Answered` tinyint(1) default '0',
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_OutgoingLog`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `OutgoingLog`
--

LOCK TABLES `OutgoingLog` WRITE;
/*!40000 ALTER TABLE `OutgoingLog` DISABLE KEYS */;
/*!40000 ALTER TABLE `OutgoingLog` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PhoneNumber`
--

DROP TABLE IF EXISTS `PhoneNumber`;
CREATE TABLE `PhoneNumber` (
  `PK_PhoneNumber` int(11) NOT NULL auto_increment,
  `FK_Contact` int(11) default NULL,
  `FK_PhoneType` int(11) default NULL,
  `CountryCode` varchar(5) default NULL,
  `AreaCode` varchar(6) default NULL,
  `PhoneNumber` varchar(15) default NULL,
  `Extension` varchar(10) default NULL,
  `DialAs` varchar(20) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_PhoneNumber`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=59 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `PhoneNumber`
--

LOCK TABLES `PhoneNumber` WRITE;
/*!40000 ALTER TABLE `PhoneNumber` DISABLE KEYS */;
INSERT INTO `PhoneNumber` VALUES (1,1,1,NULL,NULL,'99999',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(2,1,2,NULL,NULL,'00000',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(3,1,3,NULL,NULL,'77777',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(4,1,5,NULL,NULL,'77777',NULL,NULL,NULL,NULL,NULL,0,'2006-03-24 12:00:06',NULL),(9,4,6,'1','45634','45645','456456','914563445645',NULL,NULL,NULL,0,'2006-11-13 13:38:26',NULL),(34,5,1,'1','222','555','666','91222555',NULL,NULL,NULL,0,'2006-11-13 13:57:56',NULL),(55,31,1,'','','111','',NULL,NULL,NULL,NULL,0,'2006-11-15 14:48:02',NULL),(56,31,2,'','','555','',NULL,NULL,NULL,NULL,0,'2006-11-15 14:48:02',NULL),(57,31,3,'','','333','',NULL,NULL,NULL,NULL,0,'2006-11-15 14:48:02',NULL),(58,31,5,'','','222','',NULL,NULL,NULL,NULL,0,'2006-11-15 14:48:02',NULL);
/*!40000 ALTER TABLE `PhoneNumber` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PhoneNumber_Users_Sync`
--

DROP TABLE IF EXISTS `PhoneNumber_Users_Sync`;
CREATE TABLE `PhoneNumber_Users_Sync` (
  `FK_PhoneNumber` int(11) NOT NULL default '0',
  `EK_Users` int(11) NOT NULL default '0',
  `SyncID` varchar(30) default NULL,
  `LastModPluto` datetime default NULL,
  `LastModSync` datetime default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`FK_PhoneNumber`,`EK_Users`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `PhoneNumber_Users_Sync`
--

LOCK TABLES `PhoneNumber_Users_Sync` WRITE;
/*!40000 ALTER TABLE `PhoneNumber_Users_Sync` DISABLE KEYS */;
/*!40000 ALTER TABLE `PhoneNumber_Users_Sync` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PhoneType`
--

DROP TABLE IF EXISTS `PhoneType`;
CREATE TABLE `PhoneType` (
  `PK_PhoneType` int(11) NOT NULL auto_increment,
  `Description` varchar(20) default NULL,
  `Define` varchar(20) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_PhoneType`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `PhoneType`
--

LOCK TABLES `PhoneType` WRITE;
/*!40000 ALTER TABLE `PhoneType` DISABLE KEYS */;
INSERT INTO `PhoneType` VALUES (1,'Home',NULL,1,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(2,'Work',NULL,2,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(3,'Mobile',NULL,3,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(4,'Pager',NULL,4,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(5,'Fax',NULL,5,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(6,'Yahoo',NULL,6,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(7,'Skype',NULL,7,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(8,'IAXTel',NULL,8,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(9,'FWD',NULL,9,NULL,NULL,0,'0000-00-00 00:00:00',NULL),(10,'Inphonex',NULL,10,NULL,NULL,0,'0000-00-00 00:00:00',NULL);
/*!40000 ALTER TABLE `PhoneType` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PriorityCallers`
--

DROP TABLE IF EXISTS `PriorityCallers`;
CREATE TABLE `PriorityCallers` (
  `PK_PriorityCallers` int(11) NOT NULL auto_increment,
  `EK_Users` int(11) default NULL,
  `PhoneNumber` varchar(100) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_PriorityCallers`)
) ENGINE=MyISAM AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `PriorityCallers`
--

LOCK TABLES `PriorityCallers` WRITE;
/*!40000 ALTER TABLE `PriorityCallers` DISABLE KEYS */;
INSERT INTO `PriorityCallers` VALUES (8,33136,'345345',NULL,NULL,NULL,0,'2007-03-14 16:09:59',NULL);
/*!40000 ALTER TABLE `PriorityCallers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SpeedDial`
--

DROP TABLE IF EXISTS `SpeedDial`;
CREATE TABLE `SpeedDial` (
  `PK_SpeedDial` int(11) unsigned NOT NULL default '0',
  `EK_Users` int(11) default NULL,
  `SortOrder` int(11) default NULL,
  `FK_PhoneNumber` int(11) default NULL,
  `PhoneNumber` varchar(30) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_SpeedDial`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `SpeedDial`
--

LOCK TABLES `SpeedDial` WRITE;
/*!40000 ALTER TABLE `SpeedDial` DISABLE KEYS */;
/*!40000 ALTER TABLE `SpeedDial` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `UserRouting`
--

DROP TABLE IF EXISTS `UserRouting`;
CREATE TABLE `UserRouting` (
  `PK_UserRouting` int(11) NOT NULL auto_increment,
  `EK_Users` int(11) NOT NULL default '0',
  `EK_UserMode` int(11) NOT NULL default '0',
  `SendStatusFirst` tinyint(2) default NULL,
  `IsPriorityCaller` tinyint(2) default NULL,
  `StepOrder` int(11) NOT NULL default '0',
  `Routing` text NOT NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_UserRouting`)
) ENGINE=MyISAM AUTO_INCREMENT=139 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `UserRouting`
--

LOCK TABLES `UserRouting` WRITE;
/*!40000 ALTER TABLE `UserRouting` DISABLE KEYS */;
INSERT INTO `UserRouting` VALUES (5,33129,1,0,0,3,'user,33136',4,NULL,NULL,0,'2005-12-05 08:52:46',NULL),(9,35773,1,0,1,1,'user,35775',NULL,NULL,NULL,0,'2005-12-05 08:55:17',NULL),(10,35773,1,0,0,1,'voicemail,35773',NULL,NULL,NULL,0,'2005-12-05 14:14:16',NULL),(56,34335,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-02-21 13:02:25',NULL),(55,34335,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-02-21 13:02:25',NULL),(11,35773,1,0,0,2,'user,35775',NULL,NULL,NULL,0,'2005-12-05 14:28:35',NULL),(12,34328,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:31:16',NULL),(13,34328,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:31:16',NULL),(14,34328,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:31:16',NULL),(15,34328,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:31:16',NULL),(16,34328,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:31:16',NULL),(17,34328,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:31:16',NULL),(18,34329,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:32:16',NULL),(19,34329,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:32:16',NULL),(20,34329,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:32:16',NULL),(21,34329,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:32:16',NULL),(22,34329,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:32:16',NULL),(23,34329,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:32:16',NULL),(24,34330,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:34:01',NULL),(25,34330,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:34:01',NULL),(26,34330,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:34:01',NULL),(27,34330,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:34:01',NULL),(28,34330,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:34:01',NULL),(29,34330,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:34:01',NULL),(30,34331,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:37:04',NULL),(31,34331,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:37:04',NULL),(32,34331,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:37:04',NULL),(33,34331,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:37:04',NULL),(34,34331,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:37:04',NULL),(35,34331,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:37:04',NULL),(36,34332,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:38:45',NULL),(37,34332,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:38:45',NULL),(38,34332,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:38:45',NULL),(39,34332,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:38:45',NULL),(40,34332,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:38:45',NULL),(41,34332,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:38:45',NULL),(42,34333,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:39:20',NULL),(43,34333,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:39:20',NULL),(44,34333,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:39:20',NULL),(45,34333,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:39:20',NULL),(46,34333,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:39:20',NULL),(47,34333,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:39:20',NULL),(48,34334,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:39:34',NULL),(49,34334,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:39:34',NULL),(50,34334,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:39:34',NULL),(51,34334,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:39:34',NULL),(52,34334,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-01-03 09:39:34',NULL),(53,34334,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-01-03 09:39:34',NULL),(54,33129,1,0,0,4,'voicemail,33129',NULL,NULL,NULL,0,'2006-01-03 13:30:49',NULL),(57,34335,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-02-21 13:02:25',NULL),(58,34335,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-02-21 13:02:25',NULL),(59,34335,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-02-21 13:02:25',NULL),(60,34335,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-02-21 13:02:25',NULL),(61,34336,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-02-22 13:05:36',NULL),(62,34336,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-02-22 13:05:36',NULL),(63,34336,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-02-22 13:05:36',NULL),(64,34336,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-02-22 13:05:36',NULL),(65,34336,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-02-22 13:05:36',NULL),(66,34336,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-02-22 13:05:36',NULL),(67,34874,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:06:13',NULL),(68,34874,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:06:13',NULL),(69,34874,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:06:13',NULL),(70,34874,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:06:13',NULL),(71,34874,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:06:13',NULL),(72,34874,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:06:13',NULL),(73,34874,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:09:33',NULL),(74,34874,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:09:33',NULL),(75,34874,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:09:33',NULL),(76,34874,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:09:33',NULL),(77,34874,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:09:33',NULL),(78,34874,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:09:33',NULL),(79,34875,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:13:13',NULL),(80,34875,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:13:13',NULL),(81,34875,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:13:13',NULL),(82,34875,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:13:13',NULL),(83,34875,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:13:13',NULL),(84,34875,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:13:13',NULL),(85,34876,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:13:26',NULL),(86,34876,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:13:26',NULL),(87,34876,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:13:26',NULL),(88,34876,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:13:26',NULL),(89,34876,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-03-23 13:13:26',NULL),(90,34876,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-03-23 13:13:26',NULL),(91,37332,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-04-06 13:17:05',NULL),(92,37332,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-06 13:17:05',NULL),(93,37332,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-06 13:17:05',NULL),(94,37332,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-06 13:17:05',NULL),(95,37332,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-06 13:17:05',NULL),(96,37332,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-06 13:17:05',NULL),(97,37333,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:05',NULL),(98,37333,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:05',NULL),(99,37333,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:05',NULL),(100,37333,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:05',NULL),(101,37333,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:05',NULL),(102,37333,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:05',NULL),(103,37334,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:27',NULL),(104,37334,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:27',NULL),(105,37334,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:27',NULL),(106,37334,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:27',NULL),(107,37334,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:27',NULL),(108,37334,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:27',NULL),(109,37335,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:43',NULL),(110,37335,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:43',NULL),(111,37335,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:43',NULL),(112,37335,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:43',NULL),(113,37335,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2006-04-11 13:18:43',NULL),(114,37335,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2006-04-11 13:18:43',NULL),(115,34881,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:27:55',NULL),(116,34881,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:27:55',NULL),(117,34881,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:27:55',NULL),(118,34881,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:27:55',NULL),(119,34881,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:27:55',NULL),(120,34881,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:27:55',NULL),(121,34882,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:31:13',NULL),(122,34882,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:31:13',NULL),(123,34882,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:31:13',NULL),(124,34882,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:31:13',NULL),(125,34882,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:31:13',NULL),(126,34882,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:31:13',NULL),(127,34883,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:31:24',NULL),(128,34883,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:31:24',NULL),(129,34883,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:31:24',NULL),(130,34883,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:31:24',NULL),(131,34883,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-02-28 12:31:24',NULL),(132,34883,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-02-28 12:31:24',NULL),(133,34884,1,0,0,1,'ring,',NULL,NULL,NULL,0,'2007-03-05 10:02:24',NULL),(134,34884,2,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-03-05 10:02:24',NULL),(135,34884,4,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-03-05 10:02:24',NULL),(136,34884,4,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-03-05 10:02:24',NULL),(137,34884,3,0,0,1,'voicemail,0',NULL,NULL,NULL,0,'2007-03-05 10:02:24',NULL),(138,34884,3,1,1,1,'ring,',NULL,NULL,NULL,0,'2007-03-05 10:02:24',NULL);
/*!40000 ALTER TABLE `UserRouting` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Users`
--

DROP TABLE IF EXISTS `Users`;
CREATE TABLE `Users` (
  `EK_Users` int(11) NOT NULL default '0',
  `SyncSettings` text,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`EK_Users`),
  UNIQUE KEY `psc_id` (`psc_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Users`
--

LOCK TABLES `Users` WRITE;
/*!40000 ALTER TABLE `Users` DISABLE KEYS */;
/*!40000 ALTER TABLE `Users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_telecom_batdet`
--

DROP TABLE IF EXISTS `psc_telecom_batdet`;
CREATE TABLE `psc_telecom_batdet` (
  `PK_psc_telecom_batdet` int(11) NOT NULL auto_increment,
  `FK_psc_telecom_bathdr` int(11) NOT NULL default '0',
  `Tablename` varchar(60) NOT NULL default '',
  `New` int(11) NOT NULL default '0',
  `Deleted` int(11) NOT NULL default '0',
  `Modified` int(11) NOT NULL default '0',
  `FK_psc_telecom_bathdr_orig` int(11) NOT NULL default '0',
  `FK_psc_telecom_bathdr_auth` int(11) NOT NULL default '0',
  `FK_psc_telecom_bathdr_unauth` int(11) NOT NULL default '0',
  PRIMARY KEY  (`PK_psc_telecom_batdet`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_telecom_batdet`
--

LOCK TABLES `psc_telecom_batdet` WRITE;
/*!40000 ALTER TABLE `psc_telecom_batdet` DISABLE KEYS */;
/*!40000 ALTER TABLE `psc_telecom_batdet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_telecom_bathdr`
--

DROP TABLE IF EXISTS `psc_telecom_bathdr`;
CREATE TABLE `psc_telecom_bathdr` (
  `PK_psc_telecom_bathdr` int(11) NOT NULL auto_increment,
  `IPAddress` varchar(16) default NULL,
  `date` datetime default NULL,
  `comments` text,
  PRIMARY KEY  (`PK_psc_telecom_bathdr`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_telecom_bathdr`
--

LOCK TABLES `psc_telecom_bathdr` WRITE;
/*!40000 ALTER TABLE `psc_telecom_bathdr` DISABLE KEYS */;
/*!40000 ALTER TABLE `psc_telecom_bathdr` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_telecom_batuser`
--

DROP TABLE IF EXISTS `psc_telecom_batuser`;
CREATE TABLE `psc_telecom_batuser` (
  `PK_psc_telecom_batuser` int(11) NOT NULL auto_increment,
  `FK_psc_telecom_bathdr` int(11) NOT NULL default '0',
  `psc_user` int(11) NOT NULL default '0',
  `is_sup` tinyint(1) NOT NULL default '0',
  `no_pass` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`PK_psc_telecom_batuser`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_telecom_batuser`
--

LOCK TABLES `psc_telecom_batuser` WRITE;
/*!40000 ALTER TABLE `psc_telecom_batuser` DISABLE KEYS */;
/*!40000 ALTER TABLE `psc_telecom_batuser` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_telecom_repset`
--

DROP TABLE IF EXISTS `psc_telecom_repset`;
CREATE TABLE `psc_telecom_repset` (
  `PK_psc_telecom_repset` int(11) NOT NULL auto_increment,
  `Setting` varchar(30) NOT NULL default '',
  `Value` text,
  PRIMARY KEY  (`PK_psc_telecom_repset`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_telecom_repset`
--

LOCK TABLES `psc_telecom_repset` WRITE;
/*!40000 ALTER TABLE `psc_telecom_repset` DISABLE KEYS */;
INSERT INTO `psc_telecom_repset` VALUES (1,'schema','3');
/*!40000 ALTER TABLE `psc_telecom_repset` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_telecom_schema`
--

DROP TABLE IF EXISTS `psc_telecom_schema`;
CREATE TABLE `psc_telecom_schema` (
  `PK_psc_telecom_schema` int(11) NOT NULL auto_increment,
  `Value` text NOT NULL,
  PRIMARY KEY  (`PK_psc_telecom_schema`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_telecom_schema`
--

LOCK TABLES `psc_telecom_schema` WRITE;
/*!40000 ALTER TABLE `psc_telecom_schema` DISABLE KEYS */;
INSERT INTO `psc_telecom_schema` VALUES (2,'ALTER TABLE `Contact` ADD COLUMN `JobDescription` VARCHAR (60) DEFAULT NULL AFTER `Company`, ADD COLUMN `Email` VARCHAR (60) DEFAULT NULL AFTER `Title`'),(3,'ALTER TABLE `Contact` ADD COLUMN `EntryID` VARCHAR (60) DEFAULT NULL AFTER `Email`');
/*!40000 ALTER TABLE `psc_telecom_schema` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_telecom_tables`
--

DROP TABLE IF EXISTS `psc_telecom_tables`;
CREATE TABLE `psc_telecom_tables` (
  `PK_psc_telecom_tables` int(11) NOT NULL auto_increment,
  `Tablename` varchar(60) NOT NULL default '',
  `filter` varchar(255) default NULL,
  `frozen` tinyint(1) NOT NULL default '0',
  `anonymous` tinyint(1) NOT NULL default '0',
  `last_psc_id` int(11) NOT NULL default '0',
  `last_psc_batch` int(11) NOT NULL default '0',
  PRIMARY KEY  (`PK_psc_telecom_tables`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_telecom_tables`
--

LOCK TABLES `psc_telecom_tables` WRITE;
/*!40000 ALTER TABLE `psc_telecom_tables` DISABLE KEYS */;
INSERT INTO `psc_telecom_tables` VALUES (1,'Contact','WHERE 0',0,0,0,0),(2,'Contact_Users_Sync','WHERE 0',0,0,0,0),(3,'IncomingLog','WHERE 0',0,0,0,0),(4,'OutgoingLog','WHERE 0',0,0,0,0),(5,'PhoneNumber','WHERE 0',0,0,0,0),(6,'PhoneNumber_Users_Sync','WHERE 0',0,0,0,0),(7,'PhoneType',NULL,0,0,0,0),(8,'SpeedDial','WHERE 0',0,0,0,0),(9,'Users','WHERE 0',0,0,0,0),(10,'CallersForMe','WHERE 0',0,0,0,0),(11,'Line_HouseMode','WHERE 0',0,0,0,0),(12,'PriorityCallers','WHERE 0',0,0,0,0),(13,'UserRouting','WHERE 0',0,0,0,0);
/*!40000 ALTER TABLE `psc_telecom_tables` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2008-05-06 23:06:12
