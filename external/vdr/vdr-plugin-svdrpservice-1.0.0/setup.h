/*
 * setup.h: Settings
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _SVDRPSERVICE_SETUP__H
#define _SVDRPSERVICE_SETUP__H

#include <vdr/config.h>
#include <vdr/menuitems.h>

#define MAX_IP_LENGTH 16

struct cSvdrpServiceSetup {
	static const char *opt_serverIp;
	static const char *opt_serverPort;

	char serverIp[MAX_IP_LENGTH];
	int serverPort;
	int connectTimeout;
	int readTimeout;

	bool Parse(const char *Name, const char *Value);
	cSvdrpServiceSetup& operator=(const cSvdrpServiceSetup &Setup);
	cSvdrpServiceSetup();
};

extern cSvdrpServiceSetup SvdrpServiceSetup;

class cSvdrpServiceMenuSetup: public cMenuSetupPage {
	private:
		cSvdrpServiceSetup setupTmp;
	protected:
		virtual void Store(void);
	public:
		cSvdrpServiceMenuSetup();
		virtual ~cSvdrpServiceMenuSetup();
};

#endif //_SVDRPSERVICE_SETUP__H
