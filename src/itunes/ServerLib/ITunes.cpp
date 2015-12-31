
#include "stdafx.h"
#include "ITunes.h"
#include "ITunesExceptions.h"
#include "ITunesEventsSink.h"
#include "Log.h"

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster;


ITunes::ITunes(void)
:	_iTunes(NULL)
,	_iTunesEventsSink(NULL)
,	_iTunesDetachEvent(NULL)
,	_iTunesDbUpdateEvent(NULL)
,	_library4Update(NULL) {

	REGISTER_RUNTIME_POINT("ITunes::ITunes", __FILE__, __LINE__);

}


ITunes::~ITunes(void) {
	REGISTER_RUNTIME_POINT("ITunes::~ITunes", __FILE__, __LINE__);
	detach();
}


void ITunes::stopApplication() {
	REGISTER_RUNTIME_POINT("ITunes::stopApplication", __FILE__, __LINE__);
	_iTunes->Quit();
	detach();
}


void ITunes::attach(HANDLE iTunesUserQuitEvent) {

	REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);

	if (_iTunes) {
		REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);
		return;
	}

	HRESULT hResult = ::CoCreateInstance(
		CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, reinterpret_cast<PVOID*>(&_iTunes));
	try {

		unsigned int errorsCount = 0;

		for (; errorsCount < 20; ++errorsCount) {

			try {
		
				REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);

				checkComOperationResult(hResult);
				if (!_iTunes) {
					REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);
					throw Exceptions::ITunesCom(hResult);
				}
				
				IConnectionPointContainer  * pCPC;
				_iTunes->QueryInterface(IID_IConnectionPointContainer, (void **)&pCPC);
				
				_IiTunesEvents * pITEvent;
				pCPC->FindConnectionPoint (__uuidof(pITEvent), &_iTunesEventsSinkCPoint);
				pCPC->Release();

				if (!_iTunesDetachEvent) {
					REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);
					_iTunesDetachEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
				}
				ResetEvent(_iTunesDetachEvent);

				IUnknown * pSinkUnk;
				_iTunesEventsSink = new ITunesEventsSink(*this, iTunesUserQuitEvent, _iTunesDetachEvent);
				_iTunesEventsSink->QueryInterface(IID_IUnknown, (void**)&pSinkUnk);

				DWORD cookie;
				_iTunesEventsSinkCPoint->Advise(pSinkUnk, &cookie);

				break;

			} catch (Exceptions::ITunesCom) {
				REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);
				++errorsCount;
				Sleep(5000);
			}

		}

		if (!(errorsCount < 20)) {
			REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);
			detach();
		}

	} catch (...) {
		REGISTER_RUNTIME_POINT("ITunes::attach", __FILE__, __LINE__);
		detach();
		throw;
	}

}


void ITunes::detach() {
	REGISTER_RUNTIME_POINT("ITunes::detach", __FILE__, __LINE__);
	stopWatchOverITunesLibraryChanges();
	if (_iTunesDetachEvent) {
		REGISTER_RUNTIME_POINT("ITunes::detach", __FILE__, __LINE__);
		SetEvent(_iTunesDetachEvent);
		CloseHandle(_iTunesDetachEvent);
		_iTunesDetachEvent = NULL;
	}
	if (_iTunes) {
		REGISTER_RUNTIME_POINT("ITunes::detach", __FILE__, __LINE__);
		_iTunes->Release();
		_iTunes = NULL;
	}
	if (_iTunesEventsSink) {
		REGISTER_RUNTIME_POINT("ITunes::detach", __FILE__, __LINE__);
		_iTunesEventsSink->Release();
		_iTunesEventsSinkCPoint->Release();
		_iTunesEventsSink = NULL;
	}
}


void ITunes::checkComOperationResult(HRESULT operationResult) throw(Exceptions::ITunesCom) {
	if (operationResult != S_OK) {
		throw Exceptions::ITunesCom(operationResult);
	}
}


ITunes& ITunes::operator>>(LibraryWritable& library) {
	REGISTER_RUNTIME_POINT("ITunes::operator>>", __FILE__, __LINE__);
	TrackStreamRedirector<LibraryWritable> streamRedirector(library);
	try {
		getTracksList(&streamRedirector);
	} catch (Exceptions::ITunesCom) {
		REGISTER_RUNTIME_POINT("ITunes::operator>>", __FILE__, __LINE__);
	}
	return *this;
}

