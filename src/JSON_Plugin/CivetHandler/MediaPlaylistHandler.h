#ifndef CIVETHANDLER_MEDIAPLAYLISTHANDLER_H
#define CIVETHANDLER_MEDIAPLAYLISTHANDLER_H

#include "../civetweb/CivetServer.h"

#include "../../Media_Plugin/Media_Plugin.h"

class MediaPlaylistHandler : public CivetHandler
{
	public:
		MediaPlaylistHandler(Media_Plugin *pMedia_Plugin);
		virtual ~MediaPlaylistHandler() {};

		virtual bool handleGet(CivetServer *server, struct mg_connection *conn);

	protected:
		Media_Plugin *m_pMedia_Plugin;
};

#endif /* CIVETHANDLER_MEDIAPLAYLISTHANDLER_H */
