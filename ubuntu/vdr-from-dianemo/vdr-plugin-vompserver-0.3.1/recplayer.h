/*
    Copyright 2004-2005 Chris Tallon

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

#ifndef RECPLAYER_H
#define RECPLAYER_H

#include <stdio.h>
#include <vdr/recording.h>

#include "defines.h"
#include "log.h"

class Segment
{
  public:
    ULLONG start;
    ULLONG end;
};

class RecPlayer
{
  public:
    RecPlayer(cRecording* rec);
    ~RecPlayer();
    ULLONG getLengthBytes();
    ULONG getLengthFrames();
    unsigned long getBlock(unsigned char* buffer, ULLONG position, unsigned long amount);
    int openFile(int index);
    ULLONG getLastPosition();
    cRecording* getCurrentRecording();
    void scan();
    ULLONG positionFromFrameNumber(ULONG frameNumber);
    ULONG frameNumberFromPosition(ULLONG position);
    bool getNextIFrame(ULONG frameNumber, ULONG direction, ULLONG* rfilePosition, ULONG* rframeNumber, ULONG* rframeLength);

  private:
    Log* log;
    cRecording* recording;
    cIndexFile* indexFile;
    FILE* file;
    int fileOpen;
    Segment* segments[1000];
    ULLONG totalLength;
    ULLONG lastPosition;
    ULONG totalFrames;
};

#endif
