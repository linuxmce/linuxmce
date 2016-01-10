#include "i18n.h"
#include "setup.h"
//#include <vdr/menuitems.h>
#include <vdr/themes.h>

extern const char *OSDSKIN;

cSkinElchiSetup SkinElchiSetup;

cTheme Theme;

static const char allowed[] = { " abcdefghijklmnopqrstuvwxyz0123456789-_/" };
static const char hex[] = { "0123456789abcdef" };

#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
const char * FormatItems[] = {NULL, NULL};
#endif
const char * ShowAsItems[] = {NULL, NULL, NULL};
#else
const char * ShowAsItems[] = {NULL, NULL};
#endif
#ifdef HAVE_IMAGEMAGICK
const char * EpgImageItems[] = {NULL, NULL, NULL, NULL};
const char * ResizeItems[] = {NULL, NULL, NULL};
const char * dirfilenamechars = NULL;
#endif

static const char * ColorNames [] = {
			    "clrBackground",
			    "clrButtonRedFg","clrButtonRedBg","clrButtonGreenFg","clrButtonGreenBg","clrButtonYellowFg","clrButtonYellowBg","clrButtonBlueFg","clrButtonBlueBg",
			    "clrMessageStatusFg","clrMessageStatusBg","clrMessageInfoFg","clrMessageInfoBg","clrMessageWarningFg","clrMessageWarningBg","clrMessageErrorFg","clrMessageErrorBg",
			    "clrVolumePrompt","clrVolumeBarUpper","clrVolumeBarLower","clrVolumeSymbolMuteBg","clrVolumeSymbolMuteFg","clrVolumeSymbolVolumeBg","clrVolumeSymbolVolumeFg",
			    "clrChannelNameFg","clrChannelNameBg","clrChannelNumberFg","clrChannelNumberBg","clrChannelDateFg","clrChannelDateBg","clrChannelEpgTimeFg","clrChannelEpgTimeBg","clrChannelEpgTitle","clrChannelEpgTitleBg","clrChannelEpgShortText","clrChannelSymbolOn","clrChannelSymbolOff","clrChannelSymbolRecFg","clrChannelSymbolRecBg","clrChannelTimebarSeen","clrChannelTimebarRest",
			    "clrMenuTitleFg","clrMenuTitleBg","clrMenuDate","clrMenuItemCurrentFg","clrMenuItemCurrentBg","clrMenuItemSelectable","clrMenuItemNonSelectable","clrMenuEventTime","clrMenuEventVpsFg","clrMenuEventVpsBg","clrMenuEventTitle","clrMenuEventShortText","clrMenuEventDescription","clrMenuScrollbarTotal","clrMenuScrollbarShown","clrMenuText",
			    "clrReplayTitleFg","clrReplayTitleBg","clrReplayCurrent","clrReplayTotal","clrReplayModeJump","clrReplayProgressSeen","clrReplayProgressRest","clrReplayProgressSelected","clrReplayProgressMark","clrReplayProgressCurrent","clrReplaySymbolOn","clrReplaySymbolOff",
			    "Transparent",
			    "changing",
};

int lastcurrent;
int coloritemoffset;

// --- cSkinElchiSetup ----------------------------------------------------------

cSkinElchiSetup::cSkinElchiSetup(void)
{
	logo = 0;
//for epgimages
#ifdef HAVE_IMAGEMAGICK
	epgimages = 0;
	epgshowtime = 5;
	EpgImageItems[0] = tr("no");
	EpgImageItems[1] = "120x90 (epg4vdr)";
	EpgImageItems[2] = "max. 180x144";
	EpgImageItems[3] = "120x120 (tvmovie2vdr)";
	resize = 0;
	ResizeItems[0] = "pixel algo";
	ResizeItems[1] = "ratio algo";
	ResizeItems[2] = "zoom image";
	strcpy(epgimagedir, "/video/epgimages");
	commandlinedir = false;
	char *s;
    	asprintf(&s, "%s/_", tr(FileNameChars));
	dirfilenamechars = strdup(s);
	free(s);
#endif
//
	singlearea = 0;
	vidwin = 0;
	bit = 0;
#ifdef VIDEOSTATUS
	videostatus = 0;
	switchvideodisplayformat = 0;
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
	format = 0;
	quality = 100;
	intervall = 5;
	videoxoffset = 32;
	videoyoffset = 4;
#endif
#endif
	hidemenu = 0;
	strncpy(menuentry,"Elchi_VDR_Skin-Plugin", sizeof(menuentry));
	usenofixedfont = 0;
	color = 0;
	ShowAsItems[0] = tr("no");
	ShowAsItems[1] = tr("as skin setup");
#ifdef HAVE_YAEPG_PATCH
	ShowAsItems[2] = tr("as plugin");
#ifdef HAVE_IMAGEMAGICK
	FormatItems[0] = "pnm";
	FormatItems[1] = "jpg";
#endif
#endif
}


