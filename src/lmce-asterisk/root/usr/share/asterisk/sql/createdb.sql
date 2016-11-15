/*
 * Create LinuxMCE asterisk database
 * and realtime tables
 *
 * v0.4 - 27/09/2016 - phenigma - updates for mysql >v5.5 datetime/timestamp changes
 * v0.3 - 16/11/2015 - phenigma - updates for chan_sccp-v4.2
 * v0.2 - 15/04/2012 - foxi352 - removed asteriskcdrdb and created cdr table in asterisk db
 * v0.1 - 22/09/2011 - foxi352 - initial version
 * 
*/

--
-- Don't leave semi commmited stuff if ever script breaks
--
SET AUTOCOMMIT=0;

START TRANSACTION;

--
-- Create LinuxMCE asterisk databases
--
CREATE DATABASE IF NOT EXISTS asterisk;

--
-- Drop database we created in older installs. This drop can be removed on release
--
DROP DATABASE IF EXISTS asteriskcdrdb;

--
-- Set LinuxMCE user privileges to asterisk databases
-- Create LinuxMCE user @same time if not exists 
--
GRANT ALL PRIVILEGES ON asterisk.* TO asteriskuser@localhost;

--
-- Reset LinuxMCE db user password to default if maybe still set to freepbx default
--
USE mysql;
UPDATE user SET password=PASSWORD('lmce') WHERE User='asteriskuser';
FLUSH PRIVILEGES;

--
-- Switch to LinuxMCE's asterisk db
--
USE asterisk;

--
-- Table structure for main statis configuration
--
CREATE TABLE IF NOT EXISTS`ast_config` (
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
);
 
--
-- Table with line-configuration for SCCP devices
--
DROP TABLE IF EXISTS `sccpdevice`;
CREATE TABLE IF NOT EXISTS `sccpdevice` (
  `type` varchar(45) default NULL,
  `addon` varchar(45) default NULL,
  `description` varchar(45) default NULL,
  `tzoffset` varchar(5) default NULL,
  `transfer` varchar(5) default 'off',
  `cfwdall` varchar(5) default 'off',
  `cfwdbusy` varchar(5) default 'off',
  `dtmfmode` varchar(10) default 'inband',
  `imageversion` varchar(45) default NULL,
  `deny` varchar(45) default NULL,
  `permit` varchar(45) default NULL,
  `trustphoneip` varchar(5) default NULL,
  `directrtp` varchar(3) default 'off',
  `earlyrtp` varchar(8) default 'none',
  `mwilamp` varchar(5) default 'on',
  `mwioncall` varchar(5) default 'off',
  `pickupexten` varchar(5) default 'on',
  `pickupcontext` varchar(100) default '',
  `pickupmodeanswer` varchar(5) default 'on',
  `private` varchar(5) default 'off',
  `privacy` varchar(100) default 'full',
  `nat` varchar(4) default 'auto',
  `softkeyset` varchar(100) default '',
  `setvar` varchar(100) default NULL,
  `disallow` varchar(255) DEFAULT NULL,
  `allow` varchar(255) DEFAULT NULL,
  `name` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`name`)
);

--
-- Table with device-configuration for SCCP devices
--
DROP TABLE IF EXISTS `sccpline`;
CREATE TABLE IF NOT EXISTS `sccpline` (
  `id` varchar(45) default NULL,
  `pin` varchar(45) default NULL,
  `label` varchar(45) default NULL,
  `description` varchar(45) default NULL,
  `context` varchar(45) default NULL,
  `incominglimit` varchar(45) default '2',
  `transfer` varchar(45) default 'off',
  `mailbox` varchar(45) default NULL,
  `vmnum` varchar(45) default '600',
  `cid_name` varchar(45) default NULL,
  `cid_num` varchar(45) default NULL,
  `trnsfvm` varchar(45) default '1000',
  `secondary_dialtone_digits` varchar(45) default '9',
  `secondary_dialtone_tone` varchar(45) default '0x22',
  `musicclass` varchar(45) default 'default',
  `language` varchar(45) default 'en',
  `accountcode` varchar(45) default NULL,
  `echocancel` varchar(45) default NULL,
  `silencesuppression` varchar(45) default 'off',
  `callgroup` varchar(45) default NULL,
  `pickupgroup` varchar(45) default NULL,
  `amaflags` varchar(45) default NULL,
  `dnd` varchar(7) default 'reject',
  `setvar` varchar(50) default NULL,
  `name` varchar(45) NOT NULL default '',
  PRIMARY KEY  (`name`)
);

