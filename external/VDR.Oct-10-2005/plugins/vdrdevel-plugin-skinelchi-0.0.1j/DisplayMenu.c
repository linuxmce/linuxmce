#ifdef HAVE_IMAGEMAGICK
#ifdef HAVE_C295
#include <stl.h>
#endif
#endif
#include "DisplayMenu.h"
#ifdef HAVE_IMAGEMAGICK
#include "bitmap.h"
cOSDImageBitmap image;
#ifdef HAVE_YAEPG_PATCH
cBitmap bmpVideo(8,8,1);
int X4, Y3;
int intervall;
bool epgwin = true;
bool imagereadwriteready = false;
bool imageconvertready = false;
#endif

//for epgimages
cBitmap bmpEpgImage(8,8,1);
int epgimagecolors = 0;
bool loading = false;
bool epgimageconvertready = false;
u_int16_t eventID;
const char *epgimagebuttons[] = {NULL, NULL, NULL, NULL};
const char *title = NULL;
//

#else
#warning WARNING: YOU WILL NOT BE ABLE TO USE EPG-IMAGES
#warning UNCOMMENT "HAVE_IMAGEMAGICK = 1" IN "MAKEFILE" IF YOU WANT THIS
#endif

// --- cSkinElchiDisplayMenu -----------------------------------------------

cSkinElchiDisplayMenu::cSkinElchiDisplayMenu(void)
{
    Show();
}

//for epgimages
#ifdef HAVE_IMAGEMAGICK
void cSkinElchiDisplayMenu::ShowEpgImage(void)
{
  if (!epgimages) {
    epgimages = true;
    epgimageconvertready = false;
    loading = false;
    epgimage = new cEpgImage;
    delete osd;
    if (SkinElchiSetup.epgimages == 1) {
	x4 = x6 - 120;
	y4 = y3 + 90;
	}
    else {
	if (SkinElchiSetup.epgimages == 3) {
	    x4 = x6 - 120;
	    y4 = y3 + 120;
	    }
	}
    osd = cOsdProvider::NewOsd(0, 0);
    tArea Areas[] = { { x0, y0, x8 - 1, y8 - 1, 8} };
    if ((osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) && (SkinElchiSetup.singlearea)) {
	osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
	osd->GetBitmap(0)->Reset();
	for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++)
    	    osd->GetBitmap(0)->SetColor(i, SkinElchiSetup.ThemeColors[i]);
	epgimagecolors = 256 - MAX_ELCHI_THEME_COLORS;
	area = 0;
	}
    else {
/*    	tArea Areas[] = { { x4, y3, x6 - 1, y4 - 1, 8 },
			{ x0, y0, x8 - 1, y2 - 1, 2 },
            		{ x0, y2, x8 - 1, y3 - 1, 2 },
            		{ x0, y3, x4 - 1, y4 - 1, 2 },
			{ x0, y4, x6 - 1, y5 - 1, 2 },
                	{ x6, y3, x8 - 1, y5 - 1, 2 },
            		{ x0, y5, x8 - 1, y8 - 1, 4 },
                	};
	if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
	    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
	    for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++)
    		osd->GetBitmap(0)->SetColor(i, SkinElchiSetup.ThemeColors[i]);
	    epgimagecolors = 256 - MAX_ELCHI_THEME_COLORS;
	    area = 4;
	    }
	else */{
	    tArea Areas[] = { { x0, y0, x8 - 1, y2 - 1, 2 },
                	    { x0, y2, x8 - 1, y3 - 1, 2 },
                	    { x0, y3, x4 - 1, y4 - 1, 2 },
			    { x4, y3, x6 - 1, y4 - 1, 4 },
			    { x0, y4, x6 - 1, y5 - 1, 2 },
                	    { x6, y3, x8 - 1, y5 - 1, 2 },
                    	    { x0, y5, x8 - 1, y8 - 1, 4 },
                	    };
    	    if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
		osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
    		epgimagecolors = 16;
		area = 4;
		}
	    else {
		tArea Areas[] = { { x0, y0, x8 - 1, y2 - 1, 2 },
			        { x0, y2, x6 - 1, y5 - 1, 2 },
		    		{ x6, y2, x8 - 1, y5 - 1, 2 },
            	    		{ x0, y5, x8 - 1, y8 - 1, 4 },
            	    		};
		osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
		area = 1;
		epgimages = false;
		}
	    }
	}	
    if (epgimages) {
	const cFont *font = cFont::GetFont(fontOsd);
	lineHeight = font->Height();
	menuHeight = y5 - y4 - lineHeight;
	menuTop = y5 - menuHeight;
	}
    else {
	menuHeight = y5 - y3;
	menuTop = y3;
	}
    tColor tr = Theme.Color(Transparent);
    tColor tb = Theme.Color(clrMenuTitleBg);
    osd->DrawRectangle(x0, y0, x2 - 1, y1 - 1, tr );
    osd->DrawEllipse(x0, y0,x2 - 1, y1 - 1, tb, 2);
    osd->DrawRectangle(x0, y1, x2 - 1, y2 - 1, tb);
    osd->DrawRectangle(x6, y0, x8 - 1, y1 - 1, tr);
    osd->DrawEllipse(x6, y0, x8 - 1, y1  - 1, tb, 1);
    osd->DrawRectangle(x6, y1, x8 - 1, y2 - 1, tb);
    tColor bg = Theme.Color(clrBackground);
    osd->DrawRectangle(x0, y2, x6 - 1, y3 - 1, bg);
    osd->DrawRectangle(x6, y2, x8 - 1, y3 - 1, bg);
    osd->DrawRectangle(x0, y3, x4 - 1, y4 - 1, bg);
    osd->DrawRectangle(x4, y3, x6 - 1, y4 - 1, bg);
    osd->DrawRectangle(x0, y4, x6 - 1, y5 - 1, bg);
    osd->DrawRectangle(x6, y3, x8 - 1, y4 - 1, bg);
    osd->DrawRectangle(x6, y4, x8 - 1, y5 - 1, bg);
    osd->DrawRectangle(x0, y5, x8 - 1, y6 - 1, bg);
    }
  SetTitle(title);
  SetButtons(epgimagebuttons[0], epgimagebuttons[1], epgimagebuttons[2], epgimagebuttons[3]);
}
#endif
//

