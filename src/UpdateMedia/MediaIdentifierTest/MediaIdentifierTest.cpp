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
#include <iostream>
#include "UpdateMedia/MediaIdentifier.h"
#include "DCE/Logger.h"
using namespace std;

using namespace DCE;

int main(int argc, char* argv[]) 
{
	if(argc == 2)
		cout << "Media type: " << MediaIdentifier::GetFileMediaType(string(argv[1])) << endl;
	else
		cout << "Usage: MediaIdentifierTest <file>" << endl;
	
	return 0;
}
