/*
 * Copyright (C) 2009 MB
 * *
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
#include "PlaylistParser.h"

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/HttpUtils.h"

#include <unistd.h>
#include <stdio.h>
#include <string>

using namespace std;
using namespace DCE;

PlaylistParser::PlaylistParser(string URL) {
        m_sURL = URL;
        load();
}

PlaylistParser::~PlaylistParser() {

}

void PlaylistParser::load() {
        vector<string> vectLines;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlaylistParser::load url %s", m_sURL.c_str());
        if (StringUtils::StartsWith(m_sURL, "http://")) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlaylistParser::load http download");
		ReadURLIntoVector(m_sURL, vectLines);
	} else {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlaylistParser::load() regular file, just load it");
		FileUtils::ReadFileIntoVector(m_sURL, vectLines);
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlaylistParser::load() number of lines %d", vectLines.size());

        parseLines(vectLines);

}

bool PlaylistParser::ReadURLIntoVector(string sUrl, vector<string> &vectLines)
{
        size_t size;
	string sBuffer;
	HttpGet(sUrl, &sBuffer);
	if( sBuffer.size() > 0 )
	{
		StringUtils::Tokenize(sBuffer,"\n",vectLines);
	} else {
	        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PlaylistParser::ReadURLIntoVector() no data returned");
		return false;
	}
	// Strip any \r that will be in a Windows file
	for(uint s=0;s<vectLines.size();++s)
		vectLines[s] = StringUtils::Replace(&vectLines[s],"\r","");

	return true;
}

void PlaylistParser::parseLines(vector<string> &vectLines) {
        // pls playlist detection
        m_bIsPlaylist = vectLines.size() > 0 && StringUtils::StartsWith(vectLines[0], "[playlist]");
				
        if (!m_bIsPlaylist) {
                return;
        }
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlaylistParser::parseLines is playlist (pls)");

	map<int, PlaylistItem> mapItem;
	int maxNo = 0;
	for(vector<string>::iterator it=vectLines.begin();it!=vectLines.end();++it)
	{
		string sLine = *it;
		uint pos = sLine.find("=");
		if (pos != string::npos) {
			if (StringUtils::StartsWith(sLine, "File") || 
			    StringUtils::StartsWith(sLine, "Title")) {
			        string sNo = "";
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"PlaylistParser::parseLines sLine = %s", sLine.c_str());
			        if (StringUtils::StartsWith(sLine, "File")) {
				  	sNo = sLine.substr(4, pos - 4);
					LoggerWrapper::GetInstance()->Write(LV_DEBUG,"PlaylistParser::parseLines file : no = %s", sNo.c_str());
				} else {
				        sNo = sLine.substr(5, pos - 5);
					LoggerWrapper::GetInstance()->Write(LV_DEBUG,"PlaylistParser::parseLines title: no = %s", sNo.c_str());
				}
				int no = atoi(sNo.c_str());
				if (no > maxNo) {
				        maxNo = no;
				}
				map<int, PlaylistItem>::iterator it;
				it = mapItem.find(no);
				if (it == mapItem.end()) {
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"PlaylistParser::parseLines item not found, add new one");
				        PlaylistItem item;
				        mapItem[no] = item;
					it = mapItem.find(no);
				}
LoggerWrapper::GetInstance()->Write(LV_DEBUG,"PlaylistParser::parseLines updating item");
				
			        if (StringUtils::StartsWith(sLine, "File")) {
				        it->second.url = sLine.substr(pos+1);
				} else {
				        it->second.title = sLine.substr(pos+1);
				}
				

			}
		}

	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlaylistParser::parseLines done adding items, now add them all to list");
				
	map<int, PlaylistItem>::iterator it;
	for (int i = 1; i <= maxNo; i++) {
	        it = mapItem.find(i);
		if (it != mapItem.end()) {
		         m_dequeItems.push_back(it->second);
		}		
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PlaylistParser::parseLines end items = %d", m_dequeItems.size());
}

bool PlaylistParser::isPlaylist() {
        return m_bIsPlaylist;
}

deque<PlaylistItem> PlaylistParser::getItems() {
        return m_dequeItems;
}