void cSkinElchiDisplayMenu::Show(void)
{
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();

#ifdef HAVE_YAEPG_PATCH
  vidwin = SkinElchiSetup.vidwin;
#ifdef HAVE_IMAGEMAGICK
  if (vidwin)
    bit = SkinElchiSetup.bit;
  else
#endif
    bit = false;
#else
#warning WARNING: YOU WILL NOT BE ABLE TO USE VIDEOWINDOW
#warning UNCOMMENT "HAVE_YAEPG_PATCH = 1" IN "MAKEFILE" IF YOU WANT THIS
  vidwin = false;
  bit = false;
#endif
  epgimages = false;
  x0 = Setup.OSDLeft & ~0x07;
  x1 = x0 + lineHeight / 2;
  x8 = (x0 + Setup.OSDWidth) & ~0x07;
  x6 = (x8 - lineHeight + lineHeight / 2) & ~0x07;
  x2 = x0 + x8 - x6;
  x4 = x6 - 720 / 4;
  x3 = x4 - lineHeight / 2;
  x5 = x6 - lineHeight / 2;
  x7 = x8 - lineHeight / 2;
  roundness = x2 - x0;
  y0 = Setup.OSDTop;
  y1 = y0 + roundness;
  y2 = (y0 + lineHeight);
  y3 = y2 + lineHeight;
  y4 = y3 + 576 / 4;
  y8 = (y0 + Setup.OSDHeight);
  y7 = y8 - roundness;
  y6 = y8 - lineHeight;
  y5 = (y6 - lineHeight);
  osd = cOsdProvider::NewOsd(0, 0);
  tArea Areas[] = { { x0, y0, x8 - 1, y8 - 1, 4 + bit * 4} };
  if ((osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) && (SkinElchiSetup.singlearea)) {
#ifdef HAVE_YAEPG_PATCH
    /*
    * Create the video window
    *///changed original from yaepg
    if (vidwin) {
	osd->vidWin.x1 = x4;
	osd->vidWin.y1 = y3;
	osd->vidWin.x2 = x8;
	osd->vidWin.y2 = y4;
	osd->vidWin.bpp = 13;
	}
#endif
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
    if (bit) {
	for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++) {
	    osd->GetBitmap(0)->SetColor(i,SkinElchiSetup.ThemeColors[i]);
	    }
	}
    area = 0;
    }
  else {
    tArea Areas[] = { { x0, y0, x8 - 1, y8 - 1, 4} };
    if ((osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) && (SkinElchiSetup.singlearea) && (bit)) {
	bit = false;
#ifdef HAVE_YAEPG_PATCH
        /*
	* Create the video window
	*///changed original from yaepg
	if (vidwin) {
	    osd->vidWin.x1 = x4;
	    osd->vidWin.y1 = y3;
	    osd->vidWin.x2 = x8;
	    osd->vidWin.y2 = y4;
	    osd->vidWin.bpp = 13;
	    }
#endif
	osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
	area = 0;
	}
    else { 
	  bit = false;
#ifdef HAVE_YAEPG_PATCH
	  if (vidwin) {
	    tArea Areas[] = { { x0, y0, x8 - 1, y2 - 1, 2 },
                	    { x0, y2, x8 - 1, y3 - 1, 2 },
                	    { x0, y3, x4 - 1, y4 - 1, 2 },
                	    { x0, y4, x6 - 1, y5 - 1, 2 },
                	    { x6, y3, x8 - 1, y5 - 1, 2 }, //dont't show - i think because the videowindow is show - is no videowindow then this area will be show
                    	    { x0, y5, x8 - 1, y8 - 1, 4 },
                    	    };
    	    if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
		osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
		area = 3;
		osd->GetBitmap(3)->SetColor(0, Theme.Color(clrBackground));
		osd->GetBitmap(3)->SetColor(1, Theme.Color(clrMenuItemCurrentBg));
		osd->GetBitmap(3)->SetColor(2, Theme.Color(clrMenuItemCurrentFg));
		osd->GetBitmap(3)->SetColor(3, Theme.Color(clrMenuItemSelectable));
		//clrMenuItemNonSelectable is not available because 2-bit-area contains only 4 colors
    		/*
		* Create the video window
		*///changed original from yaepg
		osd->vidWin.x1 = x4;
		osd->vidWin.y1 = y3;
		osd->vidWin.x2 = x8;
		osd->vidWin.y2 = y4;
		osd->vidWin.bpp = 13;
		}
	    else
		vidwin = false;
	    }
	  if (!vidwin) {
#else
	  if (vidwin)
	    vidwin = false;
	  {
#endif
	    tArea Areas[] = { { x0, y0, x8 - 1, y2 - 1, 2 },
		    	    { x0, y2, x6 - 1, y5 - 1, 2 },
		    	    { x6, y2, x8 - 1, y5 - 1, 2 },
            	    	    { x0, y5, x8 - 1, y8 - 1, 4 },
            	    	    };
	    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
	    area = 1;
	    osd->GetBitmap(1)->SetColor(0, Theme.Color(clrBackground));
	    osd->GetBitmap(1)->SetColor(1, Theme.Color(clrMenuItemCurrentBg));
	    osd->GetBitmap(1)->SetColor(2, Theme.Color(clrMenuItemCurrentFg));
	    osd->GetBitmap(1)->SetColor(3, Theme.Color(clrMenuItemSelectable));
	    //clrMenuItemNonSelectable is not available because 2-bit-area contains only 4 colors
	    }
	}
    }
  menuHeight = y5 - (vidwin ? (y4 + lineHeight) : y3);
  menuTop = vidwin ? (y5 - menuHeight) : y3;

  tColor tr = Theme.Color(Transparent);
  tColor tb = Theme.Color(clrMenuTitleBg);
