/*
 * media_player.h: Media and image players
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: media_player.h,v 1.24 2012-03-19 11:07:52 phintuka Exp $
 *
 */

#ifndef __XINELIB_PLAYER_H
#define __XINELIB_PLAYER_H

class cPlaylist;
class cXinelibDevice;

class cPlayerFactory
{
  public:

    // interact with current player

    static bool IsOpen(void);
    static void Queue (cXinelibDevice *Dev, const char *Mrl);

    // launch new media player

    static bool Launch(cXinelibDevice *Dev, const char *Mrl, const char *SubFile = NULL) { return Launch(Dev, pmNone, Mrl, SubFile); };

    static bool Launch(cXinelibDevice *Dev, ePlayMode PlayMode, const char *Mrl, const char *SubFile = NULL, bool BackToMenu = false);
    static bool Launch(cXinelibDevice *Dev, ePlayMode PlayMode, cPlaylist *Playlist, bool BackToMenu = false);
};

#endif // __XINELIB_PLAYER_H
