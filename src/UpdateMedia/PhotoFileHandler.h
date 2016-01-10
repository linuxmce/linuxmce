/*
 * PhotoFileHandler.h
 *
 *  Scans photos, i.e., JPG files, for usable metadata in EXIF and IPTC format
 *  @author: Christian Schleiffer <christian@schleiffer.net>
 *
 *  The following EXIF fields will be synchronized with the given mapping (EXIF -> Attribute):
 *    * Exif.Image.DateTime           -> Year
 *    * Exif.?.ImageDescription       -> Title
 *    * Exif.?.Artist                 -> Composer/Writer
 *    * Exif.Image.Model              -> Manufacturer
 *    * Iptc.Application2.Keywords    -> Keyword
 *
 *    Some more mappings are prepared but have to be implemented yet.
 */

#ifndef __PHOTO_FILE_HANDLER_H__
#define __PHOTO_FILE_HANDLER_H__

#include "GenericFileHandler.h"

#include <map>
#include <list>
using namespace std;

/**
 * Extracting EXIF and IPTC metadata from jpg files
 */
class PhotoFileHandler: public GenericFileHandler {
public:
	/**
	 * Constructor for a new file handler to process JPEG files (.jpg)
	 * @param	sDirectory		Directory of the file to process (string)
	 * @param	sFile			Name of the file to process (string)
	 */
	PhotoFileHandler(string sDirectory, string sFile);
	~PhotoFileHandler();

	string AttributeToTag(int attribute);

	/**
	 * Loads attributes and thumbnails from the file.
	 * @param	pPlutoMediaAttributes	Returns all attributes found in the file (multimap)
	 * @param	listPicturesForTags		???
	 */
	bool LoadAttributes(PlutoMediaAttributes *pPlutoMediaAttributes, list<pair<char *, size_t> >& listPicturesForTags);

	/**
	 *
	 */
	bool SaveAttributes(PlutoMediaAttributes *pPlutoMediaAttributes);

	/**
	 *
	 */
	bool RemoveAttribute(int nTagType, string sValue, PlutoMediaAttributes *pPlutoMediaAttributes);

	/**
	 *
	 */
	bool FileAttributeExists();

	/**
	 *
	 */
	string GetFileAttribute();

	/**
	 *
	 */
	string GetFileSourceForDB();

protected:
	/**
	 * Read embedded EXIF and IPTC tags from a file
	 * @param sFilename			The full path of the file to read the embedded tags from (string)
	 * @param mmapAttributes	Embedded attributes mapped to LMCE attributes (multimap)
	 * @param listPictures 		?
	 */
	void getTagsFromFile(string sFilename, multimap<int,string>& mmapAttributes, list<pair<char *, size_t> >& listPictures);
};

#endif /* __PHOTO_FILE_HANDLER_H__ */
