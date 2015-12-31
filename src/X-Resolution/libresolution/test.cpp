#include "libresolution.h"

#include <iostream>
using namespace std;

int main()
{
	ResolutionConf ResConf("../Resolutions.conf");
	ResResolutionVector ResVect;

	ResConf.GetResolutionList(ResVect);
	for (size_t i = 0; i < ResVect.size(); i++)
	{
		ResResolution & Resolution = ResVect[i];
		cout << Resolution.Name << " " << Resolution.AspectRatio << endl;
		for (size_t j = 0; j < Resolution.Modeline.size(); j++)
		{
			ResModeline & Modeline = Resolution.Modeline[j];
			cout << "  " << Modeline.Refresh << "Hz " << (Modeline.BuiltIn ? "built in" : Modeline.X11) << endl;
			for (size_t k = 0; k < Modeline.Workaround.size(); k++)
			{
				ResWorkaround & Workaround = Modeline.Workaround[k];
				string drivers;
				StringVector & Driver = Workaround.Driver;
				if (Driver.size() == 0)
				{
					drivers = "all ";
				}
				else
				{
					for (size_t l = 0; l < Driver.size(); l++)
						drivers += Driver[l] + " ";
				}
				cout << "    " + drivers + ": " + Workaround.X11 << endl;
			}
		}
	}

	return 0;
}
