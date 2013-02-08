#include "DisplayTracks.h"
#include "setup.h"

#include "symbols/audioleft.xpm"
#include "symbols/audioright.xpm"
#include "symbols/audiostereo.xpm"
#include "symbols/arrowdown.xpm"
#include "symbols/arrowup.xpm"

// --- cSkinElchiDisplayTracks ---------------------------------------------
cBitmap cSkinElchiDisplayTracks::bmAudioLeft(audioleft_xpm);
cBitmap cSkinElchiDisplayTracks::bmAudioRight(audioright_xpm);
cBitmap cSkinElchiDisplayTracks::bmAudioStereo(audiostereo_xpm);
cBitmap cSkinElchiDisplayTracks::bmArrowDown(arrowdown_xpm);
cBitmap cSkinElchiDisplayTracks::bmArrowUp(arrowup_xpm);

cSkinElchiDisplayTracks::cSkinElchiDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks)
{
  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smallfont = cFont::GetFont(fontSml);
  lineHeight = font->Height();
  currentIndex = -1;
  index = -1;
  offset = 0;
  numTracks = NumTracks;
  int ItemsWidth = font->Width(Title);
  for (int i = 0; i < NumTracks; i++)
    ItemsWidth = max(ItemsWidth, font->Width(Tracks[i]));
  ItemsWidth += 10;
  x0 = 0;
  x1 = x0 + lineHeight / 2;
  x3 = x0 + Setup.OSDWidth;
  x2 = x3 - lineHeight / 2;
  int d = x3 - x0;
  if (d > (ItemsWidth + lineHeight)) {
     d = (d - ItemsWidth - lineHeight) & ~0x07; // must be multiple of 8
     x2 -= d;
     x3 -= d;
     }
  y0 = 0;
  y1 = y0 + lineHeight;
  y2 = y1 + lineHeight;
  y3 = y2 + NumTracks * lineHeight;
  y4 = y3 + lineHeight;
  y5 = y4 + lineHeight;
  if (y5 > (y0 + Setup.OSDHeight)) { //all right ??? i think, must be change !
    y5 = y0 + Setup.OSDHeight;
    y4 = y5 - lineHeight;
    y3 = y4 - lineHeight;
    }
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + (Setup.ChannelInfoPos ? 0 : Setup.OSDHeight - y5));
  tArea Areas[] = { { x0, y0, x3 - 1, y5 - 1, 4 } };
  osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  osd->DrawRectangle(x0, y0, x1 - 1, y1 - lineHeight / 2 - 1, clrTransparent);
  osd->DrawEllipse(x0, y0, x1 - 1, y1 - lineHeight / 2 - 1, Theme.Color(clrMenuTitleBg), 2);
  osd->DrawRectangle(x0, y0 + lineHeight / 2 , x1 - 1, y1 - 1, Theme.Color(clrMenuTitleBg));
  osd->DrawRectangle(x1, y0, x2 - 1, y1 - 1, Theme.Color(clrMenuTitleBg));
  osd->DrawText(x1, y0, Title, Theme.Color(clrMenuTitleFg), clrTransparent, smallfont, x2 - x1);
  osd->DrawRectangle(x2, y0, x3 - 1, y1 - lineHeight / 2 - 1, clrTransparent);
  osd->DrawEllipse(x2, y0, x3 - 1, y1 - lineHeight / 2  - 1, Theme.Color(clrMenuTitleBg), 1);
  osd->DrawRectangle(x2, y0 + lineHeight / 2 , x3 - 1, y1 - 1, Theme.Color(clrMenuTitleBg));
  osd->DrawRectangle(x0, y4, x1 - 1, y5 - lineHeight / 2 - 1, Theme.Color(clrMenuTitleBg));
  osd->DrawRectangle(x0, y4 + lineHeight / 2, x1 - 1, y5 - 1, clrTransparent);
  osd->DrawEllipse(x0, y4 + lineHeight / 2 , x1 - 1, y5 - 1, Theme.Color(clrMenuTitleBg), 3);
  osd->DrawRectangle(x1, y4, x2 - 1, y5 - 1, Theme.Color(clrMenuTitleBg));
  osd->DrawRectangle(x2, y4, x3 - 1, y5 - lineHeight / 2 - 1, Theme.Color(clrMenuTitleBg));
  osd->DrawRectangle(x2, y4 + lineHeight / 2, x3 - 1, y5 - 1, clrTransparent);
  osd->DrawEllipse(x2, y4 + lineHeight / 2 , x3 - 1, y5 - 1, Theme.Color(clrMenuTitleBg), 4);
  osd->DrawRectangle(x0, y1, x3 - 1, y2 - 1, Theme.Color(clrBackground));
  osd->DrawRectangle(x0, y3, x3 - 1, y4 - 1, Theme.Color(clrBackground));
  int MaxTracks = MaxItems();
  Clear();
  if (MaxTracks >= (numTracks - offset))
    MaxTracks = numTracks - offset;
  for (int i = offset; i < offset + MaxTracks; i++)
    SetItem(Tracks[i], i, false);
  if (numTracks > (offset + MaxTracks)) {
    cBitmap bm(arrowdown_xpm);
    osd->DrawBitmap(x2, y3 + (y4 - y3 - bm.Height()) / 2, bm, Theme.Color(clrMenuItemSelectable), Theme.Color(clrBackground));
    }
}

