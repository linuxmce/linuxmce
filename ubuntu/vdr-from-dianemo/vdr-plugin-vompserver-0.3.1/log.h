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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>

class Log
{
  public:
    Log();
    ~Log();
    static Log* getInstance();

    int init(int defaultLevel, char* fileName);
    int shutdown();
    int log(const char *fromModule, int level, const char *message, ...);
    void upLogLevel();
    void downLogLevel();

    const static int CRAZY  = 0; // mad crazy things that should never happen
    const static int EMERG  = 1; // human assist required NOW
    const static int ALERT  = 2; // system unusable, but happy to sit there
    const static int CRIT   = 3; // still working, but maybe about to die
    const static int ERR    = 4; // that response is not even listed...
    const static int WARN   = 5; // this could be a bad thing. still running tho
    const static int NOTICE = 6; // significant good thing
    const static int INFO   = 7; // verbose good thing
    const static int DEBUG  = 8; // debug-level messages

  private:
    static Log* instance;
    int initted;
    int logLevel;
    int enabled;

    FILE *logfile;
};

#endif

/*

Documentation
-------------

This class is intended to be instatiated once by the core.
For a one off use:

Log::getInstance()->log("<module-name>", Log::<levelname>, "<message>");

Or, a pointer can be stored and used:

Log *myptr = Log::getInstance();

myptr->log("<module-name>", Log::<levelname>, "<message>");
myptr->log("<module-name>", Log::<levelname>, "<message>");

Level usages are above.

The message parameter in the log function can be used in the same way as printf, eg.

myptr->log("<module-name>", Log::<levelname>, "Success: %s %i", stringpointer, integer);

*/
