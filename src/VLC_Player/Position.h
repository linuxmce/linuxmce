/**
 * Position - A simple class for serializing Xine_Player compatible positions/bookmarks/
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef POSITION_H
#define POSITION_H

#include <string>

using namespace std;

namespace DCE
{
  
  class Position
  {
  private:
    int m_iTitle;
    int m_iChapter;
    int m_iPos;
    int m_iSubtitle;
    int m_iAudio;
    int m_iTotal;
  public:
    Position();
    Position(int iTitle, int iChapter, int iPos, int iSubtitle, int iAudio, int iTotal);
    Position(int iTitle, int iChapter);
    Position(string sFromString);
    ~Position();

    void setTitle(int iTitle);
    int getTitle();
    void setChapter(int iChapter);
    int getChapter();
    void setPosition(int iPos);
    int getPosition();
    void setSubtitle(int iSubtitle);
    int getSubtitle();
    void setAudio(int iAudio);
    int getAudio();
    void setTotal(int iTotal);
    int getTotal();

    void ParseString(string sFromString);
    string toString();

    bool hasOnlyPos();

  };

}

#endif /** POSITION_H */
