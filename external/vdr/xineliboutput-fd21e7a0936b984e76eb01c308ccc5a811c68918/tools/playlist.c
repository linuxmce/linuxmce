/*
 * playlist.c: Media player playlist
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: playlist.c,v 1.34 2012-01-30 07:47:36 phintuka Exp $
 *
 */

#include "../features.h"

#include <stdlib.h> 

#ifdef HAVE_LIBEXTRACTOR
# include <extractor.h>
  // libextractor 0.5.20 (2008-03-20) adds support for track numbers
# if EXTRACTOR_VERSION < 0x00052000
#  warning libextractor version too old (0.5.20 required for track numbers)
#  undef HAVE_LIBEXTRACTOR
# endif
#endif

#include <vdr/config.h> 
#include <vdr/tools.h> 
#include <vdr/thread.h> 

#include "../config.h"

#include "playlist.h"

#include "../logdefs.h"


#ifndef PLAYLIST_CACHE
#  define PLAYLIST_CACHE ".xineliboutput-playlist.pls"
#endif

#define MAX_PLAYLIST_FILES 1024

static void strip_extension(cString& fname)
{
  const char *ext = strrchr(fname, '.');
  if (ext)
    fname.Truncate(ext - fname);
}

//
// cPlaylistItem
//

cPlaylistItem::cPlaylistItem(const char *filename)
{
  const char *pt;

  Filename = filename;
  Position = -1;

  if(NULL != (pt = strrchr(filename, '/')))
    Title = pt + 1;
  else
    Title = filename;

  strip_extension(Title);
}

cPlaylistItem::cPlaylistItem(const char *filename, 
			     const char *path,
			     const char *title, 
			     int position)
{
  if(path[strlen(path)-1] != '/')
    Filename = cString::sprintf("%s/%s", path, filename);
  else
    Filename = cString::sprintf("%s%s", path, filename);
  Position = position;
  Title = title ?: filename;

  if (!title)
    strip_extension(Title);
}

int cPlaylistItem::Compare(const cListObject &ListObject) const 
{
  ///< Must return 0 if this object is equal to ListObject, a positive value
  ///< if it is "greater", and a negative value if it is "smaller".

  const cPlaylistItem *o = (cPlaylistItem *)&ListObject;

  // Use Position (if defined in playlist file)
  // compare as unsigned --> -1 goes to last position
  if(Position != o->Position) 
    return ((unsigned int)Position) > ((unsigned int)o->Position) ? 1 : -1;

  // same position (or no positions definend) -> alphabetical order
#if 0
  return strcmp(Title, o->Title);
#else
  // use filename, because:
  //  - implicit playlist has no track names available when sorting 
  //    (track names are read during playback), so track name is 
  //    just file name without path.
  //    using full path allows sorting of each album and tracks inside albums...
  //  - "normal" playlist is ordered using Position,
  //     so track names are never compared anyway ...
  return strcmp(Filename, o->Filename);
#endif
}


//
// cID3Scanner
//

#ifndef HAVE_LIBEXTRACTOR
static const char *shell_escape(char *buf, int buflen, const cString& src, char ch)
{
  const char *pt = *src;
  int n = 0;

  if(pt) {
    while(*pt && n < buflen-2) {
      if(*pt == ch || *pt == '\\' /*|| *pt == '\"' || *pt == '\''*/) {
        buf[n++] = '\\';
      }
      buf[n++] = *pt++;
    }
    buf[n] = 0;
    return buf;
  }
  return "";
}
#endif

#if defined(HAVE_LIBEXTRACTOR) && EXTRACTOR_VERSION >= 0x00060000
static int extractor_callback_id3(void *priv,
                                  const char *plugin_name,
                                  enum EXTRACTOR_MetaType type,
                                  enum EXTRACTOR_MetaFormat format,
                                  const char *data_mime_type,
                                  const char *data,
                                  size_t data_len)
{
  if (format == EXTRACTOR_METAFORMAT_UTF8) {
    cPlaylistItem *Item = (cPlaylistItem *)priv;
    switch (type) {
      case EXTRACTOR_METATYPE_TITLE:
        Item->Title = data;
        break;
      case EXTRACTOR_METATYPE_ARTIST:
        Item->Artist = data;
        break;
      case EXTRACTOR_METATYPE_ALBUM:
        Item->Album = data;
        break;
      case EXTRACTOR_METATYPE_TRACK_NUMBER:
        Item->Tracknumber = strlen(data) == 1 ? *cString::sprintf("0%s", data) : data;
        break;
      default:
        break;
    }
  }
  return 0;
}
#endif // defined(HAVE_LIBEXTRACTOR) && EXTRACTOR_VERSION >= 0x00060000


