/*
 * general_remote.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: general_remote.h,v 1.1 2006-06-03 10:04:27 phintuka Exp $
 *
 */

#ifndef __GENERAL_REMOTE_H
#define __GENERAL_REMOTE_H


//----------------------------- cGeneralRemote --------------------------------

#include <vdr/remote.h>

class cGeneralRemote : public cRemote {
 public:
  cGeneralRemote(const char *Name) : cRemote(Name) {};
  bool Put(const char *Code, bool Repeat=false, bool Release=false) 
    { return cRemote::Put(Code, Repeat, Release); };
};


#endif
