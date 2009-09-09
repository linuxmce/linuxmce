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
			if (sDirectory.find("MAME") == string::npos)
				pFileHandler = new RomFileHandler(sDirectory, sFile, ROMTYPE_COWERING);
			else
				// Assume that this should be handled by the MAME/Default handler.
				pFileHandler = new RomFileHandler(sDirectory, sFile, ROMTYPE_DEFAULT);
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
	// With the advent of MESS support, the entire ROM code needed revamping.
	// It's not perfect. But if you have a better way to hand off to the RomFileHandler,
	// then please, help and do a patch. -tschak
	
	// The Point here is merely to do a series of quick checks, and if ANY of them pass, then
	// set a file handler type, which will be passed further up the factory line and more
	// detailed checks will be done, to pass parameters to the FileHandler to do the final work.

	string sExtension = FileUtils::FindExtension(sFile);
	bool bHasRomExtension = (sExtension == "zip") ||
			   (sExtension == "bin") || 
			   (sExtension == "a26") ||
			   (sExtension == "a52") ||
			   (sExtension == "a78") ||
			   (sExtension == "col") ||
			   (sExtension == "int");

	return bHasRomExtension;	// do we do more tests here?
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
