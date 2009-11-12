/*
    Copyright 2004-2005 Chris Tallon, Andreas Vogel

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "medialauncher.h"
#include "media.h"
#include <iostream>
#include "log.h"
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#define MAXCMD 50

MediaLauncher::MediaLauncher(Config *c) {
  cfg=c;
  numcommands=0;
  commands=NULL;
  pnum=-1;
  child=-1;
}

MediaLauncher::~MediaLauncher(){
  if (commands) {
    for (int i=0;i<numcommands;i++)
      delete commands[i];
    delete commands;
  }
  };

MediaLauncher::MCommand::MCommand(const char *n,ULONG t,const char *ext) {
  command=new char[strlen(n)+1];
  strcpy(command,n);
  mediaType=t;
  extension=new char[strlen(ext)+1];
  strcpy(extension,ext);
}
MediaLauncher::MCommand::~MCommand() {
  delete command;
  delete extension;
}

#define NUMTYPES 4

static struct {
  const char* mtypename;
  ULONG mtypeid;
} mediatypes[]= {
  {"PICTURE",MEDIA_TYPE_PICTURE},
  {"AUDIO",MEDIA_TYPE_AUDIO},
  {"VIDEO",MEDIA_TYPE_VIDEO},
  {"LIST",MEDIA_TYPE_DIR}
};

static ULONG typeIdFromName(const char *name) {
  for(int i=0;i<NUMTYPES;i++){
    if (strcasecmp(mediatypes[i].mtypename,name) == 0) return mediatypes[i].mtypeid;
  }
  return MEDIA_TYPE_UNKNOWN;
}

int MediaLauncher::init() {
  Log::getInstance()->log("MediaLauncher",Log::DEBUG,"init");

  commands=new Pcmd[MAXCMD];
  char buf[100];
  for(int i=1;i<=MAXCMD;i++){
    sprintf(buf,"Command.Name.%d",i);
    const char *cmname=cfg->getValueString("Media",buf);
    if (!cmname) continue;
    sprintf(buf,"Command.Extension.%d",i);
    const char *cmext=cfg->getValueString("Media",buf);
    if (!cmext) continue;
    sprintf(buf,"Command.Type.%d",i);
    const char *cmtype=cfg->getValueString("Media",buf);
    if (! cmtype) continue;
    ULONG cmtypeid=typeIdFromName(cmtype);
    if (cmtypeid == MEDIA_TYPE_UNKNOWN) {
      Log::getInstance()->log("MediaLauncher",Log::ERR,"unknown media type %s",cmtype);
      continue;
    }
    commands[numcommands]=new MCommand(cmname,cmtypeid,cmext);
    Log::getInstance()->log("MediaLauncher",Log::DEBUG,"found command %s for ext %s, type %s",cmname,cmext,cmtype);
    //check the command
    char cbuf[strlen(cmname)+40];
    sprintf(cbuf,"%s check",cmname);
    int rt=system(cbuf);
    if (rt != 0) {
      Log::getInstance()->log("MediaLauncher",Log::ERR,"testting command %s failed, ignore",cmname);
      continue;
    }
    numcommands++;
  }

  Log::getInstance()->log("MediaLauncher",Log::DEBUG,"found %d commands",numcommands);
  return 0;
}

int MediaLauncher::init(MediaLauncher *cp) {
  commands=new Pcmd[MAXCMD];
  for (int i=0;i<cp->numcommands;i++) {
    commands[i]=new MCommand(cp->commands[i]->command,cp->commands[i]->mediaType,cp->commands[i]->extension);
  }
  numcommands=cp->numcommands;
  return 0;
}

  

int MediaLauncher::findCommand(const char *name){
  const char *ext=name+strlen(name);
  while (*ext != '.' && ext > name) ext--;
  if (*ext == '.') ext++;
  //Log::getInstance()->log("MediaLauncher",Log::DEBUG,"found extension %s for name %s",ext,name);
  for (int i=0;i<numcommands;i++) {
    if (strcasecmp(ext,commands[i]->extension) == 0) {
      Log::getInstance()->log("MediaLauncher",Log::DEBUG,"found command %s to handle name %s",commands[i]->command,name);
      return i;
    }
  }
  return -1;
}

ULONG MediaLauncher::getTypeForName(const char *name) {
  int rt=findCommand(name);
  Log::getInstance()->log("MediaLauncher",Log::DEBUG,"getTypeForName %s entry %d",name,rt);
  if (rt>=0) return commands[rt]->mediaType;
  return MEDIA_TYPE_UNKNOWN;
}

int MediaLauncher::openStream(const char *fname,ULONG xsize,ULONG ysize,const char * command) {
  if (command == NULL) command="play";
  Log::getInstance()->log("MediaLauncher",Log::DEBUG,"open stream for %s command %s",fname,command);
  int cmnum=findCommand(fname);
  if (cmnum < 0) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"unable to find command for %s",fname);
    return -1;
  }
  if (pnum >= 0) closeStream();
  int pfd[2];
  if (pipe(pfd) == -1) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"unable to create pipe");
    return -1;
  }
  pnum=pfd[0];
  child=fork();
  if (child == -1) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"unable to fork");
    return -1;
  }
  if (child == 0) {
    //we are the child
    close(pfd[0]);
    dup2(pfd[1],fileno(stdout));
    close(fileno(stdin));
    dup2(pfd[1],fileno(stderr));
    //try to close all open FDs
    for (int i=0;i<=sysconf(_SC_OPEN_MAX);i++) {
       if (i != fileno(stderr) && i != fileno(stdout)) close(i);
    }
    char buf1[30];
    char buf2[30];
    sprintf(buf1,"%u",xsize);
    sprintf(buf2,"%u",ysize);
    execlp(commands[cmnum]->command,commands[cmnum]->command,command,fname,buf1,buf2,NULL);
    //no chance for logging here after close... Log::getInstance()->log("MediaLauncher",Log::ERR,"unable to execlp %s",commands[cmnum]->command);
    exit(-1);
  }
  if (fcntl(pnum,F_SETFL,fcntl(pnum,F_GETFL)|O_NONBLOCK) != 0) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"unable to set to nonblocking");
    closeStream();
    return -1;
  }
  //the parent
  return child;
}


int MediaLauncher::closeStream() {
  if (child <= 0) return -1;
  Log::getInstance()->log("MediaLauncher",Log::DEBUG,"close stream for child %d",child);
  close(pnum);
  if (kill(child,0) == 0) {
      Log::getInstance()->log("MediaLauncher",Log::DEBUG,"trying to kill child %d",child);
      kill(child,SIGINT);
  }
  waitpid(child,NULL,WNOHANG);
  for (int i=0;i< 150;i++) {
    if (kill(child,0) == 0) {
      usleep(10000);
      waitpid(child,NULL,WNOHANG);
    }
  }
  if (kill(child,0) == 0) {
    Log::getInstance()->log("MediaLauncher",Log::DEBUG,"child %d aktive after wait, kill -9",child);
    kill(child,SIGKILL);
  }
  for (int i=0;i< 100;i++) {
    if (kill(child,0) == 0) {
      usleep(10000);
    }
  }
  waitpid(child,NULL,WNOHANG);
  child=-1;
  pnum=-1;
  return 0;
}

int MediaLauncher::getNextBlock(ULONG size,unsigned char **buffer,ULONG *readLen) {
  Log::getInstance()->log("MediaLauncher",Log::DEBUG,"get Block buf %p, len %lu",*buffer,size);
  if (pnum <= 0) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"stream not open in getnextBlock");
    return -1;
  }
  *readLen=0;
  struct timeval to;
  to.tv_sec=0;
  to.tv_usec=100000; //100ms
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(pnum,&readfds);
  int rt=select(pnum+1,&readfds,NULL,NULL,&to);
  if (rt < 0) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"error in select");
    return -1;
  }
  if (rt == 0) {
    Log::getInstance()->log("MediaLauncher",Log::DEBUG,"read 0 bytes (no data within 100ms)");
    waitpid(child,NULL,WNOHANG);
    if (kill(child,0) != 0) {
      Log::getInstance()->log("MediaLauncher",Log::DEBUG,"child is dead, returning EOF");
      return 1;
    }
    return 0;
  }
  if (! FD_ISSET(pnum,&readfds)) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"error in select - nothing read");
    return -1;
  }
  if (! *buffer) {
    *buffer=(UCHAR *)malloc(size);
  }
  if (! *buffer) {
    Log::getInstance()->log("MediaLauncher",Log::ERR,"unable to allocate buffer");
    return -1;
  }
  ssize_t rdsz=read(pnum,*buffer,size);
  *readLen=(ULONG)rdsz;
  Log::getInstance()->log("MediaLauncher",Log::DEBUG,"read %lu bytes",*readLen);
  if (rdsz == 0) return 1; //EOF
  return 0;
}

bool MediaLauncher::isOpen() {
  return pnum > 0;
}








    

