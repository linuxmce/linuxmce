/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "FileBrowser.h"

FileBrowser::FileBrowser(string SearchFolder)
{
	FirstFile = true;
}

FileBrowser::~FileBrowser(void)
{
}

string FileBrowser::NextFile()
{
	string Result;

	FirstFile = !FirstFile;
	if(FirstFile)
		Result = "/IMG_0001.jpg";
	else
		Result = "/IMG_0004.jpg";
	return Result;
}