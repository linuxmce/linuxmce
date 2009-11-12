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

#include "config.h"

Config::Config()
{
  initted = 0;
  lastLineLength = 0;
  log = Log::getInstance();
}

int Config::init(char* takeFileName)
{
  if (initted) return 1;

  pthread_mutex_init(&fileLock, NULL);

  if (strlen(takeFileName) > (MAX_FILENAME_LENGTH - 1))
  {
    log->log("Config", Log::DEBUG, "Config error: Config filename too long");
    return 0;
  }

  strcpy(fileName, takeFileName);
  strcpy(fileNameTemp, takeFileName);
  strcat(fileNameTemp, ".tmp");

  file = fopen(fileName, "r");
  if (!file)
  {
    file = fopen(fileName, "w");
    if (!file)
    {
      log->log("Config", Log::DEBUG, "Config error: Could not access config file");
      return 0;
    }
  }
  fclose(file);

  initted = 1;
  log->log("Config", Log::DEBUG, "Opened config file: %s", fileName);

  return 1;
}

int Config::status()
{
  return initted;
}

int Config::shutdown()
{
  if (!initted) return 1;

  pthread_mutex_lock(&fileLock);
  initted = 0;
  pthread_mutex_unlock(&fileLock);
  pthread_mutex_destroy(&fileLock);

  return 1;
}

int Config::openFile()
{
  if (!initted) return 0;
  if (pthread_mutex_lock(&fileLock))
  {
    log->log("Config", Log::DEBUG, "Config error: Could not get lock");
    return 0;
  }
  if (!initted)
  {
    log->log("Config", Log::DEBUG, "Config error: Initted 0 after lock");
    pthread_mutex_unlock(&fileLock);
    return 0;
  }

  file = fopen(fileName, "r");
  if (!file)
  {
    log->log("Config", Log::DEBUG, "Config error: Could not open config file");
    pthread_mutex_unlock(&fileLock);
    return 0;
  }
  return 1;
}

void Config::closeFile()
{
  if (!initted) return;

  fclose(file);
  file = NULL;
  pthread_mutex_unlock(&fileLock);
}

int Config::readLine()
{
  if (!initted || !file) { log->log("Config", Log::DEBUG, "1"); return 0; }
  if (!fgets(buffer, BUFFER_LENGTH-1, file)) { /*log->log("Config", Log::DEBUG, "2");*/ return 0; }
  lastLineLength = strlen(buffer);
//  log->log("Config", Log::DEBUG, "buffer before trim: '%s'", buffer);
  trim(buffer);
//  log->log("Config", Log::DEBUG, "buffer after trim: '%s'", buffer);
  return 1;
}

// START HERE

FILE* Config::copyToHere(long position)
{
  FILE* newFile = fopen(fileNameTemp, "w");

  if (!newFile) return NULL;

  long newPos = 0;
  rewind(file);

  while (newPos < position)
  {
    fgets(buffer, BUFFER_LENGTH-1, file);
    fputs(buffer, newFile);
    newPos += strlen(buffer);
  }
  return newFile;
}

int Config::copyRest(FILE* newFile)
{
  if (newFile)
  {
    while(fgets(buffer, BUFFER_LENGTH-1, file))
    {
      fputs(buffer, newFile);
    }

    fclose(newFile);
  }
  fclose(file);
  file = NULL;

  if (newFile) rename(fileNameTemp, fileName);

  pthread_mutex_unlock(&fileLock);
  return 1;
}

int Config::deleteValue(const char* section, char* key)
{
  if (!initted) return 0;
  if (!openFile()) return 0;

  if (!findSection(section))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Section %s not found", section);
    return 0;
  }
  if (!findKey(key))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Key %s not found", key);
    return 0;
  }

  FILE* newFile = copyToHere(ftell(file) - lastLineLength);
  fgets(buffer, BUFFER_LENGTH-1, file);

  return copyRest(newFile);
}

