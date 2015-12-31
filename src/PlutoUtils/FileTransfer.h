/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file FileTransfer.h
 Header file for  ???
 */
#ifndef FILETRANSFER_H
#define FILETRANSFER_H

struct FileInfo
{
	int Length;
	int M, D, Y, h, m, s;
};

typedef enum { FU_UPDATED, FU_SKIPPED, FU_NOFILE, FU_NOSERVER } FileUpdateResponses;

#endif

