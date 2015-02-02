/**
 * Position - A simple class for serializing Xine_Player compatible positions/bookmarks/
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "Position.h"

using namespace DCE;

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>

Position::Position()
{
  m_iTitle=m_iChapter=m_iPos=m_iSubtitle=m_iAudio=m_iTotal=0;
}

Position::Position(int iTitle, int iChapter, int iPos, int iSubtitle, int iAudio, int iTotal)
{
  m_iTitle=iTitle;
  m_iChapter=iChapter;
  m_iPos=iPos;
  m_iSubtitle=iSubtitle;
  m_iAudio=iAudio;
  m_iTotal=iTotal;
}


Position::Position(int iTitle, int iChapter)
{
  m_iTitle=iTitle;
  m_iChapter=iChapter;
  m_iPos=0;
  m_iSubtitle=0;
  m_iAudio=0;
  m_iTotal=0;

}

Position::Position(string sFromString)
{
  m_iTitle=m_iChapter=m_iPos=m_iSubtitle=m_iAudio=m_iTotal=0;
  ParseString(sFromString);
}

Position::~Position()
{
}

void Position::setTitle(int iTitle)
{
  m_iTitle=iTitle;
}

int Position::getTitle()
{
  return m_iTitle;
}

void Position::setChapter(int iChapter)
{
  m_iChapter=iChapter;
}

int Position::getChapter()
{
  return m_iChapter;
}

void Position::setPosition(int iPos)
{
  m_iPos=iPos;
}

int Position::getPosition()
{
  return m_iPos;
}

void Position::setSubtitle(int iSubtitle)
{
  m_iSubtitle=iSubtitle;
}

int Position::getSubtitle()
{
  return m_iSubtitle;
}

void Position::setAudio(int iAudio)
{
  m_iAudio=iAudio;
}

int Position::getAudio()
{
  return m_iAudio;
}

void Position::setTotal(int iTotal)
{
  m_iTotal=iTotal;
}

int Position::getTotal()
{
  return m_iTotal;
}

void Position::ParseString(string sFromString)
{
  size_t pos=0;  // Parsing cursor
  string sKey=""; // current key
  int* piVal=NULL; // current value 
  int nulVal=0; // The default catch-all for value.

  piVal=&nulVal; // Make sure by default, pointer is set to null value so that we don't go off rails.

  if (sFromString.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Position::ParseString() - string empty. Returning.");
      return;
    }

  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString(%s)",sFromString.c_str());
    
  while (pos < sFromString.size())
    {
      // Parse the key
      sKey = StringUtils::Tokenize(sFromString,":",pos);
      if (sKey==" TITLE")
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString() - Title fragment found.");
	  piVal=&m_iTitle;
	}
      else if (sKey==" CHAPTER")
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString() - Chapter fragment found.");
	  piVal=&m_iChapter;
	}
      else if (sKey==" POS")
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString() - Position fragment found.");
	  piVal=&m_iPos;
	}
      else if (sKey==" SUBTITLE")
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString() - Subtitle fragment found.");
	  piVal=&m_iSubtitle;
	}
      else if (sKey==" AUDIO")
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString() - Audio fragment found.");
	  piVal=&m_iAudio;
	}
      else if (sKey==" TOTAL")
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString() - Total fragment found.");
	  piVal=&m_iTotal;
	}

      // Parse the value
      *piVal=atoi(StringUtils::Tokenize(sFromString," ",pos).c_str());
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Position::ParseString() - Value set for above to %d",*piVal);

      // Correct the position value (back 1, so that the loop will work.)
      pos--;

      // And loop back around.
    }

}

string Position::toString()
{
  string sPosition="";
  if (getTitle()!=0)
    {
      sPosition += " TITLE:" + StringUtils::itos(getTitle());
    }
  if (getChapter()!=0)
    {
      sPosition += " CHAPTER:" + StringUtils::itos(getChapter());
    }
  if (getPosition()!=0)
    {
      sPosition += " POS:" + StringUtils::itos(getPosition());
    }
  if (getSubtitle()!=0)
    {
      sPosition += " SUBTITLE:" + StringUtils::itos(getSubtitle());
    }
  if (getAudio()!=0)
    {
      sPosition += " AUDIO:" + StringUtils::itos(getAudio());
    }
  if (getTotal()!=0)
    {
      sPosition += " TOTAL:"+StringUtils::itos(getTotal());
    }
  return sPosition;
}

bool Position::hasOnlyPos()
{
  return (getTitle()==0 &&
	  getChapter()==0 &&
	  getPosition()>0 &&
	  getSubtitle()==0 &&
	  getAudio()==0);
}
