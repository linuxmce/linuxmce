/*
    Copyright 2004-2005 Chris Tallon
    Copyright 2004-2005 University Of Bradford

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include "log.h"

#define MAX_FILENAME_LENGTH 500
#define BUFFER_LENGTH 1500

class Config
{
  public:
    Config();

    int init(char* fileName);
    int shutdown();
    int status();

    char* getValueString(const char* section, const char* key);
    long getValueLong(const char* section, const char* key, int* failure);
    long long getValueLongLong(char* section, char* key, int* failure);
    double getValueDouble(char* section, char* key, int* failure);

    int setValueString(const char* section, const char* key, const char* newValue);
    int setValueLong(const char* section, char* key, long newValue);
    int setValueLongLong(char* section, char* key, long long newValue);
    int setValueDouble(char* section, char* key, double newValue);

    int deleteValue(const char* section, char* key); // err.. delete "key".
    char* getSectionKeyNames(const char* section, int& numberOfReturns, int& length);

  private:
    pthread_mutex_t fileLock;
    int initted;
    int lastLineLength;
    Log* log;

    char fileName[MAX_FILENAME_LENGTH];
    char fileNameTemp[MAX_FILENAME_LENGTH];

    FILE* file;
    char buffer[BUFFER_LENGTH];

    int openFile();
    void closeFile();
    int readLine();
    int findSection(const char* section);
    int findKey(const char* key);
    void trim(char* sting);
    FILE* copyToHere(long position);
    int copyRest(FILE* newFile);
};

#endif
