#include "i18n.h"
#include "DisplayVolume.h"
#include "setup.h"

#include "symbols/mute.xpm"
#include "symbols/volume.xpm"

// --- cSkinElchiDisplayVolume ---------------------------------------------
cBitmap cSkinElchiDisplayVolume::bmMute(mute_xpm);
cBitmap cSkinElchiDisplayVolume::bmVolume(volume_xpm);

cSkinElchiDisplayVolume::cSkinElchiDisplayVolume(void)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int lineHeight = font->Height();
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - 2 * lineHeight);
  tArea Areas[] = { { 0, 0, Setup.OSDWidth - 1, 2 * lineHeight - 1, 4 } };
  osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  mute = -1;
  current = -1;
}

cSkinElchiDisplayVolume::~cSkinElchiDisplayVolume()
{
  delete osd;
}

void cSkinElchiDisplayVolume::SetVolume(int Current, int Total, bool Mute)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int lineHeight = font->Height();
  if (Mute) {
     if (mute != int(Mute)){
	 char *Prompt;
         asprintf(&Prompt,"%03d", Current);
         int l = font->Width(Prompt) + lineHeight / 2;
         osd->DrawRectangle(0, lineHeight, osd->Width() - 1, osd->Height() - 1, clrTransparent);
         osd->DrawText(lineHeight / 2 + 3, lineHeight + 1, Prompt, Theme.Color(clrBackground), clrTransparent, font);
         osd->DrawText(lineHeight / 2, lineHeight - 2, Prompt, Theme.Color(clrVolumePrompt), clrTransparent, font);
         free(Prompt);
         osd->DrawBitmap(l + 3, lineHeight + (lineHeight - bmMute.Height()) / 2, bmMute, Theme.Color(clrVolumeSymbolMuteFg), Theme.Color(clrVolumeSymbolMuteBg));
	 mute = (int)Mute;
	 }
     }
  else {
     if (mute != int(Mute)){
         const char *Prompt = tr("Volume ");
         int l = font->Width(Prompt) + lineHeight / 2;
         osd->DrawRectangle(0, lineHeight, osd->Width() - 1, osd->Height() - 1, clrTransparent);
         osd->DrawEllipse(0, lineHeight, lineHeight / 2 - 1, osd->Height() - 1, Theme.Color(clrVolumeBarLower), 7);
         osd->DrawRectangle(lineHeight / 2, lineHeight, osd->Width() - lineHeight / 2 - 1, osd->Height() - 1, Theme.Color(clrVolumeBarLower));
         osd->DrawEllipse(osd->Width() - lineHeight / 2, lineHeight, osd->Width() - 1, osd->Height() - 1, Theme.Color(clrVolumeBarLower), 5);
         osd->DrawRectangle(0, 0, osd->Width() - 1, lineHeight - 1, clrTransparent);
         osd->DrawText(lineHeight / 2 + 3, 1, Prompt, Theme.Color(clrBackground), clrTransparent, font);
         osd->DrawText(lineHeight / 2, -2, Prompt, Theme.Color(clrVolumePrompt), clrTransparent, font);
         osd->DrawBitmap(l + 3, (lineHeight - bmVolume.Height()) / 2, bmVolume, Theme.Color(clrVolumeSymbolVolumeFg), clrTransparent);
	 mute = (int)Mute;
	 }
     if (current != Current){
         int p = (osd->Width() - lineHeight) * Current / Total;
         osd->DrawRectangle(lineHeight / 2, lineHeight + 4, lineHeight / 2 + p - 1, osd->Height() - 5, Theme.Color(clrVolumeBarUpper));
	 osd->DrawRectangle(lineHeight / 2 + p, lineHeight + 4, osd->Width() - lineHeight / 2 - 1, osd->Height() - 5, Theme.Color(clrVolumeBarLower));
         current = Current;
	 }
     }
}

void cSkinElchiDisplayVolume::Flush(void)
{
  osd->Flush();
}
