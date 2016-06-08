#include "FileHandlerFactory.h"
#include "Logger.h"

#include "VDRInfoFileHandler.h"
#include "ID3FileHandler.h"
#include "TagFileHandler.h"
#include "RomFileHandler.h"
#include "PhotoFileHandler.h"
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
        string sExtension = FileUtils::FindExtension(sFile);
        
	switch(GetFileHandlerType(sDirectory, sFile))
	{
		case fhtRom:
			if (sDirectory.find("/other/") != string::npos)
			{
				// The file is in a public or user only share.
				if (sFile.find(".zip") != string::npos)
				{
					// for now, zip files are assumed to be mame roms here.
					pFileHandler = new RomFileHandler(sDirectory, sFile, ROMTYPE_DEFAULT);
				}
				else
				{
					pFileHandler = new RomFileHandler(sDirectory, sFile, ROMTYPE_COWERING);
				}
			}
			else
			{
				// This is probably on a LinuxMCE File Structure disk, in the games/MAME 
				// folder.
				if (sDirectory.find("MAME") == string::npos)
					pFileHandler = new RomFileHandler(sDirectory, sFile, ROMTYPE_COWERING);
				else
					// Assume that this should be handled by the MAME/Default handler.
					pFileHandler = new RomFileHandler(sDirectory, sFile, ROMTYPE_DEFAULT);
			}
			break;

		case fhtVdr:
		        if ( sExtension == "rec" ) 
		        {
                                sDirectory += "/" + sFile;
                                sFile = "info";
                        }
			pFileHandler = new VDRInfoFileHandler(sDirectory, sFile);
			break;

		case fhtTag:
			pFileHandler = new TagFileHandler(sDirectory, sFile);
			break;

		case fhtId3:
			pFileHandler = new ID3FileHandler(sDirectory, sFile);
			break;

		case fhtImage:
			pFileHandler = new PhotoFileHandler(sDirectory, sFile);
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

	if(IsValidVDRFile(sDirectory, sFile)) {
#ifdef UPDATEMEDIA_DEBUG
	        LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Filehandler::GetFileHandlerType: The directory %s is VDR file!", sDirectory.c_str());
#endif
		type = fhtVdr;
 	} else if(IsValidTagFile(sDirectory, sFile)) {
#ifdef UPDATEMEDIA_DEBUG
	        LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Filehandler::GetFileHandlerType: File %s in %s is tag file!", sFile.c_str(),  sDirectory.c_str());
#endif
		type = fhtTag;
	} else if(IsValidRomFile(sDirectory, sFile)) {
#ifdef UPDATEMEDIA_DEBUG
	        LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Filehandler::GetFileHandlerType: File %s in %s is ROM file!", sFile.c_str(),  sDirectory.c_str());
#endif
		type = fhtRom;
	} else if(IsValidImageFile(sDirectory, sFile)) {
#ifdef UPDATEMEDIA_DEBUG
	        LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Filehandler::GetFileHandlerType: File %s in %s is image file!", sFile.c_str(),  sDirectory.c_str());
#endif
		type = fhtImage;
	} else { 
#ifdef UPDATEMEDIA_DEBUG
	        LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Filehandler::GetFileHandlerType: File %s in %s is id3 file!", sFile.c_str(),  sDirectory.c_str());
#endif
//		type = fhtId3;
		type = fhtTag;
        }     
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
	sExtension = StringUtils::ToLower(sExtension);
	bool bHasRomExtension = (sExtension == "zip") ||
			   (sExtension == "bin") || 
			   (sExtension == "a26") ||
			   (sExtension == "a52") ||
			   (sExtension == "a78") ||
			   (sExtension == "col") ||
			   (sExtension == "int") ||
			   (sExtension == "sms") ||
			   (sExtension == "sg") ||
			   (sExtension == "nes") || 
			   (sExtension == "fam") ||
			   (sExtension == "smc") ||
			   (sExtension == "sfc") ||
			   (sExtension == "fig") ||
			   (sExtension == "swc") ||
			   (sExtension == "smd") ||
		           (sExtension == "md") ||
			   (sExtension == "gen") ||
			   (sExtension == "pce") ||
			   (sExtension == "vec") ||
			   (sExtension == "pce") ||
			   (sExtension == "dsk") ||
			   (sExtension == "nib") ||
			   (sExtension == "po") ||
			   (sExtension == "do") ||
			   (sExtension == "j64") ||
			   (sExtension == "20") ||
			   (sExtension == "40") ||
		           (sExtension == "60") ||
			   (sExtension == "70") ||
			   (sExtension == "80") ||
		 	   (sExtension == "a0") ||
			   (sExtension == "b0") ||
			   (sExtension == "prg") ||
			   (sExtension == "p00") ||
			   (sExtension == "t64") ||
			   (sExtension == "g64") ||
			   (sExtension == "d64") ||
			   (sExtension == "crt") ||
	  (sExtension == "atr") ||
	  (sExtension == "xfd") ||
	  (sExtension == "rom") ||
          (sExtension == "xex") ||
			   (sExtension == "fds") ||
	  (sExtension == "rpk");

	return bHasRomExtension;	// do we do more tests here?
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidVDRFile(string sDirectory, string sFile)
{
        string sExtension = FileUtils::FindExtension(sFile);

	return ( (sFile == "info.vdr" && FileUtils::FileExists(sDirectory + "/001.vdr")) 
	           || (sFile == "info"  && FileUtils::FileExists(sDirectory + "/00001.ts")) 
	           || (sExtension == "rec"  && FileUtils::FileExists(sDirectory + "/" + sFile + "/00001.ts")) 
               );
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidTagFile(string sDirectory, string sFile)
{
        const string csSupportedExtensions("mp3:ogg:flac:mp4:wv:wav::oga:mpc:spx:opus:tta:m4a:m4r:m4p:3g2:m4v:wma:asf:aif:aiff:afc:aifc:ape:s3m");
        string sExtension = StringUtils::ToLower(FileUtils::FindExtension(sFile));

        if(sExtension.empty())
                return false;

        return csSupportedExtensions.find(sExtension) != string::npos;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool FileHandlerFactory::IsValidImageFile(string sDirectory, string sFile)
{
        const string csSupportedExtensions("jpg");
        string sExtension = StringUtils::ToLower(FileUtils::FindExtension(sFile));

        if(sExtension.empty())
                return false;

        return csSupportedExtensions.find(sExtension) != string::npos;
}
//-----------------------------------------------------------------------------------------------------