int Config::setValueLong(const char* section, char* key, long newValue)
{
  char longBuffer[50];
  sprintf(longBuffer, "%li", newValue);
  return setValueString(section, key, longBuffer);
}

int Config::setValueLongLong(char* section, char* key, long long newValue)
{
  char longBuffer[50];
  sprintf(longBuffer, "%lli", newValue);
  return setValueString(section, key, longBuffer);
}

int Config::setValueDouble(char* section, char* key, double newValue)
{
  char doubleBuffer[50];
  sprintf(doubleBuffer, "%f", newValue);
  return setValueString(section, key, doubleBuffer);
}

int Config::setValueString(const char* section, const char* key, const char* newValue)
{
  if (!initted) return 0;
  if (!openFile()) return 0;

  if (findSection(section))
  {
    if (findKey(key))
    {
      FILE* newFile = copyToHere(ftell(file) - lastLineLength);
      if (!newFile)
      {
        closeFile();
        log->log("Config", Log::DEBUG, "Config error: Could not write temp config file");
        return 0;
      }

      fgets(buffer, BUFFER_LENGTH-1, file);
      fprintf(newFile, "%s = %s\n", key, newValue);
      return copyRest(newFile);
    }
    else
    {
      rewind(file);
      findSection(section);
      FILE* newFile = copyToHere(ftell(file));
      if (!newFile)
      {
        closeFile();
        log->log("Config", Log::DEBUG, "Config error: Could not write temp config file");
        return 0;
      }

      fprintf(newFile, "%s = %s\n", key, newValue);
      return copyRest(newFile);
    }
  }
  else
  {
    // section not found
    fseek(file, 0, SEEK_END);
    FILE* newFile = copyToHere(ftell(file));
    if (!newFile)
    {
      closeFile();
      log->log("Config", Log::DEBUG, "Config error: Could not write temp config file");
      return 0;
    }

    fprintf(newFile, "[%s]\n%s = %s\n", section, key, newValue);
    return copyRest(newFile);
  }
}

char* Config::getSectionKeyNames(const char* section, int& numberOfReturns, int& allKeysSize)
{
  numberOfReturns = 0;
  allKeysSize = 0;
  char* allKeys = NULL;
  int allKeysIndex = 0;
  int keyLength;
  char* equalspos;

  if (!initted) return NULL;
  if (!openFile()) return NULL;
  if (!findSection(section)) return NULL;

  char foundKey[BUFFER_LENGTH];

  while(readLine())
  {
    // Is this line a section header? if so, exit
    if ((buffer[0] == '[') && (buffer[strlen(buffer)-1] == ']')) break;

    equalspos = strstr(buffer, "=");
    if (!equalspos) continue;  // if there is no = then it's not a key
    memcpy(foundKey, buffer, equalspos-buffer);
    foundKey[equalspos-buffer] = '\0';
    trim(foundKey);
    keyLength = strlen(foundKey);
    allKeysSize += keyLength + 1;
    allKeys = (char*)realloc(allKeys, allKeysSize);
    memcpy(&allKeys[allKeysIndex], foundKey, keyLength);
    allKeysIndex += keyLength;
    allKeys[allKeysIndex] = '\0';
    allKeysIndex++;
    numberOfReturns++;
  }

  closeFile();
  return allKeys;
}


// END HERE

int Config::findSection(const char* section)
{
  if (!initted || !file) return 0;
  if (strlen(section) > (BUFFER_LENGTH-2))
  {
    log->log("Config", Log::DEBUG, "Config error: Section given exceeds max length");
    return 0;
  }

  char toFind[BUFFER_LENGTH];
  toFind[0] = '[';
  toFind[1] = '\0';
  strcat(toFind, section);
  strcat(toFind, "]");

  while(readLine())
  {
//    log->log("Config", Log::DEBUG, "to find '%s' this line '%s'", toFind, buffer);
    if (!strcmp(toFind, buffer)) return 1;
  }
  return 0;
}

