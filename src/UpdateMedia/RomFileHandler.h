#ifndef __ROM_FILE_HANDLER_H__
#define __ROM_FILE_HANDLER_H__

//-----------------------------------------------------------------------------------------------------
#include "ID3FileHandler.h"
#include "GAMEROM.h"
#include <map>
#include <algorithm>
using namespace std;
//-- Rom Type Defines ---------------------------------------------------------------------------------
#define ROMTYPE_DEFAULT 0
#define ROMTYPE_COWERING 1
//-----------------------------------------------------------------------------------------------------

class RomFileHandler : public ID3FileHandler
{
public:
	RomFileHandler(string sDirectory, string sFile, int iRomType);
	~RomFileHandler(void);

	bool LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, 
		list<pair<char *, size_t> >& listPicturesForTags);
	bool SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes);
	bool RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes);
	bool FileAttributeExists();
	string GetFileAttribute();
	string GetFileSourceForDB();

	int m_iRomType;
	string m_sROMSystem, m_sROMTitle, m_sROMSubtitle, m_sROMManufacturer, m_sROMYear, m_sROMGenre;

private:
	void GetRomInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures);
	void getMAMEData(string sRomName);
	void getCoweringData(string sRomName);
	void getSystem(string sFilename);
	string FileWithAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, bool bCreateId3File);
	bool IsSupported();
	void GetTagInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures);
	void SetTagInfo(string sFilename, const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures);
	void RemoveTag(string sFilename, int nTagType, string sValue);
	string ExtractAttribute(const map<int,string>& mapAttributes, int key);

	// trim from both ends
	static inline std::string &trim(std::string &s) 
	{
	        return ltrim(rtrim(s));
	}

	// trim from start
	static inline std::string &ltrim(std::string &s) 
	{
        	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        	return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) 
	{
        	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        	return s;
	}

	//tag utils
private:

protected:
	GAMEROM *m_pGAMEROM;

};
//-----------------------------------------------------------------------------------------------------

#endif //__ROM_FILE_HANLDER_H__
