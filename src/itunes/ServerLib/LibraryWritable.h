/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:51 (local)
    --------------------------------------------------------------
    Purpose:	Writable library
**************************************************************************/

#include "Library.h"
#include "api.h"

#ifndef __HEADER_INCLUDED_LIBRARYWRITABLE__
#define __HEADER_INCLUDED_LIBRARYWRITABLE__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

// Writable library 
class LibraryWritable : public Library {

public:

	/// Binds a local and iTunes IDs.
	class IdsCache {
		friend class LibraryWritable;
	public:
		IdsCache(LibraryWritable&);
		~IdsCache();
		/// Binds a local and iTunes IDs. Adds new tracks into local database.
		IdsCache& operator<<(const Library::LocalTrackInfo&);
		/// Removes a track by iTunes database ID
		void remove(const long iTunesDatabaseId);
	private:
		IdsCache(const IdsCache&);
		IdsCache& operator=(const IdsCache&);
	private:
		typedef std::map<long, unsigned long> IdsRelations;
		IdsRelations _idsRelations;
		LibraryWritable& _lib;
	} idsCache;

public:

	LibraryWritable(const std::wstring& onwerSid);
	~LibraryWritable(void);

	LibraryWritable& operator <<(const Library::LocalTrackInfo&);

	/// Removes a track by ID
	void remove(const unsigned long trackId);

private:

	LibraryWritable(const LibraryWritable&);
	void operator=(const LibraryWritable&);

public:

	/**
	 * Saves a document
	 *
	 */
	void save();

	void load();

	/**
	 * Creates a new blank library
	 *
	 */
	void createNew();

	/// Returns a TRUE if local database was modified
	BOOL isModified() const;

protected:

	/// Modification flag
	BOOL _isModified;

	/// SID of the records owner
	const _variant_t _ownerSid;

	/// Appends a new track. Returns a track ID.
	unsigned long appendTrack(const Library::LocalTrackInfo&);

	/// Updates a track and registers a new SID into track-node. Returns a track ID.
	unsigned long updateTrack(	const Library::LocalTrackInfo&,
								MSXML2::IXMLDOMElementPtr trackNode);

	virtual void createNewDocumentObject();

	MSXML2::IXMLDOMElementPtr selectTrackNodeByFileName(std::wstring);

private:

	void fillTrackInfoNode(	const Library::LocalTrackInfo&,
							MSXML2::IXMLDOMElementPtr trackNode);

};

	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_LIBRARYWRITABLE__
