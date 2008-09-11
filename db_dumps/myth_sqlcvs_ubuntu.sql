-- MySQL dump 10.11
--
-- Host: localhost    Database: myth_sqlcvs
-- ------------------------------------------------------
-- Server version	5.0.45-Debian_1ubuntu3.1-log

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
-- Table structure for table `DTV_Channel`
--

DROP TABLE IF EXISTS `DTV_Channel`;
CREATE TABLE `DTV_Channel` (
  `PK_DTV_Channel` int(11) NOT NULL auto_increment,
  `FK_Provider` int(11) NOT NULL default '0',
  `Callsign` varchar(10) NOT NULL default '',
  `Frequency` varchar(10) NOT NULL default '',
  `Modulation` varchar(10) NOT NULL default '',
  `APID` int(11) NOT NULL default '0',
  `VPID` int(11) NOT NULL default '0',
  `TPID` int(11) NOT NULL default '0',
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_DTV_Channel`)
) ENGINE=MyISAM AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `DTV_Channel`
--

LOCK TABLES `DTV_Channel` WRITE;
/*!40000 ALTER TABLE `DTV_Channel` DISABLE KEYS */;
INSERT INTO `DTV_Channel` VALUES (1,25,'HBOHDP','519000000','qam_256',0,0,1,1,2,NULL,0,'2006-06-17 03:38:38',NULL),(2,25,'KGODT','507000000','qam_256',0,0,2,2,2,NULL,0,'2006-06-17 03:38:38',NULL),(3,25,'HDNET','507000000','qam_256',0,0,1,3,2,NULL,0,'2006-06-17 03:38:38',NULL),(4,25,'KNBCDT','591000000','qam_256',0,0,1,4,2,NULL,0,'2006-06-17 03:38:34',NULL),(5,25,'KABCDT','591000000','qam_256',0,0,2,5,2,NULL,0,'2006-06-17 03:38:35',NULL),(6,25,'KTTVDT','597000000','qam_256',0,0,2,6,2,NULL,0,'2006-06-17 03:38:35',NULL),(7,25,'KCETDT','597000000','qam_256',0,0,1,7,2,NULL,0,'2006-06-17 03:38:36',NULL),(8,25,'KNBCDT2','579000000','qam_256',0,0,6,8,2,NULL,0,'2006-06-17 03:38:36',NULL),(9,25,'KCBS','699000000','qam_256',0,0,7,9,2,NULL,0,'2006-06-17 03:38:36',NULL),(10,25,'KTLADT','741000000','qam_256',0,0,1,10,2,NULL,0,'2006-06-17 03:38:36',NULL),(11,25,'KCBSDT','753000000','qam_256',0,0,9,11,2,NULL,0,'2006-06-17 03:38:36',NULL);
/*!40000 ALTER TABLE `DTV_Channel` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `DTV_Channel_pschist`
--

DROP TABLE IF EXISTS `DTV_Channel_pschist`;
CREATE TABLE `DTV_Channel_pschist` (
  `PK_DTV_Channel` int(11) default NULL,
  `FK_Provider` int(11) default NULL,
  `Callsign` varchar(10) default NULL,
  `Frequency` varchar(10) default NULL,
  `Modulation` varchar(10) default NULL,
  `APID` int(11) default NULL,
  `VPID` int(11) default NULL,
  `TPID` int(11) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  `psc_toc` tinyint(1) default '0',
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `DTV_Channel_pschist`
--

LOCK TABLES `DTV_Channel_pschist` WRITE;
/*!40000 ALTER TABLE `DTV_Channel_pschist` DISABLE KEYS */;
INSERT INTO `DTV_Channel_pschist` VALUES (1,24,'HBOHDP','519000000','qam_256',0,0,1,1,1,NULL,NULL,'2006-05-29 09:44:36',NULL,0),(1,25,'HBOHDP','519000000','qam_256',0,0,1,1,2,NULL,NULL,'2006-06-17 03:38:38',NULL,2),(2,24,'KGODT','507000000','qam_256',0,0,2,2,1,NULL,NULL,'2006-05-29 09:44:36',NULL,0),(2,25,'KGODT','507000000','qam_256',0,0,2,2,2,NULL,NULL,'2006-06-17 03:38:38',NULL,2),(3,24,'HDNET','507000000','qam_256',0,0,1,3,1,NULL,NULL,'2006-05-29 09:44:37',NULL,0),(3,25,'HDNET','507000000','qam_256',0,0,1,3,2,NULL,NULL,'2006-06-17 03:38:38',NULL,2),(4,25,'KNBCDT','591000000','qam_256',0,0,1,4,2,NULL,NULL,'2006-06-17 03:38:34',NULL,0),(5,25,'KABCDT','591000000','qam_256',0,0,2,5,2,NULL,NULL,'2006-06-17 03:38:35',NULL,0),(6,25,'KTTVDT','597000000','qam_256',0,0,2,6,2,NULL,NULL,'2006-06-17 03:38:35',NULL,0),(7,25,'KCETDT','597000000','qam_256',0,0,1,7,2,NULL,NULL,'2006-06-17 03:38:36',NULL,0),(8,25,'KNBCDT2','579000000','qam_256',0,0,6,8,2,NULL,NULL,'2006-06-17 03:38:36',NULL,0),(9,25,'KCBS','699000000','qam_256',0,0,7,9,2,NULL,NULL,'2006-06-17 03:38:36',NULL,0),(10,25,'KTLADT','741000000','qam_256',0,0,1,10,2,NULL,NULL,'2006-06-17 03:38:36',NULL,0),(11,25,'KCBSDT','753000000','qam_256',0,0,9,11,2,NULL,NULL,'2006-06-17 03:38:36',NULL,0);
/*!40000 ALTER TABLE `DTV_Channel_pschist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `DTV_Channel_pschmask`
--

DROP TABLE IF EXISTS `DTV_Channel_pschmask`;
CREATE TABLE `DTV_Channel_pschmask` (
  `PK_DTV_Channel` tinyint(1) default NULL,
  `FK_Provider` tinyint(1) default NULL,
  `Callsign` tinyint(1) default NULL,
  `Frequency` tinyint(1) default NULL,
  `Modulation` tinyint(1) default NULL,
  `APID` tinyint(1) default NULL,
  `VPID` tinyint(1) default NULL,
  `TPID` tinyint(1) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` tinyint(1) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` tinyint(1) default NULL,
  `psc_restrict` tinyint(1) default NULL,
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `DTV_Channel_pschmask`
--

LOCK TABLES `DTV_Channel_pschmask` WRITE;
/*!40000 ALTER TABLE `DTV_Channel_pschmask` DISABLE KEYS */;
INSERT INTO `DTV_Channel_pschmask` VALUES (NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,1,NULL,NULL,NULL,NULL),(NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,1,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,2,1,NULL,NULL,NULL,NULL),(NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,2,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,3,1,NULL,NULL,NULL,NULL),(NULL,1,NULL,NULL,NULL,NULL,NULL,NULL,3,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,4,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,5,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,6,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,7,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,8,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,9,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,10,2,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,11,2,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `DTV_Channel_pschmask` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Picture`
--

DROP TABLE IF EXISTS `Picture`;
CREATE TABLE `Picture` (
  `EK_Picture` int(11) NOT NULL default '0',
  `chanid` int(11) unsigned default NULL,
  `seriesid` varchar(12) default NULL,
  `programid` varchar(20) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`EK_Picture`),
  KEY `programid` (`programid`),
  KEY `seriesid` (`seriesid`),
  KEY `chanid` (`chanid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Picture`
--

LOCK TABLES `Picture` WRITE;
/*!40000 ALTER TABLE `Picture` DISABLE KEYS */;
/*!40000 ALTER TABLE `Picture` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Picture_pschist`
--

DROP TABLE IF EXISTS `Picture_pschist`;
CREATE TABLE `Picture_pschist` (
  `EK_Picture` int(11) default NULL,
  `chanid` int(11) unsigned default NULL,
  `seriesid` varchar(12) default NULL,
  `programid` varchar(20) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  `psc_toc` tinyint(1) default '0',
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Picture_pschist`
--

LOCK TABLES `Picture_pschist` WRITE;
/*!40000 ALTER TABLE `Picture_pschist` DISABLE KEYS */;
/*!40000 ALTER TABLE `Picture_pschist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Picture_pschmask`
--

DROP TABLE IF EXISTS `Picture_pschmask`;
CREATE TABLE `Picture_pschmask` (
  `EK_Picture` tinyint(1) default NULL,
  `chanid` tinyint(1) default NULL,
  `seriesid` tinyint(1) default NULL,
  `programid` tinyint(1) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` tinyint(1) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` tinyint(1) default NULL,
  `psc_restrict` tinyint(1) default NULL,
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Picture_pschmask`
--

LOCK TABLES `Picture_pschmask` WRITE;
/*!40000 ALTER TABLE `Picture_pschmask` DISABLE KEYS */;
/*!40000 ALTER TABLE `Picture_pschmask` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PostalCode_Provider`
--

DROP TABLE IF EXISTS `PostalCode_Provider`;
CREATE TABLE `PostalCode_Provider` (
  `EK_PostalCode` int(11) NOT NULL default '0',
  `FK_Provider` int(11) NOT NULL default '0',
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`EK_PostalCode`,`FK_Provider`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `PostalCode_Provider`
--

LOCK TABLES `PostalCode_Provider` WRITE;
/*!40000 ALTER TABLE `PostalCode_Provider` DISABLE KEYS */;
INSERT INTO `PostalCode_Provider` VALUES (38072,3,1,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,4,2,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,5,3,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,6,4,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,8,5,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,18,6,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,19,7,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,20,8,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,21,9,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,22,10,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38072,23,11,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,3,12,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,4,13,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,5,14,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,6,15,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,8,16,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,11,17,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,12,18,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,13,19,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,14,20,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,15,21,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,16,22,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(38633,17,23,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,1,24,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,2,25,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,3,26,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,4,27,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,5,28,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,6,29,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,7,30,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,8,31,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,9,32,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(39743,10,33,NULL,NULL,0,'2005-12-25 07:04:59',NULL);
/*!40000 ALTER TABLE `PostalCode_Provider` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PostalCode_Provider_pschist`
--

DROP TABLE IF EXISTS `PostalCode_Provider_pschist`;
CREATE TABLE `PostalCode_Provider_pschist` (
  `EK_PostalCode` int(11) default NULL,
  `FK_Provider` int(11) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  `psc_toc` tinyint(1) default '0',
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `PostalCode_Provider_pschist`
--

LOCK TABLES `PostalCode_Provider_pschist` WRITE;
/*!40000 ALTER TABLE `PostalCode_Provider_pschist` DISABLE KEYS */;
INSERT INTO `PostalCode_Provider_pschist` VALUES (38072,3,1,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,4,2,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,5,3,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,6,4,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,8,5,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,18,6,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,19,7,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,20,8,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,21,9,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,22,10,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38072,23,11,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,3,12,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,4,13,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,5,14,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,6,15,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,8,16,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,11,17,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,12,18,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,13,19,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,14,20,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,15,21,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,16,22,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(38633,17,23,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,1,24,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,2,25,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,3,26,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,4,27,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,5,28,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,6,29,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,7,30,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,8,31,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,9,32,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(39743,10,33,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(94066,24,34,1,NULL,NULL,'2006-05-29 09:44:38',NULL,0),(94066,25,34,2,NULL,NULL,'2006-06-17 03:38:38',NULL,2),(NULL,NULL,34,3,NULL,NULL,'2006-08-25 17:43:06',NULL,1),(91354,25,35,2,NULL,NULL,'2006-06-17 03:38:37',NULL,0),(NULL,NULL,35,3,NULL,NULL,'2006-08-25 17:43:07',NULL,1);
/*!40000 ALTER TABLE `PostalCode_Provider_pschist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PostalCode_Provider_pschmask`
--

DROP TABLE IF EXISTS `PostalCode_Provider_pschmask`;
CREATE TABLE `PostalCode_Provider_pschmask` (
  `EK_PostalCode` tinyint(1) default NULL,
  `FK_Provider` tinyint(1) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` tinyint(1) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` tinyint(1) default NULL,
  `psc_restrict` tinyint(1) default NULL,
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `PostalCode_Provider_pschmask`
--

LOCK TABLES `PostalCode_Provider_pschmask` WRITE;
/*!40000 ALTER TABLE `PostalCode_Provider_pschmask` DISABLE KEYS */;
INSERT INTO `PostalCode_Provider_pschmask` VALUES (1,1,1,0,1,1,1,1),(1,1,2,0,1,1,1,1),(1,1,3,0,1,1,1,1),(1,1,4,0,1,1,1,1),(1,1,5,0,1,1,1,1),(1,1,6,0,1,1,1,1),(1,1,7,0,1,1,1,1),(1,1,8,0,1,1,1,1),(1,1,9,0,1,1,1,1),(1,1,10,0,1,1,1,1),(1,1,11,0,1,1,1,1),(1,1,12,0,1,1,1,1),(1,1,13,0,1,1,1,1),(1,1,14,0,1,1,1,1),(1,1,15,0,1,1,1,1),(1,1,16,0,1,1,1,1),(1,1,17,0,1,1,1,1),(1,1,18,0,1,1,1,1),(1,1,19,0,1,1,1,1),(1,1,20,0,1,1,1,1),(1,1,21,0,1,1,1,1),(1,1,22,0,1,1,1,1),(1,1,23,0,1,1,1,1),(1,1,24,0,1,1,1,1),(1,1,25,0,1,1,1,1),(1,1,26,0,1,1,1,1),(1,1,27,0,1,1,1,1),(1,1,28,0,1,1,1,1),(1,1,29,0,1,1,1,1),(1,1,30,0,1,1,1,1),(1,1,31,0,1,1,1,1),(1,1,32,0,1,1,1,1),(1,1,33,0,1,1,1,1),(NULL,NULL,34,1,NULL,NULL,NULL,NULL),(NULL,1,34,2,NULL,NULL,NULL,NULL),(NULL,NULL,35,2,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `PostalCode_Provider_pschmask` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Provider`
--

DROP TABLE IF EXISTS `Provider`;
CREATE TABLE `Provider` (
  `PK_Provider` int(11) NOT NULL auto_increment,
  `Description` varchar(50) default NULL,
  `FK_ProviderType` int(11) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_Provider`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Provider`
--

LOCK TABLES `Provider` WRITE;
/*!40000 ALTER TABLE `Provider` DISABLE KEYS */;
INSERT INTO `Provider` VALUES (1,'Comcast - Daly City',1,1,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(2,'Comcast - Daly City - Digital',2,2,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(3,'4DTV - USA - Satellite',3,3,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(4,'AFN Satellite - USA - Satellite',3,4,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(5,'C-Band - USA - Satellite',3,5,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(6,'DIRECTV - USA - Satellite',3,6,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(7,'DIRECTV San Francisco - San Francisco - Satellite',3,7,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(8,'DISH Network - USA - Satellite',3,8,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(9,'DISH San Francisco - San Francisco - Satellite',3,9,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(10,'Local Broadcast Listings - Antenna (for 94005)',4,10,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(11,'Comcast - Newhall',1,11,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(12,'SBC - Los Angeles',1,12,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(13,'SBC Entertainment Plus - Los Angeles',1,13,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(14,'Comcast - Newhall - Digital',2,14,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(15,'DIRECTV Los Angeles - Los Angeles - Satellite',3,15,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(16,'DISH Los Angeles - Los Angeles - Satellite',3,16,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(17,'Local Broadcast Listings - Antenna (for 91355)',4,17,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(18,'Country Club Towers - Las Vegas',1,18,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(19,'Cox Communications - Las Vegas',1,19,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(20,'Cox Communications - Las Vegas - Digital',2,20,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(21,'DIRECTV Las Vegas - Las Vegas - Satellite',3,21,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(22,'DISH Las Vegas - Las Vegas - Satellite',3,22,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(23,'Local Broadcast Listings - Antenna (for 89109)',4,23,NULL,NULL,0,'2005-12-25 07:04:59',NULL);
/*!40000 ALTER TABLE `Provider` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ProviderType`
--

DROP TABLE IF EXISTS `ProviderType`;
CREATE TABLE `ProviderType` (
  `PK_ProviderType` int(11) NOT NULL auto_increment,
  `Description` varchar(35) default NULL,
  `Define` varchar(35) default NULL,
  `psc_id` int(11) default NULL,
  `psc_batch` int(11) default NULL,
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default '0',
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  PRIMARY KEY  (`PK_ProviderType`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `ProviderType`
--

LOCK TABLES `ProviderType` WRITE;
/*!40000 ALTER TABLE `ProviderType` DISABLE KEYS */;
INSERT INTO `ProviderType` VALUES (1,'Cable',NULL,1,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(2,'Cable Digital',NULL,2,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(3,'Satellite',NULL,3,NULL,NULL,0,'2005-12-25 07:04:59',NULL),(4,'Local Broadcast',NULL,4,NULL,NULL,0,'2005-12-25 07:04:59',NULL);
/*!40000 ALTER TABLE `ProviderType` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ProviderType_pschist`
--

DROP TABLE IF EXISTS `ProviderType_pschist`;
CREATE TABLE `ProviderType_pschist` (
  `PK_ProviderType` int(11) default NULL,
  `Description` varchar(35) default NULL,
  `Define` varchar(35) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  `psc_toc` tinyint(1) default '0',
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `ProviderType_pschist`
--

LOCK TABLES `ProviderType_pschist` WRITE;
/*!40000 ALTER TABLE `ProviderType_pschist` DISABLE KEYS */;
INSERT INTO `ProviderType_pschist` VALUES (1,'Cable',NULL,1,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(2,'Cable Digital',NULL,2,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(3,'Satellite',NULL,3,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(4,'Local Broadcast',NULL,4,0,NULL,0,'2005-12-25 07:04:59',NULL,0);
/*!40000 ALTER TABLE `ProviderType_pschist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ProviderType_pschmask`
--

DROP TABLE IF EXISTS `ProviderType_pschmask`;
CREATE TABLE `ProviderType_pschmask` (
  `PK_ProviderType` tinyint(1) default NULL,
  `Description` tinyint(1) default NULL,
  `Define` tinyint(1) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` tinyint(1) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` tinyint(1) default NULL,
  `psc_restrict` tinyint(1) default NULL,
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `ProviderType_pschmask`
--

LOCK TABLES `ProviderType_pschmask` WRITE;
/*!40000 ALTER TABLE `ProviderType_pschmask` DISABLE KEYS */;
INSERT INTO `ProviderType_pschmask` VALUES (1,1,1,1,0,1,1,1,1),(1,1,1,2,0,1,1,1,1),(1,1,1,3,0,1,1,1,1),(1,1,1,4,0,1,1,1,1);
/*!40000 ALTER TABLE `ProviderType_pschmask` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Provider_pschist`
--

DROP TABLE IF EXISTS `Provider_pschist`;
CREATE TABLE `Provider_pschist` (
  `PK_Provider` int(11) default NULL,
  `Description` varchar(50) default NULL,
  `FK_ProviderType` int(11) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` int(11) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `psc_restrict` int(11) default NULL,
  `psc_toc` tinyint(1) default '0',
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Provider_pschist`
--

LOCK TABLES `Provider_pschist` WRITE;
/*!40000 ALTER TABLE `Provider_pschist` DISABLE KEYS */;
INSERT INTO `Provider_pschist` VALUES (1,'Comcast - Daly City',1,1,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(2,'Comcast - Daly City - Digital',2,2,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(3,'4DTV - USA - Satellite',3,3,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(4,'AFN Satellite - USA - Satellite',3,4,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(5,'C-Band - USA - Satellite',3,5,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(6,'DIRECTV - USA - Satellite',3,6,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(7,'DIRECTV San Francisco - San Francisco - Satellite',3,7,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(8,'DISH Network - USA - Satellite',3,8,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(9,'DISH San Francisco - San Francisco - Satellite',3,9,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(10,'Local Broadcast Listings - Antenna (for 94005)',4,10,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(11,'Comcast - Newhall',1,11,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(12,'SBC - Los Angeles',1,12,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(13,'SBC Entertainment Plus - Los Angeles',1,13,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(14,'Comcast - Newhall - Digital',2,14,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(15,'DIRECTV Los Angeles - Los Angeles - Satellite',3,15,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(16,'DISH Los Angeles - Los Angeles - Satellite',3,16,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(17,'Local Broadcast Listings - Antenna (for 91355)',4,17,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(18,'Country Club Towers - Las Vegas',1,18,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(19,'Cox Communications - Las Vegas',1,19,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(20,'Cox Communications - Las Vegas - Digital',2,20,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(21,'DIRECTV Las Vegas - Las Vegas - Satellite',3,21,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(22,'DISH Las Vegas - Las Vegas - Satellite',3,22,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(23,'Local Broadcast Listings - Antenna (for 89109)',4,23,0,NULL,0,'2005-12-25 07:04:59',NULL,0),(24,'San Bruno Cable',2,24,1,NULL,NULL,'2006-05-29 09:44:39',NULL,0),(NULL,NULL,NULL,24,3,NULL,NULL,'2006-08-25 17:43:09',NULL,1),(25,'Comcast',2,25,2,NULL,NULL,'2006-06-17 03:38:37',NULL,0),(NULL,NULL,NULL,25,3,NULL,NULL,'2006-08-25 17:43:09',NULL,1);
/*!40000 ALTER TABLE `Provider_pschist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Provider_pschmask`
--

DROP TABLE IF EXISTS `Provider_pschmask`;
CREATE TABLE `Provider_pschmask` (
  `PK_Provider` tinyint(1) default NULL,
  `Description` tinyint(1) default NULL,
  `FK_ProviderType` tinyint(1) default NULL,
  `psc_id` int(11) NOT NULL default '0',
  `psc_batch` int(11) NOT NULL default '0',
  `psc_user` tinyint(1) default NULL,
  `psc_frozen` tinyint(1) default NULL,
  `psc_mod` tinyint(1) default NULL,
  `psc_restrict` tinyint(1) default NULL,
  PRIMARY KEY  (`psc_id`,`psc_batch`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `Provider_pschmask`
--

LOCK TABLES `Provider_pschmask` WRITE;
/*!40000 ALTER TABLE `Provider_pschmask` DISABLE KEYS */;
INSERT INTO `Provider_pschmask` VALUES (1,1,1,1,0,1,1,1,1),(1,1,1,2,0,1,1,1,1),(1,1,1,3,0,1,1,1,1),(1,1,1,4,0,1,1,1,1),(1,1,1,5,0,1,1,1,1),(1,1,1,6,0,1,1,1,1),(1,1,1,7,0,1,1,1,1),(1,1,1,8,0,1,1,1,1),(1,1,1,9,0,1,1,1,1),(1,1,1,10,0,1,1,1,1),(1,1,1,11,0,1,1,1,1),(1,1,1,12,0,1,1,1,1),(1,1,1,13,0,1,1,1,1),(1,1,1,14,0,1,1,1,1),(1,1,1,15,0,1,1,1,1),(1,1,1,16,0,1,1,1,1),(1,1,1,17,0,1,1,1,1),(1,1,1,18,0,1,1,1,1),(1,1,1,19,0,1,1,1,1),(1,1,1,20,0,1,1,1,1),(1,1,1,21,0,1,1,1,1),(1,1,1,22,0,1,1,1,1),(1,1,1,23,0,1,1,1,1),(NULL,NULL,NULL,24,1,NULL,NULL,NULL,NULL),(NULL,NULL,NULL,25,2,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `Provider_pschmask` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_myth_batdet`
--

DROP TABLE IF EXISTS `psc_myth_batdet`;
CREATE TABLE `psc_myth_batdet` (
  `PK_psc_myth_batdet` int(11) NOT NULL auto_increment,
  `FK_psc_myth_bathdr` int(11) NOT NULL default '0',
  `Tablename` varchar(60) NOT NULL default '',
  `New` int(11) NOT NULL default '0',
  `Deleted` int(11) NOT NULL default '0',
  `Modified` int(11) NOT NULL default '0',
  `FK_psc_myth_bathdr_orig` int(11) NOT NULL default '0',
  `FK_psc_myth_bathdr_auth` int(11) NOT NULL default '0',
  `FK_psc_myth_bathdr_unauth` int(11) NOT NULL default '0',
  PRIMARY KEY  (`PK_psc_myth_batdet`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_myth_batdet`
--

LOCK TABLES `psc_myth_batdet` WRITE;
/*!40000 ALTER TABLE `psc_myth_batdet` DISABLE KEYS */;
INSERT INTO `psc_myth_batdet` VALUES (1,1,'DTV_Channel',3,0,0,1,0,0),(2,1,'PostalCode_Provider',1,0,0,1,0,0),(3,1,'Provider',1,0,0,1,0,0),(4,2,'DTV_Channel',8,0,0,2,0,0),(5,2,'PostalCode_Provider',1,0,0,2,0,0),(6,2,'Provider',1,0,0,2,0,0),(7,2,'DTV_Channel',0,0,3,2,0,0),(8,2,'PostalCode_Provider',0,0,1,2,0,0),(9,3,'PostalCode_Provider',0,2,0,3,0,0),(10,3,'Provider',0,2,0,3,0,0);
/*!40000 ALTER TABLE `psc_myth_batdet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_myth_bathdr`
--

DROP TABLE IF EXISTS `psc_myth_bathdr`;
CREATE TABLE `psc_myth_bathdr` (
  `PK_psc_myth_bathdr` int(11) NOT NULL auto_increment,
  `IPAddress` varchar(16) default NULL,
  `date` datetime default NULL,
  `comments` text,
  PRIMARY KEY  (`PK_psc_myth_bathdr`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_myth_bathdr`
--

LOCK TABLES `psc_myth_bathdr` WRITE;
/*!40000 ALTER TABLE `psc_myth_bathdr` DISABLE KEYS */;
INSERT INTO `psc_myth_bathdr` VALUES (1,'65.172.233.171','2006-05-29 02:44:35',''),(2,'68.183.156.190','2006-06-16 20:38:33',''),(3,'193.77.90.224','2006-08-25 10:43:05','');
/*!40000 ALTER TABLE `psc_myth_bathdr` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_myth_batuser`
--

DROP TABLE IF EXISTS `psc_myth_batuser`;
CREATE TABLE `psc_myth_batuser` (
  `PK_psc_myth_batuser` int(11) NOT NULL auto_increment,
  `FK_psc_myth_bathdr` int(11) NOT NULL default '0',
  `psc_user` int(11) NOT NULL default '0',
  `is_sup` tinyint(1) NOT NULL default '0',
  `no_pass` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`PK_psc_myth_batuser`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_myth_batuser`
--

LOCK TABLES `psc_myth_batuser` WRITE;
/*!40000 ALTER TABLE `psc_myth_batuser` DISABLE KEYS */;
INSERT INTO `psc_myth_batuser` VALUES (1,1,0,1,0),(2,2,0,1,0),(3,3,0,1,0);
/*!40000 ALTER TABLE `psc_myth_batuser` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_myth_repset`
--

DROP TABLE IF EXISTS `psc_myth_repset`;
CREATE TABLE `psc_myth_repset` (
  `PK_psc_myth_repset` int(11) NOT NULL auto_increment,
  `Setting` varchar(30) NOT NULL default '',
  `Value` text,
  PRIMARY KEY  (`PK_psc_myth_repset`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_myth_repset`
--

LOCK TABLES `psc_myth_repset` WRITE;
/*!40000 ALTER TABLE `psc_myth_repset` DISABLE KEYS */;
INSERT INTO `psc_myth_repset` VALUES (1,'schema','1');
/*!40000 ALTER TABLE `psc_myth_repset` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_myth_schema`
--

DROP TABLE IF EXISTS `psc_myth_schema`;
CREATE TABLE `psc_myth_schema` (
  `PK_psc_myth_schema` int(11) NOT NULL auto_increment,
  `Value` text NOT NULL,
  PRIMARY KEY  (`PK_psc_myth_schema`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_myth_schema`
--

LOCK TABLES `psc_myth_schema` WRITE;
/*!40000 ALTER TABLE `psc_myth_schema` DISABLE KEYS */;
/*!40000 ALTER TABLE `psc_myth_schema` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_myth_tables`
--

DROP TABLE IF EXISTS `psc_myth_tables`;
CREATE TABLE `psc_myth_tables` (
  `PK_psc_myth_tables` int(11) NOT NULL auto_increment,
  `Tablename` varchar(60) NOT NULL default '',
  `filter` varchar(255) default NULL,
  `frozen` tinyint(1) NOT NULL default '0',
  `anonymous` tinyint(1) NOT NULL default '0',
  `last_psc_id` int(11) NOT NULL default '0',
  `last_psc_batch` int(11) NOT NULL default '0',
  PRIMARY KEY  (`PK_psc_myth_tables`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;

--
-- Dumping data for table `psc_myth_tables`
--

LOCK TABLES `psc_myth_tables` WRITE;
/*!40000 ALTER TABLE `psc_myth_tables` DISABLE KEYS */;
INSERT INTO `psc_myth_tables` VALUES (1,'PostalCode_Provider',NULL,0,0,35,3),(2,'Provider',NULL,0,0,25,3),(3,'ProviderType',NULL,0,0,0,0),(4,'DTV_Channel',NULL,0,0,11,2),(5,'Picture',NULL,0,0,0,0);
/*!40000 ALTER TABLE `psc_myth_tables` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2008-04-09 23:45:23
