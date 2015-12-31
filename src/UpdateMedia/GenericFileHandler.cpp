#include "GenericFileHandler.h"
//-----------------------------------------------------------------------------------------------------
GenericFileHandler::GenericFileHandler(string sDirectory, string sFile) :
	m_sDirectory(sDirectory), m_sFile(sFile)
{
	m_sFullFilename = m_sDirectory + "/" + m_sFile;
}
//-----------------------------------------------------------------------------------------------------
GenericFileHandler::~GenericFileHandler(void)
{
}
//-----------------------------------------------------------------------------------------------------
bool GenericFileHandler::LoadAttributes(PlutoMediaAttributes * /*pPlutoMediaAttributes*/,
										list<pair<char *, size_t> >& /*listPicturesForTags*/)
{
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool GenericFileHandler::SaveAttributes(PlutoMediaAttributes * /*pPlutoMediaAttributes*/)
{
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool GenericFileHandler::RemoveAttribute(int /*nTagType*/, string /*sValue*/, PlutoMediaAttributes * /*pPlutoMediaAttributes*/)
{
	return true;
}
//-----------------------------------------------------------------------------------------------------
 bool GenericFileHandler::FileAttributeExists()
 {
	 return true;
 }
 //-----------------------------------------------------------------------------------------------------
string GenericFileHandler::GetFileAttribute()
{
	return m_sDirectory + "/" + m_sFile;
}
//-----------------------------------------------------------------------------------------------------
string GenericFileHandler::GetFileSourceForDB()
{
	return "F";
}
//-----------------------------------------------------------------------------------------------------