void ITunes::getTracksList(ITunes::TrackStreamRedirectorType *const streamRedirector) throw(Exceptions::ITunesCom) {
	
	REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);

	IITSource *source = NULL;
	IITPlaylistCollection *playlists = NULL;
	long playlistsCount = 0;

	executeComMethod(_iTunes, &IiTunes::get_LibrarySource, &source);

	try {

		executeComMethod(source, &IITSource::get_Playlists, &playlists);
		executeComMethod(playlists, &IITPlaylistCollection::get_Count, &playlistsCount);

		set<long> ids;

		for (long i = 1; i <= playlistsCount; ++i) {

			REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
			
			IITPlaylist *playlist = NULL;
			IITTrackCollection *tracks = NULL;
			long tracksCount = 0;

			executeComMethod(playlists, &IITPlaylistCollection::get_Item, i, &playlist);

			try {
				executeComMethod(playlist, &IITPlaylist::get_Tracks, &tracks);
				executeComMethod(tracks, &IITTrackCollection::get_Count, &tracksCount);

				for (long j = 1; j <= tracksCount; ++j) {
					REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
					IITTrack *track = NULL;
					executeComMethod(tracks, &IITTrackCollection::get_Item, j, &track);
					try {
					
						if (track) {
							REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
							long id;
							executeComMethod(track, &IITTrack::get_TrackDatabaseID, &id);
							if (ids.find(id) == ids.end()) {
								REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
								Library::LocalTrackInfo trackStructure;
								try {
									if (fillLocalTrack(trackStructure, track)) {
										REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
										streamRedirector->append(trackStructure);
									}
									ids.insert(id);
								} catch (const Exceptions::ITunesCom) {
									REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
								}
							}
						}

						REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
						track->Release();
					
					} catch (...) {
						REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
						track->Release();
						throw;
					}

				}

				REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
				tracks->Release();
				REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
				playlist->Release();

			} catch (...) {
				if (tracks) {
					REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
					tracks->Release();
				}
				REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
				playlist->Release();
				throw;
			}

		}

		REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
		playlists->Release();
		REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
		source->Release();

	} catch (...) {
		if (playlists) {
			REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
			playlists->Release();
		}
		REGISTER_RUNTIME_POINT("ITunes::getTracksList", __FILE__, __LINE__);
		source->Release();
		throw;
	}

}


BOOL ITunes::fillLocalTrack(Library::LocalTrackInfo &track, IITTrack *const iTunesTrack) throw(Exceptions::ITunesCom) {
	REGISTER_RUNTIME_POINT("ITunes::fillLocalTrack", __FILE__, __LINE__);
	track.file = getTrackFileLocation(iTunesTrack);
	const BOOL isLocal = !track.file.empty();
	if (isLocal) {
		REGISTER_RUNTIME_POINT("ITunes::fillLocalTrack", __FILE__, __LINE__);
		BSTR buffer;
		long lBuffer;
		executeComMethod(iTunesTrack, &IITFileOrCDTrack::get_TrackDatabaseID, &lBuffer);
		track.dataSourceId = lBuffer;
		executeComMethod(iTunesTrack, &IITFileOrCDTrack::get_Album, &buffer);
		if (buffer) {
			REGISTER_RUNTIME_POINT("ITunes::fillLocalTrack", __FILE__, __LINE__);
			track.album = buffer;
		}
		executeComMethod(iTunesTrack, &IITFileOrCDTrack::get_Artist, &buffer);
		if (buffer) {
			REGISTER_RUNTIME_POINT("ITunes::fillLocalTrack", __FILE__, __LINE__);
			track.artist = buffer;
		}
		executeComMethod(iTunesTrack, &IITFileOrCDTrack::get_Duration, &lBuffer);
		track.durationSec = lBuffer;
		executeComMethod(iTunesTrack, &IITFileOrCDTrack::get_KindAsString, &buffer);
		if (buffer)  {
			REGISTER_RUNTIME_POINT("ITunes::fillLocalTrack", __FILE__, __LINE__);
			track.kind = buffer;
		}
		executeComMethod(iTunesTrack, &IITFileOrCDTrack::get_Size, &lBuffer);
		track.size = lBuffer;
	};
	return isLocal;
}


