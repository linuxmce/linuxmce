DROP DATABASE IF EXISTS lmce_game;

CREATE DATABASE lmce_game;

USE lmce_game;

CREATE TABLE NameHash (
       PK_NameHash INTEGER AUTO_INCREMENT NOT NULL,
       NameHash CHAR(40) UNIQUE NOT NULL,
       Description VARCHAR(1024),
       PRIMARY KEY(PK_NameHash)
);

CREATE TABLE Rom (
       PK_Rom INTEGER AUTO_INCREMENT NOT NULL,
       IK_Hash CHAR(40) UNIQUE NOT NULL,
       PRIMARY KEY(PK_Rom),
       KEY(IK_Hash)
);

CREATE TABLE Game (
       PK_Game INTEGER AUTO_INCREMENT NOT NULL,
       Define VARCHAR(128),
       NameHash CHAR(40) UNIQUE NOT NULL,
       Subtitle VARCHAR(128),
       PRIMARY KEY (PK_Game)
);

CREATE TABLE Game_GameSystem (
       PK_Game_GameSystem INTEGER AUTO_INCREMENT NOT NULL,
       FK_Game INTEGER NOT NULL,
       FK_GameSystem INTEGER NOT NULL,
       FK_Manufacturer INTEGER NOT NULL DEFAULT '-1',
       FK_Genre INTEGER NOT NULL DEFAULT '-1',
       FK_Year INTEGER,
       PRIMARY KEY (PK_Game_GameSystem),
       KEY(FK_Game),
       KEY(FK_GameSystem)
);

CREATE TABLE Manufacturer (
       PK_Manufacturer INTEGER AUTO_INCREMENT NOT NULL,
       Description VARCHAR(256),
       PRIMARY KEY (PK_Manufacturer)
);

CREATE TABLE Manufacturer_NameHash (
       PK_Manufacturer_NameHash INTEGER AUTO_INCREMENT NOT NULL,
       FK_Manufacturer INTEGER NOT NULL,
       NameHash CHAR(40) UNIQUE NOT NULL,
       PRIMARY KEY(PK_Manufacturer_NameHash),
       KEY(FK_Manufacturer),
       KEY(NameHash)
);

CREATE TABLE Genre (
       PK_Genre INTEGER AUTO_INCREMENT NOT NULL,
       FK_Genre_Parent INTEGER,
       Description VARCHAR(256),
       PRIMARY KEY(PK_Genre)
);

CREATE TABLE Genre_NameHash (
       PK_Genre_NameHash INTEGER AUTO_INCREMENT NOT NULL,
       FK_Genre INTEGER NOT NULL,
       NameHash CHAR(40) UNIQUE NOT NULL,
       PRIMARY KEY(PK_Genre_NameHash),
       KEY(FK_Genre),
       KEY(NameHash)
);

CREATE TABLE GameSystem (
       PK_GameSystem INTEGER AUTO_INCREMENT NOT NULL,
       Define VARCHAR(64),
       Description VARCHAR(128),
       PRIMARY KEY (PK_GameSystem)
);

CREATE TABLE Game_GameSystem_Rom (
       PK_Game_Rom INTEGER AUTO_INCREMENT NOT NULL,
       FK_Game INTEGER NOT NULL,
       FK_GameSystem INTEGER NOT NULL,
       FK_Rom INTEGER NOT NULL,
       PRIMARY KEY(PK_Game_Rom),
       KEY(FK_Game),
       KEY(FK_GameSystem),
       KEY(FK_Rom)
);

CREATE TABLE Picture (
       PK_Picture INTEGER AUTO_INCREMENT NOT NULL,
       URL VARCHAR(4096),
       PRIMARY KEY(PK_Picture)
);

CREATE TABLE GameSystem_Picture (
       PK_GameSystem_Picture INTEGER AUTO_INCREMENT NOT NULL,
       FK_GameSystem INTEGER NOT NULL,
       FK_Picture INTEGER NOT NULL,
       PRIMARY KEY(PK_GameSystem_Picture),
       KEY(FK_GameSystem),
       KEY(FK_Picture)
);

CREATE TABLE Game_GameSystem_Picture (
       PK_Game_GameSystem_Picture INTEGER AUTO_INCREMENT NOT NULL,
       FK_Game INTEGER NOT NULL,
       FK_GameSystem INTEGER NOT NULL,
       FK_Picture INTEGER NOT NULL,
       PRIMARY KEY(PK_Game_GameSystem_Picture),
       KEY(FK_Game),
       KEY(FK_GameSystem),
       KEY(FK_Picture)
);

CREATE TABLE Configuration (
       PK_Configuration INTEGER AUTO_INCREMENT NOT NULL,
       Define VARCHAR(64),
       Comments TEXT,
       Configuration TEXT,
       PRIMARY KEY(PK_Configuration)
);

CREATE TABLE Game_GameSystem_Configuration (
       PK_Game_GameSystem_Configuration INTEGER AUTO_INCREMENT NOT NULL,
       FK_Game INTEGER NOT NULL,
       FK_GameSystem INTEGER NOT NULL,
       FK_Configuration INTEGER NOT NULL,
       PRIMARY KEY(PK_Game_GameSystem_Configuration),
       KEY(FK_Game),
       KEY(FK_GameSystem),
       KEY(FK_Configuration)
);

CREATE TABLE Game_GameSystem_Rom_Configuration (
       PK_Game_GameSystem_Rom_Configuration INTEGER AUTO_INCREMENT NOT NULL,
       FK_Game INTEGER NOT NULL,
       FK_GameSystem INTEGER NOT NULL,
       FK_Rom INTEGER NOT NULL,
       FK_Configuration INTEGER NOT NULL,
       PRIMARY KEY(PK_Game_GameSYstem_Rom_Configuration),
       KEY(FK_Game),
       KEY(FK_GameSystem),
       KEY(FK_Rom),
       KEY(FK_Configuration)
);
