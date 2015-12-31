#ifndef LIBRESOLITION_H
#define LIBRESOLITION_H

#include <string>
#include <vector>
using namespace std;

typedef vector<string> StringVector;

struct ResWorkaround
{
	StringVector Driver;
	string X11;
};

typedef vector<ResWorkaround> ResWorkaroundVector;

struct ResModeline
{
	int Refresh;
	bool BuiltIn;
	ResWorkaroundVector Workaround;
	string X11;
};

typedef vector<ResModeline> ResModelineVector;

struct ResResolution
{
	string Name;
	int Width, Height;
	string AspectRatio;
	ResModelineVector Modeline;
	bool Visible;
};

typedef vector<ResResolution> ResResolutionVector;

#include <confuse.h>

class ResolutionConf
{
	public:
		ResolutionConf(string ConfigFilename);
		~ResolutionConf();
		void GetResolutionList(ResResolutionVector & ResolutionVector);
		bool GetResolutionData(const string & ResolutionName, ResResolution & Data);

	private:
		static cfg_opt_t cfgWorkaround[], cfgModeline[], cfgResolution[], cfgTop[];
		cfg_t * cfg;
};

#endif /* LIBRESOLITION_H */