wstring ITunes::getTrackFileLocation(IITTrack *const track) throw(Exceptions::ITunesCom) {

	REGISTER_RUNTIME_POINT("ITunes::getTrackFileLocation", __FILE__, __LINE__);

	ITTrackKind trackKind = ITTrackKindUnknown;
	IITFileOrCDTrack *fileTrack = NULL;
	CComBSTR location = "";

	executeComMethod(track, &IITTrack::get_Kind, &trackKind);
	switch (trackKind) {
		case ITTrackKindFile: // (1) File track (IITFileOrCDTrack).
		case ITTrackKindCD: // (2) CD track (IITFileOrCDTrack).
			REGISTER_RUNTIME_POINT("ITunes::getTrackFileLocation", __FILE__, __LINE__);
			try {
				executeComMethod(track, &IITTrack::QueryInterface, __uuidof(fileTrack), (void**)&fileTrack);
				executeComMethod(fileTrack, &IITFileOrCDTrack::get_Location, &location);
				fileTrack->Release();
			} catch (...) {
				if (fileTrack) {
					fileTrack->Release();
				}
				throw;
			}
			break;
		case ITTrackKindURL: // (3) URL track (IITURLTrack).
		case ITTrackKindDevice: // (4) Device track.
		case ITTrackKindUnknown:  // (0) Unknown track kind.  
		case ITTrackKindSharedLibrary: // (5) Shared library track.  
		default:
			REGISTER_RUNTIME_POINT("ITunes::getTrackFileLocation", __FILE__, __LINE__);
			break;
	}

	return wstring(location);

}


HANDLE ITunes::watchOverITunesLibraryChanges(const std::wstring& userSid) {
	REGISTER_RUNTIME_POINT("ITunes::watchOverITunesLibraryChanges", __FILE__, __LINE__);
	if (!_iTunesDbUpdateEvent) {
		REGISTER_RUNTIME_POINT("ITunes::watchOverITunesLibraryChanges", __FILE__, __LINE__);
		_iTunesDbUpdateEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		ResetEvent(_iTunesDbUpdateEvent);
	}
	if (_library4Update) {
		REGISTER_RUNTIME_POINT("ITunes::watchOverITunesLibraryChanges", __FILE__, __LINE__);
		delete _library4Update;
	}
	_library4Update = new LibraryWritable(userSid);
	_library4Update->load();
	TrackStreamRedirector<LibraryWritable::IdsCache> streamRedirector(_library4Update->idsCache);
	try {
		getTracksList(&streamRedirector);
	} catch (Exceptions::ITunesCom) {
		REGISTER_RUNTIME_POINT("ITunes::watchOverITunesLibraryChanges", __FILE__, __LINE__);
	}

	if (_library4Update->isModified()) {
		REGISTER_RUNTIME_POINT("ITunes::watchOverITunesLibraryChanges", __FILE__, __LINE__);
			_library4Update->save();
		SetEvent(_iTunesDbUpdateEvent);
	}

	return _iTunesDbUpdateEvent;
}


void ITunes::stopWatchOverITunesLibraryChanges() {
	REGISTER_RUNTIME_POINT("ITunes::stopWatchOverITunesLibraryChanges", __FILE__, __LINE__);
	if (_iTunesDbUpdateEvent) {
		REGISTER_RUNTIME_POINT("ITunes::stopWatchOverITunesLibraryChanges", __FILE__, __LINE__);
		CloseHandle(_iTunesDbUpdateEvent);
		_iTunesDbUpdateEvent = NULL;
	}
	if (_library4Update) {
		REGISTER_RUNTIME_POINT("ITunes::stopWatchOverITunesLibraryChanges", __FILE__, __LINE__);
		delete _library4Update;
		_library4Update = NULL;
	}
}