//  area0(if video) 0,0,8,2 //titleline
  osd->DrawRectangle(x0, y0, x2 - 1, y1 - 1, tr);
  osd->DrawEllipse(x0, y0,x2 - 1, y1 - 1, tb, 2);
  osd->DrawRectangle(x0, y1, x2 - 1, y2 - 1, tb);
  osd->DrawRectangle(x6, y0, x8 - 1, y1 - 1, tr);
  osd->DrawEllipse(x6, y0, x8 - 1, y1  - 1, tb, 1);
  osd->DrawRectangle(x6, y1, x8 - 1, y2 - 1, tb);

  tColor bg = Theme.Color(clrBackground);
// area1(if video) 0,2,6,5 //between titleline and menu/video
  osd->DrawRectangle(x0, y2, x6 - 1, y3 - 1, bg);
  osd->DrawRectangle(x6, y2, x8 - 1, y3 - 1, bg);

//area2(if video) 0,3,4,4 //menuarea
  osd->DrawRectangle(x0, y3, x4 - 1, y4 - 1, bg);

//area3(if video) 0,4,6,5 //menuarea
  osd->DrawRectangle(x0, y4, x6 - 1, y5 - 1, bg);

//area4(if video) 6,4,8,5 //scrollarea
  osd->DrawRectangle(x6, y3, x8 - 1, y4 - 1, bg);
  osd->DrawRectangle(x6, y4, x8 - 1, y5 - 1, bg);

//area5(if video) 0,5,8,8 //messageline
  osd->DrawRectangle(x0, y5, x8 - 1, y6 - 1, bg);

