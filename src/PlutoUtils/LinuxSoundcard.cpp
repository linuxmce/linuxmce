/** 
 * LinuxSoundcard
 *
 * A small utility to translate PCI ID to a sound card number for ALSA apps that need it
 * (currently SimplePhone)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "PlutoUtils/LinuxSoundcard.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"

string TranslateSoundcard(string sInput,string sIPAddress)
{
#ifndef WIN32	
	string sCmd, sOutput, sStdErr;
	if ( sIPAddress.empty()==false )
	{
		// Remote address, use ssh.
		const char *args[] = {"/usr/pluto/bin/TranslateRemoteSoundcard.sh",sIPAddress.c_str(),sInput.c_str(),NULL};
		ProcessUtils::GetCommandOutput(args[0],args,sOutput,sStdErr);
	}
	else
	{
		// This is local, just execute it.
		const char *args[] = {"/usr/pluto/bin/TranslateSoundCard.sh",sInput.c_str(),NULL};
		ProcessUtils::GetCommandOutput(args[0],args,sOutput,sStdErr);
	}

	StringUtils::TrimSpaces(sOutput);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TranslateSoundcard %s result %s", sInput.c_str(), sOutput.c_str());
#endif
#ifdef WIN32
	string sOutput = "";  // fix so win32 will compile.
#endif
	return sOutput;
}