class cID3Scanner : public cThread 
{
 public:
  cPlaylist& m_List;
  cID3Scanner(cPlaylist& List) : cThread("Metadata scanner"), m_List(List), m_Done(false) {};
  
  void CancelScanner(void) { Cancel(3); }

 private:
  bool m_Done;

  virtual void Action(void)
  {
    cPlaylistItem *Item = NULL;
    unsigned int Version = 0;
    const int priority = 10;
    errno = 0;
    if((nice(priority) == -1) && errno)
      LOGDBG("ID3Scanner: Can't nice to value: %d", priority);

    LOGDBG("ID3Scanner Started");
    while(Running()) {

      cMutexLock ml(&m_List.m_Lock);

      if(Version < m_List.m_Version) {
	// restart after sort, add, del
	Item = NULL;
	Version = m_List.m_Version;
      }

      if(!(Item = m_List.Next(Item)))
        break;

      if(xc.IsAudioFile(Item->Filename)) {
        LOGDBG("Scanning metainfo for file %s", *Item->Filename);
#ifdef HAVE_LIBEXTRACTOR
#  if EXTRACTOR_VERSION >= 0x00060000
	EXTRACTOR_PluginList * plugins;
	plugins = EXTRACTOR_plugin_add_defaults(EXTRACTOR_OPTION_DEFAULT_POLICY);
	EXTRACTOR_extract(plugins, *Item->Filename, NULL, 0, (EXTRACTOR_MetaDataProcessor)&extractor_callback_id3, Item);
	EXTRACTOR_plugin_remove_all(plugins); /* unload plugins */
#  else // EXTRACTOR_VERSION >= 0x00060000
        EXTRACTOR_ExtractorList * plugins;
        EXTRACTOR_KeywordList   * md_list;
        plugins = EXTRACTOR_loadDefaultLibraries();
        md_list = EXTRACTOR_getKeywords(plugins, *Item->Filename);
        const char *key;
        while(md_list) {
          if ((key=EXTRACTOR_getKeywordTypeAsString(md_list->keywordType))) {
            if (!strcasecmp(key,"title"))
              Item->Title = md_list->keyword;
            else if (!strcasecmp(key,"artist"))
              Item->Artist = md_list->keyword;
            else if (!strcasecmp(key,"album"))
              Item->Album = md_list->keyword;
            else if (!strcasecmp(key,"track number"))
             Item->Tracknumber = cString::sprintf("%s%s", strlen(md_list->keyword) == 1 ? "0" : "", md_list->keyword);
            md_list=md_list->next; 
           }
         }
        EXTRACTOR_freeKeywords(md_list);
        EXTRACTOR_removeAll(plugins); /* unload plugins */
#  endif // EXTRACTOR_VERSION >= 0x00060000
#else // HAVE_LIBEXTRACTOR
        char buf[4096];
        cString Cmd = "";
        if(!strcasecmp((Item->Filename) + strlen(Item->Filename) - 5, ".flac"))
            Cmd = cString::sprintf("metaflac "
                                   " --show-tag=TITLE "
                                   " --show-tag=ALBUM "
                                   " --show-tag=ARTIST "
                                   " --show-tag=TRACKNUMBER "
                                   " \"%s\"",
                                   shell_escape(buf, sizeof(buf)-1, Item->Filename, '\"'));
        else
            Cmd = cString::sprintf("mp3info -p \""
                                   "ARTIST=%%a\\r\\n"
                                   "ALBUM=%%l\\r\\n"
                                   "TITLE=%%t\\r\\n"
                                   "TRACKNUMBER=%%n\\r\\n\""
                                   " \"%s\"",
                                   shell_escape(buf, sizeof(buf)-1, Item->Filename, '\"'));

        cPipe p;
        if(p.Open(*Cmd, "r")) {
          cReadLine r;
          char *pt;
          while(NULL != (pt = r.Read(p))) {
            if(!strncasecmp(pt, "ARTIST=", 7) && strlen(pt) > 8)
              Item->Artist = (pt+7);
            else if(!strncasecmp(pt, "ALBUM=", 6) && strlen(pt) > 7)
              Item->Album = (pt+6);
            else if(!strncasecmp(pt, "TITLE=", 6) && strlen(pt) > 7)
              Item->Title = (pt+6);
            else if(!strncasecmp(pt, "TRACKNUMBER=", 12) && strlen(pt) > 12)
              Item->Tracknumber = cString::sprintf("%s%s", strlen(pt) == 13 ? "0" : "", (pt+12));
          }
        }
#endif // HAVE_LIBEXTRACTOR
      }
    }
    LOGDBG("ID3Scanner Done.");

    m_List.PlaylistChanged(Item);
    m_Done = true;
  }
};

