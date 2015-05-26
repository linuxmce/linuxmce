/*
 * metainfo_menu.c: Media file info menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: metainfo_menu.c,v 1.10 2010-03-12 20:26:43 phintuka Exp $
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

struct CallbackData
{
  uint8_t *seen_types;
  char    *text;
  size_t   text_len;
  size_t   text_size;

  CallbackData(void)
  {
#if defined(HAVE_LIBEXTRACTOR) && EXTRACTOR_VERSION >= 0x00060000
    seen_types = (uint8_t*)calloc(1, EXTRACTOR_metatype_get_max());
#else
    seen_types = NULL;
#endif
    text_size  = 4096;
    text       = (char*)malloc(text_size);
    text[0]    = 0;
    text_len   = 0;
  }
  ~CallbackData()
  {
    free(seen_types);
    free(text);
  }
  void Append(const char *key, const char *data)
  {
    if (text_len < text_size - 1) {
      cString s = cString::sprintf("%s: %s\n", key, data);
      strn0cpy(text + text_len, s, text_size - text_len);
      text_len = strlen(text);
    }
  }
};

#if defined(HAVE_LIBEXTRACTOR) && EXTRACTOR_VERSION >= 0x00060000
static int extractor_callback_metainfo(void *priv,
                                       const char *plugin_name,
                                       enum EXTRACTOR_MetaType type,
                                       enum EXTRACTOR_MetaFormat format,
                                       const char *data_mime_type,
                                       const char *data,
                                       size_t data_len)
{
  struct CallbackData *cd = (struct CallbackData *)priv;

  if (format == EXTRACTOR_METAFORMAT_UTF8 &&
      type   != EXTRACTOR_METATYPE_THUMBNAIL &&
      cd   && !cd->seen_types[type] &&
      data && data[0]) {

    const char *key = EXTRACTOR_metatype_to_string(type);
    if (key) {
      cd->Append(key, data);
      cd->seen_types[type] = 1;
    }
  }

  return 0;
}
#endif // defined(HAVE_LIBEXTRACTOR) && EXTRACTOR_VERSION >= 0x00060000


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

  CallbackData data;

#ifdef HAVE_LIBEXTRACTOR
#  if EXTRACTOR_VERSION >= 0x00060000

  EXTRACTOR_PluginList * plugins;

  plugins = EXTRACTOR_plugin_add_defaults(EXTRACTOR_OPTION_DEFAULT_POLICY);
  EXTRACTOR_extract(plugins, m_Filename, NULL, 0, (EXTRACTOR_MetaDataProcessor)&extractor_callback_metainfo, &data);
  EXTRACTOR_plugin_remove_all(plugins); /* unload plugins */

#  else // EXTRACTOR_VERSION >= 0x00060000

  EXTRACTOR_ExtractorList * plugins;
  EXTRACTOR_KeywordList   * md_list;

  plugins = EXTRACTOR_loadDefaultLibraries();
  md_list = EXTRACTOR_getKeywords(plugins, m_Filename);
  md_list = EXTRACTOR_removeEmptyKeywords (md_list);
  md_list = EXTRACTOR_removeDuplicateKeywords(md_list, 0);
  md_list = EXTRACTOR_removeKeywordsOfType(md_list, EXTRACTOR_THUMBNAILS);

  while(md_list) {
    const char *key = EXTRACTOR_getKeywordTypeAsString(md_list->keywordType);
    if (key)
      data.Append(key, md_list->keyword);
    md_list = md_list->next;
  }

  EXTRACTOR_freeKeywords(md_list);
  EXTRACTOR_removeAll(plugins); /* unload plugins */

#  endif // EXTRACTOR_VERSION >= 0x00060000
#else // HAVE_LIBEXTRACTOR

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
    data.text_len = fread(data.text, 1, data.text_size - 1, p);
    if (data.text_len > 0) {
      data.text[data.text_len] = 0;
      strreplace(data.text, ',', '\n');
    }
  }

#endif // HAVE_LIBEXTRACTOR

  DisplayMenu()->SetText(data.text, false);
  data.text = NULL;

  cStatus::MsgOsdTextItem(cString::sprintf("%s\n%s", tr("Metainfo"), *m_Filename));
}

eOSState cMetainfoMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  return state;
}
