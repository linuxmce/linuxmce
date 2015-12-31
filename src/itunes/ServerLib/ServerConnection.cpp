
#include "stdafx.h"
#include "ServerConnection.h"
#include "Log.h"

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster;

///////////////////////////////////////////////////////////////////////////
// Class:
///////////////////////////////////////////////////////////////////////////

ServerConnection::ServerConnection(SOCKET s)
:	_socket(s)
,	_lib(NULL) {

	REGISTER_RUNTIME_POINT("ServerConnection::ServerConnection", __FILE__, __LINE__);

	for (BOOL processResult = TRUE; processResult; ) {
		switch (receiveCmd()) {
			case SCMD_GET_PLAYLIST:
				processResult = sendPlaylist();
				break;
			case SCMD_GET_FILES:
				processResult = sendFiles();
				break;
			case SCMD_GET_FILES_ALL:
				processResult = sendAllFiles();
				break;
			case SCMD_QUIT:
			case SCMD_FAIL:
			default:
				processResult = FALSE;
				break;
		}
	}

}


ServerConnection::~ServerConnection(void) {
	REGISTER_RUNTIME_POINT("ServerConnection::~ServerConnection", __FILE__, __LINE__);
	if (_lib) {
		REGISTER_RUNTIME_POINT("ServerConnection::~ServerConnection", __FILE__, __LINE__);
		delete _lib;
	}
}


BOOL ServerConnection::sendAllFiles() {
	REGISTER_RUNTIME_POINT("ServerConnection::sendAllFiles", __FILE__, __LINE__);
	vector<unsigned long> ids;
	if (getLibrary().getAllIds(ids)) {
		for (	vector<unsigned long>::const_iterator i = ids.begin();
				i != ids.end();
				++i) {
			if (!sendFile(*i, FALSE)) {
				return FALSE;
			}
		}
		return sendZeroFile();
	}

	return FALSE;
}


BOOL ServerConnection::sendFiles() {
	REGISTER_RUNTIME_POINT("ServerConnection::sendFiles", __FILE__, __LINE__);
	BOOL result = TRUE;
	unsigned long trackId;
	while (result = receive((char*)&trackId, sizeof(unsigned long))) {
		result = sendFile(trackId);
	}
	return result;
}


BOOL ServerConnection::send(	const char *const buffer,
								const unsigned long dataSize,
								const unsigned long timeout) {
	
	REGISTER_RUNTIME_POINT("ServerConnection::send", __FILE__, __LINE__);

	timeval time;
	time.tv_sec = timeout;
	time.tv_usec = 0;
    
	fd_set wFd, eFd;
	FD_ZERO(&wFd);
	FD_ZERO(&eFd);
	FD_SET(_socket, &wFd);
	FD_SET(_socket, &eFd);

	int rc = ::select(NULL, NULL, &wFd, &eFd, &time);
	if (	rc == SOCKET_ERROR
		||	rc == 0
		||	FD_ISSET(_socket, &eFd)
		||	!FD_ISSET(_socket, &wFd)) {
		return FALSE;
	}

#	pragma warning(disable:4267)
	rc = ::send(_socket, buffer, dataSize, 0);
#	pragma warning(default:4267)

#	pragma warning(disable:4389)
	return		rc != SOCKET_ERROR
			&&	rc == dataSize;
#	pragma warning(default:4389)

}


inline BOOL ServerConnection::sendZeroFile() {
	REGISTER_RUNTIME_POINT("ServerConnection::sendZeroFile", __FILE__, __LINE__);
	TrackFileInfoPacket fileInfo;
	fileInfo.size = 0;
	return send((char*)&fileInfo, sizeof fileInfo);
}


BOOL ServerConnection::sendPlaylist() {
	REGISTER_RUNTIME_POINT("ServerConnection::sendPlaylist", __FILE__, __LINE__);
	BOOL rc = TRUE;
	vector<Library::LocalTrackInfo> tracks;
	if (getLibrary().getAllTracks(tracks)) {
		const unsigned long recordsNumb = tracks.size();
		BOOL rc = send((char*)&recordsNumb, sizeof recordsNumb);
		if (rc) {
			TrackInfoPacket *const packets = new TrackInfoPacket[recordsNumb];
			unsigned long c = 0;
			for (	vector<Library::LocalTrackInfo>::const_iterator i = tracks.begin();
					i != tracks.end();
					++i, ++c) {
				packets[c].id			= i->id;
				packets[c].size			= i->size;
				packets[c].durationSec	= i->durationSec;
				convertStringToPacketString(i->album, packets[c].album);
				convertStringToPacketString(i->artist, packets[c].artist);
				convertStringToPacketString(i->kind, packets[c].kind);
				size_t pos = i->file.rfind(L"\\");
				convertStringToPacketString(
					pos != wstring::npos && ++pos < i->file.size()
						?	i->file.substr(pos)
						:	i->file,
					packets[c].file);
			}
			rc = send((char*)packets, sizeof(TrackInfoPacket) * recordsNumb);
			delete[] packets;
		}
	}
	return rc;
}