//
// cPlaylistReader
//

class cPlaylistReader 
{
 private:
  cPlaylist& m_Playlist;

 protected:
  cString    m_Title;
  int        m_Position;

  cPlaylistItem *Prev(void) { return m_Playlist.Last(); }

 public:
  cPlaylistReader(cPlaylist& Playlist) : m_Playlist(Playlist) {}
  virtual ~cPlaylistReader() {}

  virtual char *Parse(char *line) = 0;

  void ResetCache(void) { m_Title = NULL; m_Position = -1; }
  const char *Title(void) { return m_Title; }
  int Position(void) { return m_Position; }
};

class cM3uReader : public cPlaylistReader
{
 public:
  cM3uReader(cPlaylist& Playlist) : cPlaylistReader(Playlist), m_Next(1) {}

 protected:
  int m_Next;
  virtual char *Parse(char *line) {
    if(!*line)
      return NULL;
    if(*line == '#') {
      if(!strncmp(line, "#EXTINF:", 8)) {
	int len = -1;
	sscanf(line+8,"%d", &len);
	while(*line && *line != ',')
	  line++;
	m_Title = *line ? (line+1) : NULL;
	m_Position = m_Next++;
      }
      return NULL;
    }
    return *line ? line : NULL;
  }
};

class cPlsReader : public cPlaylistReader
{
 public:
  cPlsReader(cPlaylist& Playlist) : cPlaylistReader(Playlist), m_Current(0) {}

 protected:
  int  m_Current;
  virtual char *Parse(char *line) {
    char *t = strchr(line, '=');  
    if(t) {
      int n;
      if(!strncasecmp(line, "file", 4) && 
	 1 == sscanf(line + 4, "%d=", &n)) {
	m_Current = n;
	m_Position = n;
	if(*(t+1))
	  return t+1;
      }
      else if(!strncasecmp(line, "title", 5) && 
	      1 == sscanf(line + 5, "%d=", &n)) {
	if(*(t+1)) {
	  if(n == m_Current)
	    Prev()->Title = t+1;
	  else
	    m_Title = t+1;
	}
      }
      //else if(!strncasecmp(line, "length", 6) && 
      //        1 == sscanf(line + 4, "%d=", &n)) {
      //}
    }
    return NULL;
  }
};

class cAsxReader : public cPlaylistReader
{
 public:
  cAsxReader(cPlaylist& Playlist) : cPlaylistReader(Playlist) {}

 protected:
  virtual char *Parse(char *line) {
    char *pt = strstr(line, "<REF HREF");
    if(!pt)
      pt = strstr(line, "<ref href");
    if(!pt)
      pt = strstr(line, "<ENTRY HREF");
    if(!pt)
      pt = strstr(line, "<entry href");
    if(pt) {
      pt = strchr(pt, '=');
      if(pt) {
	pt = strchr(pt, '\"');
	if(pt) {
	  pt++;
	  if(strchr(pt, '\"'))
	    *strchr(pt, '\"') = 0;
	  return pt;
	}
      }
    }

    pt = strstr(line, "<TITLE>");
    if(!pt)
      pt = strstr(line, "<title>");
    if(pt) {
      pt += 7;
      if(strstr(line, "</"))
	*strstr(line, "</") = 0;
      m_Title = pt;
    }

    if(*m_Title) {
      pt = strstr(line, "<ENTRY>");
      if(!pt)
	pt = strstr(line, "<entry>");
      if(pt) {
	if(*m_Title && Prev()) {
	  Prev()->Title = m_Title;
	  m_Title = NULL;
	}
      }
    }
    return NULL;
  }
};


