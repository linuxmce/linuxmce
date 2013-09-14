-- MySQL dump 10.13  Distrib 5.1.67, for debian-linux-gnu (i486)
--
-- Host: localhost    Database: lmce_game
-- ------------------------------------------------------
-- Server version	5.1.67-0ubuntu0.10.04.1

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
-- Table structure for table `Configuration`
--

DROP TABLE IF EXISTS `Configuration`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Configuration` (
  `PK_Configuration` int(11) NOT NULL AUTO_INCREMENT,
  `Define` varchar(50) DEFAULT NULL,
  `FK_GameSystem` int(11) NOT NULL,
  `Description` varchar(1024) DEFAULT NULL,
  `Configuration` text,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`PK_Configuration`),
  KEY `FK_GameSystem` (`FK_GameSystem`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Configuration`
--

LOCK TABLES `Configuration` WRITE;
/*!40000 ALTER TABLE `Configuration` DISABLE KEYS */;
/*!40000 ALTER TABLE `Configuration` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `GameSystem`
--

DROP TABLE IF EXISTS `GameSystem`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `GameSystem` (
  `PK_GameSystem` int(11) NOT NULL AUTO_INCREMENT,
  `Define` varchar(50) DEFAULT NULL,
  `Description` varchar(50) DEFAULT NULL,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`PK_GameSystem`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `GameSystem`
--

LOCK TABLES `GameSystem` WRITE;
/*!40000 ALTER TABLE `GameSystem` DISABLE KEYS */;
INSERT INTO `GameSystem` VALUES (1,NULL,'Arcade',1,1,38890,0,'0000-00-00 00:00:00',NULL);
/*!40000 ALTER TABLE `GameSystem` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Rom`
--

DROP TABLE IF EXISTS `Rom`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Rom` (
  `PK_Rom` int(11) NOT NULL AUTO_INCREMENT,
  `FK_GameSystem` int(11) NOT NULL,
  `Romname` varchar(255) DEFAULT NULL,
  `md5` varchar(32) DEFAULT NULL,
  `FK_Configuration` int(11) DEFAULT NULL,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`PK_Rom`),
  KEY `FK_GameSystem` (`FK_GameSystem`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Rom`
--

LOCK TABLES `Rom` WRITE;
/*!40000 ALTER TABLE `Rom` DISABLE KEYS */;
/*!40000 ALTER TABLE `Rom` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `RomAttribute`
--

DROP TABLE IF EXISTS `RomAttribute`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `RomAttribute` (
  `PK_RomAttribute` int(11) NOT NULL AUTO_INCREMENT,
  `FK_RomAttributeType` int(11) NOT NULL,
  `FK_GameSystem` int(11) NOT NULL,
  `Name` longtext,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`PK_RomAttribute`),
  KEY `FK_RomAttributeType` (`FK_RomAttributeType`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `RomAttribute`
--

LOCK TABLES `RomAttribute` WRITE;
/*!40000 ALTER TABLE `RomAttribute` DISABLE KEYS */;
/*!40000 ALTER TABLE `RomAttribute` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `RomAttributeType`
--

DROP TABLE IF EXISTS `RomAttributeType`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `RomAttributeType` (
  `PK_RomAttributeType` int(11) NOT NULL AUTO_INCREMENT,
  `Define` varchar(50) DEFAULT NULL,
  `Description` varchar(50) DEFAULT NULL,
  `PicPriority` int(11) DEFAULT NULL,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`PK_RomAttributeType`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `RomAttributeType`
--

LOCK TABLES `RomAttributeType` WRITE;
/*!40000 ALTER TABLE `RomAttributeType` DISABLE KEYS */;
INSERT INTO `RomAttributeType` VALUES (1,'title','Title',NULL,1,1,38890,0,'0000-00-00 00:00:00',NULL),(2,'year','Year',NULL,2,1,38890,0,'0000-00-00 00:00:00',NULL),(3,'manufacturer','Manufacturer',NULL,3,1,38890,0,'0000-00-00 00:00:00',NULL),(4,'genre','Genre',NULL,4,1,38890,0,'0000-00-00 00:00:00',NULL),(5,'system','System',NULL,5,1,38890,0,'0000-00-00 00:00:00',NULL),(6,'subtitle','Subtitle',NULL,6,1,38890,0,'0000-00-00 00:00:00',NULL);
/*!40000 ALTER TABLE `RomAttributeType` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `RomTitles`
--

DROP TABLE IF EXISTS `RomTitles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `RomTitles` (
  `PK_RomTitles` int(11) NOT NULL,
  `Title` varchar(255) DEFAULT NULL,
  `TitleHash` varchar(64) DEFAULT NULL,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`PK_RomTitles`),
  KEY `TitleHash` (`TitleHash`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `RomTitles`
--

LOCK TABLES `RomTitles` WRITE;
/*!40000 ALTER TABLE `RomTitles` DISABLE KEYS */;
/*!40000 ALTER TABLE `RomTitles` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Rom_Configuration`
--

DROP TABLE IF EXISTS `Rom_Configuration`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Rom_Configuration` (
  `PK_Rom_Configuration` int(11) NOT NULL AUTO_INCREMENT,
  `FK_Rom` int(11) NOT NULL,
  `FK_Configuration` int(11) NOT NULL,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`PK_Rom_Configuration`),
  KEY `FK_Configuration` (`FK_Configuration`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Rom_Configuration`
--

LOCK TABLES `Rom_Configuration` WRITE;
/*!40000 ALTER TABLE `Rom_Configuration` DISABLE KEYS */;
/*!40000 ALTER TABLE `Rom_Configuration` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Rom_RomAttribute`
--

DROP TABLE IF EXISTS `Rom_RomAttribute`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Rom_RomAttribute` (
  `FK_Rom` int(11) NOT NULL,
  `FK_RomAttribute` int(11) NOT NULL,
  `psc_id` int(11) DEFAULT NULL,
  `psc_batch` int(11) DEFAULT NULL,
  `psc_user` int(11) DEFAULT NULL,
  `psc_frozen` tinyint(1) DEFAULT '0',
  `psc_mod` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `psc_restrict` int(11) DEFAULT NULL,
  PRIMARY KEY (`FK_Rom`,`FK_RomAttribute`),
  KEY `FK_Rom` (`FK_Rom`),
  KEY `FK_RomAttribute` (`FK_RomAttribute`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Rom_RomAttribute`
--

LOCK TABLES `Rom_RomAttribute` WRITE;
/*!40000 ALTER TABLE `Rom_RomAttribute` DISABLE KEYS */;
/*!40000 ALTER TABLE `Rom_RomAttribute` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_game_batdet`
--

DROP TABLE IF EXISTS `psc_game_batdet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `psc_game_batdet` (
  `PK_psc_game_batdet` int(11) NOT NULL AUTO_INCREMENT,
  `FK_psc_game_bathdr` int(11) NOT NULL,
  `Tablename` varchar(60) NOT NULL,
  `New` int(11) NOT NULL,
  `Deleted` int(11) NOT NULL,
  `Modified` int(11) NOT NULL,
  `FK_psc_game_bathdr_orig` int(11) NOT NULL,
  `FK_psc_game_bathdr_auth` int(11) NOT NULL,
  `FK_psc_game_bathdr_unauth` int(11) NOT NULL,
  PRIMARY KEY (`PK_psc_game_batdet`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `psc_game_batdet`
--

LOCK TABLES `psc_game_batdet` WRITE;
/*!40000 ALTER TABLE `psc_game_batdet` DISABLE KEYS */;
/*!40000 ALTER TABLE `psc_game_batdet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_game_bathdr`
--

DROP TABLE IF EXISTS `psc_game_bathdr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `psc_game_bathdr` (
  `PK_psc_game_bathdr` int(11) NOT NULL AUTO_INCREMENT,
  `IPAddress` varchar(16) DEFAULT NULL,
  `date` datetime DEFAULT NULL,
  `comments` text,
  PRIMARY KEY (`PK_psc_game_bathdr`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `psc_game_bathdr`
--

LOCK TABLES `psc_game_bathdr` WRITE;
/*!40000 ALTER TABLE `psc_game_bathdr` DISABLE KEYS */;
INSERT INTO `psc_game_bathdr` VALUES (1,NULL,'2013-06-16 14:08:33',NULL);
/*!40000 ALTER TABLE `psc_game_bathdr` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_game_batuser`
--

DROP TABLE IF EXISTS `psc_game_batuser`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `psc_game_batuser` (
  `PK_psc_game_batuser` int(11) NOT NULL AUTO_INCREMENT,
  `FK_psc_game_bathdr` int(11) NOT NULL,
  `psc_user` int(11) NOT NULL,
  `is_sup` tinyint(1) NOT NULL,
  `no_pass` tinyint(1) NOT NULL,
  PRIMARY KEY (`PK_psc_game_batuser`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `psc_game_batuser`
--

LOCK TABLES `psc_game_batuser` WRITE;
/*!40000 ALTER TABLE `psc_game_batuser` DISABLE KEYS */;
/*!40000 ALTER TABLE `psc_game_batuser` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_game_repset`
--

DROP TABLE IF EXISTS `psc_game_repset`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `psc_game_repset` (
  `PK_psc_game_repset` int(11) NOT NULL AUTO_INCREMENT,
  `Setting` varchar(30) NOT NULL,
  `Value` text,
  PRIMARY KEY (`PK_psc_game_repset`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `psc_game_repset`
--

LOCK TABLES `psc_game_repset` WRITE;
/*!40000 ALTER TABLE `psc_game_repset` DISABLE KEYS */;
INSERT INTO `psc_game_repset` VALUES (1,'schema','1');
/*!40000 ALTER TABLE `psc_game_repset` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_game_schema`
--

DROP TABLE IF EXISTS `psc_game_schema`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `psc_game_schema` (
  `PK_psc_game_schema` int(11) NOT NULL AUTO_INCREMENT,
  `Value` text NOT NULL,
  PRIMARY KEY (`PK_psc_game_schema`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `psc_game_schema`
--

LOCK TABLES `psc_game_schema` WRITE;
/*!40000 ALTER TABLE `psc_game_schema` DISABLE KEYS */;
/*!40000 ALTER TABLE `psc_game_schema` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `psc_game_tables`
--

DROP TABLE IF EXISTS `psc_game_tables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `psc_game_tables` (
  `PK_psc_game_tables` int(11) NOT NULL AUTO_INCREMENT,
  `Tablename` varchar(60) NOT NULL,
  `filter` varchar(255) DEFAULT NULL,
  `frozen` tinyint(1) NOT NULL DEFAULT '0',
  `anonymous` tinyint(1) NOT NULL DEFAULT '0',
  `last_psc_id` int(11) NOT NULL DEFAULT '0',
  `last_psc_batch` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`PK_psc_game_tables`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `psc_game_tables`
--

LOCK TABLES `psc_game_tables` WRITE;
/*!40000 ALTER TABLE `psc_game_tables` DISABLE KEYS */;
INSERT INTO `psc_game_tables` VALUES (1,'Configuration',NULL,0,0,0,0),(2,'GameSystem',NULL,0,0,0,0),(3,'Rom',NULL,0,0,0,0),(4,'RomAttribute',NULL,0,0,0,0),(5,'RomAttributeType',NULL,0,0,0,0),(6,'RomTitles',NULL,0,0,0,0),(7,'Rom_Configuration',NULL,0,0,0,0),(8,'Rom_RomAttribute',NULL,0,0,0,0);
/*!40000 ALTER TABLE `psc_game_tables` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-06-16 14:46:27
