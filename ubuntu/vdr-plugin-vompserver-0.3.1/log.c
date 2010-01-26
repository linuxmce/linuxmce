/*
    Copyright 2004-2005 Chris Tallon
    Copyright 2003-2004 University Of Bradford

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

#include "log.h"

Log* Log::instance = NULL;

Log::Log()
{
  if (instance) return;
  instance = this;
  logfile = NULL;
  initted = 0;
  logLevel = 0;
}

Log::~Log()
{
  instance = NULL;
}

Log* Log::getInstance()
{
  return instance;
}

void Log::upLogLevel()
{
  if (!initted) return;

  if (logLevel == Log::DEBUG)
  {
    log("Log", logLevel, "Log level is at its highest already");
    return;
  }

  logLevel++;
  log("Log", logLevel, "Log level is now %i", logLevel);
}

void Log::downLogLevel()
{
  if (!initted) return;

  if (logLevel == Log::CRAZY)
  {
    log("Log", logLevel, "Log level is at its lowest already");
    return;
  }

  logLevel--;
  log("Log", logLevel, "Log level is now %i", logLevel);
}

int Log::init(int startLogLevel, char* fileName)
{
  logLevel = startLogLevel;

  logfile = fopen(fileName, "a");
  if (logfile)
  {
    initted = 1;
    return 1;
  }
  else
  {
    return 0;
  }
}

int Log::shutdown()
{
  if (!initted) return 1;
  initted = 0;
  if (logfile) fclose(logfile);
  return 1;
}

int Log::log(const char *fromModule, int level, const char* message, ...)
{
  if (!initted) return 0;

  if (level > logLevel) return 1;

  int lineLength = 250;

  char buffer[lineLength + 1];
  int spaceLeft = lineLength;

  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm* tm = localtime(&tv.tv_sec);
  spaceLeft -= strftime(buffer, spaceLeft, "%H:%M:%S.", tm);
  spaceLeft -= snprintf(&buffer[lineLength-spaceLeft], spaceLeft, "%06lu ", (unsigned long)tv.tv_usec);


  char levelString[10];
  if (level == CRAZY)   strcpy(levelString, "[CRAZY] ");
  if (level == EMERG)   strcpy(levelString, "[EMERG] ");
  if (level == ALERT)   strcpy(levelString, "[ALERT] ");
  if (level == CRIT)    strcpy(levelString, "[CRIT]  ");
  if (level == ERR)     strcpy(levelString, "[ERR]   ");
  if (level == WARN)    strcpy(levelString, "[WARN]  ");
  if (level == NOTICE)  strcpy(levelString, "[notice]");
  if (level == INFO)    strcpy(levelString, "[info]  ");
  if (level == DEBUG)   strcpy(levelString, "[debug] ");

  spaceLeft -= snprintf(&buffer[lineLength-spaceLeft], spaceLeft, "%s %s - ", levelString, fromModule);

  va_list ap;
  va_start(ap, message);
  spaceLeft = vsnprintf(&buffer[lineLength-spaceLeft], spaceLeft, message, ap);
  va_end(ap);

  int messageLength = strlen(buffer);
  if (messageLength < lineLength)
  {
    buffer[messageLength] = '\n';
    buffer[messageLength+1] = '\0';
  }
  else
  {
    buffer[lineLength-1] = '\n';
    buffer[lineLength] = '\0';
  }

  int success = fputs(buffer, logfile);
  fflush(NULL);

  if (success != EOF)
    return 1;
  else
    return 0;

}