//
// cPlaylist
//

cPlaylist::cPlaylist()
{
  m_Origin  = eImplicit;
  m_Menu    = NULL;
  m_Scanner = NULL;
  m_Current = NULL;
  m_Version = 1;
}

cPlaylist::~cPlaylist()
{
  if(m_Scanner) {
    m_Scanner->CancelScanner();
    delete m_Scanner;
  }

  if(m_Origin == eImplicit)
    StoreCache();
}

void cPlaylist::Listen(cPlaylistChangeNotify *Menu)
{
  cMutexLock ml(&m_Lock);
  m_Menu = Menu;
}

void cPlaylist::PlaylistChanged(const cPlaylistItem *Item)
{
  cMutexLock ml(&m_Lock);
  /*if(m_Origin == eImplicit)*/
    Sort();
  if(m_Menu)
    m_Menu->PlaylistChanged(Item);
}

void cPlaylist::Sort(void) 
{
  cMutexLock ml(&m_Lock);
  cListBase::Sort(); 
  m_Version++;
}

int cPlaylist::Count(void) const 
{
  return cListBase::Count(); 
}

cPlaylistItem *cPlaylist::Next(const cPlaylistItem *i)
{
  cMutexLock ml(&m_Lock);
  return i ? cList<cPlaylistItem>::Next(i) : cList<cPlaylistItem>::First(); 
}

cPlaylistItem *cPlaylist::Current(void)
{
  cMutexLock ml(&m_Lock);
  return m_Current ?: First(); 
}

void cPlaylist::Del(cPlaylistItem *it)
{
  cMutexLock ml(&m_Lock);

  if(!it || Count() < 2)
    return;

  if(m_Current == it)
    m_Current = cList<cPlaylistItem>::Next(Current()) ?:
		cList<cPlaylistItem>::Prev(Current());

  cListBase::Del(it);
  m_Version++;
}

void cPlaylist::Move(int From, int To)
{
  cMutexLock ml(&m_Lock);

  if (Count() < 3)
    return;

  cListBase::Move(From, To);
  m_Version++;
}

void cPlaylist::SetCurrent(cPlaylistItem *current) 
{
  cMutexLock ml(&m_Lock);
  m_Current = current; 
}

cPlaylistItem *cPlaylist::Next(void) 
{
  cMutexLock ml(&m_Lock);
  if(Current())
    return m_Current = (cList<cPlaylistItem>::Next(Current()) ?: First());
  return NULL;
}

cPlaylistItem *cPlaylist::Prev(void) 
{
  cMutexLock ml(&m_Lock);
  if(Current())
    return m_Current = (cList<cPlaylistItem>::Prev(Current()) ?: Last());
  return NULL;
}

cPlaylistItem *cPlaylist::Seek(int Rel)
{
  cMutexLock ml(&m_Lock);
  if (!Current())
    return NULL;

  if (Rel > 0) {
    while (Rel--)
      m_Current = (cList<cPlaylistItem>::Next(Current()) ?: Last());

  } else if (Rel < 0) {
    while (Rel++)
      m_Current = (cList<cPlaylistItem>::Prev(Current()) ?: First());
  }

  return Current();
}

