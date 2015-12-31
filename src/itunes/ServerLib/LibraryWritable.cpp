
#include "stdafx.h"
#include "LibraryWritable.h"
#include "Log.h"

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster;


LibraryWritable::IdsCache::IdsCache(LibraryWritable& lib)
:	_lib(lib) {
	REGISTER_RUNTIME_POINT("LibraryWritable::IdsCache::IdsCache", __FILE__, __LINE__);
}


LibraryWritable::IdsCache::~IdsCache() {
	REGISTER_RUNTIME_POINT("LibraryWritable::IdsCache::~IdsCache", __FILE__, __LINE__);
}


void LibraryWritable::IdsCache::remove(const long iTunesDatabaseId) {
	REGISTER_RUNTIME_POINT("LibraryWritable::IdsCache::remove", __FILE__, __LINE__);
	IdsRelations::iterator toDel = _idsRelations.find(iTunesDatabaseId);
	if (toDel != _idsRelations.end()) {
		REGISTER_RUNTIME_POINT("LibraryWritable::IdsCache::remove", __FILE__, __LINE__);
		_lib.remove(toDel->second);
		// TODO: move removing to LibraryWritable (see LibraryWritable::remove)
		_idsRelations.erase(toDel);
	}
}


LibraryWritable::IdsCache& LibraryWritable::IdsCache::operator<<(const Library::LocalTrackInfo& trackInfo) {

	REGISTER_RUNTIME_POINT("LibraryWritable::IdsCache::operator<<", __FILE__, __LINE__);

	MSXML2::IXMLDOMElementPtr track = _lib.selectTrackNodeByFileName(trackInfo.file);
		
	if (track != NULL) {
		REGISTER_RUNTIME_POINT("LibraryWritable::IdsCache::operator<<", __FILE__, __LINE__);
		wistringstream ss(track->getAttribute(L"Id").bstrVal);
		ss >> _idsRelations[trackInfo.dataSourceId];
	} else {
		REGISTER_RUNTIME_POINT("LibraryWritable::IdsCache::operator<<", __FILE__, __LINE__);
		_idsRelations[trackInfo.dataSourceId] = _lib.appendTrack(trackInfo);
	}
	
	return *this;

}


LibraryWritable::LibraryWritable(const wstring& onwerSid)
:	idsCache(*this)
,	_isModified(FALSE)
,	_ownerSid(onwerSid.c_str()) {
	REGISTER_RUNTIME_POINT("LibraryWritable::LibraryWritable", __FILE__, __LINE__);
}


LibraryWritable::~LibraryWritable(void) {
	REGISTER_RUNTIME_POINT("LibraryWritable::~LibraryWritable", __FILE__, __LINE__);
}


LibraryWritable& LibraryWritable::operator<<(const Library::LocalTrackInfo& trackInfo) {

	REGISTER_RUNTIME_POINT("LibraryWritable::operator<<", __FILE__, __LINE__);

	MSXML2::IXMLDOMNodePtr trackNode = selectTrackNodeByFileName(trackInfo.file);
	idsCache._idsRelations[trackInfo.dataSourceId] = trackNode
		?	updateTrack(trackInfo, trackNode)
		:	appendTrack(trackInfo);
	
	return *this;

}


void LibraryWritable::remove(const unsigned long trackId) {
	REGISTER_RUNTIME_POINT("LibraryWritable::remove", __FILE__, __LINE__);
	MSXML2::IXMLDOMNodePtr track = selectTrackNodeById(trackId);
	if (track != NULL) {
		REGISTER_RUNTIME_POINT("LibraryWritable::remove", __FILE__, __LINE__);
		wostringstream ss;
		ss << L"User[@Sid != '" << _ownerSid.bstrVal << L"']";
		if (track->selectSingleNode(ss.str().c_str()) != NULL) {
			REGISTER_RUNTIME_POINT("LibraryWritable::remove", __FILE__, __LINE__);
			ss.str(L"");
			ss << L"User[@Sid = '" << _ownerSid.bstrVal << L"']";
			MSXML2::IXMLDOMNodePtr user = track->selectSingleNode(ss.str().c_str());
			if (user != NULL) {
				track->removeChild(user);
				_isModified = TRUE;
			}
		} else {
			REGISTER_RUNTIME_POINT("LibraryWritable::remove", __FILE__, __LINE__);
			_tracks->removeChild(track);
			_isModified = TRUE;
		}
		// TODO: remove from cache (see IdsCache::remove)
	}
}


void LibraryWritable::createNew() {
	REGISTER_RUNTIME_POINT("LibraryWritable::createNew", __FILE__, __LINE__);
	Library::createNew();
	_isModified = TRUE;
}

void LibraryWritable::createNewDocumentObject() {
	REGISTER_RUNTIME_POINT("LibraryWritable::createNewDocumentObject", __FILE__, __LINE__);
	Library::createNewDocumentObject();
	_doc->setProperty(L"SelectionLanguage", L"XPath");
}

void LibraryWritable::load() {
	REGISTER_RUNTIME_POINT("LibraryWritable::load", __FILE__, __LINE__);
	_isModified = FALSE;
	Library::load();
}

