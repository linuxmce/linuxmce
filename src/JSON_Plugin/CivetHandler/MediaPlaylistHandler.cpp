#include "MediaPlaylistHandler.h"

#include "../../PlutoUtils/MultiThreadIncludes.h"
#include "../EscapeJS.h"

#include <string>

MediaPlaylistHandler::MediaPlaylistHandler(Media_Plugin *pMedia_Plugin)
{
	m_pMedia_Plugin = pMedia_Plugin;
}

bool MediaPlaylistHandler::handleGet(CivetServer *server, struct mg_connection *conn)
{
	const char *responseTemplateBegin =
		"{\n"
		"	\"ea\": %d,\n"
		"	\"playlist\": [\n"
	;
	const char *playlistRowTemplate =
		"		%s{\n" // list separating comma
		"			\"text\": \"%s\",\n"
		"			\"value\": \"%s\",\n"
		"			\"message\": \"%s\"\n"
		"		}\n"
	;
	const char *responseTemplateEnd =
		"	]\n"
		"}\n"
	;

	EntertainArea *pEntertainArea = NULL;
	MediaStream *pMediaStream = NULL;

	int iPK_Variable;
	string sValue_To_Assign;
	DCE::Message *pMessage = NULL;
	DataGridTable *pDatagrid = NULL;

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);

	std::string parmEA;
	if (!CivetServer::getParam(conn, "ea", parmEA)) goto fail_param;

	pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(atoi(parmEA.c_str()));
	if (pEntertainArea == NULL) goto fail_ea;

	mg_printf(conn, "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\n\r\n");

	pMessage = new DCE::Message(
		0, // from
		0, // to
		PRIORITY_NORMAL,
		MESSAGETYPE_COMMAND,
		0, // PK_Command
		1, // parameter count
		COMMANDPARAMETER_PK_EntertainArea_CONST, parmEA.c_str()
	);

	pDatagrid = m_pMedia_Plugin->CurrentMediaSections("plist", "", NULL, &iPK_Variable, &sValue_To_Assign, pMessage);
	delete pMessage;
	pMessage = NULL;

	pMediaStream = pEntertainArea->m_pMediaStream;
	mg_printf(conn, responseTemplateBegin,
		pEntertainArea->m_iPK_EntertainArea // ea
	);

	if (pMediaStream != NULL)
	{
		int RowCount = pDatagrid->GetRows();
		int ColumnCount = pDatagrid->GetCols();

		for (int i = 0; i < RowCount; ++i)
		{
			for (int j = 0; j < ColumnCount; ++j)
			{
				DataGridCell *Cell = pDatagrid->GetData(j, i);
				pMessage = Cell->m_pMessage;
				mg_printf(conn, playlistRowTemplate,
					(i > 0 || j > 0) ? "," : "",                 // list separating comma
					Cell->GetText(),                             // text
					Cell->GetValue(),                            // value
					pMessage ? pMessage->ToString().c_str() : "" // message
				);
			}
		}
	}

	mg_printf(conn, responseTemplateEnd);

	delete pDatagrid;
	pDatagrid = NULL;
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