//videowindow 4,3,6,4 //videoarea
#ifdef HAVE_YAEPG_PATCH
  if (vidwin) {
    osd->DrawRectangle(x4, y3, x6 - 1, y4 - 1, tr);
#ifdef HAVE_IMAGEMAGICK
    if (bit) {
    	X4 = x4 + SkinElchiSetup.videoxoffset;
    	Y3 = y3 + SkinElchiSetup.videoyoffset;
    	imageconvertready = false;
    	imagereadwriteready = true;
    	makeimage = new cBackgroundImage;
	epgwin = !SkinElchiSetup.bit;
    	}
#endif
    }
  else
#endif
       {
    osd->DrawRectangle(x4, y3, x6 - 1, y4 - 1, bg);
    }
}

cSkinElchiDisplayMenu::~cSkinElchiDisplayMenu()
{
#ifdef HAVE_IMAGEMAGICK
  loading = true;
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
  intervall = 0;
#endif
#endif
  delete osd;
}

void cSkinElchiDisplayMenu::SetScrollbar(void)
{
  if (textScroller.CanScroll()) {
    int yt = textScroller.Top();
    int yb = yt + textScroller.Height();
    if (area == 1) {
	osd->GetBitmap(2)->Reset();
	osd->DrawRectangle (x6, y2, x8 - 1, yt - 1, Theme.Color(clrBackground));
	osd->DrawRectangle (x6, yb, x8 - 1, y5 - 1, Theme.Color(clrBackground));
	}
    int st = yt;
    int sb = yb;
    int tt = st + (sb - st) * textScroller.Offset() / textScroller.Total();
    int tb = tt + (sb - st) * textScroller.Shown() / textScroller.Total();
    int xl = x6 + 2;
    osd->DrawRectangle(xl, st, x8 - 1, sb, Theme.Color(clrMenuScrollbarTotal));
    osd->DrawRectangle(xl, tt, x8 - 1, tb, Theme.Color(clrMenuScrollbarShown));
    }
}

void cSkinElchiDisplayMenu::Scroll(bool Up, bool Page)
{
  cSkinDisplayMenu::Scroll(Up, Page);
  SetScrollbar();
}

int cSkinElchiDisplayMenu::MaxItems(void)
{
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  return (menuHeight / lineHeight);
}

void cSkinElchiDisplayMenu::Clear(void)
{

//for epgimages
#ifdef HAVE_IMAGEMAGICK
  if (epgimages) {
    epgimages = false;
    loading = true;
    delete osd;
    Show();
    }    
#endif
//

  textScroller.Reset();
  tColor bg = Theme.Color(clrBackground);

#ifdef HAVE_YAEPG_PATCH
  if (vidwin) {
#ifdef HAVE_IMAGEMAGICK
    if (epgwin) {
	osd->DrawRectangle(x0, y3, x4 - 1, y4 - 1, bg);
	if (bit)
	    epgwin = false;
	}
#endif
    if (area == 0)
	osd->DrawRectangle(x4, y3, x6 - 1, y4 - 1, Theme.Color(Transparent));
    else { 
	if (area == 3) {
	    osd->GetBitmap(3)->Reset();
	    osd->GetBitmap(3)->SetColor(0, bg);
	    osd->GetBitmap(3)->SetColor(1, Theme.Color(clrMenuItemCurrentBg));
	    osd->GetBitmap(3)->SetColor(2, Theme.Color(clrMenuItemCurrentFg));
	    osd->GetBitmap(3)->SetColor(3, Theme.Color(clrMenuItemSelectable));
	    //clrMenuItemNonSelectable is not available because 2-bit-area contains only 4 colors
	    }
	}
    } 
  else
#endif
       {
    if (area == 1) {
	osd->GetBitmap(1)->Reset();
	osd->GetBitmap(1)->SetColor(0, bg);
	osd->GetBitmap(1)->SetColor(1, Theme.Color(clrMenuItemCurrentBg));
	osd->GetBitmap(1)->SetColor(2, Theme.Color(clrMenuItemCurrentFg));
	osd->GetBitmap(1)->SetColor(3, Theme.Color(clrMenuItemSelectable));
	//clrMenuItemNonSelectable is not available because 2-bit-area contains only 4 colors
	osd->GetBitmap(2)->Reset();
	}
    osd->DrawRectangle(x0, y3, x4 - 1, y4 - 1, bg);
    osd->DrawRectangle(x4, y3, x6 - 1, y4 - 1, bg);
    }
  osd->DrawRectangle(x6, y3, x8 - 1, y4 - 1, bg);
  osd->DrawRectangle(x0, y4, x6 - 1, y5 - 1, bg);
  osd->DrawRectangle(x6, y4, x8 - 1, y5 - 1, bg);
}

