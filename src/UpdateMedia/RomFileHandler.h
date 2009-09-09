#ifndef __ROM_FILE_HANDLER_H__
#define __ROM_FILE_HANDLER_H__

//-----------------------------------------------------------------------------------------------------
#include "GenericFileHandler.h"
#include "GAMEROM.h"
#include <map>
using namespace std;
//-- Rom Type Defines ---------------------------------------------------------------------------------
#define ROMTYPE_DEFAULT 0
#define ROMTYPE_COWERING 1
//-----------------------------------------------------------------------------------------------------

class RomFileHandler : public GenericFileHandler
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
	string m_sROMSystem, m_sROMTitle, m_sROMManufacturer, m_sROMYear, m_sROMGenre;

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

	//tag utils
private:

protected:
	GAMEROM *m_pGAMEROM;

};
//-----------------------------------------------------------------------------------------------------

#endif //__ROM_FILE_HANLDER_H__
