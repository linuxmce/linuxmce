CREATE DATABASE lmce_game;

USE lmce_game;

CREATE TABLE RomAttribute (
	PK_RomAttribute INTEGER AUTO_INCREMENT NOT NULL,
	FK_RomAttributeType INTEGER NOT NULL,
	FK_GameSystem INTEGER NOT NULL,
	Name longtext,
	PRIMARY KEY(PK_Attribute),
	KEY(FK_AttributeType)
);

CREATE TABLE RomAttributeType (
	PK_RomAttributeType INTEGER AUTO_INCREMENT NOT NULL,
	Define VARCHAR(50),
	Description VARCHAR(50),
	PicPriority INTEGER,
	PRIMARY KEY(PK_AttributeType)
);

CREATE TABLE GameSystem (
	PK_GameSystem INTEGER AUTO_INCREMENT NOT NULL,
	Define VARCHAR(50),
	Description VARCHAR(50),
	PRIMARY KEY(PK_GameSystem)
);

CREATE TABLE Rom (
	PK_Rom INTEGER AUTO_INCREMENT NOT NULL,
	FK_GameSystem INTEGER NOT NULL,
	Filename VARCHAR(255),
	PRIMARY KEY(PK_File),
	KEY(FK_GameSystem)
);

CREATE TABLE Rom_RomAttribute (
	FK_Rom INTEGER NOT NULL,
	FK_RomAttribute INTEGER NOT NULL,
	PRIMARY KEY(FK_Rom,FK_RomAttribute),
	KEY(FK_File),
	KEY(FK_Attribute)
);

ALTER TABLE Rom ADD md5 VARCHAR(32) DEFAULT NULL AFTER Romname;

CREATE TABLE Configuration (PK_Configuration INTEGER AUTO_INCREMENT NOT NULL, Define VARCHAR(50), FK_GameSystem INTEGER NOT NULL, Description VARCHAR(1024), Configuration TEXT, psc_id INTEGER, psc_batch INTEGER, psc_user INTEGER, psc_frozen TINYINT(1) DEFAULT 0, psc_mod TIMESTAMP DEFAULT CURRENT_TIMESTAMP NULL, psc_restrict INTEGER, PRIMARY KEY(PK_Configuration), KEY(FK_GameSystem));

CREATE TABLE Rom_Configuration (FK_Rom INTEGER NOT NULL, FK_Configuration INTEGER NOT NULL, psc_id INTEGER, psc_batch INTEGER, psc_user INTEGER, psc_frozen TINYINT(1) DEFAULT 0, psc_mod TIMESTAMP DEFAULT CURRENT_TIMESTAMP NULL, psc_restrict INTEGER, KEY(FK_Rom), KEY(FK_Configuration));

ALTER TABLE Rom ADD FK_Configuration INTEGER AFTER md5;

