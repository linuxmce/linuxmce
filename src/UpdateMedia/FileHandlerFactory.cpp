#include "FileHandlerFactory.h"

#include "VDRInfoFileHandler.h"
#include "ID3FileHandler.h"
#include "TagFileHandler.h"
#include "RomFileHandler.h"
#include "PlutoUtils/FileUtils.h"
#include "FileUtils/file_utils.h"
//-----------------------------------------------------------------------------------------------------
FileHandlerFactory::FileHandlerFactory(void)
{
}
//-----------------------------------------------------------------------------------------------------
FileHandlerFactory::~FileHandlerFactory(void)
{
}
//-----------------------------------------------------------------------------------------------------
/*static*/ GenericFileHandler *FileHandlerFactory::CreateFileHandler(string sDirectory, string sFile)
{
	GenericFileHandler *pFileHandler = NULL;

	switch(GetFileHandlerType(sDirectory, sFile))
	{
		case fhtRom:
			pFileHandler = new RomFileHandler(sDirectory, sFile);
			break;

		case fhtVdr:
			pFileHandler = new VDRInfoFileHandler(sDirectory, sFile);
			break;

		case fhtTag:
			pFileHandler = new TagFileHandler(sDirectory, sFile);
			break;

		case fhtId3:
			pFileHandler = new ID3FileHandler(sDirectory, sFile);
			break;

		default:
			pFileHandler = new GenericFileHandler(sDirectory, sFile);
			break;
	}

	return pFileHandler;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ FileHandlerType FileHandlerFactory::GetFileHandlerType(string sDirectory, string sFile)
{
	FileHandlerType type = fhtGeneric;

	if(IsValidVDRFile(sDirectory, sFile))
		type = fhtVdr;
	else if(IsValidTagFile(sDirectory, sFile))
		type = fhtTag;
	else if(IsValidRomFile(sDirectory, sFile))
		type = fhtRom;
	else 
		type = fhtId3;

	return type;
}	
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidRomFile(string sDirectory, string sFile)
{
	string sExtension = FileUtils::FindExtension(sFile);
	return sExtension == "zip";
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidVDRFile(string sDirectory, string sFile)
{
	return sFile == "001.vdr" && FileUtils::FileExists(sDirectory + "/info.vdr");
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidTagFile(string sDirectory, string sFile)
{
	string sExtension = FileUtils::FindExtension(sFile);
	return sExtension == "ogg" || sExtension == "flac";
}
//-----------------------------------------------------------------------------------------------------