ServerConnection::Cmds ServerConnection::receiveCmd(const unsigned long timeout) {
	REGISTER_RUNTIME_POINT("ServerConnection::receiveCmd", __FILE__, __LINE__);
	Cmds cmd = SCMD_FAIL;
	if (!receive((char*)&cmd, sizeof(int), timeout)) {
		cmd = SCMD_FAIL;
	}
	return cmd;
}


BOOL ServerConnection::receive(	char *const buffer,
								const unsigned long dataSize,
								const unsigned long timeout) {

	REGISTER_RUNTIME_POINT("ServerConnection::receive", __FILE__, __LINE__);

	timeval time;
	time.tv_sec = timeout;
	time.tv_usec = 0;
    
	fd_set rFd, eFd;
	FD_ZERO(&rFd);
	FD_ZERO(&eFd);

	for (unsigned long currentSize = 0; currentSize < dataSize; ) {
	
		FD_SET(_socket, &rFd);
		FD_SET(_socket, &eFd);

		int rc = ::select(NULL, &rFd, NULL, &eFd, &time);
		if (	rc == SOCKET_ERROR
			||	rc == 0
			||	FD_ISSET(_socket, &eFd)
			||	!FD_ISSET(_socket, &rFd)) {
			return FALSE;
		}

		rc = ::recv(_socket, buffer + currentSize, dataSize - currentSize, 0);
		if (rc == SOCKET_ERROR || rc == 0) {
			return FALSE;
		}
		currentSize += rc;

	}

	return TRUE;

}


BOOL ServerConnection::sendFile(	const unsigned long id,
									const BOOL sendZero4Inaccessible) {
	REGISTER_RUNTIME_POINT("ServerConnection::sendFile", __FILE__, __LINE__);
	wstring path;
	unsigned long size;
	return getLibrary().getFilePathByTrackId(id, path, size)
		?	sendFile(path, sendZero4Inaccessible)
		:	(sendZero4Inaccessible ? sendZeroFile() : TRUE);
}


BOOL ServerConnection::sendFile(	const wstring &file,
									const BOOL sendZero4Inaccessible) {
	REGISTER_RUNTIME_POINT("ServerConnection::sendFile", __FILE__, __LINE__);
	LARGE_INTEGER sizeInLarge;
	HANDLE hFile = CreateFileW(
		file.c_str(), FILE_READ_DATA, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		return sendZero4Inaccessible ? sendZeroFile() : TRUE;
	} else if (!GetFileSizeEx(hFile, &sizeInLarge)) {
		CloseHandle(hFile);
		return sendZero4Inaccessible ? sendZeroFile() : TRUE;
	}
	TrackFileInfoPacket fileInfo;
#	pragma warning(disable:4244)
	fileInfo.size = sizeInLarge.QuadPart;
#	pragma warning(default:4244)
	wstring fileName;
	size_t pos = file.rfind(L"\\");
	convertStringToPacketString(
		pos != wstring::npos && ++pos < file.size()
			?	file.substr(pos)
			:	file,
		fileInfo.name);

	BOOL result = FALSE;
	DWORD read;
	char *buffer = new char[fileInfo.size];
	if (	ReadFile(hFile, (void*)buffer, fileInfo.size, &read, NULL)
		&&	read == fileInfo.size) {
		result =	send((char*)&fileInfo, sizeof fileInfo)
				&&	send(buffer, fileInfo.size);
	} else {
		result = sendZero4Inaccessible ? sendZeroFile(): TRUE;
	}
	delete[] buffer;

	CloseHandle(hFile);

	return result;

}


void ServerConnection::convertStringToPacketString(	const wstring& str,
													char* const target) {
	REGISTER_RUNTIME_POINT("ServerConnection::convertStringToPacketString", __FILE__, __LINE__);
	const wchar_t *const wStrPtr = str.c_str();
	const size_t strSize = str.size();
	locale loc;
	use_facet<ctype<wchar_t> >(loc).narrow(wStrPtr, wStrPtr + strSize , '_', target);
	target[strSize] = 0;
}


Library& ServerConnection::getLibrary() {
	REGISTER_RUNTIME_POINT("ServerConnection::getLibrary", __FILE__, __LINE__);
	if (!_lib) {
		_lib = new Library();
		_lib->load();
	}
	return *_lib;
}

