-- MySQL dump 10.9
--
-- Host: localhost    Database: asterisk
-- ------------------------------------------------------
-- Server version	4.1.20

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `admin`
--

CREATE TABLE `admin` (
  `variable` varchar(20) NOT NULL default '',
  `value` varchar(80) NOT NULL default '',
  PRIMARY KEY  (`variable`)
);

--
-- Table structure for table `ampusers`
--

CREATE TABLE `ampusers` (
  `username` varchar(20) NOT NULL default '',
  `password` varchar(20) NOT NULL default '',
  `extension_low` varchar(20) NOT NULL default '',
  `extension_high` varchar(20) NOT NULL default '',
  `deptname` varchar(20) NOT NULL default '',
  `sections` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`username`)
);

--
-- Table structure for table `devices`
--

CREATE TABLE `devices` (
  `id` varchar(20) NOT NULL default '',
  `tech` varchar(10) NOT NULL default '',
  `dial` varchar(50) NOT NULL default '',
  `devicetype` varchar(5) NOT NULL default '',
  `user` varchar(50) default NULL,
  `description` varchar(50) default NULL,
  `emergency_cid` varchar(100) default NULL
);

--
-- Table structure for table `extensions`
--

CREATE TABLE `extensions` (
  `context` varchar(45) NOT NULL default 'default',
  `extension` varchar(45) NOT NULL default '',
  `priority` varchar(5) NOT NULL default '1',
  `application` varchar(45) NOT NULL default '',
  `args` varchar(255) default NULL,
  `descr` text,
  `flags` int(1) NOT NULL default '0',
  PRIMARY KEY  (`context`,`extension`,`priority`)
);

--
-- Table structure for table `featurecodes`
--

CREATE TABLE `featurecodes` (
  `modulename` varchar(50) NOT NULL default '',
  `featurename` varchar(50) NOT NULL default '',
  `description` varchar(200) NOT NULL default '',
  `defaultcode` varchar(20) default NULL,
  `customcode` varchar(20) default NULL,
  `enabled` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`modulename`,`featurename`),
  KEY `enabled` (`enabled`)
);

--
-- Table structure for table `freepbx_log`
--

CREATE TABLE `freepbx_log` (
  `id` int(11) NOT NULL auto_increment,
  `time` datetime NOT NULL default '0000-00-00 00:00:00',
  `section` varchar(50) default NULL,
  `level` enum('error','warning','debug','devel-debug') NOT NULL default 'error',
  `status` int(11) NOT NULL default '0',
  `message` text NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `time` (`time`,`level`)
);

--
-- Table structure for table `globals`
--

CREATE TABLE `globals` (
  `variable` char(20) NOT NULL default '',
  `value` char(50) NOT NULL default '',
  PRIMARY KEY  (`variable`)
);

--
-- Table structure for table `iax`
--

CREATE TABLE `iax` (
  `id` varchar(20) NOT NULL default '-1',
  `keyword` varchar(30) NOT NULL default '',
  `data` varchar(255) NOT NULL default '',
  `flags` int(1) NOT NULL default '0',
  PRIMARY KEY  (`id`,`keyword`)
);

--
-- Table structure for table `incoming`
--

CREATE TABLE `incoming` (
  `cidnum` varchar(20) default NULL,
  `extension` varchar(20) default NULL,
  `destination` varchar(50) default NULL,
  `faxexten` varchar(20) default NULL,
  `faxemail` varchar(50) default NULL,
  `answer` tinyint(1) default NULL,
  `wait` int(2) default NULL,
  `privacyman` tinyint(1) default NULL,
  `alertinfo` varchar(255) default NULL,
  `channel` varchar(20) default NULL,
  `ringing` varchar(20) default NULL,
  `mohclass` varchar(80) NOT NULL default 'default',
  `description` varchar(80) default NULL,
	`grppre` varchar (80) default NULL 
);

--
-- Table structure for table `modules`
--

CREATE TABLE `modules` (
  `id` int(11) NOT NULL auto_increment,
  `modulename` varchar(50) NOT NULL default '',
  `version` varchar(20) NOT NULL default '',
  `enabled` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`id`)
);

--
-- Table structure for table `module_xml`
--

CREATE TABLE `module_xml` (
	`id` varchar(20) NOT NULL default 'xml',
	`time` int(11) NOT NULL default '0',
	`data` blob NOT NULL,
  PRIMARY KEY  (`id`)
);


--
-- Table structure for table `queues`
--

