/*
 * metainfo_menu.c: Media file info menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: metainfo_menu.c,v 1.8 2009/08/18 10:35:45 phintuka Exp $
 *
 */

#include "../features.h"

#ifdef HAVE_LIBEXTRACTOR
# include <extractor.h>
#endif

#include <vdr/status.h>
#include <vdr/i18n.h>

#include "../config.h"

#include "metainfo_menu.h"


//
// cMetainfoMenu
//

cMetainfoMenu::cMetainfoMenu(cString Filename) :
     cOsdMenu(Filename),
     m_Filename(Filename)
{
  const char *Title = strrchr(Filename, '/');
  if(Title && *(Title+1))
    SetTitle(Title+1);
}

cMetainfoMenu::~cMetainfoMenu()
{
}

void cMetainfoMenu::Display(void)
{
  cOsdMenu::Display();

  char metadata[4096];
  metadata[0] = 0;

#ifdef HAVE_LIBEXTRACTOR
  EXTRACTOR_ExtractorList * plugins;
  EXTRACTOR_KeywordList   * md_list;
  plugins = EXTRACTOR_loadDefaultLibraries();
  md_list = EXTRACTOR_getKeywords(plugins, m_Filename);
  md_list = EXTRACTOR_removeEmptyKeywords (md_list);
  md_list = EXTRACTOR_removeDuplicateKeywords(md_list, 0);
  md_list = EXTRACTOR_removeKeywordsOfType(md_list, EXTRACTOR_THUMBNAILS);

  uint pos = 0;
  int n;
  while(md_list) {
    const char *key = EXTRACTOR_getKeywordTypeAsString(md_list->keywordType);
    if(key && pos < sizeof(metadata))
      if(0 < (n = snprintf(metadata+pos, sizeof(metadata)-pos, "%s: %s\n", key, md_list->keyword)))
	pos += n;
    md_list = md_list->next;
  }
  metadata[sizeof(metadata)-1] = 0;

  EXTRACTOR_freeKeywords(md_list);
  EXTRACTOR_removeAll(plugins); /* unload plugins */
#else
  cString cmd;
  if(xc.IsPlaylistFile(m_Filename))
    cmd = cString::sprintf("file -b '%s'; cat '%s'", *m_Filename, *m_Filename);
  else if(xc.IsAudioFile(m_Filename))
    cmd = cString::sprintf("mp3info -x '%s' ; file -b '%s'", *m_Filename, *m_Filename);
  else if(xc.IsVideoFile(m_Filename))
    cmd = cString::sprintf("file -b '%s'; midentify '%s'", *m_Filename, *m_Filename);
  else if(xc.IsImageFile(m_Filename))
    cmd = cString::sprintf("file -b '%s'; identify '%s'", *m_Filename, *m_Filename);
  else
    cmd = cString::sprintf("file -b '%s'", *m_Filename);

  cPipe p;
  if(p.Open(*cmd, "r")) {
    int n = fread(metadata, 1, sizeof(metadata)-1, p);
    if(n>0) {
      metadata[n] = 0;
      strreplace(metadata, ',', '\n');
    }
  }
#endif
  DisplayMenu()->SetText(metadata, false);
  cStatus::MsgOsdTextItem(cString::sprintf("%s\n%s", tr("Metainfo"), *m_Filename));
}

eOSState cMetainfoMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  return state;
}