--
-- Table with button-configuration for SCCP devices
--
DROP TABLE IF EXISTS `buttonconfig`;
CREATE TABLE IF NOT EXISTS `buttonconfig` (
  `device` varchar(15) NOT NULL default '',
  `instance` tinyint(4) NOT NULL default '0',
  `type` enum('line','speeddial','service','feature','empty') NOT NULL default 'line',
  `name` varchar(36) default NULL,
  `options` varchar(100) default NULL,
  PRIMARY KEY  (`device`,`instance`),
  KEY `device` (`device`)
);

--
-- View for merging device and button configuration
--
CREATE OR REPLACE ALGORITHM = MERGE
VIEW sccpdeviceconfig AS
	SELECT GROUP_CONCAT( CONCAT_WS( ',', buttonconfig.type, buttonconfig.name, buttonconfig.options )
	ORDER BY instance ASC
	SEPARATOR ';' ) AS button, sccpdevice.*
	FROM sccpdevice
	LEFT JOIN buttonconfig ON ( buttonconfig.device = sccpdevice.name )
GROUP BY sccpdevice.name;

--
-- Table with device-configuration for SIP devices
--
DROP TABLE IF EXISTS `sip_devices`;
CREATE TABLE IF NOT EXISTS `sip_devices` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(80) NOT NULL DEFAULT '',
  `context` varchar(80) DEFAULT NULL,
  `callingpres` enum('allowed_not_screened','allowed_passed_screen','allowed_failed_screen','allowed','prohib_not_screened','prohib_passed_screen','prohib_failed_screen','prohib','unavailable') DEFAULT 'allowed_not_screened',
  `deny` varchar(95) DEFAULT NULL,
  `permit` varchar(95) DEFAULT NULL,
  `secret` varchar(80) DEFAULT NULL,
  `md5secret` varchar(80) DEFAULT NULL,
  `remotesecret` varchar(250) DEFAULT NULL,
  `transport` enum('tcp','udp','udp,tcp') DEFAULT 'udp,tcp',
  `host` varchar(31) NOT NULL DEFAULT 'dynamic',
  `nat` varchar(19) NOT NULL DEFAULT 'force_rport,comedia',
  `type` enum('user','peer','friend') NOT NULL DEFAULT 'friend',
  `accountcode` varchar(20) DEFAULT NULL,
  `amaflags` varchar(13) DEFAULT NULL,
  `callgroup` varchar(10) DEFAULT NULL,
  `callerid` varchar(80) DEFAULT NULL,
  `defaultip` varchar(15) DEFAULT NULL,
  `dtmfmode` varchar(7) DEFAULT 'auto',
  `fromuser` varchar(80) DEFAULT NULL,
  `fromdomain` varchar(80) DEFAULT NULL,
  `insecure` varchar(20) DEFAULT NULL,
  `language` char(2) DEFAULT NULL,
  `mailbox` varchar(50) DEFAULT NULL,
  `pickupgroup` varchar(10) DEFAULT NULL,
  `qualify` char(5) DEFAULT 'no',
  `regexten` varchar(80) DEFAULT NULL,
  `rtptimeout` char(3) DEFAULT NULL,
  `rtpholdtimeout` char(3) DEFAULT NULL,
  `setvar` varchar(100) DEFAULT NULL,
  `disallow` varchar(100) DEFAULT 'all',
  `allow` varchar(100) DEFAULT 'alaw;ulaw',
  `fullcontact` varchar(80) NOT NULL DEFAULT '',
  `ipaddr` varchar(15) NOT NULL DEFAULT '',
  `port` mediumint(5) unsigned NOT NULL DEFAULT '0',
  `defaultuser` varchar(80) NOT NULL DEFAULT '',
  `subscribecontext` varchar(80) DEFAULT NULL,
  `directmedia` enum('yes','no') DEFAULT 'no',
  `trustrpid` enum('yes','no') DEFAULT NULL,
  `sendrpid` enum('yes','no') DEFAULT NULL,
  `progressinband` enum('never','yes','no') DEFAULT NULL,
  `promiscredir` enum('yes','no') DEFAULT NULL,
  `useclientcode` enum('yes','no') DEFAULT NULL,
  `callcounter` enum('yes','no') DEFAULT NULL,
  `busylevel` int(10) unsigned DEFAULT NULL,
  `allowoverlap` enum('yes','no') DEFAULT 'yes',
  `allowsubscribe` enum('yes','no') DEFAULT 'yes',
  `allowtransfer` enum('yes','no') DEFAULT 'yes',
  `ignoresdpversion` enum('yes','no') DEFAULT 'no',
  `template` varchar(100) DEFAULT NULL,
  `videosupport` enum('yes','no','always') DEFAULT 'no',
  `maxcallbitrate` int(10) unsigned DEFAULT NULL,
  `rfc2833compensate` enum('yes','no') DEFAULT 'yes',
  `session-timers` enum('originate','accept','refuse') DEFAULT 'accept',
  `session-expires` int(5) unsigned DEFAULT '1800',
  `session-minse` int(5) unsigned DEFAULT '90',
  `session-refresher` enum('uac','uas') DEFAULT 'uas',
  `t38pt_usertpsource` enum('yes','no') DEFAULT NULL,
  `outboundproxy` varchar(250) DEFAULT NULL,
  `callbackextension` varchar(250) DEFAULT NULL,
  `registertrying` enum('yes','no') DEFAULT 'yes',
  `timert1` int(5) unsigned DEFAULT '500',
  `timerb` int(8) unsigned DEFAULT NULL,
  `qualifyfreq` int(5) unsigned DEFAULT '120',
  `contactpermit` varchar(250) DEFAULT NULL,
  `contactdeny` varchar(250) DEFAULT NULL,
  `lastms` int(11) NOT NULL,
  `regserver` varchar(100) NOT NULL DEFAULT '',
  `regseconds` int(11) NOT NULL DEFAULT '0',
  `useragent` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`,`fromuser`)
);

--
-- Table with device-configuration for IAX devices
--
DROP TABLE IF EXISTS `iax_devices`;
CREATE TABLE IF NOT EXISTS iax_devices (
	id int(11) NOT NULL AUTO_INCREMENT,
	name varchar(30) NOT NULL,
	username varchar(30),
	type enum('user','peer','friend') NOT NULL DEFAULT 'friend',
	secret varchar(50),
	md5secret varchar(32),
	dbsecret varchar(100),
	notransfer enum('yes','no') DEFAULT 'yes',
	requirecalltoken enum('yes','no') DEFAULT 'yes',
	inkeys varchar(100),
	outkey varchar(100),
	setvar varchar(100),
	auth varchar(100),
	accountcode varchar(100),
	amaflags varchar(100),
	callerid varchar(100),
	context varchar(100) DEFAULT 'from-internal',
	defaultip varchar(15),
	host varchar(31) NOT NULL default 'dynamic',
	language char(5) DEFAULT 'en',
	mailbox varchar(50),
	deny varchar(95),
	permit varchar(95),
	qualify enum('yes','no') DEFAULT 'yes',
	disallow varchar(100),
	allow varchar(100),
	ipaddr varchar(15),
	port integer default 4569,
	regseconds integer default 0,
	PRIMARY KEY (id)
);

--
-- Table for dialplan / extensions
--
CREATE TABLE IF NOT EXISTS `extensions` (
`id` int(11) NOT NULL auto_increment,
`context` varchar(30) NOT NULL default '',
`exten` varchar(50) NOT NULL default '',
`priority` tinyint(4) NOT NULL default '0',
`app` varchar(20) NOT NULL default '',
`appdata` varchar(150) NOT NULL default '',
PRIMARY KEY (`context`,`exten`,`priority`),
KEY `id` (`id`)
);

--
-- Table for vociemail entries
--
CREATE TABLE IF NOT EXISTS `voicemail_users` (
 `uniqueid` INT(4) NOT NULL AUTO_INCREMENT,
 `customer_id` VARCHAR(10) DEFAULT NULL,
 `context` VARCHAR(10) NOT NULL,
 `mailbox` VARCHAR(10) NOT NULL,
 `password` INT(4) NOT NULL,
 `fullname` VARCHAR(150) DEFAULT NULL,
 `email` VARCHAR(50) DEFAULT NULL,
 `pager` VARCHAR(50) DEFAULT NULL,
 `tz` VARCHAR(10) DEFAULT 'central',
 `attach` ENUM('yes','no') NOT NULL DEFAULT 'yes',
 `saycid` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `dialout` VARCHAR(10) DEFAULT NULL,
 `callback` VARCHAR(10) DEFAULT NULL,
 `review` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `operator` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `envelope` ENUM('yes','no') NOT NULL DEFAULT 'yes',
 `sayduration` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `saydurationm` TINYINT(4) NOT NULL DEFAULT '1',
 `sendvoicemail` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `delete` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `nextaftercmd` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `forcename` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `forcegreetings` ENUM('yes','no') NOT NULL DEFAULT 'no',
 `hidefromdir` ENUM('yes','no') NOT NULL DEFAULT 'yes',
 `stamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
 `attachfmt` VARCHAR(10) DEFAULT NULL,
 `searchcontexts` ENUM('yes','no') DEFAULT NULL,
 `cidinternalcontexts` VARCHAR(10) DEFAULT NULL,
 `exitcontext` VARCHAR(10) DEFAULT NULL,
 `volgain` VARCHAR(4) DEFAULT NULL,
 `tempgreetwarn` ENUM('yes','no') DEFAULT 'yes',
 `messagewrap` ENUM('yes','no') DEFAULT 'no',
 `minpassword` INT(2) DEFAULT '4',
 `vm-password` VARCHAR(10) DEFAULT NULL,
 `vm-newpassword` VARCHAR(10) DEFAULT NULL,
 `vm-passchanged` VARCHAR(10) DEFAULT NULL,
 `vm-reenterpassword` VARCHAR(10) DEFAULT NULL,
 `vm-mismatch` VARCHAR(10) DEFAULT NULL,
 `vm-invalid-password` VARCHAR(10) DEFAULT NULL,
 `vm-pls-try-again` VARCHAR(10) DEFAULT NULL,
 `listen-control-forward-key` VARCHAR(2) DEFAULT NULL,
 `listen-control-reverse-key` VARCHAR(1) DEFAULT NULL,
 `listen-control-pause-key` VARCHAR(1) DEFAULT NULL,
 `listen-control-restart-key` VARCHAR(1) DEFAULT NULL,
 `listen-control-stop-key` VARCHAR(13) DEFAULT NULL,
 `backupdeleted` VARCHAR(3) DEFAULT '25',
  PRIMARY KEY  (`uniqueid`),
 KEY `mailbox_context` (`mailbox`,`context`)
); 

