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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"
#include "PlutoDHCP.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#include "BD/PhoneDevice.h"

#define EXTERN_REVISION_DATE
#include "version.h"

using namespace std;
using namespace DCE;

PlutoDHCP::PlutoDHCP(int PK_Installation,string host, string user, string pass, string db_name, int port) 
{
    m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
    if( !m_pDatabase_pluto_main->Connect( host, user, pass, db_name, port ) )
	{
		cerr << "Cannot connect to database" << endl;
		m_bConnected=false;
	}
	else
		m_bConnected=true;

	if (PK_Installation > 0)
		m_iPK_Installation = PK_Installation;
	else
	{
		vector<Row_Installation *> vectRow_Installation;
		m_pDatabase_pluto_main->Installation_get()->GetRows("1=1",&vectRow_Installation);
		if( vectRow_Installation.size()==0 )
		{
			cerr << "ERROR: Cannot determine Installation #" << endl;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot determine installation");
			return;
		}
		else if( vectRow_Installation.size()>1 )
		{
			cerr << "WARNING: Cannot determine Installation # definitively" << endl;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot determine Installation # definitively");
		}
		m_iPK_Installation = vectRow_Installation[0]->PK_Installation_get();
	}
}

bool PlutoDHCP::IsConnected()
{
	return m_bConnected;
}

void PlutoDHCP::ReadAllIPs()
{
	vector<Row_Device *> m_vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("IPAddress IS NOT NULL and IPAddress<>'' AND FK_Installation=" + StringUtils::itos(m_iPK_Installation),&m_vectRow_Device);
	for(size_t s=0;s<m_vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = m_vectRow_Device[s];
		IPAddress ip(pRow_Device->IPaddress_get());
		if( ip.BadIP() )
			cerr << "Warning: Device " << pRow_Device->PK_Device_get() << " has a bad IP" << endl;
		else
			m_mapIP_Device[ip.AsInt()] = pRow_Device;
	}
}

Row_Device *PlutoDHCP::DetermineCore()
{
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Core_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation),&vectRow_Device);
	if( vectRow_Device.size()==0 )
	{
		cerr << "ERROR: Cannot determine Core #" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot determine core");
		return NULL;
	}
	else if( vectRow_Device.size()>1 )
	{
		cerr << "WARNING: Cannot determine core # definitively" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot determine core # definitively");
	}
	return vectRow_Device[0];
}

bool PlutoDHCP::DetermineIPRange(IPAddress &ipAddressDhcpStart,IPAddress &ipAddressDhcpStop,IPAddress &ipAddressPlutoStart,IPAddress &ipAddressPlutoStop)
{
	Row_Device *pRow_Device = DetermineCore();
	if( !pRow_Device )
		return false;
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_DHCP_CONST);
	if (!pRow_Device_DeviceData)
	{
		cerr << "ERROR: Cannot find dhcp data" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find dhcp data");
		return false;
	}

	// This format: 192.168.80.2-192.168.80.128[,192.168.80.129-192.168.80.254]
	// <Pluto Start IP>-<Pluto End IP>[,<Generic Start IP>-<Generic End IP>]
	string sData = pRow_Device_DeviceData->IK_DeviceData_get();
	string::size_type posComma = sData.find(",");
	string::size_type posDash = sData.find("-");
	string::size_type pos2ndDash = posComma==string::npos ? string::npos : sData.find("-",posComma);

	bool bGenericExists = (posComma != string::npos);
	
	// first dash always exists; second dash must exist if comma exists
	if (posDash == string::npos || (bGenericExists && pos2ndDash == string::npos))
	{
		cerr << "ERROR: dhcp data incomplete" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"dhcp data incomplete");
		return false;
	}

	ipAddressPlutoStart.ParseIP(sData.substr(0, posDash));
	ipAddressPlutoStop.ParseIP(sData.substr(posDash + 1, posComma - posDash - 1));

	// this code is to be run only if the generic range part exists
	if (bGenericExists)
	{
		ipAddressDhcpStart.ParseIP(sData.substr(posComma + 1, pos2ndDash - posComma - 1));
		ipAddressDhcpStop.ParseIP(sData.substr(pos2ndDash + 1));
	}
	else
	{
		ipAddressDhcpStart.ParseIP("0.0.0.0");
		ipAddressDhcpStop.ParseIP("0.0.0.0");
	}

	if (ipAddressPlutoStart.BadIP() || ipAddressPlutoStop.BadIP() || 
		(bGenericExists && (ipAddressDhcpStart.BadIP() || ipAddressDhcpStop.BadIP())))
	{
		cerr << "ERROR: dhcp data malformed" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"dhcp data malformed");
		return false;
	}

	return true;
}

