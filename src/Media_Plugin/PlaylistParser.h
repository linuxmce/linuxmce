/*
 * Copyright (C) 2009 MB
 *
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef PlaylistParser_h
#define PlaylistParser_h

#include <deque>
#include <vector>
#include <string>

using namespace std;

struct PlaylistItem {
        string url;
        string title;
};

class PlaylistParser {

private:
        string m_sURL;
        bool m_bIsPlaylist;
        deque<PlaylistItem> m_dequeItems;

        void load();
public:
        PlaylistParser(string URL);
        ~PlaylistParser();
        bool isPlaylist();
        void parseLines(vector<string> &vectLines);
        deque<PlaylistItem> getItems();
        bool ReadURLIntoVector(string sUrl, vector<string> &vectLines);

};

#endif