void LibraryWritable::save() {

	REGISTER_RUNTIME_POINT("LibraryWritable::save", __FILE__, __LINE__);
	
	if (!_isModified) {
		REGISTER_RUNTIME_POINT("LibraryWritable::save", __FILE__, __LINE__);
		return;
	}
	
	wostringstream stringSream;

	stringSream << _lastTrackId;
	const _variant_t lastId = stringSream.str().c_str();
	_tracks->setAttribute(L"LastId", lastId);
	stringSream.str(L"");

	__time64_t ltime;
	tzset();
	_time64(&ltime);
	stringSream << ltime;
	_root->setAttribute(L"ModificationTime", _variant_t(stringSream.str().c_str()));

	LPCWSTR file = getPathToStorage().c_str();
	LPCWSTR dir = getDirForStorage().c_str();

	SECURITY_DESCRIPTOR secDesc;
	SECURITY_ATTRIBUTES sa;
	InitializeSecurityDescriptor(&secDesc, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&secDesc, TRUE, NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = &secDesc;
	sa.bInheritHandle = FALSE;

	if (!PathIsDirectoryW(dir)) {
		REGISTER_RUNTIME_POINT("LibraryWritable::save", __FILE__, __LINE__);
		CreateDirectoryW(dir, &sa);
	}
	try {
		REGISTER_RUNTIME_POINT("LibraryWritable::save", __FILE__, __LINE__);
		_doc->save(file);
		SetFileSecurityW(file, DACL_SECURITY_INFORMATION, &secDesc);
	} catch (_com_error) {
		REGISTER_RUNTIME_POINT("LibraryWritable::save", __FILE__, __LINE__);
	}
	

	_isModified = FALSE;

}

unsigned long LibraryWritable::appendTrack(const Library::LocalTrackInfo& trackInfo) {

	REGISTER_RUNTIME_POINT("LibraryWritable::appendTrack", __FILE__, __LINE__);

	MSXML2::IXMLDOMElementPtr track = _doc->createElement(L"Track");
	
	fillTrackInfoNode(trackInfo, track);
	wostringstream ss;
	ss << (++_lastTrackId);
	track->setAttribute(L"Id", _variant_t(ss.str().c_str()));

	MSXML2::IXMLDOMElementPtr sid = _doc->createElement(L"User");
	sid->setAttribute(L"Sid", _ownerSid);

	_tracks->appendChild(track);
	track->appendChild(sid);

	_isModified = TRUE;

	return _lastTrackId;

}

unsigned long LibraryWritable::updateTrack(	const Library::LocalTrackInfo& trackInfo,
											MSXML2::IXMLDOMElementPtr trackNode) {

	REGISTER_RUNTIME_POINT("LibraryWritable::updateTrack", __FILE__, __LINE__);

	wostringstream ss;
	ss << L"User[@Sid = '" << _ownerSid.bstrVal << L"']";
	MSXML2::IXMLDOMElementPtr sidNode = trackNode->selectSingleNode(ss.str().c_str());
	if (sidNode == NULL) {
		REGISTER_RUNTIME_POINT("LibraryWritable::updateTrack", __FILE__, __LINE__);
		sidNode = _doc->createElement(L"User");
		sidNode->setAttribute(L"Sid", _ownerSid);
		trackNode->appendChild(sidNode);
	}

	fillTrackInfoNode(trackInfo, trackNode);

	_isModified = TRUE;

	wistringstream iss(trackNode->getAttribute(L"Id").bstrVal);
	unsigned long id;
	iss >> id;

	return id;

}

void LibraryWritable::fillTrackInfoNode(	const Library::LocalTrackInfo& trackInfo,
											MSXML2::IXMLDOMElementPtr trackNode) {

	REGISTER_RUNTIME_POINT("LibraryWritable::fillTrackInfoNode", __FILE__, __LINE__);

	wostringstream stringSream;
	stringSream << trackInfo.durationSec;
	const _variant_t durationBuffer = stringSream.str().c_str();
	stringSream.str(L"");
	stringSream << trackInfo.size;
	const _variant_t sizeBuffer = stringSream.str().c_str();

	trackNode->setAttribute(L"Album",			_variant_t(trackInfo.album.c_str()));
	trackNode->setAttribute(L"Artist",			_variant_t(trackInfo.artist.c_str()));
	trackNode->setAttribute(L"Duration",		durationBuffer);
	trackNode->setAttribute(L"Kind",			_variant_t(trackInfo.kind.c_str()));
	trackNode->setAttribute(L"Size",			sizeBuffer);
	trackNode->setAttribute(L"AbsolutePath",	_variant_t(trackInfo.file.c_str()));

}


BOOL LibraryWritable::isModified() const {
	REGISTER_RUNTIME_POINT("LibraryWritable::isModified", __FILE__, __LINE__);
	return _isModified;
}

MSXML2::IXMLDOMElementPtr LibraryWritable::selectTrackNodeByFileName(wstring fileName) {
	REGISTER_RUNTIME_POINT("LibraryWritable::selectTrackNodeByFileName", __FILE__, __LINE__);
	wostringstream ss;
	ss << L"Track[@AbsolutePath = \"" << fileName << L"\"]";
	return _tracks->selectSingleNode(ss.str().c_str());
}
