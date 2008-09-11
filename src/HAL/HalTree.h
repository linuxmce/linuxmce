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
//
// C++ Interface: seriald
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef HalTree_h
#define HalTree_h

#include <cstdlib>
#include <vector>
#include <map>
#include <string>

using namespace std;

namespace DCE
{
	class HalValue
	{
	public:
		virtual ~HalValue() {}

		enum HalValueType { hvt_string, hvt_string_list, hvt_int, hvt_bool };
		string m_sValueAsString;

		virtual HalValueType GetType()=0;
		virtual void Assign(string sValue) { m_sValueAsString=sValue; }
		static HalValue *GetValue(string sType);
	};

	class HalValue_int : public HalValue
	{
	public:
		virtual ~HalValue_int() {}

		int m_iValue;
		virtual HalValueType GetType() { return hvt_int; }
		virtual void Assign(string sValue) { HalValue::Assign(sValue); m_iValue=atoi(sValue.c_str()); }
	};

	class HalValue_string : public HalValue
	{
	public:
		virtual ~HalValue_string() {}

		string m_sValue;
		virtual HalValueType GetType() { return hvt_string; }
		virtual void Assign(string sValue) { HalValue::Assign(sValue); m_sValue=sValue; }
	};

	class HalValue_bool : public HalValue
	{
	public:
		virtual ~HalValue_bool() {}

		bool m_bValue;
		virtual HalValueType GetType() { return hvt_bool; }
		virtual void Assign(string sValue) { HalValue::Assign(sValue); m_bValue = (sValue=="true"); }
	};

	class HalDevice
	{
	public:
		map<string,HalValue *> m_mapHalValue;
		HalValue *m_mapHalValue_Find(string sToken) { map<string,HalValue *>::iterator it = m_mapHalValue.find(sToken); return it==m_mapHalValue.end() ? NULL : (*it).second; }
		int m_iID;
		string m_sUdi;

		~HalDevice();
	};

	class HalTree
	{
	public:
		map<int,HalDevice *> m_mapHalDevice;

		~HalTree();

		void Populate();
		void PurgeTree();
		HalDevice *GetDeviceWithParm(string sKey,string sValue);
	};
};

#endif