bool cSkinElchiSetup::SetupParse(const char *Name, const char *Value)
{
	if (strcmp(Name, "logo") == 0) logo = atoi(Value);
//for epgimages
#ifdef HAVE_IMAGEMAGICK
	else if (strcmp(Name, "epgimages") == 0) epgimages = atoi(Value);
	else if (strcmp(Name, "resize")	== 0) resize  = atoi(Value);
	else if (!strcasecmp(Name, "epgimagedir") && !commandlinedir) strn0cpy(epgimagedir, Value , sizeof(epgimagedir));
	else if (strcmp(Name, "epgshowtime") == 0) epgshowtime = atoi(Value);
#endif
//
	else if (strcmp(Name, "singlearea") == 0) singlearea = atoi(Value);
	else if (strcmp(Name, "vidwin") == 0) vidwin = atoi(Value);
	else if (strcmp(Name, "bit") == 0) bit = atoi(Value);
#ifdef VIDEOSTATUS
	else if (strcmp(Name, "videostatus") == 0) videostatus = atoi(Value);
	else if (strcmp(Name, "switchvideodisplayformat") == 0) switchvideodisplayformat = atoi(Value);
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
	else if (strcmp(Name, "format") == 0) format = atoi(Value);
	else if (strcmp(Name, "quality") == 0) quality = atoi(Value);
	else if (strcmp(Name, "intervall") == 0) intervall = atoi(Value);
	else if (strcmp(Name, "videoxoffset") == 0) videoxoffset = atoi(Value);
	else if (strcmp(Name, "videoyoffset") == 0) videoyoffset = atoi(Value);
#endif
#endif
	else if	(strcmp(Name, "usenofixedfont") == 0) usenofixedfont = atoi(Value);
	else if	(strcmp(Name, "hidemenu") == 0) hidemenu = atoi(Value);
	else if (!strcasecmp(Name, "menuentry")) snprintf(menuentry, sizeof(menuentry), Value);
	else return false;
	return true;
}


// --- cSkinElchiSetupPage ------------------------------------------------------