CREATE TABLE `queues` (
  `id` varchar(45) NOT NULL default '-1',
  `keyword` varchar(30) NOT NULL default '',
  `data` varchar(150) NOT NULL default '',
  `flags` int(1) NOT NULL default '0',
  PRIMARY KEY  (`id`,`keyword`,`data`)
);

--
-- Table structure for table `sip`
--

CREATE TABLE `sip` (
  `id` varchar(20) NOT NULL default '-1',
  `keyword` varchar(30) NOT NULL default '',
  `data` varchar(255) NOT NULL default '',
  `flags` int(1) NOT NULL default '0',
  PRIMARY KEY  (`id`,`keyword`)
);

--
-- Table structure for table `users`
--

CREATE TABLE `users` (
  `extension` varchar(20) NOT NULL default '',
  `password` varchar(20) default NULL,
  `name` varchar(50) default NULL,
  `voicemail` varchar(50) default NULL,
  `ringtimer` int(3) default NULL,
  `noanswer` varchar(100) default NULL,
  `recording` varchar(50) default NULL,
  `outboundcid` varchar(50) default NULL,
  `directdid` varchar(50) default NULL,
  `didalert` varchar(50) default NULL,
  `faxexten` varchar(20) default NULL,
  `faxemail` varchar(50) default NULL,
  `answer` tinyint(1) default NULL,
  `wait` int(2) default NULL,
  `privacyman` tinyint(1) default NULL,
  `mohclass` varchar(80) NOT NULL default 'default',
  `sipname` varchar(50) default NULL
);

--
-- Table structure for table `zap`
--

CREATE TABLE `zap` (
  `id` varchar(20) NOT NULL default '-1',
  `keyword` varchar(30) NOT NULL default '',
  `data` varchar(255) NOT NULL default '',
  `flags` int(1) NOT NULL default '0',
  PRIMARY KEY  (`id`,`keyword`)
);

-- 
-- Table structure for table `notifications`
-- 

CREATE TABLE notifications (
  module varchar(24) NOT NULL default '',
  id varchar(24) NOT NULL default '',
  `level` int(11) NOT NULL default '0',
  display_text varchar(255) NOT NULL default '',
  extended_text blob NOT NULL,
  link varchar(255) NOT NULL default '',
  `reset` tinyint(4) NOT NULL default '0',
	candelete tinyint(4) NOT NULL default '0',
  `timestamp` int(11) NOT NULL default '0',
  PRIMARY KEY  (`module`,`id`)
);

CREATE TABLE IF NOT EXISTS `cronmanager` (
  `module` varchar(24) NOT NULL default '',
  `id` varchar(24) NOT NULL default '',
  `time` varchar(5) default NULL,
  `freq` int(11) NOT NULL default '0',
  `lasttime` int(11) NOT NULL default '0',
  `command` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`module`,`id`)
);

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

ALTER TABLE incoming ADD channel VARCHAR( 20 ) DEFAULT "";
ALTER TABLE incoming ADD mohclass VARCHAR ( 80 ) DEFAULT "default";
ALTER TABLE incoming ADD description VARCHAR ( 80 ) NULL;
ALTER TABLE incoming ADD alertinfo VARCHAR( 255 ) NULL;
ALTER TABLE incoming ADD ringing VARCHAR ( 20 ) NULL;
ALTER TABLE incoming ADD grppre VARCHAR ( 80 ) NULL;
ALTER TABLE users ADD directdid VARCHAR( 50 ) NULL;
ALTER TABLE users ADD didalert VARCHAR( 50 ) NULL;
ALTER TABLE users ADD mohclass VARCHAR ( 80 ) DEFAULT "default";
ALTER TABLE users ADD faxexten VARCHAR ( 20 ) NULL;
ALTER TABLE users ADD faxemail VARCHAR ( 50 ) NULL;
ALTER TABLE users ADD answer TINYINT ( 1 ) NULL;
ALTER TABLE users ADD wait INT ( 2 ) NULL;
ALTER TABLE users ADD privacyman TINYINT ( 1 ) NULL;
ALTER TABLE users ADD sipname VARCHAR ( 50 ) NULL;
ALTER TABLE devices ADD emergency_cid VARCHAR( 100 ) NULL;
ALTER TABLE iax CHANGE data data VARCHAR( 255 ) NOT NULL;
ALTER TABLE sip CHANGE data data VARCHAR( 255 ) NOT NULL;
ALTER TABLE zap CHANGE data data VARCHAR( 255 ) NOT NULL;
