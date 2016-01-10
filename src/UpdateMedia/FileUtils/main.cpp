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
#include "file_utils.h"
#include "../../PlutoUtils/FileUtils.h"
#include "../../DCE/Logger.h"

#include <iostream>
using namespace std;
using namespace UpdateMediaFileUtils;

namespace DCE
{
	Logger *
}

using namespace DCE;

int main(int argc, char *argv[])
{

	if(argc != 2)
	{
		cout << "Usage: read_symlink filename" << endl;
		return 1;
	}

	string sFilePath(argv[1]);
	if(IsSymlink(sFilePath))
		cout << "symlink points to: " << SymlinkSource(sFilePath) << endl;
	else
		cout << "not a symlink" << endl;
	

	cout << "fs device : " << MountedDeviceForFile(sFilePath) << endl;
	
	map<int, int> mapMountedDevices;
	list<string> listFiles;
	FileUtils::FindDirectories(listFiles, "/mnt/device", false, true);

	MapMountedDevicesToPlutoDevices(listFiles, mapMountedDevices);

	cout << "pluto device: " << PlutoDeviceForFile(sFilePath, mapMountedDevices) << endl;

	return 0;
}
