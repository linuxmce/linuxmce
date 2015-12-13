#ifndef VDRConnection_h
#define VDRConnection_h

#include "DCE/PlainClientSocket.h"

#define VDR_SOCKET_TIMEOUT	3  // SECONDS

namespace DCE
{
class VDRConnection
{
private:
	PlainClientSocket *m_pVDRSocket;
	bool m_bConnected;

public:
        VDRConnection()
	{
		m_pVDRSocket = NULL;
		m_bConnected = false;
	}
        VDRConnection(string sIP, string sPort)
	{
	        m_pVDRSocket = new PlainClientSocket(sIP + ":" + sPort);
		m_bConnected = false;
	}

	~VDRConnection()
	{
		delete m_pVDRSocket;
		m_pVDRSocket = NULL;
	}

	void SetAddress(string sIP, string sPort)
	{
	        m_pVDRSocket = new PlainClientSocket(sIP + ":" + sPort);
	}

	bool Connect()
	{
		if ( !m_pVDRSocket ) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::Connect() no socket (no IP and port)");
			return false;
		}
		if( !m_pVDRSocket->Connect() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::Connect() Unable to connect to VDR client");
			return false;
		}
		string sResponse;
		if( !m_pVDRSocket->ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="220" )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::Connect() VDR not ready got %s",sResponse.c_str());
			return false;
		}
		m_bConnected = true;
		return true;
	}

	bool Close()
	{
		if ( !m_pVDRSocket ) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::Close() no socket (no IP and port)");
			m_bConnected = false; // not possible to be connected without a socket
			return false;
		}
		if ( !m_bConnected )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::Close() close on a non-connected connection - NOOP");
			return true;
		}
		if( !m_pVDRSocket->SendString("QUIT") )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::Close()  Could not send string");
			return false;
		}

		string sResponse;
		if( !m_pVDRSocket->ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="221" )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::Close()  VDR not ok with quit got %s",sResponse.c_str());
			return false;
		}
		
		m_pVDRSocket->Close();
		m_bConnected = false;
		return true;
	}

	bool SendOnlyVDRCommand(string sCommand)
	{
		if ( !m_pVDRSocket || !m_bConnected) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::SendOnlyVDRCommand() no valid connection");
			return false;
		}
		if( !m_pVDRSocket->SendString(sCommand) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::SendVDRCommand Could not send string (%s)", sCommand.c_str());
			return false;
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRConnection::svdrPort send-only sending command done");
		return true;
	}

	bool SendVDRCommand(string sCommand,string &sVDRResponse)
	{
		if ( !m_bConnected )
		{
			Connect();
		}
		string error = SendVDRCommandInternal(sCommand, sVDRResponse);
		int retry = 0;
		while (!error.empty() && retry < 3)
		{
			if (error == "221" || error == "---")
			{
				Connect();
				error = SendVDRCommandInternal(sCommand, sVDRResponse);
			}
			retry++;
		}
		if (error.empty())
			return true;
		else
			return false;

	}
	string SendVDRCommandInternal(string sCommand,string &sVDRResponse)
	{
		clock_t startClock = clock();
		string error;
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"SendVDRCommand Going to send command %s",sCommand.c_str());
		if ( sCommand == "HITK Up" || sCommand == "HITK Down" ) 
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort using SendOnly for %s", sCommand.c_str());
			bool bResult;
			bResult = SendOnlyVDRCommand(sCommand);
			return bResult ? "" : "---";
		}

		if ( !m_pVDRSocket || !m_bConnected) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRConnection::SendVDRCommand() no valid connection");
			return "---";
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"svdrPort sending command %s",sCommand.c_str());
		if( !m_pVDRSocket->SendString(sCommand) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SendVDRCommand Could not send string");
			return "---";
		}

		time_t reqTime = time(NULL);
		int ourTimeOut = 2; //seconds
		int BUF_SIZE = 2048;
		int nSize = BUF_SIZE-1;
		char *pData = new char[BUF_SIZE];
		memset(pData, 0, BUF_SIZE);
		
		// This code reads a character at a time from the Socket (a PlainClientSocket) to
		// avoid triggering the timeout for each command (which incurs a delay)
		// The code might not be optimized as it does a simple loop with a slight sleep to reduce load
		bool gotData = m_pVDRSocket->ReceiveData(nSize, pData, -2); // no timeout, we will detect when to stop expecting data
		bool endDetected = false;
		string leftOver = ""; // leftover from last tokenize, prepend to next chunk
		while (error.empty() && !endDetected && (difftime(time(NULL), reqTime) < ourTimeOut))
		{
			//we want a string
			string sResponse(pData);
				
			// Make sure we have at least one newline in our string
			size_t pos = sResponse.find('\n');
			if (pos == string::npos) {
				// if not, keep what we got until next time
				leftOver += sResponse;
				usleep(1000); // not a full line, let's chill out for 1000 usec
			} else {
				// we have at least one newline
				if (!leftOver.empty()) {
					sResponse = leftOver + sResponse;
					leftOver = "";
				}
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"VDR response: %s", sResponse.c_str());
				
                                // If the last line ends with new line, all the lines can be processed,
				// if not, we keep the last line as a leftover for next iteration
				bool lastLineEndsWithNewLine = StringUtils::EndsWith(sResponse, "\n");
				vector<string> vectLines;
				StringUtils::Tokenize(sResponse, "\n", vectLines);
				for (size_t i = 0; i < vectLines.size()-(lastLineEndsWithNewLine ? 0 : 1); i++)
				{
					string line = vectLines[i];
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDR line %d: %s", i, line.c_str());
					
					if( line.substr(0,3)!="250" && line.substr(0,3)!="215" && line.substr(0,3)!="550" )
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with command got %d lines then got: %s",i, line.c_str());
						error = line.substr(0, 3);
						break;
					}
					
					if( line.size()>4 )
						sVDRResponse += line.substr(4) + "\n";
					
					if( line[3]!='-' )
						endDetected = true;
				}
				if (!lastLineEndsWithNewLine)
					// No new-line at end of line, treat it as a leftover for next time
					leftOver = vectLines[vectLines.size()-1];
			}
			// Clear buffer and read more
			memset(pData, 0, BUF_SIZE);
			nSize = BUF_SIZE-1;
			gotData = m_pVDRSocket->ReceiveData(nSize, pData, -2); // no timeout, we will detect when to stop expecting data
		}	
		//deallocated received data buffer
		delete [] pData;
		pData = NULL;

		LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDRConnection::SendVDRCommand end, time taken = %f", ((float)(clock() - startClock))/CLOCKS_PER_SEC);

		return error;
	}
};
}
#endif