--
-- Table for phoneline providers
--
CREATE TABLE IF NOT EXISTS `phonelines` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `enabled` enum('yes','no') NOT NULL DEFAULT 'yes',
  `isfax` enum('yes','no') NOT NULL DEFAULT 'no',
  `prefix` varchar(5) NOT NULL,
  `protocol` varchar(5) NOT NULL DEFAULT 'SIP',
  `dtmfmode` VARCHAR(10) NOT NULL DEFAULT 'auto',
  `name` varchar(50) NOT NULL DEFAULT '',
  `host` varchar(50) NOT NULL DEFAULT 'default',
  `port` varchar(5) NOT NULL DEFAULT '5060',
  `phonenumber` varchar(50) NOT NULL,
  `username` varchar(50) NOT NULL DEFAULT '',
  `password` varchar(50) NOT NULL DEFAULT '',
  `faxmail` varchar(5) DEFAULT NULL,
  `channels` int(11) NOT NULL DEFAULT 5,
  `faxheader` varchar(30) DEFAULT 'LinuxMCE fax',
  PRIMARY KEY (`id`)
);

--
-- Table for fax_list
--
CREATE TABLE IF NOT EXISTS `fax_list` (
  `PK_Fax` smallint(6) unsigned NOT NULL AUTO_INCREMENT,
  `DateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Incoming` tinyint(1) NOT NULL,
  `remoteCID` varchar(30) DEFAULT NULL,
  `FileName` varchar(255) NOT NULL,
  `Result` varchar(255) DEFAULT 'N/A',
  PRIMARY KEY (`PK_Fax`)
);

