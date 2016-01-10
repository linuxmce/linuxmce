using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;


namespace PlutoReboot
{
    class UDPBroadcast : UdpClient
    {
        public string FindRouter()
        {
            try
            {
                Socket s = this.Client;
                s.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
                s.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 3000);
                Byte[] packet = System.Text.Encoding.ASCII.GetBytes("test");
                Send(packet, packet.Length, "255.255.255.255", 33333);
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
                Byte[] receiveBytes = Receive(ref RemoteIpEndPoint);
                string returnData = System.Text.Encoding.ASCII.GetString(receiveBytes);
                if (returnData.ToUpper() == "SERVICE: DCEROUTER")
                    return RemoteIpEndPoint.Address.ToString();
            }
            catch (Exception e)
            {
                string s = e.Message;
            }
            return null;
        }
    }

    public class PlutoManager
    {
        private string m_sLastError = "";

        private string m_sConnectedIP = "";
        private string m_sConnectedHost = "";

        private const string m_sRouterDefName = "dcerouter";
        const int m_iRouterPort = 3450;
        const string SEND_STRING = "HELLO -1\n";
        const string RECEIVE_STRING = "NOT IN THIS INSTALLATION";
        const string MAC_SEND_STRING = "MY_IP\n";

        public bool GetMAC( string sRouterIP, out string sMAC )
        {
            sMAC = "";
            m_sLastError = "";
            bool bRet = false;
            try
            {
                IPAddress hostIPAddress = IPAddress.Parse(sRouterIP);
                IPEndPoint connectto = new IPEndPoint(hostIPAddress, m_iRouterPort);
                using (TcpClient client = new TcpClient())
                {
                    client.Connect(connectto);
                    if (!client.Connected)
                    {
                        m_sLastError = "Can't connect!";
                        return false;
                    }

                    NetworkStream stream = client.GetStream();
                    byte[] byData = System.Text.Encoding.ASCII.GetBytes(MAC_SEND_STRING);
                    stream.Write(byData, 0, byData.Length);
                    byData = new byte[1024];
                    String responseData = String.Empty;
                    Int32 bytes = stream.Read(byData, 0, 1024);
                    responseData = System.Text.Encoding.ASCII.GetString(byData, 0, bytes);
                    int iMacIndex = responseData.ToUpper().IndexOf("MAC:");
                    if ( iMacIndex >=0 )
                    {
                        responseData = responseData.ToUpper().Substring(iMacIndex + 4);
                        iMacIndex = responseData.IndexOf('\n');
                        if (iMacIndex >= 0)
                        {
                            responseData = responseData.ToUpper().Substring( 0, iMacIndex );
                            sMAC = responseData;
                            bRet = true;                            
                        }
                    }
                    client.Close();
                }
            }
            catch(Exception e)
            {
                m_sLastError = e.Message;
                return false;
            }
            return bRet;

        }

        private bool Try_Connect2Host(string sRouterName)
        {
            m_sLastError = "";
            bool bRet = false;
            try
            {
                using (TcpClient client = new TcpClient(sRouterName, m_iRouterPort))
                {
                    NetworkStream stream = client.GetStream();
                    byte[] byData = System.Text.Encoding.ASCII.GetBytes(SEND_STRING);
                    stream.Write(byData, 0, byData.Length);
                    byData = new byte[RECEIVE_STRING.Length];
                    String responseData = String.Empty;
                    Int32 bytes = stream.Read(byData, 0, byData.Length);
                    responseData = System.Text.Encoding.ASCII.GetString(byData, 0, bytes);
                    if (responseData.ToUpper() == RECEIVE_STRING)
                    {
                        m_sConnectedHost = sRouterName;
                        m_sConnectedIP = "";
                        IPHostEntry hostip = Dns.GetHostEntry(sRouterName);
                        //IPHostEntry hostip = Dns.GetHostByName();
                        //IPHostEntry hostip = Dns.GetHostByName();
                        if (hostip.AddressList.Length >= 0)
                            m_sConnectedIP = hostip.AddressList[0].ToString();
                        bRet = true;
                    }
                    client.Close();
                }
            }
            catch (Exception e)
            {
                m_sLastError = e.Message;
                return false;
            }

            return bRet;
        }
        private bool Try_Connect2IP(string sIP)
        {
            m_sLastError = "";
            bool bRet = false;
            try
            {
                IPAddress hostIPAddress = IPAddress.Parse(sIP);
                IPEndPoint connectto = new IPEndPoint(hostIPAddress, m_iRouterPort);
                using (TcpClient client = new TcpClient())
                {
                    client.Connect(connectto);
                    if (!client.Connected)
                    {
                        m_sLastError = "Can't connect!";
                        return false;
                    }

                    NetworkStream stream = client.GetStream();
                    byte[] byData = System.Text.Encoding.ASCII.GetBytes(SEND_STRING);
                    stream.Write(byData, 0, byData.Length);
                    byData = new byte[RECEIVE_STRING.Length];
                    String responseData = String.Empty;
                    Int32 bytes = stream.Read(byData, 0, byData.Length);
                    responseData = System.Text.Encoding.ASCII.GetString(byData, 0, bytes);
                    if (responseData.ToUpper() == RECEIVE_STRING)
                    {                        
                        m_sConnectedIP = sIP;
                        bRet = true;
                    }
                    client.Close();
                }
            }
            catch (Exception e)
            {
                m_sLastError = e.Message;
                return false;
            }
            return bRet;
            /*
                        m_sLastError = "";
                        try
                        {
                            IPAddress hostIPAddress = IPAddress.Parse(sIP);
                            IPHostEntry hostInfo = Dns.GetHostByAddress(hostIPAddress);
                            if (hostInfo.Aliases.Length <= 0) return false;
                            String alias = hostInfo.Aliases[0];
                            return Try_Connect2Host(alias);
                        }
                        catch (Exception e)
                        {
                            m_sLastError = e.Message;
                            return false;
                        }
             */ 
        }

        public bool CheckHostByName(string sHost)
        {
            return Try_Connect2Host(sHost);
        }
        public bool CheckHostByIP(string sIP)
        {
            return Try_Connect2IP(sIP);
        }
        public bool CheckDefault()
        {
            return Try_Connect2Host(m_sRouterDefName);
        }

        public string LastError
        {
            get { return m_sLastError; }
        }

        public string ConnectedIP
        {
            get { return m_sConnectedIP; }
        }

        public string ConnectedHost
        {
            get { return m_sConnectedHost; }
        }

        public string DefaultHost
        {
            get { return m_sRouterDefName; }
        }

        public bool UDPFind()
        {
            try
            {
                UDPBroadcast test = new UDPBroadcast();
                string s = test.FindRouter();
                if (s != null)
                {
                    m_sConnectedIP = s;
                    m_sConnectedHost = "";
                    return true;
                }
            }
            catch (Exception e)
            {
                m_sLastError = e.Message;
            }
            return false;
        }

    }
    public class ShellCommands
    {
        private const string HaltDevice_Options = "0 4 1 323 2 0 21 \"N\" 47 ";

        private string m_sLastError;

        public string LastError
        {
            get { return m_sLastError; }
        }

        public bool Exec_HaltDevice(string router, string sMACAddress )
        {
            m_sLastError = "";
            System.Diagnostics.Process p = new System.Diagnostics.Process();
            try
            {
                p.StartInfo.FileName = "MessageSend.exe";
                //sMACAddress = "00:0D:0B:99:10:42";
                p.StartInfo.Arguments = router + " " +
                    HaltDevice_Options + "\"" + sMACAddress + "\"";
                //System.Windows.Forms.MessageBox.Show(p.StartInfo.Arguments);
                //return true;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.CreateNoWindow = true;
                p.Start();
                p.WaitForExit();
            }
            catch (Exception e)
            {
                m_sLastError = e.Message;
                return false;
            }
            finally
            {
                p.Dispose();
            }
            return true;

        }

    }
}