bool cPlaylist::StoreCache(void) 
{
  if(!xc.cache_implicit_playlists ||
     m_Origin != eImplicit ||
     !*m_Folder)
    return false;

  cString Name = cString::sprintf("%s%s", *m_Folder, PLAYLIST_CACHE);
  int len = strlen(m_Folder), entries = 0;
  FILE *f = NULL;

  for(cPlaylistItem *i = First(); i; i=Next(i)) {
    // store only items in "current" root folder
    if(!strncmp(i->Filename, m_Folder, len)) {
      if(/**i->Title ||*/ *i->Artist || *i->Album) {
	cString Filename = ((*i->Filename) + len); // relative
	if(entries < 1) {
	  f = fopen(Name, "w");
	  if(!f) {
	    LOGERR("creation of metadata cache %s%s failed", 
		   *m_Folder, PLAYLIST_CACHE);
	    return false;
	  }
	  fprintf(f, "[playlist]\r\n");
	}
	entries++;
	fprintf(f, "File%d=%s\r\n", entries, *Filename);
	if(*i->Title && (*i->Title)[0])
	  fprintf(f, "Title%d=%s\r\n", entries, *i->Title);
        if(*i->Tracknumber && (*i->Tracknumber)[0])
          fprintf(f, "Tracknumber%d=%s\r\n", entries, *i->Tracknumber);
	if(*i->Artist && (*i->Artist)[0])
	  fprintf(f, "Artist%d=%s\r\n", entries, *i->Artist);
	if(*i->Album && (*i->Album)[0])
	  fprintf(f, "Album%d=%s\r\n", entries, *i->Album);
      }
    }
  }
  
  if(entries > 0) {
    fprintf(f, "NumberOfEntries=%d\r\nVersion=2\r\n", entries);
    fclose(f);
    return true;
  }

  return false; 
}

static const char *strchrnext(const char *s, char c)
{
  return (s = strchr(s, c)) ? ((*(s+1))?(s+1):NULL) : NULL;
}

bool cPlaylist::ReadCache(void) 
{
  if(xc.cache_implicit_playlists && m_Origin == eImplicit && *m_Folder) {

    cString Name = cString::sprintf("%s%s", *m_Folder, PLAYLIST_CACHE);
    FILE *f = fopen(Name, "r");
    if(f) {
      int len = strlen(m_Folder);
      cPlaylistItem *it = NULL;
      cReadLine r;
      const char *pt;
      while(NULL != (pt = r.Read(f))) {
	if(!strncmp(pt, "File", 4)) {
	  it = NULL;
	  const char *Filename = strchrnext(pt+4, '=');
	  if(Filename && *Filename) {
	    for(cPlaylistItem *i = First(); i; i=Next(i)) {
	      if(!strncmp(i->Filename, m_Folder, len)) {
		if(!strcmp(*i->Filename + len, Filename)) {
		  it = i;
		  break;
		}
	      }
	    }
	  }
	} else if(it && !strncmp(pt, "Title", 5)) {
	  it->Title = strchrnext(pt, '=');
        } else if(it && !strncmp(pt, "Tracknumber", 11)) {
          it->Tracknumber = strchrnext(pt, '=');
	} else if(it && !strncmp(pt, "Artist", 6)) {
	  it->Artist = strchrnext(pt, '=');
	} else if(it && !strncmp(pt, "Album", 5)) {
	  it->Album = strchrnext(pt, '=');
	} else {
	  /*it = NULL;*/
	}
      }
      fclose(f);
      return true;
    }
  }

  return false; 
}

#if 0
static FILE *open_http(const char *PlaylistFile)
{
  char file[1024] = "", host[128] = "", pt;
  int  fd, port = 80;

  strn0cpy(host, PlaylistFile+strlen("http://"), sizeof(host)-1);
  pt = strchr(host, '/');
  if(pt) {
    strn0cpy(file, pt, sizeof(file)-1);
    *pt = 0;
  }
  pt = strchr(host, ':');
  if(pt) {
    *pt++ = 0;
    port = atoi(pt);
  }

  fd = tcp_connect(host, port);
  if(fd < 0) {
    LOGERR("TCP connect failed");
    return NULL;
  }

  int len = asprintf(&pt, 
		     "GET %s HTTP/1.1"  "\r\n"
		     "Host: %s"         "\r\n"
		     "\r\n",
		     file, host);
  if(len != write(fd, pt, len)) {
    LOGERR("HTTP request write failed");
    free(pt);
    close(fd);
    return NULL;
  }
  free(pt);

  int state = 0;
  FILE *f = fdopen(fd, "r");
  cReadLine r;
  while(state >= 0 && NULL != (pt = r.Read(f))) {
    switch(state) {
      case 0: if(!strncmp(pt, "HTTP/1", 6) || !strstr(pt, " 200 ")) {
	        LOGERR("HTTP error: %s", pt);
		fclose(f);
		return NULL;
              }
              state = 1;
              break;
      case 1: if(strcmp(pt, "\r\n")) 
	        break;
              return f;
      default: break;
    }
  }

  fclose(f);
  return NULL;
}
#endif