void cSkinElchiSetupPage::ShowItems(void)
{
	coloritemoffset = 0;
	Add(new cMenuEditBoolItem(tr("Channel logo"), &mySkinElchiSetup.logo, tr("no"), tr("show") ) );
	coloritemoffset +=1;
//for epgimages
#ifdef HAVE_IMAGEMAGICK
	Add(new cMenuEditStraItem(tr("Show pictures of epg"), &mySkinElchiSetup.epgimages, 4, EpgImageItems ) );
	coloritemoffset +=1;
	if (mySkinElchiSetup.epgimages) {
	    Add(new cMenuEditIntItem(tr("  Showtime for image [s]"), &mySkinElchiSetup.epgshowtime, 2, 15) );
	    coloritemoffset +=1;
	    if (mySkinElchiSetup.epgimages == 2) {
		Add(new cMenuEditStraItem("  Resize use", &mySkinElchiSetup.resize, 3, ResizeItems));
		coloritemoffset +=1;
		}
	    if (mySkinElchiSetup.commandlinedir) {
		char *s;
    		asprintf(&s, "%s: '%s'",tr("  Image directory"), mySkinElchiSetup.epgimagedir);
		cOsdItem *item = new cOsdItem(s, osUser9);
		free(s);
		item->SetSelectable(false);
		Add(item);
		}
	    else
		Add(new cMenuEditStrItem (tr("  Image directory"), mySkinElchiSetup.epgimagedir, sizeof(mySkinElchiSetup.epgimagedir), dirfilenamechars));
	    coloritemoffset +=1;
	    }	    
#endif
//
	Add(new cMenuEditBoolItem(tr("One area (if possible)"), &mySkinElchiSetup.singlearea, tr("no"), tr("yes") ) );
	coloritemoffset +=1;
#ifdef VIDEOSTATUS
	Add(new cMenuEditBoolItem(tr("Video status (if possible)"), &mySkinElchiSetup.videostatus, tr("no"), tr("show") ) );
	coloritemoffset +=1;
	if (mySkinElchiSetup.videostatus) {
	    Add(new cMenuEditBoolItem(tr("  Switch VDF on replay"), &mySkinElchiSetup.switchvideodisplayformat, tr("no"), tr("yes") ) );
	    coloritemoffset +=1;
	    }
#endif
#ifdef HAVE_YAEPG_PATCH
	Add(new cMenuEditBoolItem(tr("Video window (if possible)"), &mySkinElchiSetup.vidwin, tr("no"), tr("show") ) );
	coloritemoffset +=1;
#ifdef HAVE_IMAGEMAGICK
	if (mySkinElchiSetup.vidwin && mySkinElchiSetup.singlearea) {
	    Add(new cMenuEditBoolItem(tr(" Show snapshot (if possible)"), &mySkinElchiSetup.bit, tr("no"), tr("show") ) );
	    coloritemoffset +=1;
	    if (mySkinElchiSetup.bit) {
		Add(new cMenuEditStraItem(tr("  Format"), &mySkinElchiSetup.format, 2, FormatItems));
		coloritemoffset +=1;
		if (mySkinElchiSetup.format) {
		    Add(new cMenuEditIntItem(tr("  Quality"), &mySkinElchiSetup.quality, 1, 100) );
		    coloritemoffset +=1;
		    }
		Add(new cMenuEditIntItem(tr("  Intervall for snapshots [s]"), &mySkinElchiSetup.intervall, 2, 15) );
		coloritemoffset +=1;
		Add(new cMenuEditIntItem(tr("  Video-x-offset"), &mySkinElchiSetup.videoxoffset, -10, 60) );
		coloritemoffset +=1;
		Add(new cMenuEditIntItem(tr("  Video-y-offset"), &mySkinElchiSetup.videoyoffset, -10, 30) );
		coloritemoffset +=1;
		}
	}
#endif
#endif
	Add(new cMenuEditBoolItem(tr("Don't use 'FixedFont'"), &mySkinElchiSetup.usenofixedfont, tr("no"), tr("yes") ) );
	coloritemoffset +=1;
	Add(new cMenuEditBoolItem(tr("Colors"), &mySkinElchiSetup.color, tr("hide"), tr("show") ) );
	coloritemoffset +=1;
	if (mySkinElchiSetup.color) {
	    for (int i = 0; i < (MAX_ELCHI_THEME_COLORS - 2); i++)
	    {
		mySkinElchiSetup.ThemeColors[i] = Theme.Color(i);
		char *s;
    		asprintf(&s, "%08x", (unsigned int)mySkinElchiSetup.ThemeColors[i]);
		strncpy(mySkinElchiSetup.tempcolor, s,sizeof(mySkinElchiSetup.tempcolor));
		free(s);
		Add(new cMenuEditStrItem(ColorNames[i], mySkinElchiSetup.tempcolor, sizeof(mySkinElchiSetup.tempcolor) + 1, hex));
	    }
	}
	Add(new cMenuEditStraItem(tr("Show in main menu"), &mySkinElchiSetup.hidemenu,
#ifdef HAVE_YAEPG_PATCH
	    3
#else
	    2
#endif
	    , ShowAsItems));
	if (mySkinElchiSetup.hidemenu)
	    Add(new cMenuEditStrItem ( tr("Main menu entry"), mySkinElchiSetup.menuentry, sizeof(mySkinElchiSetup.menuentry), allowed));
}

cSkinElchiSetupPage::cSkinElchiSetupPage(void)
{
	mySkinElchiSetup = SkinElchiSetup;
	
	SkinElchiSetup.changing = false;
	lastcurrent = -1;
	ShowItems();
}