string PlutoDHCP::AssignIP(int PK_Device)
{
	// The device we are going to assign an IP address to
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device);
	if( !pRow_Device )
	{
		cout << "ERROR: Device is invalid" << endl;
		return "";
	}

	// See if it already has one
	string sIP = pRow_Device->IPaddress_get();
	if( sIP.size() && sIP!="auto" )
	{
		// It's already got a good one, use it
		IPAddress ip(sIP);
		if( !ip.BadIP() )
			return ip.AsText();
	}

	// Nope, we're going to have to find an unused IP address
	IPAddress ipAddressDhcpStart,ipAddressDhcpStop,ipAddressPlutoStart,ipAddressPlutoStop;
	if (!DetermineIPRange(ipAddressDhcpStart,ipAddressDhcpStop,ipAddressPlutoStart,ipAddressPlutoStop))
	{
		cout << "Failed to determine IP Range" << endl;
		return "";
	}
	
	for(unsigned long ip = ipAddressPlutoStart.AsInt(); ip<=ipAddressPlutoStop.AsInt(); ip++)
	{
		if( m_mapIP_Device.find(ip)==m_mapIP_Device.end() )
		{
			// We found one that's not used
			IPAddress _ip(ip);
			pRow_Device->IPaddress_set( _ip.AsText() );
			m_pDatabase_pluto_main->Device_get()->Commit();
			return _ip.AsText();
		}
	}

	cout << "No more IP Addresses!" << endl;
	return "";
}

