/*
 * metainfo_menu.c: Media file info menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: metainfo_menu.c,v 1.1 2008/05/07 13:27:15 phintuka Exp $
 *
 */

#ifdef HAVE_EXTRACTOR_H
# include <extractor.h>
#endif

#include <vdr/status.h>

#include "../i18n.h"
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
#ifdef HAVE_EXTRACTOR_H
  EXTRACTOR_ExtractorList * plugins;
  EXTRACTOR_KeywordList   * md_list;
  plugins = EXTRACTOR_loadDefaultLibraries();
  md_list = EXTRACTOR_getKeywords(plugins, m_Filename);
  md_list = EXTRACTOR_removeEmptyKeywords (md_list);
  md_list = EXTRACTOR_removeDuplicateKeywords(md_list, 0);

  const char *key;
  char * buf;
  strcpy(metadata, "");
  while(md_list) {
    if((key=EXTRACTOR_getKeywordTypeAsString(md_list->keywordType))) {
      buf = strdup(md_list->keyword);
      sprintf(metadata, "%s%s: %s\n", metadata, key, buf);
      free(buf);
     }
    md_list=md_list->next;
   }
  EXTRACTOR_freeKeywords(md_list);
  EXTRACTOR_removeAll(plugins); /* unload plugins */
#else
  cString cmd = "";
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