eOSState cSkinElchiSetupPage::ProcessKey(eKeys Key)
{
	bool oldsinglearea = mySkinElchiSetup.singlearea;
	bool oldvidwin = mySkinElchiSetup.vidwin;
	bool oldbit = mySkinElchiSetup.bit;
#ifdef HAVE_IMAGEMAGICK
	int oldepgimages = mySkinElchiSetup.epgimages;
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
	bool oldformat = mySkinElchiSetup.format;
#endif
#endif
#ifdef VIDEOSTATUS
	bool oldvideostatus = mySkinElchiSetup.videostatus;
#endif
	int oldhidemenu = mySkinElchiSetup.hidemenu;
	bool oldcolor = mySkinElchiSetup.color;
	char *oldtempcolor;
	asprintf(&oldtempcolor, "%s", mySkinElchiSetup.tempcolor);
	if ((mySkinElchiSetup.color) && (Current() > (coloritemoffset - 1) && Current() < (coloritemoffset + MAX_ELCHI_THEME_COLORS - 2))) { 
	    SkinElchiSetup.changing = true;
	    if (lastcurrent != Current()) {
	        char *s;
    	        asprintf(&s, "%08x", (int)mySkinElchiSetup.ThemeColors[Current() - coloritemoffset]);
	        strncpy(mySkinElchiSetup.tempcolor, s,sizeof(mySkinElchiSetup.tempcolor));
	        lastcurrent = Current();
	        free(s);
	    }
	}
	else SkinElchiSetup.changing = false;
	
	eOSState state = cMenuSetupPage::ProcessKey(Key);

	if (mySkinElchiSetup.color) {
	    if (Current() > (coloritemoffset - 1) && Current() < (coloritemoffset + MAX_ELCHI_THEME_COLORS - 2)) { 
		if (lastcurrent == Current()) {
		    if (strcasecmp(oldtempcolor, mySkinElchiSetup.tempcolor)) {
			unsigned int temp = 0;
			for (int i = 0; i < 8; i++) {
			    temp = temp * 16;
			    unsigned int temp1 = (unsigned int)(mySkinElchiSetup.tempcolor[i]);
			    unsigned int temp2 = (temp1 <= 0x39) ? 0x30 : 0x57;
			    temp = temp + (temp1 - temp2);
			    }
			mySkinElchiSetup.ThemeColors[Current() - coloritemoffset] = tColor (temp);
			SkinElchiSetup.ThemeColors[Current() - coloritemoffset] = mySkinElchiSetup.ThemeColors[Current() - coloritemoffset];
			}
		    }
		SkinElchiSetup.ThemeColors[MAX_ELCHI_THEME_COLORS - 1] = mySkinElchiSetup.ThemeColors[Current() - coloritemoffset];
    		Theme.AddColor("changing", SkinElchiSetup.ThemeColors[Current() - coloritemoffset]);
		Theme.AddColor(ColorNames[Current() - coloritemoffset], SkinElchiSetup.ThemeColors[Current() - coloritemoffset]);
		SkinElchiSetup.changing = true;
		Display();
		}
	    }		

	if (oldcolor != mySkinElchiSetup.color
	    || oldsinglearea != mySkinElchiSetup.singlearea
	    || oldvidwin != mySkinElchiSetup.vidwin
	    || oldbit != mySkinElchiSetup.bit
#ifdef HAVE_IMAGEMAGICK
	    || oldepgimages != mySkinElchiSetup.epgimages
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
	    || oldformat != mySkinElchiSetup.format
#endif
#endif
#ifdef VIDEOSTATUS
	    || oldvideostatus != mySkinElchiSetup.videostatus
#endif
	    || oldhidemenu != mySkinElchiSetup.hidemenu)
	{
	    int oldcurrent = Current();
	    lastcurrent = -1;
	    Clear();
	    ShowItems();
	    SetCurrent(Get(oldcurrent));
	    Display();
	}
	return state;
}

cSkinElchiSetupPage::~cSkinElchiSetupPage()
{
    SkinElchiSetup.changing = false;
}


void cSkinElchiSetupPage::Store(void)
{
    SkinElchiSetup = mySkinElchiSetup;
    SetupStore("logo", SkinElchiSetup.logo);
//for epgimages
#ifdef HAVE_IMAGEMAGICK
    SetupStore("epgimages", SkinElchiSetup.epgimages);
    SetupStore("epgshowtime", SkinElchiSetup.epgshowtime);
    SetupStore("resize", SkinElchiSetup.resize);
    SetupStore("epgimagedir", SkinElchiSetup.epgimagedir);
#endif
//
    SetupStore("singlearea", SkinElchiSetup.singlearea);
    SetupStore("vidwin", SkinElchiSetup.vidwin);
    SetupStore("bit", SkinElchiSetup.bit);
#ifdef VIDEOSTATUS
    SetupStore("videostatus", SkinElchiSetup.videostatus);
    SetupStore("switchvideodisplayformat", SkinElchiSetup.switchvideodisplayformat);
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
    SetupStore("format", SkinElchiSetup.format);
    SetupStore("quality", SkinElchiSetup.quality);
    SetupStore("intervall", SkinElchiSetup.intervall);
    SetupStore("videoxoffset", SkinElchiSetup.videoxoffset);
    SetupStore("videoyoffset", SkinElchiSetup.videoyoffset);
#endif
#endif
    SetupStore("usenofixedfont", SkinElchiSetup.usenofixedfont);
    SetupStore("hidemenu", SkinElchiSetup.hidemenu);
    SetupStore("menuentry", SkinElchiSetup.menuentry);
    char *filename;
    asprintf(&filename, "%s/../../themes/%s%s", cPlugin::ConfigDirectory("skinelchi"), OSDSKIN,"-change.theme");
    Theme.Save(filename);
    free(filename);
}

