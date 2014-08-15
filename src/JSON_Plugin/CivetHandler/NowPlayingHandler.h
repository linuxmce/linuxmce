#ifndef CIVETHANDLER_NOWPLAYINGHANDLER_H
#define CIVETHANDLER_NOWPLAYINGHANDLER_H

#include "../civetweb/CivetServer.h"

#include "../../Media_Plugin/Media_Plugin.h"

class NowPlayingHandler : public CivetHandler
{
	public:
		NowPlayingHandler(Media_Plugin *pMedia_Plugin);
		virtual ~NowPlayingHandler() {};

		virtual bool handleGet(CivetServer *server, struct mg_connection *conn);

	protected:
		Media_Plugin *m_pMedia_Plugin;
};

#endif /* CIVETHANDLER_NOWPLAYINGHANDLER_H */
