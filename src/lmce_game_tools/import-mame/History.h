/**
 * History.h - A Parser for the MAME History.dat file, to provide
 * Synopsis data for the games and Softlists in MAME
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 */

#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <vector>
#include <map>
#include <utility>

using namespace std;

class History
{
 private:
  vector<string> m_vectHistoryLines;
  map< pair<string,string>, string> m_mapMachineAndGameToHistory;
  string m_sHistoryPath;
  string m_sCurrentMachine;
  vector<string> m_vectGames;
  unsigned int m_sCurrentLine;
  
  void ParseLine(string sLine);
  void ParseHistoryLine(string sLine);
  void AddLineToEntry(string sLine);

 public:
  History(string sHistoryPath);
  virtual ~History();
  bool Parse();

  string m_mapMachineAndGameToHistory_Find(pair<string, string> sMachinePair)
  {
    map< pair < string, string >, string >::iterator it=m_mapMachineAndGameToHistory.find(sMachinePair);
    return it == m_mapMachineAndGameToHistory.end() ? "" : it->second;
  }

};

#endif /* HISTORY_H */
