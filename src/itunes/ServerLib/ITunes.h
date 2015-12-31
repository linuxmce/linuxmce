/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:45 (local)
    --------------------------------------------------------------
    Purpose:    Apple's SDK for iTunes wrapper
**************************************************************************/

#include "ITunesExceptions.h"
#include "LibraryWritable.h"
#include "api.h"

#ifndef __HEADER_INCLUDED_ITUNES__
#define __HEADER_INCLUDED_ITUNES__

struct IiTunes;
struct IITTrack;
struct IConnectionPoint;

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

class ITunesEventsSink;

/// Apple's SDK for iTunes wrapper
class PITMCB_API ITunes  {

public:

	ITunes(void);
	virtual ~ITunes(void);

	/**
	 * Makes a playlist copy into local library
	 *
	 * @throw Exceptions::ITunesCom
	 */
	ITunes& operator >>(LibraryWritable&);

	/**
	 * Attaches object to iTunes or opens new instance.
	 * Sets the iTunesUserQuitEvent-event if user quits from iTunes.
	 *
	 * @throw Exceptions::ITunesCom
	 */
	void attach(HANDLE iTunesUserQuitEvent);

	/// Detach object from iTunes
	void detach();

	/// Close a iTunes and detaches
	void stopApplication();

	/**
	 * Starts a watching over the iTunes-library changes and updates 
	 * local library. Returns an update event handler.
	 *
	 */
	HANDLE watchOverITunesLibraryChanges(const std::wstring& userSid);

	/// Stops a watching over the iTunes-library changes.
	void stopWatchOverITunesLibraryChanges();

	/// Event - iTunes database was changed
	void onITunesDatabaseChanged(VARIANT deletedObjectIDs, VARIANT changedObjectIDs);

protected:

	/**
	 * Fills and returns a local file structure.
	 * Returns a FALSE if track is not local.
	 *
	 * @throw Exceptions::ITunesCom
	 */
	BOOL fillLocalTrack(	Library::LocalTrackInfo &track,
							IITTrack *const iTunesTrack) throw(Exceptions::ITunesCom);

	/**
	 * Returns a file path for track or empty string if none
	 *
	 * @throw Exceptions::ITunesCom
	 */
	std::wstring getTrackFileLocation(IITTrack *const track) throw(Exceptions::ITunesCom);

private:

	typedef IiTunes* IiTunesPtr;
	typedef IiTunes*const IiTunesCPtr;

	/// Internal pointer for top-level iTunes application object
	IiTunesPtr _iTunes;

	/// Executes a COM-method and checks a result (throws a exception if error)
	template<	typename ComObjectPtr,
				typename MethodPtr,
				typename T>
	inline void executeComMethod(	ComObjectPtr objPtr,
									MethodPtr methodPtr,
									T param) throw(Exceptions::ITunesCom) {
		checkComOperationResult((objPtr->*methodPtr)(param));
	}

	/// Executes a COM-method and checks a result (throws a exception if error)
	template<	typename ComObjectPtr,
				typename MethodPtr,
				typename T1,
				typename T2>
	inline void executeComMethod(	ComObjectPtr objPtr,
									MethodPtr methodPtr,
									T1 param1,
									T2 param2) throw(Exceptions::ITunesCom) {
		checkComOperationResult((objPtr->*methodPtr)(param1, param2));
	}

	/// Sets the iTunesUserQuitEvent-event if user quits from iTunes.
	HANDLE _iTunesDetachEvent;

	/// update iTunes DB event
	HANDLE _iTunesDbUpdateEvent;

	/// Library for updating
	LibraryWritable *_library4Update;

	/// Events sink
	ITunesEventsSink * _iTunesEventsSink;
	IConnectionPoint * _iTunesEventsSinkCPoint;

	/// Checks a operation result and throws a exception if error
	inline void checkComOperationResult(HRESULT operationResult) throw(Exceptions::ITunesCom);

private:

	class TrackStreamRedirectorType {
	public:
		virtual void append(const Library::LocalTrackInfo&) = 0;
	};

	template<typename Stream>
	class TrackStreamRedirector : public ITunes::TrackStreamRedirectorType {
	public:
		TrackStreamRedirector(Stream& stream)
		:	_stream(stream) {
			/*...*/
		}
		~TrackStreamRedirector() {
			/*...*/
		}
		void append(const Library::LocalTrackInfo &trackInfo) {
			_stream << trackInfo;
		}
	private:
		Stream& _stream;
	};

	void getTracksList(ITunes::TrackStreamRedirectorType*const) throw(Exceptions::ITunesCom);


	class TrackActionRedirectorType {
	public:
		TrackActionRedirectorType(ITunes& iTunes)
		:	_iTunes(iTunes) {
			/*...*/
		}
		virtual ~TrackActionRedirectorType() {
			/*...*/
		}
		virtual void process(	const long sourceId,
								const long playlistId,
								const long trackId,
								const long databaseId) = 0;
	protected:
		ITunes& _iTunes;
	};

	template<class ProcessPolicy>
	class TrackActionRedirector : public TrackActionRedirectorType {
	public:
		TrackActionRedirector(ITunes& iTunes)
		:	TrackActionRedirectorType(iTunes) {
			/*...*/
		}
		void process(	const long sourceId,
						const long playlistId,
						const long trackId,
						const long databaseId) {
			ProcessPolicy::process(_iTunes, sourceId, playlistId, trackId, databaseId);
		}
	};

	struct TrackActionRedirectorRemovingPolicy {
		static void process(ITunes&, const long, const long, const long, const long) throw(Exceptions::ITunesCom);
	};

	struct TrackActionRedirectorAddingPolicy {
		static void process(ITunes&, const long, const long, const long, const long) throw(Exceptions::ITunesCom);
	};

	void updateDbFromEvent(	SAFEARRAY *const changedIds,
							TrackActionRedirectorType *const actionRedirector);

};


	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_ITUNES__
