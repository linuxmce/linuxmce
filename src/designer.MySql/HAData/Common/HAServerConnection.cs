using System;
using System.Net;
using System.Net.Sockets;
using System.Windows.Forms;

namespace HAData.Common
{
	/// <summary>
	/// Summary description for HAServerConnection.
	/// </summary>
	public class HAServerConnection
	{
		Socket m_Socket = null;
		string m_ServerName;
		int m_ServerPort = 3450;
		IPHostEntry m_IPHostEntry=null;

		public HAServerConnection(string ServerName)
		{
			m_ServerName = ServerName;
		}

		public void ConnectToServer()
		{
			if( m_Socket==null )
			{
				m_Socket = new Socket(AddressFamily.InterNetwork,SocketType.Stream,ProtocolType.IP);

				//        m_Socket = socket(AF_INET, SOCK_STREAM, 0);
				//	SOCKET s2 = socket(AF_INET, SOCK_STREAM, 0);
				//        BOOL bSuccess = FALSE;
				try
				{
					m_IPHostEntry=null;
					m_IPHostEntry = Dns.GetHostByName(m_ServerName);
				}
				catch(Exception)
				{
					DisconnectFromServer();
					return;
				}

				if( m_IPHostEntry.AddressList.Length!=1 )
				{
					MessageBox.Show("Error getting IP Address");
					return;
				}

				IPAddress ip = m_IPHostEntry.AddressList[0];

				IPEndPoint ipe = new IPEndPoint(ip,m_ServerPort);

				int Retries = 0;
				while(Retries < 10)
				{
					try
					{
						m_Socket.Connect(ipe);
						break;
					}
					catch(Exception)
					{
						Retries++;
					}
				}
			}
		}


/*

				if( m_Socket.
     
		if ( m_Socket != INVALID_SOCKET )
        {
			int                 nRetries = 0, nRet;
			struct sockaddr_in  addrT;
			PHOSTENT            phe;

			// ppp_peer returns the IP adress of the host machine
			// if CE Services is running or NULL otherwise
			if ( !(phe = gethostbyname(m_ServerAddress.c_str())) )
			{
		//		::MessageBox(NULL, _T("Error"),  _T("Can't find host"), MB_OK);
				DisconnectFromServer();
				return;
			}
	 
			for (bSuccess = FALSE;
				 nRetries < MAX_RETRIES && !bSuccess;
				 nRetries++, Sleep(DELAY_TIME)
				 )
			{
				memset(&addrT, 0, sizeof(addrT));
				addrT.sin_family = AF_INET;
				addrT.sin_port = htons(m_ServerPort);
				addrT.sin_addr.s_addr = *(long *)phe->h_addr; 

				nRet = connect(m_Socket, (PSOCKADDR)&addrT, sizeof(addrT));

				if ( nRet != SOCKET_ERROR) 
					bSuccess = TRUE;
				else
				{
					int ec = WSAGetLastError();
					char s[81];
					sprintf(s, "EC %d", ec);
				}
			}

            if ( !bSuccess)
            {
          //      ::MessageBox(NULL, _T("Failed to connect"), 
           //                _T("Error"), MB_OK);
                DisconnectFromServer();
            }
		}
    }
}
*/
		public void DisconnectFromServer()
		{
			if ( m_Socket != null )
			{
				m_Socket.Close();
				m_Socket= null;
			}
		}

		public bool TransmitToServer(byte[] buffer, int Length)
		{
			if (m_Socket == null)
				ConnectToServer();

			if (m_Socket == null)
				return false;

			int nCount = m_Socket.Send(buffer, 0, Length, SocketFlags.None);
			
			return ( nCount == Length );
		}

		int ReceiveFromServer(byte[] buffer, int MaxLength)
		{
			return 0;
		}	
	}
}
