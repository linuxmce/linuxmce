/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "TestRubyEmbederCodeSupplier.h"

namespace EMBRUBY {

const char* 
TestRubyEmbederCodeSupplier::getRubyCode() {
	return 
	""
	"require 'Ruby_Generic_Serial_Device'""\n"
	"class Test < Ruby_Generic_Serial_Device::RubySerialIOWrapper""\n"
/*	
		"def TestMtd()""\n"
			"print \"curuverde in Test Class\\n\"""\n"
			"conn_.Send(\"www\")""\n"
			"str = conn_.Recv(10, 1)""\n"
			"print \"(\"""\n"
			"print str[2]""\n"
			"print \")\\n\"""\n"
		"end""\n"
*/		
		"def On()""\n"
			"conn_ = getConn()""\n"
			
			"address = \"A9\"""\n"
			"housedevcodes = [0x06, 0x0E, 0x02, 0x0A, 0x01, 0x09, 0x05, 0x0D, 0x07, 0x0F, 0x03, 0x0B, 0x00, 0x08, 0x04, 0x0C]""\n"
			"if address.length() < 2 || address.length() > 3 then""\n"
			"else""\n"
				"housecode = housedevcodes[address[0] - 65];""\n"
				"devicecode = address[1,address.length() - 1].to_i()""\n"
				"if devicecode < 1 || devicecode > 16 then""\n"
				"else""\n"
					"dimlevel = 0""\n"
			
					"devicecode = housedevcodes[devicecode - 1]""\n"
			
					"#send address""\n"
					"highbyte = 4""\n"
					"lowbyte = (housecode << 4) | devicecode""\n"
						
					"buff = \"HL\"""\n"
					"buff[0] = highbyte""\n"
					"buff[1] = lowbyte""\n"
					"conn_.Send(buff)""\n"
					
					"buff = conn_.Recv(1, 1000)""\n"
					
					"print \"Received response: \", buff[0], \"\\n\" ""\n"
					
					"buff = \"\\0\"""\n"
					"conn_.Send(buff)""\n"
					"buff = conn_.Recv(1, 1000)""\n"
					"print \"Received response: \", buff[0], \"\\n\" ""\n"
									
					"#send function""\n"
			
					"highbyte = (dimlevel << 3) | 6""\n"
					"lowbyte = (housecode << 4) | 2 # 2 - on function code""\n"

					"buff = \"HL\"""\n"
					"buff[0] = highbyte""\n"
					"buff[1] = lowbyte""\n"
					"conn_.Send(buff)""\n"
					
					"buff = conn_.Recv(1, 1000)""\n"
					"print \"Received response: \", buff[0], \"\\n\" ""\n"
					
					"buff = \"\\0\"""\n"
					"conn_.Send(buff)""\n"
					"buff = conn_.Recv(1, 1000)""\n"
					"print \"Received response: \", buff[0], \"\\n\" ""\n"
				"end""\n"
			"end""\n"
			
		"end""\n"
		
		"def Off()""\n"
		"end""\n"
	"end""\n"
	"";
}

};