void cSkinElchiDisplayMenu::SetTitle(const char *Title)
{
  const cFont *smallfont = cFont::GetFont(fontSml);
  const cFont *font = cFont::GetFont(fontOsd);

//for epgimages
#ifdef HAVE_IMAGEMAGICK
  if (SkinElchiSetup.epgimages)
    title = Title;
#endif
//

  lineHeight = font->Height();
  osd->DrawText(x2, y0, Title, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg), smallfont, x6 - x2);
  osd->DrawRectangle(x2, y2 - (font->Height() - smallfont->Height()), x6 - 1, y2 - 1, Theme.Color(clrMenuTitleBg));
}

void cSkinElchiDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
  const cFont *smallfont = cFont::GetFont(fontSml);
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  int w = x8 - x0;
  int t0 = x0;
  int t1 = x2;
  int t2 = t0 + w / 4;
  int t3 = t0 + w / 2;
  int t4 = x8 - w / 4;
  int t5 = x6;
  int t6 = x8;

//for epgimages
#ifdef HAVE_IMAGEMAGICK
  if (SkinElchiSetup.epgimages) {
    epgimagebuttons[0] = Red;
    epgimagebuttons[1] = Green;
    epgimagebuttons[2] = Yellow;
    epgimagebuttons[3] = Blue;
    }
#endif
//
  
  tColor redbg = Theme.Color(clrButtonRedBg);
  tColor bluebg = Theme.Color(clrButtonBlueBg);

  osd->DrawRectangle(t0, y6, t1 - 1, y7 - 1, Red ? redbg : Theme.Color(clrBackground));
  osd->DrawRectangle(t0, y7, t1 - 1, y8 - 1, Theme.Color(Transparent));
  osd->DrawEllipse(t0, y7, t1 - 1, y8 - 1, Red ? redbg : Theme.Color(clrBackground), 3);
  osd->DrawText(t1, y6, Red,    Theme.Color(clrButtonRedFg), Red ? redbg : Theme.Color(clrBackground), smallfont, t2 - t1, 0, taCenter);
  osd->DrawRectangle(t1, y8 - (font->Height() - smallfont->Height()), t2 - 1, y8 - 1, Red ? redbg : Theme.Color(clrBackground));
  osd->DrawText(t2, y6, Green,  Theme.Color(clrButtonGreenFg), Green ? Theme.Color(clrButtonGreenBg) : Theme.Color(clrBackground), smallfont, t3 - t2, 0, taCenter);
  osd->DrawRectangle(t2, y8 - (font->Height() - smallfont->Height()), t3 - 1, y8 - 1, Green ? Theme.Color(clrButtonGreenBg) : Theme.Color(clrBackground));
  osd->DrawText(t3, y6, Yellow, Theme.Color(clrButtonYellowFg), Yellow ? Theme.Color(clrButtonYellowBg) : Theme.Color(clrBackground), smallfont, t4 - t3, 0, taCenter);
  osd->DrawRectangle(t3, y8 - (font->Height() - smallfont->Height()), t4 - 1, y8 - 1, Yellow ? Theme.Color(clrButtonYellowBg) : Theme.Color(clrBackground));
  osd->DrawText(t4, y6, Blue,   Theme.Color(clrButtonBlueFg), Blue ? bluebg : Theme.Color(clrBackground), smallfont, t5 - t4, 0, taCenter);
  osd->DrawRectangle(t4, y8 - (font->Height() - smallfont->Height()), t6 - 1, y8 - 1, Blue ? bluebg : Theme.Color(clrBackground));
  osd->DrawRectangle(t5, y6, t6 - 1, y7 - 1, Blue ? bluebg : Theme.Color(clrBackground));
  osd->DrawRectangle(t5, y7, t6 - 1, y8 - 1, Theme.Color(Transparent));
  osd->DrawEllipse(t5, y7, t6 - 1, y8 - 1, Blue ? bluebg : Theme.Color(clrBackground), 4);
}

void cSkinElchiDisplayMenu::SetMessage(eMessageType Type, const char *Text)
{
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  int lineHeighthalf = font->Height() / 2;
  if (Text) {
     osd->DrawRectangle(x0, y5, x2 - 1, y6 - 1, Theme.Color(clrBackground));
     osd->DrawEllipse(x0, y5, x0 + lineHeighthalf - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 7);
     osd->DrawText(x0 + lineHeighthalf, y5, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), font, x6 - x0 - lineHeighthalf, 0, taCenter);
     osd->DrawRectangle(x6, y5, x8 - lineHeighthalf - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
     osd->DrawRectangle(x8 - lineHeighthalf, y5, x8 - 1, y6 - 1, Theme.Color(clrBackground));
     osd->DrawEllipse(x8 - lineHeighthalf, y5, x8 - 1, y6 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 5);
     }
  else {
     osd->DrawRectangle(x0, y5, x6 - 1, y6 - 1, Theme.Color(clrBackground));
     osd->DrawRectangle(x6, y5, x8 - 1, y6 - 1, Theme.Color(clrBackground));
  }
}

void cSkinElchiDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable)
{
  tColor ColorFg, ColorBg;

  if (Current) {
    ColorFg = Theme.Color(clrMenuItemCurrentFg);
    ColorBg = Theme.Color(clrMenuItemCurrentBg);
    currentIndex = Index;
    }
  else {
    ColorFg = Theme.Color(Selectable ? clrMenuItemSelectable : clrMenuItemNonSelectable);
    ColorBg = Theme.Color(clrBackground);
    }
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  int y = menuTop + Index * lineHeight;
  int x;
  int xx;
  if (vidwin && (y >= y3 && y < y4)) {
    x = x3;
    xx = x4;
    }
  else {
    if (area) {
	x = x5;
	xx = x6;
	}
    else {
	x = x7;
	xx = x8;
	}	
    }
  if (currentIndex == Index) {
    osd->DrawEllipse(x0, y, x1 - 1, y + lineHeight - 1, ColorBg, 7);
    osd->DrawRectangle(x1, y, x2, y + lineHeight - 1, ColorBg);
    osd->DrawEllipse(x, y, xx - 1, y + lineHeight - 1, ColorBg, 5);
  }
  
  for (int i = 0; i < MaxTabs; i++) {
    const char *s = GetTabbedText(Text, i);
    if (s) {
        int xt = x1 + Tab(i);
	osd->DrawText(xt, y, s, ColorFg, ColorBg, font, x - xt);
        }
    if (!Tab(i + 1))
        break;
        }
  SetEditableWidth(x - x1 - Tab(1));
}

void cSkinElchiDisplayMenu::SetEvent(const cEvent *Event)
{
  if (!Event)
     return;

//for epgimages
#ifdef HAVE_IMAGEMAGICK
  if (SkinElchiSetup.epgimages) {
    eventID = Event->EventID();
    
    if (!epgimages)
	ShowEpgImage();
    }
#endif
//

  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smallfont = cFont::GetFont(fontSml);
  lineHeight = font->Height();
  int smalllineHeight = smallfont->Height();
  tColor tr = Theme.Color(Transparent);
  tColor bg = Theme.Color(clrBackground);

  int xl = x2;
  int y = y3;
  int x;
#ifdef HAVE_IMAGEMAGICK
  if (vidwin || epgimages) {
    x = x3;
#ifdef HAVE_YAEPG_PATCH
    if (vidwin) {
	epgwin = true;
	imagereadwriteready = true;
	osd->DrawRectangle(x0, y3, x4 - 1, y4 - 1, bg);
	}
#endif
    if (area == 4) {
        osd->GetBitmap(4)->Reset();
        osd->DrawRectangle (x0, y4, x6 - 1, y5 - 1, bg);
        }
    }
  else
#endif
     {
    x = area ? x5 : x7;
    if (area == 1) {
	osd->GetBitmap(1)->Reset();
	osd->DrawRectangle (x0, y2, x6 - 1, y5 - 1, bg);
	}
    }
  x = x - x0;
  cTextScroller ts;
  char t[32];
  snprintf(t, sizeof(t), "%s  %s - %s", *Event->GetDateString(), *Event->GetTimeString(), *Event->GetEndTimeString());
  ts.Set(osd, xl, y, x/* - xl*/, y5 - y, t, font, Theme.Color(clrMenuEventTime), tr);
  if (Event->Vps() && Event->Vps() != Event->StartTime()) {
     y += font->Height();
     char *buffer;
     asprintf(&buffer, " VPS: %s", *Event->GetVpsString());
     osd->DrawText(xl, y, buffer, Theme.Color(clrMenuEventVpsFg), Theme.Color(clrMenuEventVpsBg), smallfont);
     free(buffer);
     }
  y += ts.Height();
  y += font->Height();
  ts.Set(osd, xl, y, x, y5 - y, Event->Title(), font, Theme.Color(clrMenuEventTitle), tr);
  y += ts.Height();
  if (!isempty(Event->ShortText())) {
     ts.Set(osd, xl, y, x/* - xl*/, y5 - y, Event->ShortText(), smallfont, Theme.Color(clrMenuEventShortText), tr);
     y += ts.Height();
     }
  y += font->Height();
  if ((vidwin || epgimages) && (y4 + smalllineHeight) > y)
     y = y4 + smalllineHeight;
  if (!isempty(Event->Description())) {
     textScroller.Set(osd, xl, y, x6 - xl, y5 - y, Event->Description(), smallfont, Theme.Color(clrMenuEventDescription), bg);
     SetScrollbar();
     }
}

