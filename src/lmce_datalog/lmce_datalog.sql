-- MySQL dump 10.11
--
-- Host: localhost    Database: lmce_datalog
-- ------------------------------------------------------
-- Server version	5.0.67-0ubuntu6

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
-- Table structure for table `Datapoints`
--

DROP TABLE IF EXISTS `Datapoints`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Datapoints` (
  `PK_Datapoints` int(11) NOT NULL auto_increment,
  `Datapoint` float default NULL,
  `FK_Unit` int(11) NOT NULL default '0',
  `EK_Device` int(11) NOT NULL default '0',
  `timestamp` timestamp NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  PRIMARY KEY  (`PK_Datapoints`)
) ENGINE=MyISAM AUTO_INCREMENT=63827 DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Datapoints`
--

LOCK TABLES `Datapoints` WRITE;
/*!40000 ALTER TABLE `Datapoints` DISABLE KEYS */;
/*!40000 ALTER TABLE `Datapoints` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Unit`
--

DROP TABLE IF EXISTS `Unit`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Unit` (
  `PK_Unit` int(11) NOT NULL auto_increment,
  `Description` varchar(30) NOT NULL,
  `Unit` varchar(30) NOT NULL,
  `Precision` int(11) NOT NULL,
  PRIMARY KEY  (`PK_Unit`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Unit`
--

LOCK TABLES `Unit` WRITE;
/*!40000 ALTER TABLE `Unit` DISABLE KEYS */;
INSERT INTO `Unit` VALUES (1,'CO2 level','ppm',0),(2,'Temperature','degC',1),(3,'Humidity','%',0),(4,'Brightness','Lux',0),(5,'State Changed','State',0),(6,'Voltage','Volts',0),(7,'Power Usage','Kilo-Watts',0),(8,'Energy Cost','$/Hour',0),(9,'Sensor','Tripped',1),(10, 'Cumulated Energy', 'kWh', 0);
/*!40000 ALTER TABLE `Unit` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-09-14 17:19:17
