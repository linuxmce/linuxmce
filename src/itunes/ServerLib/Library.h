/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:51 (local)
    --------------------------------------------------------------
    Purpose:    Library
**************************************************************************/

#include "LibraryExceptions.h"

#ifndef __HEADER_INCLUDED_LIBRARY__
#define __HEADER_INCLUDED_LIBRARY__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

// Library 
class Library {

public:

	/// Track info
	struct TrackInfo {
		/// Local database ID
		unsigned long id;
		/// iTunes-DB ID
		long dataSourceId;
		/// Album
		std::wstring album;
		/// Artist
		std::wstring artist;
		/// Length of the track (in seconds)
		long durationSec;
		/// Text description of the track (e.g. "AAC audio file")
		std::wstring kind;
		// size of the track (in bytes)
		long size;
	};

	/// Local track (in file) info
	struct LocalTrackInfo : public TrackInfo {
		/// File name
		std::wstring file;
	};

public:

	Library(void);
	virtual ~Library(void);

	virtual void load();

	static const std::wstring& getDirForStorage();

	/// Returns a path to the library XML-file
	static const std::wstring& getPathToStorage();

	/// Creates a new blank library
	virtual void createNew();

	/// Returns a path by track ID
	virtual BOOL getFilePathByTrackId(	unsigned int id,
										std::wstring& path,
										unsigned long& fileSize);

	virtual BOOL getAllIds(std::vector<unsigned long>&);

	virtual BOOL getAllTracks(std::vector<Library::LocalTrackInfo>&);

private:

	Library(const Library&);
	void operator=(const Library&);

protected:

	/// DOM document
	MSXML2::IXMLDOMDocument2Ptr _doc;
	/// Document root
	MSXML2::IXMLDOMElementPtr _root;
	/// Tracks list node
	MSXML2::IXMLDOMElementPtr _tracks;

	/// last inserted track ID
	unsigned long _lastTrackId;

	/// Path to the library XML-file
	static std::wstring _pathToStorage;
	static std::wstring _dirForStorage;

	virtual void release();

	virtual void createNewDocumentObject();

	MSXML2::IXMLDOMNodePtr selectTrackNodeById(const unsigned long);
	
};

	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_LIBRARY__
