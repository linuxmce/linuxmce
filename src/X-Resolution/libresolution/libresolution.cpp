#include "libresolution.h"

#include <confuse.h>

// class ResolutionConf

cfg_opt_t ResolutionConf::cfgWorkaround[] =
{
	CFG_STR_LIST("driver", "{}", CFGF_NONE),
	CFG_STR("x11", "", CFGF_NONE),
	CFG_END()
};

cfg_opt_t ResolutionConf::cfgModeline[] =
{
	CFG_INT("refresh", 60, CFGF_NONE),
	CFG_BOOL("builtin", cfg_false, CFGF_NONE),
	CFG_SEC("workaround", ResolutionConf::cfgWorkaround, CFGF_MULTI),
	CFG_STR("x11", "", CFGF_NONE),
	CFG_END()
};

cfg_opt_t ResolutionConf::cfgResolution[] =
{
	CFG_STR("name", "-unspecified-", CFGF_NONE),
	CFG_INT("width", 0, CFGF_NONE),
	CFG_INT("height", 0, CFGF_NONE),
	CFG_STR("aspect_ratio", "", CFGF_NONE),
	CFG_SEC("modeline", ResolutionConf::cfgModeline, CFGF_MULTI),
	CFG_BOOL("visible", cfg_false, CFGF_NONE),
	CFG_END()
};

cfg_opt_t ResolutionConf::cfgTop[] =
{
	CFG_SEC("resolution", ResolutionConf::cfgResolution, CFGF_MULTI),
	CFG_END()
};

ResolutionConf::ResolutionConf(string ConfigFilename)
{
	cfg = cfg_init(cfgTop, CFGF_NONE);
	cfg_parse(cfg, ConfigFilename.c_str());
}

ResolutionConf::~ResolutionConf()
{
	cfg_free(cfg);
}

void ResolutionConf::GetResolutionList(ResResolutionVector & ResolutionVector)
{
	for (size_t iRes = 0; iRes < cfg_size(cfg, "resolution"); ++iRes)
	{
		cfg_t * cfg_resolution = cfg_getnsec(cfg, "resolution", iRes);
		ResResolution Resolution;

		Resolution.Name = cfg_getstr(cfg_resolution, "name");
		Resolution.Width = cfg_getint(cfg_resolution, "width");
		Resolution.Height = cfg_getint(cfg_resolution, "height");
		Resolution.AspectRatio = cfg_getstr(cfg_resolution, "aspect_ratio");
		for (size_t iModline = 0; iModline < cfg_size(cfg_resolution, "modeline"); ++iModline)
		{
			cfg_t * cfg_modeline = cfg_getnsec(cfg_resolution, "modeline", iModline);
			ResModeline Modeline;

			Modeline.Refresh = cfg_getint(cfg_modeline, "refresh");
			Modeline.BuiltIn = cfg_getbool(cfg_modeline, "builtin");
			for (size_t iWorkaround = 0; iWorkaround < cfg_size(cfg_modeline, "workaround"); ++iWorkaround)
			{
				cfg_t * cfg_workaround = cfg_getnsec(cfg_modeline, "workaround", iWorkaround);
				ResWorkaround Workaround;

				for (size_t iDriver = 0; iDriver < cfg_size(cfg_workaround, "driver"); iDriver++)
					Workaround.Driver.push_back(cfg_getnstr(cfg_workaround, "driver", iDriver));
				Workaround.X11 = cfg_getstr(cfg_workaround, "x11");
				Modeline.Workaround.push_back(Workaround);
			}
			Modeline.X11 = cfg_getstr(cfg_modeline, "x11");
			Resolution.Modeline.push_back(Modeline);
		}
		Resolution.Visible = cfg_getbool(cfg_resolution, "visible");
		ResolutionVector.push_back(Resolution);
	}
}

bool ResolutionConf::GetResolutionData(const string & ResolutionName, ResResolution & Data)
{
	ResResolutionVector ResolutionVector;
	GetResolutionList(ResolutionVector);
	for (size_t i = 0; i < ResolutionVector.size(); i++)
	{
		char buffer[1024];
		snprintf(buffer, 1024, "%dx%d", ResolutionVector[i].Width, ResolutionVector[i].Height);
		string ResolutionAltName = buffer;
		if (ResolutionVector[i].Name == ResolutionName || ResolutionAltName == ResolutionName)
		{
			Data = ResolutionVector[i];
			return true;
		}
	}
	return false;
}
