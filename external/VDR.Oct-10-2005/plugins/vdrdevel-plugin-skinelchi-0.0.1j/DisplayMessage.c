#include "DisplayMessage.h"
#include "setup.h"

// --- cSkinElchiDisplayMessage --------------------------------------------

cSkinElchiDisplayMessage::cSkinElchiDisplayMessage(void)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int lineHeight = font->Height();
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - lineHeight);
  tArea Areas[] = { { 0, 0, Setup.OSDWidth - 1, lineHeight - 1, 2 } };
  osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
}

cSkinElchiDisplayMessage::~cSkinElchiDisplayMessage()
{
  delete osd;
}

void cSkinElchiDisplayMessage::SetMessage(eMessageType Type, const char *Text)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int lineHeight = font->Height();
  osd->DrawRectangle(0, 0, lineHeight / 2 - 1, osd->Height() - 1, clrTransparent);
  osd->DrawEllipse(0, 0, lineHeight / 2 - 1, osd->Height() - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 7);
  osd->DrawText(lineHeight / 2, 0, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), font, Setup.OSDWidth - lineHeight, 0, taCenter);
  osd->DrawRectangle(osd->Width() - lineHeight / 2 - 1, 0, osd->Width() - lineHeight / 2, osd->Height() - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
  osd->DrawRectangle(osd->Width() - lineHeight / 2, 0, osd->Width() - 1, osd->Height() - 1, clrTransparent);
  osd->DrawEllipse(osd->Width() - lineHeight / 2, 0, osd->Width() - 1, osd->Height() - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 5);
}

void cSkinElchiDisplayMessage::Flush(void)
{
  osd->Flush();
}
