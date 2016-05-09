#ifndef __TAG_FILE_HANDLER_H__
#define __TAG_FILE_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "GenericFileHandler.h"

#include <map>

#include <tag.h>
#include <fileref.h>
#include <tpropertymap.h>

using namespace std;
//-----------------------------------------------------------------------------------------------------
class TagFileHandler : public GenericFileHandler
{
public:
	TagFileHandler(string sDirectory, string sFile);
	~TagFileHandler(void);

	bool LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, 
		list<pair<char *, size_t> >& listPicturesForTags);
	bool SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes);
	bool RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes);
	bool FileAttributeExists();
	string GetFileAttribute();
	string GetFileSourceForDB();

private:

	string FileWithAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, bool bCreateId3File);
	bool IsSupported();


	//tag utils
private:

	void GetTagInfo(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures);
	void SetTagInfo(string sFilename, const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures);
	void InsertTagValues(TagLib::FileRef *&f , string sName, string sParameters);
	void RemoveTagValue(TagLib::FileRef *&f, const string sName, string sValue);
	void RemoveTag(string sFilename, int nTagType, string sValue);

	void GetUserDefinedInformation(string sFilename, char *&pData, size_t& Size);
	void SetUserDefinedInformation(string sFilename, char *pData, size_t& Size);

	string ExtractAttribute(const map<int,string>& mapAttributes, int key);

	void stov(string s, std::vector<string> &v, string acDelimiters = ";,");
	string vtos(std::vector<string> v, const char cDelimiter = ';');
};
//-----------------------------------------------------------------------------------------------------
#endif //__TAG_FILE_HANDLER_H__
