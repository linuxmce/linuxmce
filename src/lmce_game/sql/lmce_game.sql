CREATE DATABASE lmce_game;

USE lmce_game;

CREATE TABLE RomAttribute (
	PK_RomAttribute INTEGER AUTO_INCREMENT NOT NULL,
	FK_RomAttributeType INTEGER NOT NULL,
	FK_GameSystem INTEGER NOT NULL,
	Name longtext,
	PRIMARY KEY(PK_RomAttribute),
	KEY(FK_RomAttributeType)
);

CREATE TABLE RomAttributeType (
	PK_RomAttributeType INTEGER AUTO_INCREMENT NOT NULL,
	Define VARCHAR(50),
	Description VARCHAR(50),
	PicPriority INTEGER,
	PRIMARY KEY(PK_RomAttributeType)
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
	Romname VARCHAR(255),
	PRIMARY KEY(PK_Rom),
	KEY(FK_GameSystem)
);

CREATE TABLE Rom_RomAttribute (
	FK_Rom INTEGER NOT NULL,
	FK_RomAttribute INTEGER NOT NULL,
	PRIMARY KEY(FK_Rom,FK_RomAttribute),
	KEY(FK_Rom),
	KEY(FK_RomAttribute)
);

ALTER TABLE Rom ADD md5 VARCHAR(32) DEFAULT NULL AFTER Romname;

CREATE TABLE Configuration (PK_Configuration INTEGER AUTO_INCREMENT NOT NULL, Define VARCHAR(50), FK_GameSystem INTEGER NOT NULL, Description VARCHAR(1024), Configuration TEXT, PRIMARY KEY(PK_Configuration), KEY(FK_GameSystem));

CREATE TABLE Rom_Configuration (PK_Rom_Configuration INTEGER AUTO_INCREMENT NOT NULL, FK_Rom INTEGER NOT NULL, FK_Configuration INTEGER NOT NULL, PRIMARY KEY(PK_Rom_Configuration), KEY(FK_Configuration));

ALTER TABLE Rom ADD FK_Configuration INTEGER AFTER md5;