int cPlaylist::ScanFolder(const char *FolderName, 
			  bool Recursive,
			  bool (*Filter)(const char *))
{
  cMutexLock ml(&m_Lock);  
  static int depth = 0;

  DIR *d = opendir(FolderName);

  if (d) {
    LOGDBG("ScanFolder(%s)", FolderName);
    struct dirent *e;
    int n = 0, warn = -1;
    while ((e = readdir(d)) != NULL) {
      cString Buffer = cString::sprintf("%s%s", FolderName, e->d_name);
      struct stat st;
      if (stat(Buffer, &st) == 0) {
	if(S_ISDIR(st.st_mode)) {
	  if (Recursive && !S_ISLNK(st.st_mode)) { /* don't want to loop ... */
	    if(depth > 4) {
	      LOGMSG("ScanFolder: Too deep directory tree");
	    } else if(e->d_name[0]=='.') {
	    } else {
	      if(n<MAX_PLAYLIST_FILES) {
		depth++; /* limit depth */
		Buffer = cString::sprintf("%s/", *Buffer);
		n += ScanFolder(Buffer, Recursive, Filter);
		depth--;
	      } else {
		if(!++warn)
		  LOGMSG("ScanFolder: Found over %d matching files, list truncated!", n);
		break;
	      }
	    }
	  }
	} else /* == if(!S_ISDIR(st.st_mode))*/ {
	  // check symlink destination
	  if (S_ISLNK(st.st_mode)) {
	    Buffer = ReadLink(Buffer);
	    if (!*Buffer)
	      continue;
	    if (stat(Buffer, &st) != 0)
	      continue;
	  }
	  if((*Filter)(Buffer)) {
	    /* TODO: Should ScanDir add contents of playlist files ... ? */
	    if(Filter == &config_t::IsPlaylistFile || !xc.IsPlaylistFile(Buffer)) {
	      n++;
	      if(n<MAX_PLAYLIST_FILES) {
		Add(new cPlaylistItem(e->d_name, FolderName));
		//LOGDBG("ScanFolder: %s", e->d_name);
	      } else {
		if(!++warn)
		  LOGMSG("ScanFolder: Found over %d matching files, list truncated!", n);
		break;
	      }
	    }
	  }
	}
      }
    }
    LOGDBG("ScanFolder: Found %d matching files from %s", n, FolderName);
    closedir(d);

    return n;
  }

  LOGERR("ScanFolder: Error opening %s", FolderName);
  return 0;
}

void cPlaylist::StartScanner(void)
{
  cMutexLock ml(&m_Lock);

  if(m_Scanner) {
    if(m_Scanner->Active())
      return;
    delete m_Scanner;
    m_Scanner = NULL;
  }

  /* check if cache is already up-to-date */
  cString CacheName = cString::sprintf("%s%s", *m_Folder, PLAYLIST_CACHE);
  struct stat stf, stc;
  if(!stat(m_Folder, &stf)) {
    if(!stat(CacheName, &stc)) {
      //LOGDBG("ID3 Cache modified %d, folder modified %d, diff %d",
      //       (unsigned int)stc.st_mtime, (unsigned int)stf.st_mtime, 
      //       (unsigned int)(stc.st_mtime - stf.st_mtime));
      if(stc.st_mtime >= stf.st_mtime) {
	if(ReadCache()) {
	  LOGDBG("cPlaylist: using up-to-date ID3 cache");
	  //LOGMSG("  Cache read OK.");
	  return;
	}
	LOGMSG("cPlaylist: ID3 cache read FAILED");
      } else {
	LOGDBG("cPlaylist: ID3 cache not up-to-date, using old cache and scanning for changes");
	ReadCache();
      }
    }
    //else LOGERR("cPlaylist: stat(%s) failed");
  }
  //else LOGERR("cPlaylist: stat(%s) failed");

  if(xc.enable_id3_scanner) {
    m_Scanner = new cID3Scanner(*this);
    m_Scanner->Start();
  }
}