string PlutoDHCP::GetDHCPConfig()
{
	string sCoreInternalAddress, sInternalSubnet, sInternalSubnetMask, sReverseInternalSubnet, sInternalDomainName;
	IPAddress ipAddressDhcpStart, ipAddressDhcpStop, ipAddressPlutoStart, ipAddressPlutoStop;

	GetNetParams(sCoreInternalAddress, sInternalSubnet, sInternalSubnetMask, sInternalDomainName);
	DetermineIPRange(ipAddressDhcpStart, ipAddressDhcpStop, ipAddressPlutoStart, ipAddressPlutoStop);

	if (sCoreInternalAddress.size() == 0 || sInternalSubnetMask.size() == 0)
	{
		cerr << "Missing internal network data" << endl;
		return "";
	}

	int iMoonNumber = 0, iNoBootNumber = 0;
	string sMoonEntries, sNoBootEntries, sNoMacEntries;
	int *p_iIpAddress = (int *) malloc (sizeof(int) * m_mapIP_Device.size());
	int i_NoOfIpAdresses = 0;

	for(map<unsigned long,class Row_Device *>::iterator it=m_mapIP_Device.begin();it!=m_mapIP_Device.end();++it)
	{
		Row_Device *pRow_Device = it->second;

		// Confirm the mac address is valid.  Use Bluetooth's phoneDevice class.  Read it in as a string
		// (which converts it to an int and stores as uppercase), and then read in the int it converted to.
		// If they are the same, we got a valid mac address.  If they are different, then something is wrong with it
		PhoneDevice pd_1("",pRow_Device->MACaddress_get(),0);
		PhoneDevice pd_2("",pd_1.m_iMacAddress,0);
		if( pd_1.m_sMacAddress!=pd_2.m_sMacAddress )
		{
			// Use cerr since cout is going into the dhcp file
			cerr << "Device " << pRow_Device->PK_Device_get() << " has bad mac address " << pRow_Device->MACaddress_get() << endl;
			sNoMacEntries += "# \t" + pRow_Device->Description_get() + " (" + StringUtils::itos(pRow_Device->PK_Device_get()) + ") has bad mac address: " + pRow_Device->MACaddress_get() + "\n";
			continue;
		}

		IPAddress ip(pRow_Device->IPaddress_get());
		string sDeviceComment = "\n\t# " + pRow_Device->Description_get() + " (" + StringUtils::itos(pRow_Device->PK_Device_get()) + ")";
		if( bIsMediaDirector(pRow_Device) )
		{
			sMoonEntries += sDeviceComment;
			sMoonEntries += "\n\thost moon" + StringUtils::itos(pRow_Device->PK_Device_get())  
				+ " { hardware ethernet " + pd_1.m_sMacAddress + "; fixed-address " + ip.AsText() + "; }";
		}
		else
		{
			sNoBootEntries += sDeviceComment;
			sNoBootEntries += "\n\thost pc" + StringUtils::itos(++iNoBootNumber)
				+ " { hardware ethernet " + pd_1.m_sMacAddress + "; fixed-address " + ip.AsText() + "; }";
		}

		if ((int)ip.AsInt() >= (int)ipAddressDhcpStart.AsInt() && (int)ip.AsInt() <= (int)ipAddressDhcpStop.AsInt()) {
			p_iIpAddress[i_NoOfIpAdresses++] = ip.AsInt();
		}
	}


	// The dhcp.conf stuff
	string sDynamicPool("");
	if (ipAddressDhcpStart.AsInt() != 0)
	{
		sDynamicPool =
		"pool {"																					"\n"
		"\t\t allow unknown-clients;"																"\n";
		if (i_NoOfIpAdresses > 0) {
			if (ipAddressDhcpStart.AsInt() <= p_iIpAddress[0] - 1) {
				sDynamicPool += "\t\t range " + ipAddressDhcpStart.AsText() + " " +  IPAddress(p_iIpAddress[0] - 1).AsText() +  "; \n";
			}

			for (int i = 0; i < i_NoOfIpAdresses - 1 ; i++) {
				if (p_iIpAddress[i+1] - 1 >= p_iIpAddress[i] + 1 ) {
					sDynamicPool += "\t\t range " + IPAddress(p_iIpAddress[i] + 1).AsText() + " " + IPAddress(p_iIpAddress[i+1] - 1 ).AsText() + "; \n";
				}
			}

			if (p_iIpAddress[i_NoOfIpAdresses - 1] < ipAddressDhcpStop.AsInt()) {
				sDynamicPool += "\t\t range " + IPAddress(p_iIpAddress[i_NoOfIpAdresses - 1] + 1).AsText() + " " + ipAddressDhcpStop.AsText() + "; \n";
			}
		} else {
			sDynamicPool += "\t\t range " + ipAddressDhcpStart.AsText() + " " + ipAddressDhcpStop.AsText() + "; \n"; 
		}
		sDynamicPool += "\t}\n";
	}

	// Make a reverse DNS zone compatible string
	sReverseInternalSubnet = ipAddressDhcpStart.GetOctet(3) + "." + ipAddressDhcpStart.GetOctet(2) + "." + ipAddressDhcpStart.GetOctet(1);

	// Load and use the dhcpd.conf template file
	string sTemplate = "/usr/pluto/templates/dhcpd.conf.tmpl";
	size_t size;
	char *pBuffer = FileUtils::ReadFileIntoBuffer(sTemplate,size);

	// Convert char buffer into a string
	string sConfigData;
	sConfigData += pBuffer;
	delete pBuffer;
 
	// Replace the template tags
	sConfigData=StringUtils::Replace(sConfigData,"%CORE_INTERNAL_ADDRESS%",sCoreInternalAddress);
	sConfigData=StringUtils::Replace(sConfigData,"%INTERNAL_SUBNET_MASK%",sInternalSubnetMask);
	sConfigData=StringUtils::Replace(sConfigData,"%INTERNAL_SUBNET%",sInternalSubnet);
	sConfigData=StringUtils::Replace(sConfigData,"%INTERNAL_DOMAIN_NAME%",sInternalDomainName);
	sConfigData=StringUtils::Replace(sConfigData,"%DYNAMIC_IP_RANGE%",sDynamicPool);
	sConfigData=StringUtils::Replace(sConfigData,"%MOON_ENTRIES%",sMoonEntries);
	sConfigData=StringUtils::Replace(sConfigData,"%NOBOOT_ENTRIES%",sNoBootEntries);
	sConfigData=StringUtils::Replace(sConfigData,"%NOMAC_ENTRIES%",sNoMacEntries);
	sConfigData=StringUtils::Replace(sConfigData,"%DYNAMIC_REVERSE_RANGE%",sReverseInternalSubnet);

	return sConfigData;
}

