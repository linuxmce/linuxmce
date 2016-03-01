/**
 * History.cpp - A Parser for the MAME History.dat file, to provide
 * Synopsis data for the games and Softlists in MAME
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 */

#include "History.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

using namespace DCE;

History::History(string sHistoryPath)
{
  m_sHistoryPath = sHistoryPath;
  m_sCurrentLine=0;
}

History::~History()
{

}

bool History::Parse()
{
  cout << "Parsing History file...Please wait..." << flush;
  if (!FileUtils::FileExists(m_sHistoryPath))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't find history file named %s",m_sHistoryPath.c_str());
      return false;
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Reading history file %s",m_sHistoryPath.c_str());
  FileUtils::ReadFileIntoVector(m_sHistoryPath,m_vectHistoryLines);

  m_sCurrentLine=1;

  for (vector<string>::iterator it=m_vectHistoryLines.begin(); it!=m_vectHistoryLines.end(); ++it)
    {
      ParseLine((string)*it);
      m_sCurrentLine++;
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Done reading history file.");
  m_vectHistoryLines.clear();
  
  cout << endl;
  return true;
}

void History::ParseLine(string sLine)
{
  if (sLine[0]=='$')
    {
      ParseHistoryLine(sLine);
    }
  else
    {
      AddLineToEntry(sLine);
    }
}

void History::ParseHistoryLine(string sLine)
{
  if (sLine[0]=='$' && sLine[1]=='b' && sLine[2]=='i' && sLine[3]=='o')
    {
      // Ignore this line. This may change, so keep an eye on it.
      return;
    }
  else if (sLine[0]=='$' && sLine[1]=='e' && sLine[2]=='n' && sLine[3]=='d')
    {
      m_vectGames.clear();
      m_sCurrentMachine="";
    }
  else if (sLine[0]=='$' && (sLine.find("=") == string::npos))
    {
      // This is a spurious text line that does not contain a macro to process, pass it
      // through to content.
      AddLineToEntry(sLine);
    }
  else
    {
      // Tokenize comma seperated entries, extract first entry, and subsequently
      // pop it from the alternate entries vector, leaving only the alternate entries.
      // This assumes the primary entry is listed first. 
      size_t pos=0;
      m_sCurrentMachine=StringUtils::Tokenize(sLine,"=",pos).substr(1);
      string sParameter=sLine.substr(pos);
      StringUtils::Tokenize(sParameter,",",m_vectGames);
    }
}

void History::AddLineToEntry(string sLine)
{
  // Add the line of text to each of the current game entries for the current machine
  if (m_sCurrentMachine.empty())
    return;

  for (vector<string>::iterator it=m_vectGames.begin(); it!=m_vectGames.end(); ++it)
    {
      string currentEntry = *it;
      if (!currentEntry.empty())
      m_mapMachineAndGameToHistory[make_pair(m_sCurrentMachine,currentEntry)]+=sLine + "\n";
    }
}
