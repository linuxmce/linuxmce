/*
 * PhotoFileHandler.cpp
 *
 *  @author: Christian Schleiffer <christian@schleiffer.net>
 */

#include "PhotoFileHandler.h"

#include "PlutoMediaAttributes.h"
#include "FileUtils/file_utils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Define_AttributeType.h"
using namespace DCE;

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#include <tag.h>
#include <fileref.h>

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/iptc.hpp>
#include <exiv2/convert.hpp>

// Necessary on 2204
#include <exiv2/error.hpp>

namespace UpdateMediaVars
{
	extern string sUPnPMountPoint;
	extern string sLocalUPnPServerName;
};

PhotoFileHandler::PhotoFileHandler(string sDirectory, string sFile) :
	GenericFileHandler(sDirectory, sFile) {
}

PhotoFileHandler::~PhotoFileHandler(void) {
}

string PhotoFileHandler::AttributeToTag(int attribute) {
	switch (attribute) {
	case ATTRIBUTETYPE_Title_CONST:
		return "Exif.Image.ImageDescription";
	case ATTRIBUTETYPE_Manufacturer_CONST:
		return "Exif.Image.Model";
	case ATTRIBUTETYPE_Year_CONST:
		return "Exif.Image.DateTime";
	case ATTRIBUTETYPE_ComposerWriter_CONST:
		return "Exif.Image.Artist";
	case ATTRIBUTETYPE_Keyword_CONST:
		return "Iptc.Application2.Keywords";
	default:
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::AttributeToTag() Unhandled attribute %d", attribute);
		return "";
	}
}

bool PhotoFileHandler::LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, list<pair<char *, size_t> >& listPicturesForTags) {
	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::LoadAttributes: loading %d attributes in the attribute file %s",
			pPlutoMediaAttributes->m_mapAttributes.size(), m_sFullFilename.c_str());

	multimap<int, string> mmappedAttributes;
	getTagsFromFile(m_sFullFilename, mmappedAttributes, listPicturesForTags);

	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::LoadAttributes: Mapped %d attributes", mmappedAttributes.size());

	//merge attributes
	for(multimap<int, string>::iterator it = mmappedAttributes.begin(), end = mmappedAttributes.end(); it != end; ++it) {
		int nType = it->first;
		string sValue = it->second;

		MapPlutoMediaAttributes::iterator itm = pPlutoMediaAttributes->m_mapAttributes.find(nType);
		if(itm == pPlutoMediaAttributes->m_mapAttributes.end() || itm->second->m_sName != sValue)
			pPlutoMediaAttributes->m_mapAttributes.insert(
				std::make_pair(
					nType, 
					new PlutoMediaAttribute(0,nType, sValue)
				)
			);
		else
			itm->second->m_sName = sValue;
	}
	return true;
}

bool PhotoFileHandler::SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes) {

	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::SaveAttributes: saving %d attributes in the attribute file %s",
		pPlutoMediaAttributes->m_mapAttributes.size(), m_sFullFilename.c_str());

	if (pPlutoMediaAttributes->m_mapAttributes.size() != 0)
	{
		try {
			Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(m_sFullFilename);
			if(image.get() != 0) {
				image->readMetadata();

				Exiv2::ExifData &exifData = image->exifData();
				Exiv2::IptcData &iptcData = image->iptcData();
				// Set IPTC CharacterSet to UTF-8, as that is what we save
				Exiv2::Value::AutoPtr charset = Exiv2::Value::create(Exiv2::asciiString);
				charset->read("UTF-8");
				iptcData.add(Exiv2::IptcKey("Iptc.Envelope.CharacterSet"), charset.get());

				if (pPlutoMediaAttributes->m_mapAttributes.size() != 0)
				{
					// Keep track of tags we have cleared (only clear tags we are going to save, tags/attributes that are removed are handled in RemoveAttribute)
					set<string> setClearedTags;
					for(MapPlutoMediaAttributes::iterator it = pPlutoMediaAttributes->m_mapAttributes.begin(), 
						    end = pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
					{
						
						if (it->second != NULL && it->second->m_sName != "")
						{
							string tag = AttributeToTag(it->first);
							if (tag != "") {
								LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::SaveAttributes: saving tag %s, value = %s", tag.c_str(), it->second->m_sName.c_str());
								if (setClearedTags.find(tag) == setClearedTags.end()) {
									// Clear all tags of this key before starting to add new keys
									if (StringUtils::StartsWith(tag, "Exif")) {
										Exiv2::ExifData::iterator it = exifData.findKey(Exiv2::ExifKey(tag));
										while (it != exifData.end()) {
											exifData.erase(it);
											it = exifData.findKey(Exiv2::ExifKey(tag));
										}
									} else if (StringUtils::StartsWith(tag, "Iptc")) {
										Exiv2::IptcData::iterator it = iptcData.findKey(Exiv2::IptcKey(tag));
										while (it != iptcData.end()) {
											iptcData.erase(it);
											it = iptcData.findKey(Exiv2::IptcKey(tag));
										}
									}
									setClearedTags.insert(tag);
								}
								Exiv2::Value::AutoPtr v = Exiv2::Value::create(Exiv2::asciiString);
								v->read(it->second->m_sName.c_str());
								if (StringUtils::StartsWith(tag, "Exif"))
								{
									exifData.add(Exiv2::ExifKey(tag.c_str()), v.get());
								} else if (StringUtils::StartsWith(tag, "Iptc"))
								{
									iptcData.add(Exiv2::IptcKey(tag.c_str()), v.get());
								}
							}
						}
					}
					
				}
				image->setExifData(exifData);
				image->setIptcData(iptcData);
				image->writeMetadata();
			} else {
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::SaveAttributes: unable to open file");
			}
		} catch (Exiv2::AnyError& e) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::SaveAttributes() Exiv::AnyError: %s",  e.what());
		}
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::SaveAttributes: No attributes to save");

	return true;
}

