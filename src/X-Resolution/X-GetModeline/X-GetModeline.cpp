#include "../libresolution/libresolution.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

#ifdef VERBOSE
#	define DEBUG(x) (x)
#else
#	define DEBUG(x)
#endif

int main(int argc, char * argv[])
{
	if (argc != 4)
	{
		fprintf(stderr, "Syntax: X-GetResolution <driver_name> <resolution_name> <refresh>\n");
		return 1;
	}

	const char *DriverName = argv[1];
	const char *ResName = argv[2];
	int Refresh = atoi(argv[3]);

	ResolutionConf ResConf("/usr/pluto/share/Resolutions.conf");

	ResResolution Resolution;
	if (!ResConf.GetResolutionData(ResName, Resolution))
		return 2;

	DEBUG(cout << Resolution.Name << " " << Resolution.AspectRatio << endl);
	string strModeline;
	for (size_t j = 0; j < Resolution.Modeline.size(); j++)
	{
		ResModeline & Modeline = Resolution.Modeline[j];
		if (Modeline.Refresh != Refresh)
			continue;

		DEBUG(cout << "  " << Modeline.Refresh << "Hz " << (Modeline.BuiltIn ? "built in" : Modeline.X11) << endl);
		if (Modeline.BuiltIn)
			strModeline = "builtin";
		else
			strModeline = Modeline.X11;

		for (size_t k = 0; k < Modeline.Workaround.size(); k++)
		{
			ResWorkaround & Workaround = Modeline.Workaround[k];
			DEBUG(string drivers);
			StringVector & Driver = Workaround.Driver;
			if (Driver.size() == 0)
			{
				DEBUG(drivers = "all ");
				strModeline = Workaround.X11;
			}
			else
			{
				DEBUG(
				for (size_t l = 0; l < Driver.size(); l++)
					drivers += Driver[l] + " ";
				);

				for (size_t l = 0; l < Driver.size(); l++)
				{
					if (Driver[l] == DriverName)
					{
						strModeline = Workaround.X11;
						break;
					}
				}
			}
			DEBUG(cout << "    " + drivers + ": " + Workaround.X11 << endl);
		}
		break;
	}

	if (strModeline.size())
	{
		char buffer[BUFSIZ];
		memset(buffer, 0, BUFSIZ);
		snprintf(buffer, BUFSIZ - 1, "\"%dx%d\" %s", Resolution.Width, Resolution.Height, strModeline.c_str());
		cout << buffer << endl;
	}

	return 0;
}
