/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef PlutoDHCP_h
#define PlutoDHCP_h

/*
	This program does 2 things:  
	
	1) When called with -a and -d XXX it looks up device XXX, echos its IP
	address if it already has one, or if not, assigns a new IP address within
	the range reserved for Pluto (which should never overlap the range used by dhcp).

	2) When called without -a, it writes out a list of all the mac addresses and 
	IP addresses to std out that can be merged into the dhcp.conf file
*/


//  IPAddress encapsulates an IP Address, making it available as both an unsigned long and a string

class IPAddress
{
private:
	unsigned long m_IPAddress;
	bool m_bBadIP;
public:
	IPAddress(string sIPAddress)
	{
		ParseIP(sIPAddress);
	}
	bool ParseIP(string sIPAddress)
	{
		m_bBadIP=false;
		string::size_type pos=0;
		unsigned char *pByte = ((unsigned char *) &m_IPAddress) + 3;
		for(int i=0;i<4;++i)
		{
			string sNumber = StringUtils::Tokenize(sIPAddress,".",pos);
			if( !sNumber.size() )
				m_bBadIP=true;
			*pByte = (unsigned char) atoi(sNumber.c_str());
			pByte--;
		}
		return !m_bBadIP;
	}

  string GetOctet(int pos)
  {
    string sResult;
    unsigned char *pByte = ((unsigned char *) &m_IPAddress) + 4;
    pByte-=pos;
    sResult = StringUtils::itos(*pByte);
    return sResult;
  }
	IPAddress() { m_bBadIP=true; }
	IPAddress(unsigned long IPAddress) { m_IPAddress=IPAddress; }
	unsigned long AsInt() { return m_IPAddress; }
	string AsText() 
	{
		string sResult;
		unsigned char *pByte = ((unsigned char *) &m_IPAddress) + 3;
		for(int i=0;i<4;++i)
		{
			if( i )
				sResult += ".";
			sResult += StringUtils::itos(*pByte);
			pByte--;
		}
		return sResult;
	}
	bool BadIP() { return m_bBadIP; }
};

class Database_pluto_main;

// PlutoDHCP does all the actual work
class PlutoDHCP 
{
	bool m_bConnected;  // True if we connected to the MySql server
	int m_iPK_Installation;
	map<unsigned long,class Row_Device *> m_mapIP_Device;
	Database_pluto_main *m_pDatabase_pluto_main;

public:
	PlutoDHCP(int PK_Installation,string host, string user, string pass, string db_name, int port=3306);

	bool bIsMediaDirector(Row_Device *pRow_Device);  // True if the given device is a diskless MD (pxe boot)
	void ReadAllIPs(); // Read all devices/ip's into m_mapIP_Device
	bool IsConnected(); // True if we connected to the MySql server
	Row_Device *DetermineCore();  // Return the device id of the core
	
	// Based on the Core's device data, get the range of ip addresses 
	bool DetermineIPRange(IPAddress &ipAddressDhcpStart,IPAddress &ipAddressDhcpStop,IPAddress &ipAddressPlutoStart,IPAddress &ipAddressPlutoStop);
	void GetNetParams(string &sCoreInternalAddress, string &sInternalSubnet, string &sInternalSubnetMask, string &sInternalDomainName);
	string AssignIP(int PK_Device);  // Assign a new IP address to this device
	string GetDHCPConfig(); // Write out the dhcp configuration data to stdout
};
#endif