cSkinElchiDisplayTracks::~cSkinElchiDisplayTracks()
{
  delete osd;
}

int cSkinElchiDisplayTracks::MaxItems(void)
{
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  return (y3 - y2) / lineHeight;
}

void cSkinElchiDisplayTracks::Clear(void)
{
  int MaxTracks = MaxItems();
  if (offset > 0) {
    cBitmap bm(arrowup_xpm);
    osd->DrawBitmap(x2, y1 + (y2 - y1 - bm.Height()) / 2, bm, Theme.Color(clrMenuItemSelectable), Theme.Color(clrBackground));
    }
  else {
    cBitmap bm(arrowup_xpm);
    osd->DrawBitmap(x2, y1 + (y2 - y1 - bm.Height()) / 2, bm, Theme.Color(clrBackground), Theme.Color(clrBackground));
    }
  if (numTracks > (offset + MaxTracks)) {
    cBitmap bm(arrowdown_xpm);
    osd->DrawBitmap(x2, y3 + (y4 - y3 - bm.Height()) / 2, bm, Theme.Color(clrMenuItemSelectable), Theme.Color(clrBackground));
    }
  else {
    cBitmap bm(arrowdown_xpm);
    osd->DrawBitmap(x2, y3 + (y4 - y3 - bm.Height()) / 2, bm, Theme.Color(clrBackground), Theme.Color(clrBackground));
    }
  osd->DrawRectangle(x0, y2, x3 - 1, y3 - 1, Theme.Color(clrBackground));
}

void cSkinElchiDisplayTracks::SetItem(const char *Text, int Index, bool Current)
{
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  int y = y2 + Index * lineHeight - offset * lineHeight;
  tColor ColorFg, ColorBg;
  if (Current) {
     ColorFg = Theme.Color(clrMenuItemCurrentFg);
     ColorBg = Theme.Color(clrMenuItemCurrentBg);
     currentIndex = Index;
     }
  else {
     ColorFg = Theme.Color(clrMenuItemSelectable);
     ColorBg = Theme.Color(clrBackground);
     }
  osd->DrawText(x1, y, Text, ColorFg, ColorBg, font, x2 - x1);
  if (currentIndex == Index) {
    osd->DrawEllipse(x0, y, x0 + lineHeight / 2 - 1, y + lineHeight - 1, ColorBg, 7);
    osd->DrawEllipse(x3 - lineHeight/2, y, x3 - 1, y + lineHeight - 1, ColorBg, 5);
    }
}

void cSkinElchiDisplayTracks::SetTrack(int Index, const char * const *Tracks)
{
  if (index != Index) {
    osd->DrawRectangle(x0, y2, x1 - 1, y3 - 1, Theme.Color(clrBackground));
    osd->DrawRectangle(x2, y2, x3 - 1, y3 - 1, Theme.Color(clrBackground));
    if (currentIndex >= 0) {
	int MaxTracks = MaxItems();
	if ((Index - offset) >= MaxTracks) {
	    offset += MaxTracks - 1;
  	    Clear();
	    if (MaxTracks >= (numTracks - offset))
		MaxTracks = numTracks - offset;
	    for (int i = offset; i < offset + MaxTracks; i++)
    		SetItem(Tracks[i], i, false);
	    }
        else {
	    if (Index < offset) {
	        offset -= MaxTracks - 1;
  		Clear();
		if (MaxTracks >= (numTracks - offset))
		    MaxTracks = numTracks - offset;
		for (int i = offset; i < offset + MaxTracks; i++)
    		    SetItem(Tracks[i], i, false);
    		}
	    else
		SetItem(Tracks[currentIndex], currentIndex, false);
	    }
    }
    else {
        int MaxTracks = MaxItems();
        if ((Index + 1) > MaxTracks) {
	    int temp = Index + 1;
    	    while (temp > MaxTracks) {
		offset += MaxTracks - 1;
		temp   -= MaxTracks - 1;
		}
  	    Clear();
	    if (MaxTracks >= (numTracks - offset))
		MaxTracks = numTracks - offset;
	    for (int i = offset; i < offset + MaxTracks; i++)
    		SetItem(Tracks[i], i, false);
	    }
	}
    SetItem(Tracks[Index], Index, true);
    index = Index;
  }
}

void cSkinElchiDisplayTracks::SetAudioChannel(int AudioChannel)
{
  cBitmap *bm = NULL;
  switch (AudioChannel) {
    case 0: bm = &bmAudioStereo; break;
    case 1: bm = &bmAudioLeft;   break;
    case 2: bm = &bmAudioRight;  break;
    }
  if (bm)
     osd->DrawBitmap(x1, y4 + (y5 - y4 - bm->Height()) / 2, *bm, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg));
  else
     osd->DrawRectangle(x1, y4, x2 - 1, y5 - 1, Theme.Color(clrMenuTitleBg));
}

void cSkinElchiDisplayTracks::Flush(void)
{
  osd->Flush();
}