int cPlaylist::ReadPlaylist(const char *file)
{
  static int depth = 0; /* limit recursion */
  cPipe p;
  cPlaylistReader *parser = NULL;
  FILE *f;

  if(strncmp(file, "http:", 5) && strncmp(file, "https:", 6)) {
    f = fopen(file, "r");
  } else {
    // fetch playlist from server using curl
    LOGDBG("cPlaylist: fetching remote playlist from %s", file);
    cString Cmd = cString::sprintf("curl %s", file);
    if(!p.Open(Cmd, "r")) {
      LOGERR("cPlaylist: CURL command (%s) failed", *Cmd);
      return false;
    }
    // process as normal file
    f = p;
  }
  
  if(f) {
    LOGDBG("cPlaylist: parsing %s", file);
    const char *ext = strrchr(file, '.');
    if(!strcasecmp(ext, ".pls"))
      parser = new cPlsReader(*this);
    else if(!strcasecmp(ext, ".asx"))
      parser = new cAsxReader(*this);
    else /*if(!strcasecmp(ext, ".m3u"))*/
      parser = new cM3uReader(*this); /* parses plain lists (.ram, ...) too ...*/

    /* get folder */
    cString Folder = file;
    const char *folder = strrchr(Folder, '/');
    if (folder)
      Folder.Truncate(folder - Folder + 1);

    int n = 0;
    cReadLine r;
    char *pt;
    while(NULL != (pt = r.Read(f)) && n < MAX_PLAYLIST_FILES) {
      if(NULL != (pt = parser->Parse(pt))) {

	if(xc.IsPlaylistFile(pt)) {
	  parser->ResetCache();
	  LOGDBG("cPlaylist: found playlist inside playlist");
	  if(depth > 4)
	    LOGMSG("cPlaylist: recursion too deep, skipped %s", pt);
	  else {
	    depth++;
	    if(*pt == '/' || 
	       (strstr(pt,"://")+1 == strchr(pt,'/') && 
		strchr(pt,'/') - pt < 8))
	      n += ReadPlaylist(pt);
	    else
	      n += ReadPlaylist(cString::sprintf("%s%s", *Folder, pt));
	    depth--;
	  }

	} else {
	  if(*pt == '/' || 
	     (strstr(pt,"://")+1 == strchr(pt,'/') && 
	      strchr(pt,'/') - pt < 8)) {
	    // absolute path
	    Add(new cPlaylistItem(pt));
	    if(parser->Title())
	      Last()->Title = parser->Title();
	  } else {
	    // relative path
	    Add(new cPlaylistItem(pt, Folder, parser->Title()));
	  }
	  Last()->Position = parser->Position();
	  parser->ResetCache();
	  //LOGDBG("read_playlist: %s", pt);
	  n++;
	}
      }
    }

    if(! (FILE*) p)
      fclose(f);

    if(n >= MAX_PLAYLIST_FILES) 
      LOGMSG("cPlaylist: Found over %d matching files, list truncated!", n);
    LOGDBG("cPlaylist: Found %d matching files", n);
    return n;
  }

  LOGERR("cPlaylist: Error opening %s", file);
  return 0;
}

static cString LastDir(cString& path)
{
  cString tmp = path;
  const char *pt = strrchr(tmp, '/');
  if(pt && pt > *tmp) {
    tmp.Truncate(pt - tmp);
    pt = strrchr(tmp, '/');
    if(pt)
      return cString(pt+1);
  }
  return cString(NULL);
}

