#ifndef __FILE_HANDLER_FACTORY_H__
#define __FILE_HANDLER_FACTORY_H__
//-----------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class GenericFileHandler;
//-----------------------------------------------------------------------------------------------------
enum FileHandlerType
{
	fhtGeneric,
	fhtTag,
	fhtId3,
	fhtVdr,
	fhtRom,
	fhtImage
};
//-----------------------------------------------------------------------------------------------------
class FileHandlerFactory
{
public:
	FileHandlerFactory(void);
	~FileHandlerFactory(void);

	static GenericFileHandler *CreateFileHandler(string sDirectory, string sFile);
	static FileHandlerType GetFileHandlerType(string sDirectory, string sFile);

private:

	static bool IsValidVDRFile(string sDirectory, string sFile);
	static bool IsValidTagFile(string sDirectory, string sFile);
	static bool IsValidRomFile(string sDirectory, string sFile);
	static bool IsValidImageFile(string sDirectory, string sFile);
};
//-----------------------------------------------------------------------------------------------------
#endif //__FILE_HANDLER_FACTORY_H__