int Config::findKey(const char* key)
{
  if (!initted || !file) return 0;

  if (strlen(key) > (BUFFER_LENGTH-1))
  {
    log->log("Config", Log::DEBUG, "Config error: Key given exceeds max length");
    return 0;
  }

  char prepForTest[BUFFER_LENGTH];

  // do a rough search first, this could match substrings that we don't want
  while(readLine())
  {
    // Is this line a section header? if so, exit
    if ((buffer[0] == '[') && (buffer[strlen(buffer)-1] == ']')) return 0;
    if (strstr(buffer, key))
    {
      // rough search found match
      char* equalspos = strstr(buffer, "=");
      if (!equalspos) continue;
      memcpy(prepForTest, buffer, equalspos-buffer);
      prepForTest[equalspos-buffer] = '\0';
      trim(prepForTest);

      if (!strcmp(key, prepForTest))
      {
        // in buffer, set all up to equals to space, then trim!
        for(char* curPos = buffer; curPos <= equalspos; curPos++)
        {
          *curPos = ' ';
        }
        trim(buffer);
        return 1;
      }
    }
  }
  return 0;
}

char* Config::getValueString(const char* section, const char* key)
{
  if (!initted) return NULL;
  if (!openFile()) return NULL;

  if (!findSection(section))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Section %s not found", section);
    return 0;
  }
  if (!findKey(key))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Key %s not found", key);
    return 0;
  }

  char* returnString = new char[strlen(buffer)+1];
  strcpy(returnString, buffer);

  closeFile();

  return returnString;
}

long Config::getValueLong(const char* section, const char* key, int* failure)
{
  *failure = 1;
  if (!initted) return 0;
  if (!openFile()) return 0;

  if (!findSection(section))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Section %s not found", section);
    return 0;
  }
  if (!findKey(key))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Key %s not found", key);
    return 0;
  }
  *failure = 0;

  char* check;
  long retVal = strtol(buffer, &check, 10);
  if ((retVal == 0) && (check == buffer)) *failure = 1;
  closeFile();

  return retVal;
}

long long Config::getValueLongLong(char* section, char* key, int* failure)
{
  *failure = 1;
  if (!initted) return 0;
  if (!openFile()) return 0;

  if (!findSection(section))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Section %s not found", section);
    return 0;
  }
  if (!findKey(key))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Key %s not found", key);
    return 0;
  }
  *failure = 0;

  char* check;
  long long retVal = strtoll(buffer, &check, 10);
  if ((retVal == 0) && (check == buffer)) *failure = 1;
  closeFile();

  return retVal;
}

double Config::getValueDouble(char* section, char* key, int* failure)
{
  *failure = 1;
  if (!initted) return 0;
  if (!openFile()) return 0;

  if (!findSection(section))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Section %s not found", section);
    return 0;
  }
  if (!findKey(key))
  {
    closeFile();
    log->log("Config", Log::DEBUG, "Config error: Key %s not found", key);
    return 0;
  }

  *failure = 0;

  char* check;
  double retVal = strtod(buffer, &check);
  if ((retVal == 0) && (check == buffer)) *failure = 1;

  closeFile();

  return retVal;
}



void Config::trim(char* str)
{
  int pos, len, start, end;

  // Kill comments
  len = strlen(str);
  for(pos = 0; pos < len; pos++)
  {
    if ((str[pos] == '#') || (str[pos] == ';'))
    {
      // Mod. If #/; is at start of line ok. Else, if it is after a space, ok.

      if ((pos == 0) || (isspace(str[pos - 1])))
      {
        str[pos] = '\0';
        break;
      }

    }
  }

  len = strlen(str);
  end = len;
  if (!len) return;

  start = 0;
  while(isspace(str[start])) start++;
  while(isspace(str[end-1]))
  {
    end--;
    if (end == 0)
    {
      str[0] = '\0';
      return;
    }
  }
  for(pos = start; pos < end; pos++) str[pos - start] = str[pos];
  str[end - start] = '\0';
}
