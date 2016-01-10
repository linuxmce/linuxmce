CREATE DATABASE /*!32312 IF NOT EXISTS*/ LIRC_Remote_Controls;
USE LIRC_Remote_Controls;

DROP TABLE IF EXISTS LIRC_Remote_Controls;

CREATE TABLE LIRC_Remote_Controls (
  D int(11) NOT NULL auto_increment,
  Manufacturer varchar(30) NOT NULL default '',
  Remote varchar(30) NOT NULL default '',
  Configuration longtext NOT NULL,
  EK_DeviceTemplate int(11) default NULL,
  PRIMARY KEY  (        D)
) TYPE=MyISAM;
