
#include "stdafx.h"
#include "Library.h"
#include "Log.h"

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster;

wstring Library::_pathToStorage;
wstring Library::_dirForStorage;

Library::Library(void)
:	_doc(NULL)
,	_root(NULL)
,	_tracks(NULL) {
	REGISTER_RUNTIME_POINT("Library::Library", __FILE__, __LINE__);
}

Library::~Library(void) {
	REGISTER_RUNTIME_POINT("Library::~Library", __FILE__, __LINE__);
	release();
}

void Library::release() {
	REGISTER_RUNTIME_POINT("Library::release", __FILE__, __LINE__);
	if (_doc) {
		_doc.Release();
		_doc = NULL;
	}
}

void Library::createNew() {

	REGISTER_RUNTIME_POINT("Library::createNew", __FILE__, __LINE__);

	createNewDocumentObject();
		
	MSXML2::IXMLDOMNodePtr processingInstruction
			= _doc->createProcessingInstruction(L"xml", L"version='1.0' encoding='UTF-8'");
	_root	= _doc->createElement(L"ITunesMediaConnectorLibrary");
	_tracks = _doc->createElement(L"LocalTracksList");

	_doc->appendChild(processingInstruction);
	_doc->appendChild(_root);
	// TODO: Hardcoded library format version
	_root->setAttribute(L"Version", L"1.0");
	_root->appendChild(_tracks);

	_lastTrackId = 0;

}


void Library::createNewDocumentObject() {

	REGISTER_RUNTIME_POINT("Library::createNewDocumentObject", __FILE__, __LINE__);
	
	release();

	_doc.CreateInstance(__uuidof(MSXML2::DOMDocument40));
	_doc->put_validateOnParse(false);
	_doc->put_async(false);

}


void Library::load() {

	REGISTER_RUNTIME_POINT("Library::load", __FILE__, __LINE__);

	createNewDocumentObject();

	BOOL result = FALSE;

	try {
		REGISTER_RUNTIME_POINT("Library::load", __FILE__, __LINE__);
		_doc->load(getPathToStorage().c_str());
		_root = _doc->documentElement;
		if (!StrCmpW(_root->tagName, L"ITunesMediaConnectorLibrary")) {
			REGISTER_RUNTIME_POINT("Library::load", __FILE__, __LINE__);
			_tracks	= _doc->selectSingleNode(L"/ITunesMediaConnectorLibrary/LocalTracksList");
			result = _tracks != NULL;
			wistringstream ss(_tracks->getAttribute("LastId").bstrVal);
			ss >> _lastTrackId;
		}
	} catch (_com_error ex) {
		REGISTER_RUNTIME_POINT("Library::load", __FILE__, __LINE__);
	}

	if (!result) {
		REGISTER_RUNTIME_POINT("Library::load", __FILE__, __LINE__);
		createNew();
	}

}


const wstring& Library::getPathToStorage() {
	REGISTER_RUNTIME_POINT("Library::getPathToStorage", __FILE__, __LINE__);
	if (_pathToStorage.empty()) {
		REGISTER_RUNTIME_POINT("Library::getPathToStorage", __FILE__, __LINE__);
		_pathToStorage = getDirForStorage();
		_pathToStorage += L"\\library.xml";
	}
	return _pathToStorage;
}


const wstring& Library::getDirForStorage() {
	REGISTER_RUNTIME_POINT("Library::getDirForStorage", __FILE__, __LINE__);
	if (_dirForStorage.empty()) {
		REGISTER_RUNTIME_POINT("Library::getDirForStorage", __FILE__, __LINE__);
		WCHAR path[MAX_PATH + 1];
		memset(path, 0, MAX_PATH + 1);
		SHGetSpecialFolderPathW(NULL, path, CSIDL_COMMON_APPDATA, TRUE);
		_dirForStorage = path;
		_dirForStorage += L"\\Pluto iTunes Connector";
	}
	return _dirForStorage;
}

BOOL Library::getFilePathByTrackId(	unsigned int id,
									std::wstring& path,
									unsigned long& fileSize) {
	REGISTER_RUNTIME_POINT("Library::getFilePathByTrackId", __FILE__, __LINE__);
	path.resize(0);
	fileSize = 0;
	MSXML2::IXMLDOMElementPtr node = selectTrackNodeById(id);
	if (node) {
		REGISTER_RUNTIME_POINT("Library::getFilePathByTrackId", __FILE__, __LINE__);
		path = node->getAttribute(L"AbsolutePath").bstrVal;
		wistringstream ss(node->getAttribute(L"Size").bstrVal);
		ss >> fileSize;
	}
	return !path.empty() && fileSize;
}


MSXML2::IXMLDOMNodePtr Library::selectTrackNodeById(const unsigned long id) {
	REGISTER_RUNTIME_POINT("Library::selectTrackNodeById", __FILE__, __LINE__);
	wostringstream ss;
	ss << L"Track[@Id = '" << id << L"']";
	return _tracks->selectSingleNode(ss.str().c_str());
}

BOOL Library::getAllIds(vector<unsigned long>& ids) {
	REGISTER_RUNTIME_POINT("Library::getAllIds", __FILE__, __LINE__);
	ids.resize(0);
	MSXML2::IXMLDOMNodeListPtr nodes = _tracks->selectNodes(L"Track/@Id");
	nodes->reset();
	unsigned long id;
	MSXML2::IXMLDOMNodePtr p;
	while (p = nodes->nextNode()) {
		REGISTER_RUNTIME_POINT("Library::getAllIds", __FILE__, __LINE__);
		wistringstream ss(p->nodeValue.bstrVal);
		ss >> id;
		ids.push_back(id);
	}
	return TRUE;
}


BOOL Library::getAllTracks(std::vector<Library::LocalTrackInfo>& tracks) {

	REGISTER_RUNTIME_POINT("Library::getAllTracks", __FILE__, __LINE__);
	
	tracks.resize(0);
	
	MSXML2::IXMLDOMNodeListPtr nodes = _tracks->selectNodes(L"Track");
	nodes->reset();
	
	MSXML2::IXMLDOMElementPtr node;
	while (node = nodes->nextNode()) {

		REGISTER_RUNTIME_POINT("Library::getAllTracks", __FILE__, __LINE__);
		
		LocalTrackInfo track;
		wistringstream duration(node->getAttribute(L"Duration").bstrVal);
		wistringstream size(node->getAttribute(L"Size").bstrVal);
		wistringstream id(node->getAttribute(L"Id").bstrVal);

		track.durationSec = 0;
		track.album = node->getAttribute(L"Album").bstrVal;
		track.artist = node->getAttribute(L"Artist").bstrVal;
		track.kind = node->getAttribute(L"Kind").bstrVal;
		track.file = node->getAttribute(L"AbsolutePath").bstrVal;
		duration >> track.durationSec;
		size >> track.size;
		id >> track.id;

		tracks.push_back(track);

	}

	return TRUE;

}
