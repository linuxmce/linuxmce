#ifndef __ID3_FILE_HANDLER_H__
#define __ID3_FILE_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "GenericFileHandler.h"
//-----------------------------------------------------------------------------------------------------
class ID3FileHandler : public GenericFileHandler
{
public:
	ID3FileHandler(string sDirectory, string sFile);
	~ID3FileHandler(void);

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
};
//-----------------------------------------------------------------------------------------------------
#endif //__ID3_FILE_HANDLER_H__