void ITunes::onITunesDatabaseChanged(	VARIANT deletedObjectIDs,
										VARIANT changedObjectIDs) {
	
	REGISTER_RUNTIME_POINT("ITunes::onITunesDatabaseChanged", __FILE__, __LINE__);

	if (!_iTunesDbUpdateEvent) {
		REGISTER_RUNTIME_POINT("ITunes::onITunesDatabaseChanged", __FILE__, __LINE__);
		return;
	}

	TrackActionRedirector<TrackActionRedirectorRemovingPolicy> toRemove(*this);
	updateDbFromEvent(deletedObjectIDs.parray, &toRemove);

	TrackActionRedirector<TrackActionRedirectorAddingPolicy> toAdd(*this);
	updateDbFromEvent(changedObjectIDs.parray, &toAdd);

	_library4Update->save();
	SetEvent(_iTunesDbUpdateEvent);

}


void ITunes::updateDbFromEvent(SAFEARRAY *const changedIds,
							   TrackActionRedirectorType *const actionRedirector) {

	REGISTER_RUNTIME_POINT("ITunes::updateDbFromEvent", __FILE__, __LINE__);

	wostringstream ss;

	long l, u;
	SafeArrayGetLBound(changedIds, 1, &l);
	SafeArrayGetUBound(changedIds, 1, &u);

	set<long> ids;

	for (long i = l; i <= u; ++i) {

		REGISTER_RUNTIME_POINT("ITunes::updateDbFromEvent", __FILE__, __LINE__);
		
		VARIANT sourceId, playlistId, trackId, databaseId;
		
		long pos[2] = {i, 3};
		SafeArrayGetElement(changedIds, pos, (void*)&databaseId);
		if (databaseId.lVal && ids.find(databaseId.lVal) == ids.end()) {

			REGISTER_RUNTIME_POINT("ITunes::updateDbFromEvent", __FILE__, __LINE__);
			
			pos[1] = 2;
			SafeArrayGetElement(changedIds, pos, (void*)&trackId);			
			if (trackId.lVal) {
				
				REGISTER_RUNTIME_POINT("ITunes::updateDbFromEvent", __FILE__, __LINE__);

				pos[1] = 0;
				SafeArrayGetElement(changedIds, pos, (void*)&sourceId);
				pos[1] = 1;
				SafeArrayGetElement(changedIds, pos, (void*)&playlistId);

				try {
					actionRedirector->process(
						sourceId.lVal, playlistId.lVal, trackId.lVal, databaseId.lVal);
					ids.insert(databaseId.lVal);
				} catch (Exceptions::ITunesCom) {
					REGISTER_RUNTIME_POINT("ITunes::updateDbFromEvent", __FILE__, __LINE__);
				}

			}
		
		}

	}

}

void ITunes::TrackActionRedirectorRemovingPolicy::process(	ITunes& iTunes,
															const long,
															const long,
															const long,
															const long trackDatabaseId) throw(Exceptions::ITunesCom) {

	REGISTER_RUNTIME_POINT("ITunes::TrackActionRedirectorRemovingPolicy::process", __FILE__, __LINE__);
	iTunes._library4Update->idsCache.remove(trackDatabaseId);

}


void ITunes::TrackActionRedirectorAddingPolicy::process(	ITunes& iTunes,
															const long sourceId,
															const long playlistId,
															const long trackId,
															const long databaseId) throw(Exceptions::ITunesCom) {

	
	REGISTER_RUNTIME_POINT("ITunes::TrackActionRedirectorAddingPolicy::process", __FILE__, __LINE__);

	IITObject *obj = NULL;
	iTunes._iTunes->GetITObjectByID(sourceId, playlistId, trackId, databaseId, &obj);

	if (obj) {
		
		REGISTER_RUNTIME_POINT("ITunes::TrackActionRedirectorAddingPolicy::process", __FILE__, __LINE__);

		IITTrack* track = NULL;

		try {

			iTunes.executeComMethod(
				obj, &IITObject::QueryInterface, __uuidof(track), (void**)&track);
						
			Library::LocalTrackInfo trackInfo;
			if (iTunes.fillLocalTrack(trackInfo, track)) {
				REGISTER_RUNTIME_POINT("ITunes::TrackActionRedirectorAddingPolicy::process", __FILE__, __LINE__);
				iTunes._library4Update->operator<<(trackInfo);
			}
			track->Release();
			obj->Release();
		} catch (...) {
			REGISTER_RUNTIME_POINT("ITunes::TrackActionRedirectorAddingPolicy::process", __FILE__, __LINE__);
			if (track) {
				track->Release();
			}
			obj->Release();
			throw;
		}
	
	}

}

