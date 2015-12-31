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
#ifndef WizardRemote_H_
#define WizardRemote_H_

// TODO: Write beautifully

#include <string>
using namespace std;

// Class that wraps around the Remote Control process
class RemoteProcess
{
	public:
		RemoteProcess();
		~RemoteProcess();
		void Start(string sCmd);

	private:
		string m_sCmd;
		int pid;
};

#endif