bool PhotoFileHandler::RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes) {
	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::RemoveAttributes: removing tag %d with value %s from file %s",
					    nTagType, sValue.c_str(), m_sFullFilename.c_str());

	if (pPlutoMediaAttributes->m_mapAttributes.size() != 0)
	{
		try {
			Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(m_sFullFilename);
			if(image.get() != 0) {
				image->readMetadata();
				
				Exiv2::ExifData &exifData = image->exifData();
				Exiv2::IptcData &iptcData = image->iptcData();
				string tag = AttributeToTag(nTagType);
				if (tag != "") {
					if (StringUtils::StartsWith(tag, "Exif"))
					{
						Exiv2::ExifKey key = Exiv2::ExifKey(tag);
						Exiv2::ExifData::iterator pos = exifData.begin();
						while (pos != exifData.end() && (pos->key() != tag || pos->value().toString() != sValue)) {
							pos++;
						}
						if (pos == exifData.end() || pos->value().toString() != sValue)
						{
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::RemoveAttributes: no such exif tag %s with value %s", tag.c_str(), sValue.c_str());
						} else {
							exifData.erase(pos);
						}
					} else if (StringUtils::StartsWith(tag, "Iptc"))
					{
						Exiv2::IptcKey key = Exiv2::IptcKey(tag);
						Exiv2::IptcData::iterator pos = iptcData.begin();
						while (pos != iptcData.end() && (pos->key() != tag || pos->value().toString() != sValue)) {
							pos++;
						}
						if (pos == iptcData.end() || pos->value().toString() != sValue)
						{
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::RemoveAttributes: no such iptc tag %s with value %s", tag.c_str(), sValue.c_str());
						} else {
							iptcData.erase(pos);
						}
					}
				}
				image->setExifData(exifData);
				image->setIptcData(iptcData);
				image->writeMetadata();
			}
		} catch (Exiv2::AnyError& e) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::RemoveAttributes() Exiv::AnyError: %s",  e.what());
		}
	}
	return false;
}

bool PhotoFileHandler::FileAttributeExists() {
	// ???
	return true;
}

string PhotoFileHandler::GetFileAttribute() {
	return m_sFile;
}

string PhotoFileHandler::GetFileSourceForDB() {
	if(!UpdateMediaVars::sUPnPMountPoint.empty() && StringUtils::StartsWith(m_sDirectory, UpdateMediaVars::sUPnPMountPoint))
		return "U";
	else
		return "F";
}

