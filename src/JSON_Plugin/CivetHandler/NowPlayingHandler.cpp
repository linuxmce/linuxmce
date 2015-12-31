#include "NowPlayingHandler.h"

#include "../../PlutoUtils/MultiThreadIncludes.h"
#include "../EscapeJS.h"

#include <string>

extern "C" {
#include "../base64.h"
}

#define PNG_PREFIX "data:image/png;base64,"
#define PNG_PREFIX_LENGTH 22

NowPlayingHandler::NowPlayingHandler(Media_Plugin *pMedia_Plugin)
{
	m_pMedia_Plugin = pMedia_Plugin;
}

bool NowPlayingHandler::handleGet(CivetServer *server, struct mg_connection *conn)
{
	const char *responseTemplate =
		"{\n"
		"	\"ea\": %d,\n"
		"	\"active\": %s,\n"
		"	\"media_type\": %d,\n"
		"	\"media_device\": %d,\n"
		"	\"remote_alt\": %s,\n"
		"	\"playback_speed\": %s,\n"
		"	\"stream_id\": %d,\n"
		"	\"description\": \"%s\",\n"
		"	\"sectiondescription\": \"%s\",\n"
		"	\"playlistid\": \"%d\",\n"
		"	\"timestamp\": \"%s\",\n"
		"	\"replace_cover_art\": %s,\n"
		"	\"cover_art\": \"%s\"\n"
		"}\n";

	EntertainArea *pEntertainArea = NULL;
	MediaStream *pMediaStream = NULL;

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);

	std::string parmEA;
	if (!CivetServer::getParam(conn, "ea", parmEA)) goto fail_param;

	pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(atoi(parmEA.c_str()));
	if (pEntertainArea == NULL) goto fail_ea;

	mg_printf(conn, "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\n\r\n");

	pMediaStream = pEntertainArea->m_pMediaStream;

	if (pMediaStream != NULL)
	{
		struct timespec last_update = pMediaStream->getLastUpdate();
		char *last_update_str;
		if (asprintf(&last_update_str, "%ld;%ld", last_update.tv_sec, last_update.tv_nsec) == -1)
			last_update_str = strdup("0");

		bool bReplacePicture = false;
		char *base64_picture = NULL;
		std::string parmTimestamp;
		if (!CivetServer::getParam(conn, "timestamp", parmTimestamp) || parmTimestamp != last_update_str)
		{
			char *pictureData = pMediaStream->m_pPictureData_get();
			size_t pictureSize = pMediaStream->m_iPictureSize_get();
			if (pictureSize > 0 && pictureData != NULL)
			{
				size_t outlen = base64_encoded_size(pictureSize);
				base64_picture = (char *) calloc(1, PNG_PREFIX_LENGTH + outlen + 1);
				if (base64_picture != NULL)
				{
					memmove(base64_picture, PNG_PREFIX, PNG_PREFIX_LENGTH);
					base64_encode_binary(base64_picture + PNG_PREFIX_LENGTH, (unsigned char *)pictureData, pictureSize);
				}
			}
			else
			{
				base64_picture = strdup("");
			}
			bReplacePicture = true;
		}
		else
		{
			base64_picture = strdup("");
		}

		string escapedMediaDescription = escapeJsonString(pMediaStream->m_sMediaDescription);
		string escapedSectionDescription = escapeJsonString(pMediaStream->m_sSectionDescription);
		string escapedBase64Picture = escapeJsonString(base64_picture);
		string escapedLastUpdateStr = escapeJsonString(last_update_str);

		mg_printf(conn, responseTemplate,
			pEntertainArea->m_iPK_EntertainArea, // ea
			"true",                              // active
			pMediaStream->m_iPK_MediaType,       // media_type
			pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, // media_device
			pMediaStream->m_bUseAltScreens ? "true" : "false", // remote_alt
			pMediaStream->m_pMediaDevice_Source
				? StringUtils::itos(pMediaStream->m_pMediaDevice_Source->m_iLastPlaybackSpeed).c_str()
				: "undefined",               // playback_speed
			pMediaStream->m_iStreamID_get(),     // stream_id
			escapedMediaDescription.c_str(),     // description
			escapedSectionDescription.c_str(),     // description
			pMediaStream->m_iPK_Playlist,        // Playlist ID - maybe useful for later playlist retrieval
			escapedLastUpdateStr.c_str(),        // timestamp
			bReplacePicture ? "true" : "false",  // replace_cover_art
			escapedBase64Picture.c_str()         // cover_art
		);

		free(last_update_str);
		free(base64_picture);
	}
	else
	{
		mg_printf(conn, responseTemplate,
			pEntertainArea->m_iPK_EntertainArea, // ea
			"false",                             // active
			0,                                   // media_type
			0,                                   // media_device
			"false",                             // remote_alt
			"undefined",                         // playback_speed
			0,                                   // stream_id
			"",                                  // description
			"",                                  // section description
			0,                                   // playlist ID
			"0",                                 // timestamp
			"false",                             // replace_cover_art
			""                                   // cover_art
		);
	}

	return true;

fail_ea:
	mg_printf(conn, "HTTP/1.0 404 Not Found\r\nContent-Type: application/json\r\n\r\n");
	mg_printf(conn, "{\"error\": \"EA not found\"}");
	return true;

fail_param:
	mg_printf(conn, "HTTP/1.0 404 Not Found\r\nContent-Type: application/json\r\n\r\n");
	mg_printf(conn, "{\"error\": \"must specify 'ea' parameter\"}");
	return true;
}