bool cPlaylist::Read(const char *PlaylistFile, bool Recursive) 
{
  cMutexLock ml(&m_Lock);
  bool Result = true;

  // extract playlist root folder
  if(!*m_Folder) {
    const char *pt;
    m_Folder = PlaylistFile;
    if (NULL != (pt=strrchr(m_Folder, '/')))
      m_Folder.Truncate(pt - m_Folder + 1);
  }

  if(xc.IsPlaylistFile(PlaylistFile)) {
    // Read playlist file
    Result = ReadPlaylist(PlaylistFile);
    m_Origin = ePlaylist;

    cString dir = LastDir(m_Folder);
    const char *name = strrchr(PlaylistFile, '/');
    name = name ? name+1 : NULL;
    if(*dir && name)
      m_Name = cString::sprintf("%s - %s", *dir, name);
    else
      m_Name = name ?: "";

    strip_extension(m_Name);

  } else if(PlaylistFile[                     0] == '/'  &&
            PlaylistFile[strlen(PlaylistFile)-1] == '/') {
    // Scan folder
    Result = ScanFolder(PlaylistFile, Recursive) > 0;
    m_Origin = eImplicit;
    Sort();

    if(!*m_Name) {
      m_Name = PlaylistFile;
      m_Name.Truncate( strrchr(m_Name, '/') - m_Name);
      if(strrchr(m_Name, '/')) {
	cString dir = LastDir(m_Name);
	if(*dir)
	  m_Name = cString::sprintf("%s - %s", *dir, strrchr(m_Name, '/')+1);
	else
	  m_Name = strrchr(m_Name, '/')+1;
      }
    }

  } else {
    // Single file
    Add(new cPlaylistItem(PlaylistFile));
    m_Origin = eImplicit;

    if(!*m_Name) {
      m_Name = LastDir(m_Folder);
      if(!*m_Name)
	m_Name = "";
    }
  }

  if(Count() < 1) {
    LOGMSG("Empty playlist %s !", PlaylistFile);
    Add(new cPlaylistItem(PlaylistFile));
  }

  m_Version++;
  return Result;
}

static cString EscapeString(const char *s)
{
  static const uint8_t hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  const uint8_t *fn = (const uint8_t*)s;
  int size = strlen(s) + 16;
  char *buf = (char *)malloc(size);
  int i = 0;
  LOGVERBOSE("EscapeString('%s')", fn);

  while (*fn) {
    if(size-7 < i)
      buf = (char *)realloc(buf, (size=size+16));
    switch (*fn) {
    case 1 ... ' ':
    case 127 ... 255:
    case '#':
    case '%':
    case '?':
    case ':':
    case ';':
    case '\'':
    case '\"':
    case '(':
    case ')':
      buf[i++] = '%';
      buf[i++] = hex[(*fn & 0xf0)>>4];
      buf[i++] = hex[(*fn & 0x0f)];
      break;
    default:
      buf[i++] = *fn;
      break;
    }
    fn++;
  }

  buf[i] = 0;
  LOGVERBOSE("    --> '%s'", buf);
  return cString(buf, true);
}

cString cPlaylist::BuildMrl(const char *proto, const char *s1, const char *s2, const char *s3, const char *s4)
{
  return cString::sprintf("%s:%s%s%s%s",
                          proto,
                          s1 ? *EscapeString(s1) : "",
                          s2 ? *EscapeString(s2) : "",
                          s3 ? *EscapeString(s3) : "",
                          s4 ? *EscapeString(s4) : "");
}

cString cPlaylist::GetEntry(cPlaylistItem *i, bool isPlaylist, bool isCurrent)
{
  cString Entry = "";
  if ((*i->Artist && xc.playlist_artist) || (*i->Album && xc.playlist_album)) {
      Entry = cString::sprintf("%s%s%s%s%s%s(%s%s%s)",
                               isPlaylist ? (isCurrent ? "*" : " ") : "",
                               isPlaylist ? "\t" : " ",
                               xc.playlist_tracknumber ? (*i->Tracknumber ?: "") : "",
                               xc.playlist_tracknumber ? (*i->Tracknumber ? " - " : "") : "",
                               *i->Title,
                               isPlaylist ? "\t" : " ",
                               xc.playlist_artist ? (*i->Artist ?: "") : "",
                               xc.playlist_artist && xc.playlist_album ? (*i->Artist && *i->Album ? ":" : "") : "",
                               xc.playlist_album  ? (*i->Album  ?: "") : "");
  } else {
      Entry = cString::sprintf("%s%s%s%s%s",
                               isPlaylist ? (isCurrent ? "*" : " ") : "",
                               isPlaylist ? "\t" : " ",
                               xc.playlist_tracknumber ? (*i->Tracknumber ?: "") : "",
                               xc.playlist_tracknumber ? (*i->Tracknumber ? " - " : "") : "",
                               *i->Title);
  }
  return Entry;
}