bool PlutoDHCP::bIsMediaDirector(Row_Device *pRow_Device)
{
	Row_DeviceTemplate *pRow_DeviceTemplate=NULL;
	if( (pRow_DeviceTemplate=pRow_Device->FK_DeviceTemplate_getrow())==NULL ||
		pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Media_Director_CONST )
			return false; // We're not an MD

	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(
		pRow_Device->PK_Device_get(),
		DEVICEDATA_Diskless_Boot_CONST);

	// See if we're a diskless MD, return false if the parameter isn't specified or isn't 1
	return pRow_Device_DeviceData && pRow_Device_DeviceData->IK_DeviceData_get()=="1";
}

void PlutoDHCP::GetNetParams(string &sCoreInternalAddress, string &sInternalSubnet, string &sInternalSubnetMask, string &sInternalDomainName)
{
	Row_Device * pRow_Device = DetermineCore();
	if (!pRow_Device)
		return;
	Row_Device_DeviceData * pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(), DEVICEDATA_IPv4_Network_Interfaces_CONST);
	if (!pRow_Device_DeviceData)
	{
		cerr << "ERROR: Cannot find network data" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot find network data");
		return;
	}

	string::size_type pos;
	string sDeviceData = pRow_Device_DeviceData->IK_DeviceData_get();
	
	// Format: eth0,10.0.0.89,255.255.255.0,10.0.0.1,10.0.0.150|eth1,192.168.80.1,255.255.255.0
	pos = 0;
	string sExtData = StringUtils::Tokenize(sDeviceData, "|", pos);
	string sIntData = StringUtils::Tokenize(sDeviceData, "|", pos);

	if (sIntData.size() == 0)
	{
		cerr << "ERROR: No internal interface" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No internal interface");
		return;
	}

	/*
	// external interface data
	pos = 0;
	string sExtIf = StringUtils::Tokenize(sExtData, ",", pos);
	string sExtIP = StringUtils::Tokenize(sExtData, ",", pos);
	string sExtNetMask = StringUtils::Tokenize(sExtData, ",", pos);
	string sDNS1 = StringUtils::Tokenize(sExtData, ",", pos);
	string sDNS2 = StringUtils::Tokenize(sExtData, ",", pos);
	*/

	// internal interface data
	pos = 0;
	string sIntIf = StringUtils::Tokenize(sIntData, ",", pos);
	sCoreInternalAddress = StringUtils::Tokenize(sIntData, ",", pos);
	sInternalSubnetMask = StringUtils::Tokenize(sIntData, ",", pos);

	IPAddress ipCoreInternalAddress(sCoreInternalAddress);
	IPAddress ipInternalSubnetMask(sInternalSubnetMask);
	IPAddress ipInternalSubnet(ipCoreInternalAddress.AsInt() & ipInternalSubnetMask.AsInt());
	sInternalSubnet = ipInternalSubnet.AsText();
	
	pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(), DEVICEDATA_Domain_CONST);
	if (!pRow_Device_DeviceData)
	{
		cerr << "ERROR: Cannot find domain name" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot find network data");
		return;
	}

	sInternalDomainName=pRow_Device_DeviceData->IK_DeviceData_get();
}