void cSkinElchiDisplayMenu::SetRecording(const cRecording *Recording)
{
  SetText(Recording->Summary(), false); //TODO
}

void cSkinElchiDisplayMenu::SetText(const char *Text, bool FixedFont)
{
  const cFont *font = cFont::GetFont((FixedFont && !SkinElchiSetup.usenofixedfont) ? fontFix : fontOsd);
  lineHeight = font->Height();
  textScroller.Set(osd, x2, menuTop, x6 - x2 - 2 * (x2 - x0), menuHeight, Text, font, Theme.Color(clrMenuText), Theme.Color(clrBackground));
  SetScrollbar();
}

void cSkinElchiDisplayMenu::Flush(void)
{
  cString date = DayDateTime();
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  osd->DrawText(x6 - font->Width(date), y0, date, Theme.Color(clrMenuDate), Theme.Color(clrMenuTitleBg), font);

#ifdef HAVE_IMAGEMAGICK
  const cFont *smallfont = cFont::GetFont(fontSml);
    if (epgimages) {
        tColor bg = Theme.Color(clrBackground);
        if (epgimageconvertready) {
	    loading = false;
	    epgimageconvertready = false;
	    if (area == 4) {
	        osd->GetBitmap(3)->Reset();
//		osd->DrawRectangle(x4, y3, x6 - bmpEpgImage.Width() - 1, y4 - 1, bg);
//	    	osd->DrawRectangle(x6 - bmpEpgImage.Width() - 1, y3, x6 - 1, y4 - bmpEpgImage.Height() - 1, bg);
		}
	    else {
	        osd->GetBitmap(0)->Reset();
	        for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++) {
	            osd->GetBitmap(0)->SetColor(i, SkinElchiSetup.ThemeColors[i]);
	            }
	        char *s;
	        asprintf(&s, tr("Picture"));
	        osd->DrawText(x4, y4 - (y4 - y3) / 2 - lineHeight, s, bg, bg, smallfont, x6 - x4, 0, taCenter);
	        asprintf(&s, tr("loading"));
	        osd->DrawText(x4, y4 - (y4 - y3) / 2, s, bg, bg, smallfont, x6 - x4, 0, taCenter);
	        free(s);
	        }
	    osd->DrawRectangle(x4, y3, x6 - bmpEpgImage.Width() - 1, y4 - 1, bg);
	    osd->DrawRectangle(x6 - bmpEpgImage.Width() - 1, y3, x6 - 1, y4 - bmpEpgImage.Height() - 1, bg);
	    osd->DrawBitmap( x6 - bmpEpgImage.Width(), y4 - bmpEpgImage.Height(), bmpEpgImage);
	    }
	else {
	    if (loading) {
	        loading = false;
	        char *s;
	        asprintf(&s, tr("Picture"));
	        tColor me = Theme.Color(clrMenuEventTitle);
	        osd->DrawText(x4, y4 - (y4 - y3) / 2 - lineHeight, s, me, bg, smallfont, x6 - x4, 0, taCenter);
	        asprintf(&s, tr("loading"));
	        osd->DrawText(x4, y4 - (y4 - y3) / 2, s, me, bg, smallfont, x6 - x4, 0, taCenter);
	        free(s);
	        }
	    }
	}
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
    else {
	if (vidwin) {
	    if (bit) {
		bool show = false;
		if (imageconvertready) {
		    show = true;
		    imagereadwriteready = false;
		    imageconvertready = false;
		    }
		if (!SkinElchiSetup.bit) {
		    intervall = 0;
		    osd->DrawRectangle( x2, y3 , x2 + 768/4 - 1, y3 + 576/4 - 1, Theme.Color(clrBackground));
		    bit = false;
		    }
		else {
		    if (!epgwin) {
			osd->GetBitmap(0)->Reset();
			for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++) {
			    osd->GetBitmap(0)->SetColor(i, SkinElchiSetup.ThemeColors[i]);
    			    }
    			if (show) {
	    		    osd->DrawBitmap( x2, y3 , bmpVideo);
#ifdef DEBUG
    	    	    	    isyslog("skinelchi: Image showed");
#endif
	    		    }
			imagereadwriteready = true;
			}
		    }
		}
	    }
	}
#endif
#endif

