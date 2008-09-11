CREATE DATABASE lmce_game;

USE lmce_game;

CREATE TABLE Attribute (
	PK_Attribute INTEGER AUTO_INCREMENT NOT NULL,
	FK_AttributeType INTEGER NOT NULL,
	FK_GameSystem INTEGER NOT NULL,
	Name longtext,
	PRIMARY KEY(PK_Attribute),
	KEY(FK_AttributeType)
);

CREATE TABLE AttributeType (
	PK_AttributeType INTEGER AUTO_INCREMENT NOT NULL,
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

CREATE TABLE File (
	PK_File INTEGER AUTO_INCREMENT NOT NULL,
	FK_GameSystem INTEGER NOT NULL,
	Filename VARCHAR(255),
	PRIMARY KEY(PK_File),
	KEY(FK_GameSystem)
);

CREATE TABLE File_Attribute (
	FK_File INTEGER NOT NULL,
	FK_Attribute INTEGER NOT NULL,
	PRIMARY KEY(FK_File,FK_Attribute),
	KEY(FK_File),
	KEY(FK_Attribute)
);

