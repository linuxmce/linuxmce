-- MySQL dump 9.11
--
-- Host: localhost    Database: asterisk
-- ------------------------------------------------------
-- Server version	4.0.23_Debian-1-log

--
-- Table structure for table `ast_config`
--

CREATE TABLE IF NOT EXISTS `ast_config` (
`id` int(11) NOT NULL auto_increment,
`cat_metric` int(11) NOT NULL default '0',
`var_metric` int(11) NOT NULL default '0',
`commented` int(11) NOT NULL default '0',
`filename` varchar(128) NOT NULL default '',
`category` varchar(128) NOT NULL default 'default',
`var_name` varchar(128) NOT NULL default '',
`var_val` varchar(128) NOT NULL default '',
 PRIMARY KEY (`id`),
 KEY `filename_comment` (`filename`,`commented`)
) TYPE=MyISAM;

DELETE FROM ast_config;
					
INSERT INTO `ast_config` (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES (0,0,0,'sip.conf','general','videosupport','yes');
INSERT INTO `ast_config` (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES (0,0,0,'sip.conf','general','allow','h263');
INSERT INTO `ast_config` (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES (0,0,0,'sip.conf','general','port','5060');
INSERT INTO `ast_config` (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES (0,0,0,'sip.conf','general','allow','ulaw');

INSERT INTO `ast_config` (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES (0,0,0,'sip.conf','general','maxexpirey','60');
INSERT INTO `ast_config` (cat_metric, var_metric, commented, filename, category, var_name, var_val) VALUES (0,0,0,'sip.conf','general','defaultexpirey','60');

--
-- Table structure for table `extensions_table`
--

CREATE TABLE IF NOT EXISTS `extensions_table` (
  `id` int(11) NOT NULL auto_increment,
  `context` varchar(100) default NULL,
  `exten` varchar(20) NOT NULL default '',
  `priority` tinyint(4) NOT NULL default '0',
  `app` varchar(20) NOT NULL default '',
  `appdata` varchar(255) NOT NULL default '',
  KEY (`id`)
) TYPE=MyISAM;

--
-- Dumping data for table `extensions_table`
--

DELETE FROM extensions_table;


INSERT INTO `extensions_table` VALUES (7,'trusted','_0.',1,'Goto','outgoing-local|${EXTEN}|1');
INSERT INTO `extensions_table` VALUES (2,'trusted','_9.',1,'Goto','outgoing-extern|${EXTEN}|1');

INSERT INTO `extensions_table` VALUES (12,'outgoing-extern','_.',1,'Goto','outgoing-extern-selectline|${EXTEN}|1');


#INSERT INTO `extensions_table` VALUES (10,'outgoing-extern-selectline','_.',1,'SetVar','DIALLINETYPE=SIP');
#INSERT INTO `extensions_table` VALUES (11,'outgoing-extern-selectline','_.',2,'SetVar','DIALLINE=broadvoice4');
INSERT INTO `extensions_table` VALUES (12,'outgoing-extern-selectline','_.',3,'SetVar','DIALNUM=${EXTEN:1}');
INSERT INTO `extensions_table` VALUES (13,'outgoing-extern-selectline','_.',4,'Goto','outgoing-place-call|s|1');

INSERT INTO `extensions_table` VALUES (17,'outgoing-local','_.',1,'SetVar','DIALNUM=${EXTEN:1}');
INSERT INTO `extensions_table` VALUES (18,'outgoing-local','_.',3,'Goto','outgoing-place-call|s|1');
INSERT INTO `extensions_table` VALUES (20,'outgoing-local','_.',2,'SetVar','DIALLINE=');

INSERT INTO `extensions_table` VALUES (22,'outgoing-place-call','_.',1,'GotoIf','$[\"${DIALLINE}\" != \"\"]?2:4');
INSERT INTO `extensions_table` VALUES (23,'outgoing-place-call','_.',2,'Dial','${DIALLINETYPE}/${DIALLINE}/${DIALNUM}');
INSERT INTO `extensions_table` VALUES (24,'outgoing-place-call','_.',3,'Hangup','');
INSERT INTO `extensions_table` VALUES (25,'outgoing-place-call','_.',4,'Dial','SIP/${DIALNUM}');
INSERT INTO `extensions_table` VALUES (26,'outgoing-place-call','_.',5,'Hangup','');

INSERT INTO `extensions_table` VALUES (27,'incoming-local','_.',1,'SetVar','');
INSERT INTO `extensions_table` VALUES (28,'incoming-local','_.',2,'Goto','incoming-place-call|s|1');
INSERT INTO `extensions_table` VALUES (29,'incoming-place-call','_.',1,'Dial','${INTERNALCHANNELS}');
INSERT INTO `extensions_table` VALUES (30,'incoming-place-call','_.',2,'Hangup','');
INSERT INTO `extensions_table` VALUES (31,'registered-lines','_.',1,'Goto','incoming-local|s|1');
INSERT INTO `extensions_table` VALUES (32,'default','_.',1,'Goto','incoming-local|s|1');

--
-- Table structure for table `sip_buddies`
--

CREATE TABLE IF NOT EXISTS `sip_buddies` (
  `uniqueid` int(11) NOT NULL auto_increment,
  `name` varchar(30) NOT NULL default '',
  `accountcode` varchar(30) default NULL,
  `amaflags` char(1) default NULL,
  `callgroup` varchar(30) default NULL,
  `callerid` varchar(50) default NULL,
  `canreinvite` char(1) default NULL,
  `context` varchar(30) default NULL,
  `defaultip` varchar(15) default NULL,
  `dtmfmode` varchar(7) default NULL,
  `fromuser` varchar(50) default NULL,
  `fromdomain` varchar(255) default NULL,
  `host` varchar(255) NOT NULL default '',
  `incominglimit` char(2) default NULL,
  `outgoinglimit` char(2) default NULL,
  `insecure` char(1) default NULL,
  `language` char(2) default NULL,
  `mailbox` varchar(50) default NULL,
  `md5secret` varchar(32) default NULL,
  `nat` varchar(5) default NULL,
  `permit` varchar(95) default NULL,
  `deny` varchar(95) default NULL,
  `pickupgroup` varchar(10) default NULL,
  `port` varchar(5) NOT NULL default '',
  `qualify` varchar(4) default NULL,
  `restrictcid` char(1) default NULL,
  `rtptimeout` char(3) default NULL,
  `rtpholdtimeout` char(3) default NULL,
  `secret` varchar(30) default NULL,
  `type` varchar(6) NOT NULL default '',
  `username` varchar(30) NOT NULL default '',
  `allow` varchar(100) default NULL,
  `disallow` varchar(100) default NULL,
  `regseconds` int(11) NOT NULL default '0',
  `ipaddr` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`uniqueid`),
  UNIQUE KEY `name` (`name`),
  KEY `name_2` (`name`)
) TYPE=MyISAM;

--
-- Dumping data for table `sip_buddies`
--


--
-- Table structure for table `iax_buddies`
--

CREATE TABLE IF NOT EXISTS `iax_buddies` (
  `uniqueid` int(11) NOT NULL auto_increment,
  `name` varchar(30) NOT NULL default '',
  `accountcode` varchar(30) default NULL,
  `amaflags` char(1) default NULL,
  `callgroup` varchar(30) default NULL,
  `callerid` varchar(50) default NULL,
  `canreinvite` char(1) default NULL,
  `context` varchar(30) default NULL,
  `defaultip` varchar(15) default NULL,
  `dtmfmode` varchar(7) default NULL,
  `fromuser` varchar(50) default NULL,
  `fromdomain` varchar(255) default NULL,
  `host` varchar(255) NOT NULL default '',
  `incominglimit` char(2) default NULL,
  `outgoinglimit` char(2) default NULL,
  `insecure` char(1) default NULL,
  `language` char(2) default NULL,
  `mailbox` varchar(50) default NULL,
  `md5secret` varchar(32) default NULL,
  `nat` varchar(5) default NULL,
  `permit` varchar(95) default NULL,
  `deny` varchar(95) default NULL,
  `pickupgroup` varchar(10) default NULL,
  `port` varchar(5) NOT NULL default '',
  `qualify` varchar(4) default NULL,
  `restrictcid` char(1) default NULL,
  `rtptimeout` char(3) default NULL,
  `rtpholdtimeout` char(3) default NULL,
  `secret` varchar(30) default NULL,
  `type` varchar(6) NOT NULL default '',
  `username` varchar(30) NOT NULL default '',
  `allow` varchar(100) default NULL,
  `disallow` varchar(100) default NULL,
  `regseconds` int(11) NOT NULL default '0',
  `ipaddr` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`uniqueid`),
  UNIQUE KEY `name` (`name`),
  KEY `name_2` (`name`)
) TYPE=MyISAM;

--
-- Dumping data for table `sip_buddies`
--


--
-- Table structure for table `voicemail_users`
--

CREATE TABLE IF NOT EXISTS `voicemail_users` (
  `uniqueid` int(11) NOT NULL auto_increment,
  `customer_id` int(11) NOT NULL default '0',
  `context` varchar(50) NOT NULL default '',
  `mailbox` int(5) NOT NULL default '0',
  `password` int(4) NOT NULL default '0',
  `fullname` varchar(50) NOT NULL default '',
  `email` varchar(50) NOT NULL default '',
  `pager` varchar(50) NOT NULL default '',
  `options` varchar(100) NOT NULL default '',
  `stamp` timestamp(14) NOT NULL,
  PRIMARY KEY  (`uniqueid`),
  KEY `mailbox_context` (`mailbox`,`context`)
) TYPE=MyISAM;

--
-- Dumping data for table `voicemail_users`
--