/*  else
    if (SkinElchiSetup.helptext) SetHelpText(); */ //TODO

  if (SkinElchiSetup.changing) {
    tColor searchcolor = Theme.Color(clrBackground);
    int index = osd->GetBitmap(area)->Index(searchcolor);
    osd->GetBitmap(area)->SetColor(index, Theme.Color(changing));
    osd->Flush();
    osd->GetBitmap(area)->SetColor(index, searchcolor);
    }
  else
    osd->Flush();

}

#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
cBackgroundImage::cBackgroundImage(void) {
  Start();
}

cBackgroundImage::~cBackgroundImage() {
  intervall = 0;
}

void cBackgroundImage::Action(void) {
    imageconvertready = false;
    ShowTime.Set();
    bool format =  SkinElchiSetup.format;
    int quality = SkinElchiSetup.quality;
    intervall = SkinElchiSetup.intervall;
#ifdef DEBUG
    isyslog("skinelchi: thread started (pid=%d)", getpid());
#endif
    char *filename1;
    asprintf(&filename1, "/tmp/snapshot1%s", format ? ".jpg" : ".pnm");
    char *filename2;
    asprintf(&filename2, "/tmp/snapshot2%s", format ? ".jpg" : ".pnm");
    bool picture = true;
    while(intervall) {
	if (imagereadwriteready) {
	    if ((((int)ShowTime.Elapsed())/1000) >= intervall) {
	        imagereadwriteready = false;
		imageconvertready = false;
		ShowTime.Set();
		cDevice::PrimaryDevice()->GrabImage(picture ? filename1 : filename2, format, quality, 768, 576);
		if (image.LoadZoomed(picture ? filename1 : filename2, 768/4, 576/4, X4, Y3)) {
#ifdef DEBUG
		    isyslog("skinelchi: Image converted");
#endif
		    image.Render(bmpVideo, 768/4, 576/4, 256 - MAX_ELCHI_THEME_COLORS, 0);
		    }
	        picture = !picture;
		imageconvertready = true;
		}
	    }
	    else
	        sleep(5);
	}
#ifdef DEBUG
	isyslog("skinelchi: thread end (pid=%d)", getpid());
#endif
    free(filename1);
    free(filename2);
}
#endif
#endif

//for epgimages
#ifdef HAVE_IMAGEMAGICK
cEpgImage::cEpgImage(void) {
  Start();
}

cEpgImage::~cEpgImage() {
}

void cEpgImage::Action(void) {
    ShowTime.Set();
    uint64 epgshowtime = SkinElchiSetup.epgshowtime * 1000;
    char *direventid;
    char *filename;
    const char *filetype = "png";
    int imagenumber = 0;
    int x = 180;
    int y = 144;
    if (SkinElchiSetup.epgimages == 1) {
	x = 120;
	y = 90;
	}
    if (SkinElchiSetup.epgimages == 3) {
	x = 120;
	y = 120;
	}
    asprintf(&direventid, "%s/%d", SkinElchiSetup.epgimagedir, eventID);
    asprintf(&filename, "%s.%s", direventid, filetype);
#ifdef DEBUG
    isyslog("skinelchi: load from epgimages: '%s'", filename);
#endif
    FILE *file = fopen(filename, "r");
    if (file) {
	loading = true;
	fclose(file);
#ifdef DEBUG
	if (!
#endif
	image.LoadZoomed(filename, 0, 0, 0, 0)
#ifdef DEBUG
	)
	isyslog("skinelchi: LoadZoomed failed");
#else
	;
#endif
    	image.Render(bmpEpgImage, x, y, epgimagecolors, 0);
	epgimageconvertready = true;
	while(!loading) {
	    if (ShowTime.Elapsed() >= epgshowtime) {
	        ShowTime.Set();
		imagenumber++;
		if (imagenumber)
		    asprintf(&filename, "%s_%d.%s", direventid, imagenumber, filetype);
		else
		    asprintf(&filename, "%s.%s", direventid, filetype);
		file = fopen(filename, "r");
	        if (file) {
#ifdef DEBUG
		    isyslog("skinelchi: load from epgimages: '%s'", filename);
#endif
		    fclose(file);
#ifdef DEBUG
		    if (!
#endif
		    image.LoadZoomed(filename, 0, 0, 0, 0)
#ifdef DEBUG
		    )
		    isyslog("skinelchi: LoadZoomed failed");
#else
		    ;
#endif
    		    image.Render(bmpEpgImage, x, y, epgimagecolors, 0);
		    epgimageconvertready = true;
		    }
		else {
		    if (imagenumber == 1)
			break;
		    else
			imagenumber = -1;
		    }
		}
	    else
		sleep(2);
	    }
	}
#ifdef DEBUG
    else
	isyslog("skinelchi: File '%s' not found", filename);
#endif
    free (filename);
}
#endif
//
