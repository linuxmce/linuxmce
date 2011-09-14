CREATE TABLE `RemoteAssistance` (
  `PK_Port` int(10) unsigned NOT NULL auto_increment,
  `Port` int(10) unsigned NOT NULL,
  `EK_Installation` varchar(10) default NULL,
  `Description` varchar(100) NOT NULL,
  `DateAllocated` varchar(50) default NULL,
  `LastActive` datetime NOT NULL,
  PRIMARY KEY  (`PK_Port`)
) 