--
-- Table for calls details records (cdr)
--
CREATE TABLE IF NOT EXISTS `cdr` (
  `calldate` datetime DEFAULT NULL,
  `clid` varchar(80) NOT NULL DEFAULT '',
  `src` varchar(80) NOT NULL DEFAULT '',
  `dst` varchar(80) NOT NULL DEFAULT '',
  `dcontext` varchar(80) NOT NULL DEFAULT '',
  `channel` varchar(80) NOT NULL DEFAULT '',
  `dstchannel` varchar(80) NOT NULL DEFAULT '',
  `lastapp` varchar(80) NOT NULL DEFAULT '',
  `lastdata` varchar(80) NOT NULL DEFAULT '',
  `duration` int(11) NOT NULL DEFAULT '0',
  `billsec` int(11) NOT NULL DEFAULT '0',
  `disposition` varchar(45) NOT NULL DEFAULT '',
  `amaflags` int(11) NOT NULL DEFAULT '0',
  `accountcode` varchar(20) NOT NULL DEFAULT '',
  `userfield` varchar(255) NOT NULL DEFAULT '',
  `uniqueid` varchar(32) NOT NULL DEFAULT '',
  KEY `calldate` (`calldate`),
  KEY `dst` (`dst`),
  KEY `accountcode` (`accountcode`)
);

--
-- Create view for LMCE phones in asterisk DB
--
CREATE OR REPLACE ALGORITHM = MERGE
VIEW view_lmce_phones AS
Select PK_Device
,(select IK_DeviceData from pluto_main.Device_DeviceData WHERE FK_Device=PK_Device AND FK_DeviceData=31) AS ext
,(select IK_DeviceData from pluto_main.Device_DeviceData WHERE FK_Device=PK_Device AND FK_DeviceData=29) AS type
,(select IK_DeviceData from pluto_main.Device_DeviceData WHERE FK_Device=PK_Device AND FK_DeviceData=260) AS ip
,(select IK_DeviceData from pluto_main.Device_DeviceData WHERE FK_Device=PK_Device AND FK_DeviceData=128) AS secret
,(select IK_DeviceData from pluto_main.Device_DeviceData WHERE FK_Device=PK_Device AND FK_DeviceData=305) AS videosupport
,FK_DeviceTemplate
FROM pluto_main.Device Join pluto_main.DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
WHERE FK_DeviceCategory IN (90,91)
ORDER BY PK_Device;

--
-- Everything went fine until here, so let's commit to db
--
COMMIT;