void PhotoFileHandler::getTagsFromFile(string sFilename, multimap<int,string>& mmapAttributes, list<pair<char *, size_t> >& listPictures) {
	// open file
	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::getTagsFromFile: Processing file %s", sFilename.c_str());
	try {
		Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(sFilename);
		// check if everything's okay and only proceed if there is some meta data
		if(image.get() != 0) {
			image->readMetadata();
			
			Exiv2::ExifData &exifData = image->exifData();
			Exiv2::IptcData &iptcData = image->iptcData();
			
			if(exifData.empty()) {
				LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::getTagsFromFile: File %s does not contain any EXIF information",
								    sFilename.c_str());
			} else {
				string sExifAttributesPlain = "";
				
				Exiv2::ExifData::const_iterator end = exifData.end();
				for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
					// prepare string to write out all attributes found in the file
					//cout << i->key() << " - " <<  i->tag() << " : " << i->value() << "\n";
					//				sExifAttributesPlain += i->key() +  ": " + i->value().toString() + "; ";

					// map some attributes to LMCE media attributes
					switch(i->tag()) {
					case 0x0100:
						// TODO Exif.Image.ImageWidth
						break;
					case 0x0101:
						// TODO Exif.Image.ImageLength
						break;
					case 0x0102:
						// TODO Exif.Image.BitsPerSample
						break;
					case 0x010E:
						// ImageDescription mapped to "Title"
						mmapAttributes.insert(pair<int, string>(ATTRIBUTETYPE_Title_CONST, i->value().toString()));
						break;
					case 0x0110:
						// Exif.Image.Model mapped to "Manufacturer"
						mmapAttributes.insert(pair<int, string>(ATTRIBUTETYPE_Manufacturer_CONST, i->value().toString()));
						break;
					case 0x0132:
						// Exif.Image.DateTime mapped to "Year"
						mmapAttributes.insert(pair<int, string>(ATTRIBUTETYPE_Year_CONST, i->value().toString()));
						break;
					case 0x013b:
						// Artist mapped to "Composer/Writer"
						mmapAttributes.insert(pair<int, string>(ATTRIBUTETYPE_ComposerWriter_CONST, i->value().toString()));
						break;
						
					case 271:
						// TODO Exif.Image.Make
						break;
					case 305:
						// TODO Exif.Image.Software
						break;
					case 33434:
						// TODO Exif.Photo.ExposureTime
						break;
					case 33437:
						// TODO Exif.Photo.FNumber
						break;
					case 34855:
						// TODO Exif.Photo.ISOSpeedRatings
						break;
					case 37377:
						// TODO Exif.Photo.ShutterSpeedValue
						break;
					case 37378:
						// TODO Exif.Photo.ApertureValue
						break;
					case 37386:
						// TODO Exif.Photo.Flash
						break;
					case 37510:
						// TODO Exif.Photo.UserComment mapped to "Synopsis"
						break;
					case 274:
						// TODO Exif.Image.Orientation
						break;
					case 34850:
						// TODO Exif.Photo.ExposureProgram
						break;
					case 37380:
						// TODO ExposureBiasValue
						break;
						
						// TODO GPSLatitude, GPSLongitude, GPSAltitude mapped to "Region"
						// TODO GPSSpeed, GPSTrack, GPSImageDirection, GPSDestLatitude, GPSDestLongitude
						// TODO BrightnessValue
						// TODO SubjectDistance
						// TODO MeteringMode
						// TODO LightSource
						// TODO FlashEnergy
						// TODO SceneCapturedType
						
					}
					
				}
				//						LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::getTagsFromFile: Found EXIF data \"%s\"", sExifAttributesPlain.c_str());
			}
			if (iptcData.empty()) {
				LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::getTagsFromFile: File %s does not contain any IPTC information",
								    sFilename.c_str());
			} else {
				string sIptcAttributesPlain = "";
				
				Exiv2::IptcData::iterator end = iptcData.end();
				Exiv2::IptcData::iterator charSetIt = iptcData.findKey(Exiv2::IptcKey("Iptc.Envelope.CharacterSet"));
				string sourceCharset = "ISO-8859-1"; // default to ISO-8859-1 if nothing found below (this is the same behaviour as exiftool)
				if (charSetIt != end) {
					LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::getTagsFromFile: Found IPTC charset \"%s\"", charSetIt->print().c_str());
				}
				for (Exiv2::IptcData::iterator i = iptcData.begin(); i != end; ++i) {
					LoggerWrapper::GetInstance()->Write(LV_MEDIA, "# PhotoFileHandler::getTagsFromFile: Found IPTC data '%s': '%s'",  i->key().c_str(), i->value().toString().c_str());
					
					// map some attributes to LMCE media attributes
					switch(i->tag()) {
					case 0x0019:
						// Iptc.Application2.Keywords mapped to "Keyword"
						string value = i->value().toString();
						if (!Exiv2::convertStringCharset(value, sourceCharset.c_str(), "UTF-8")) {
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::getTagsFromFile: Exiv2::convertStringCharset unable to convert from %s to UTF-8", sourceCharset.c_str());
						}
						if (value[value.length()-1] == 0) {
							value = value.substr(0, value.length()-1);
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::getTagsFromFile: removing hex 0 (null-termination) from value \"%s\"", value.c_str());
						}

						mmapAttributes.insert(pair<int, string>(ATTRIBUTETYPE_Keyword_CONST, value));
						break;
					}
					
					
				}
			}
		}
	} catch (Exiv2::AnyError& e) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "# PhotoFileHandler::getTagsFromFile() Exiv::AnyError: %s", e.what());
	}
}





