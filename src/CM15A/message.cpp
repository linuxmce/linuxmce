/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

     Author: Nosilla99, added RF decoding
             Original CM11A version by igor <igor@dexx>, (C) 2004
     Name  : CM15A
     Descr : Active Home CM15A USB X10 PC Interface and RF transceiver
             DCE Device Handler
     Date  : 10.11.2009
     File  : CM15A/message.cpp

*/
#include "message.h"
#include <cstdlib>
#include <stdio.h>

namespace CM15ADEV {

unsigned char Message::s_MapHouseDeviceCodes[] =  {
	0x06, //A
	0x0E, //B
	0x02, //C
	0x0A, //D
	0x01, //E
	0x09, //F
	0x05, //G
	0x0D, //H
	0x07, //I
	0x0F, //J
	0x03, //K
	0x0B, //L
	0x00, //M
	0x08, //N
	0x04, //O
	0x0C  //P
};

x10rftable Message::rftable[] =
{
     { 0x6000,"A1","ON" },
     { 0x6020,"A1","OFF" },
     { 0x6010,"A2","ON" },
     { 0x6030,"A2","OFF" },
     { 0x6008,"A3","ON" },
     { 0x6028,"A3","OFF" },
     { 0x6018,"A4","ON" },
     { 0x6038,"A4","OFF" },
     { 0x6040,"A5","ON" },
     { 0x6060,"A5","OFF" },
     { 0x6050,"A6","ON" },
     { 0x6070,"A6","OFF" },
     { 0x6048,"A7","ON" },
     { 0x6068,"A7","OFF" },
     { 0x6058,"A8","ON" },
     { 0x6078,"A8","OFF" },
     { 0x6400,"A9","ON" },
     { 0x6420,"A9","OFF" },
     { 0x6410,"A10","ON" },
     { 0x6430,"A10","OFF" },
     { 0x6408,"A11","ON" },
     { 0x6428,"A11","OFF" },
     { 0x6418,"A12","ON" },
     { 0x6438,"A12","OFF" },
     { 0x6440,"A13","ON" },
     { 0x6460,"A13","OFF" },
     { 0x6450,"A14","ON" },
     { 0x6470,"A14","OFF" },
     { 0x6448,"A15","ON" },
     { 0x6468,"A15","OFF" },
     { 0x6458,"A16","ON" },
     { 0x6478,"A16","OFF" },
     { 0x6088,"A","BRIGHT" },
     { 0x6098,"A","DIM" },
     { 0x7000,"B1","ON" },
     { 0x7020,"B1","OFF" },
     { 0x7010,"B2","ON" },
     { 0x7030,"B2","OFF" },
     { 0x7008,"B3","ON" },
     { 0x7028,"B3","OFF" },
     { 0x7018,"B4","ON" },
     { 0x7038,"B4","OFF" },
     { 0x7040,"B5","ON" },
     { 0x7060,"B5","OFF" },
     { 0x7050,"B6","ON" },
     { 0x7070,"B6","OFF" },
     { 0x7048,"B7","ON" },
     { 0x7068,"B7","OFF" },
     { 0x7058,"B8","ON" },
     { 0x7078,"B8","OFF" },
     { 0x7400,"B9","ON" },
     { 0x7420,"B9","OFF" },
     { 0x7410,"B10","ON" },
     { 0x7430,"B10","OFF" },
     { 0x7408,"B11","ON" },
     { 0x7428,"B11","OFF" },
     { 0x7418,"B12","ON" },
     { 0x7438,"B12","OFF" },
     { 0x7440,"B13","ON" },
     { 0x7460,"B13","OFF" },
     { 0x7450,"B14","ON" },
     { 0x7470,"B14","OFF" },
     { 0x7448,"B15","ON" },
     { 0x7468,"B15","OFF" },
     { 0x7458,"B16","ON" },
     { 0x7478,"B16","OFF" },
     { 0x7088,"B","BRIGHT" },
     { 0x7098,"B","DIM" },
     { 0x4000,"C1","ON" },
     { 0x4020,"C1","OFF" },
     { 0x4010,"C2","ON" },
     { 0x4030,"C2","OFF" },
     { 0x4008,"C3","ON" },
     { 0x4028,"C3","OFF" },
     { 0x4018,"C4","ON" },
     { 0x4038,"C4","OFF" },
     { 0x4040,"C5","ON" },
     { 0x4060,"C5","OFF" },
     { 0x4050,"C6","ON" },
     { 0x4070,"C6","OFF" },
     { 0x4048,"C7","ON" },
     { 0x4068,"C7","OFF" },
     { 0x4058,"C8","ON" },
     { 0x4078,"C8","OFF" },
     { 0x4400,"C9","ON" },
     { 0x4420,"C9","OFF" },
     { 0x4410,"C10","ON" },
     { 0x4430,"C10","OFF" },
     { 0x4408,"C11","ON" },
     { 0x4428,"C11","OFF" },
     { 0x4418,"C12","ON" },
     { 0x4438,"C12","OFF" },
     { 0x4440,"C13","ON" },
     { 0x4460,"C13","OFF" },
     { 0x4450,"C14","ON" },
     { 0x4470,"C14","OFF" },
     { 0x4448,"C15","ON" },
     { 0x4468,"C15","OFF" },
     { 0x4458,"C16","ON" },
     { 0x4478,"C16","OFF" },
     { 0x4088,"C","BRIGHT" },
     { 0x4098,"C","DIM" },
     { 0x5000,"D1","ON" },
     { 0x5020,"D1","OFF" },
     { 0x5010,"D2","ON" },
     { 0x5030,"D2","OFF" },
     { 0x5008,"D3","ON" },
     { 0x5028,"D3","OFF" },
     { 0x5018,"D4","ON" },
     { 0x5038,"D4","OFF" },
     { 0x5040,"D5","ON" },
     { 0x5060,"D5","OFF" },
     { 0x5050,"D6","ON" },
     { 0x5070,"D6","OFF" },
     { 0x5048,"D7","ON" },
     { 0x5068,"D7","OFF" },
     { 0x5058,"D8","ON" },
     { 0x5078,"D8","OFF" },
     { 0x5400,"D9","ON" },
     { 0x5420,"D9","OFF" },
     { 0x5410,"D10","ON" },
     { 0x5430,"D10","OFF" },
     { 0x5408,"D11","ON" },
     { 0x5428,"D11","OFF" },
     { 0x5418,"D12","ON" },
     { 0x5438,"D12","OFF" },
     { 0x5440,"D13","ON" },
     { 0x5460,"D13","OFF" },
     { 0x5450,"D14","ON" },
     { 0x5470,"D14","OFF" },
     { 0x5448,"D15","ON" },
     { 0x5468,"D15","OFF" },
     { 0x5458,"D16","ON" },
     { 0x5478,"D16","OFF" },
     { 0x5088,"D","BRIGHT" },
     { 0x5098,"D","DIM" },
     { 0x8000,"E1","ON" },
     { 0x8020,"E1","OFF" },
     { 0x8010,"E2","ON" },
     { 0x8030,"E2","OFF" },
     { 0x8008,"E3","ON" },
     { 0x8028,"E3","OFF" },
     { 0x8018,"E4","ON" },
     { 0x8038,"E4","OFF" },
     { 0x8040,"E5","ON" },
     { 0x8060,"E5","OFF" },
     { 0x8050,"E6","ON" },
     { 0x8070,"E6","OFF" },
     { 0x8048,"E7","ON" },
     { 0x8068,"E7","OFF" },
     { 0x8058,"E8","ON" },
     { 0x8078,"E8","OFF" },
     { 0x8400,"E9","ON" },
     { 0x8420,"E9","OFF" },
     { 0x8410,"E10","ON" },
     { 0x8430,"E10","OFF" },
     { 0x8408,"E11","ON" },
     { 0x8428,"E11","OFF" },
     { 0x8418,"E12","ON" },
     { 0x8438,"E12","OFF" },
     { 0x8440,"E13","ON" },
     { 0x8460,"E13","OFF" },
     { 0x8450,"E14","ON" },
     { 0x8470,"E14","OFF" },
     { 0x8448,"E15","ON" },
     { 0x8468,"E15","OFF" },
     { 0x8458,"E16","ON" },
     { 0x8478,"E16","OFF" },
     { 0x8088,"E","BRIGHT" },
     { 0x8098,"E","DIM" },
     { 0x9000,"F1","ON" },
     { 0x9020,"F1","OFF" },
     { 0x9010,"F2","ON" },
     { 0x9030,"F2","OFF" },
     { 0x9008,"F3","ON" },
     { 0x9028,"F3","OFF" },
     { 0x9018,"F4","ON" },
     { 0x9038,"F4","OFF" },
     { 0x9040,"F5","ON" },
     { 0x9060,"F5","OFF" },
     { 0x9050,"F6","ON" },
     { 0x9070,"F6","OFF" },
     { 0x9048,"F7","ON" },
     { 0x9068,"F7","OFF" },
     { 0x9058,"F8","ON" },
     { 0x9078,"F8","OFF" },
     { 0x9400,"F9","ON" },
     { 0x9420,"F9","OFF" },
     { 0x9410,"F10","ON" },
     { 0x9430,"F10","OFF" },
     { 0x9408,"F11","ON" },
     { 0x9428,"F11","OFF" },
     { 0x9418,"F12","ON" },
     { 0x9438,"F12","OFF" },
     { 0x9440,"F13","ON" },
     { 0x9460,"F13","OFF" },
     { 0x9450,"F14","ON" },
     { 0x9470,"F14","OFF" },
     { 0x9448,"F15","ON" },
     { 0x9468,"F15","OFF" },
     { 0x9458,"F16","ON" },
     { 0x9478,"F16","OFF" },
     { 0x9088,"F","BRIGHT" },
     { 0x9098,"F","DIM" },
     { 0xa000,"G1","ON" },
     { 0xa020,"G1","OFF" },
     { 0xa010,"G2","ON" },
     { 0xa030,"G2","OFF" },
     { 0xa008,"G3","ON" },
     { 0xa028,"G3","OFF" },
     { 0xa018,"G4","ON" },
     { 0xa038,"G4","OFF" },
     { 0xa040,"G5","ON" },
     { 0xa060,"G5","OFF" },
     { 0xa050,"G6","ON" },
     { 0xa070,"G6","OFF" },
     { 0xa048,"G7","ON" },
     { 0xa068,"G7","OFF" },
     { 0xa058,"G8","ON" },
     { 0xa078,"G8","OFF" },
     { 0xa400,"G9","ON" },
     { 0xa420,"G9","OFF" },
     { 0xa410,"G10","ON" },
     { 0xa430,"G10","OFF" },
     { 0xa408,"G11","ON" },
     { 0xa428,"G11","OFF" },
     { 0xa418,"G12","ON" },
     { 0xa438,"G12","OFF" },
     { 0xa440,"G13","ON" },
     { 0xa460,"G13","OFF" },
     { 0xa450,"G14","ON" },
     { 0xa470,"G14","OFF" },
     { 0xa448,"G15","ON" },
     { 0xa468,"G15","OFF" },
     { 0xa458,"G16","ON" },
     { 0xa478,"G16","OFF" },
     { 0xa088,"G","BRIGHT" },
     { 0xa098,"G","DIM" },
     { 0xb000,"H1","ON" },
     { 0xb020,"H1","OFF" },
     { 0xb010,"H2","ON" },
     { 0xb030,"H2","OFF" },
     { 0xb008,"H3","ON" },
     { 0xb028,"H3","OFF" },
     { 0xb018,"H4","ON" },
     { 0xb038,"H4","OFF" },
     { 0xb040,"H5","ON" },
     { 0xb060,"H5","OFF" },
     { 0xb050,"H6","ON" },
     { 0xb070,"H6","OFF" },
     { 0xb048,"H7","ON" },
     { 0xb068,"H7","OFF" },
     { 0xb058,"H8","ON" },
     { 0xb078,"H8","OFF" },
     { 0xb400,"H9","ON" },
     { 0xb420,"H9","OFF" },
     { 0xb410,"H10","ON" },
     { 0xb430,"H10","OFF" },
     { 0xb408,"H11","ON" },
     { 0xb428,"H11","OFF" },
     { 0xb418,"H12","ON" },
     { 0xb438,"H12","OFF" },
     { 0xb440,"H13","ON" },
     { 0xb460,"H13","OFF" },
     { 0xb450,"H14","ON" },
     { 0xb470,"H14","OFF" },
     { 0xb448,"H15","ON" },
     { 0xb468,"H15","OFF" },
     { 0xb458,"H16","ON" },
     { 0xb478,"H16","OFF" },
     { 0xb088,"H","BRIGHT" },
     { 0xb098,"H","DIM" },
     { 0xe000,"I1","ON" },
     { 0xe020,"I1","OFF" },
     { 0xe010,"I2","ON" },
     { 0xe030,"I2","OFF" },
     { 0xe008,"I3","ON" },
     { 0xe028,"I3","OFF" },
     { 0xe018,"I4","ON" },
     { 0xe038,"I4","OFF" },
     { 0xe040,"I5","ON" },
     { 0xe060,"I5","OFF" },
     { 0xe050,"I6","ON" },
     { 0xe070,"I6","OFF" },
     { 0xe048,"I7","ON" },
     { 0xe068,"I7","OFF" },
     { 0xe058,"I8","ON" },
     { 0xe078,"I8","OFF" },
     { 0xe400,"I9","ON" },
     { 0xe420,"I9","OFF" },
     { 0xe410,"I10","ON" },
     { 0xe430,"I10","OFF" },
     { 0xe408,"I11","ON" },
     { 0xe428,"I11","OFF" },
     { 0xe418,"I12","ON" },
     { 0xe438,"I12","OFF" },
     { 0xe440,"I13","ON" },
     { 0xe460,"I13","OFF" },
     { 0xe450,"I14","ON" },
     { 0xe470,"I14","OFF" },
     { 0xe448,"I15","ON" },
     { 0xe468,"I15","OFF" },
     { 0xe458,"I16","ON" },
     { 0xe478,"I16","OFF" },
     { 0xe088,"I","BRIGHT" },
     { 0xe098,"I","DIM" },
     { 0xf000,"J1","ON" },
     { 0xf020,"J1","OFF" },
     { 0xf010,"J2","ON" },
     { 0xf030,"J2","OFF" },
     { 0xf008,"J3","ON" },
     { 0xf028,"J3","OFF" },
     { 0xf018,"J4","ON" },
     { 0xf038,"J4","OFF" },
     { 0xf040,"J5","ON" },
     { 0xf060,"J5","OFF" },
     { 0xf050,"J6","ON" },
     { 0xf070,"J6","OFF" },
     { 0xf048,"J7","ON" },
     { 0xf068,"J7","OFF" },
     { 0xf058,"J8","ON" },
     { 0xf078,"J8","OFF" },
     { 0xf400,"J9","ON" },
     { 0xf420,"J9","OFF" },
     { 0xf410,"J10","ON" },
     { 0xf430,"J10","OFF" },
     { 0xf408,"J11","ON" },
     { 0xf428,"J11","OFF" },
     { 0xf418,"J12","ON" },
     { 0xf438,"J12","OFF" },
     { 0xf440,"J13","ON" },
     { 0xf460,"J13","OFF" },
     { 0xf450,"J14","ON" },
     { 0xf470,"J14","OFF" },
     { 0xf448,"J15","ON" },
     { 0xf468,"J15","OFF" },
     { 0xf458,"J16","ON" },
     { 0xf478,"J16","OFF" },
     { 0xf088,"J","BRIGHT" },
     { 0xf098,"J","DIM" },
     { 0xc000,"K1","ON" },
     { 0xc020,"K1","OFF" },
     { 0xc010,"K2","ON" },
     { 0xc030,"K2","OFF" },
     { 0xc008,"K3","ON" },
     { 0xc028,"K3","OFF" },
     { 0xc018,"K4","ON" },
     { 0xc038,"K4","OFF" },
     { 0xc040,"K5","ON" },
     { 0xc060,"K5","OFF" },
     { 0xc050,"K6","ON" },
     { 0xc070,"K6","OFF" },
     { 0xc048,"K7","ON" },
     { 0xc068,"K7","OFF" },
     { 0xc058,"K8","ON" },
     { 0xc078,"K8","OFF" },
     { 0xc400,"K9","ON" },
     { 0xc420,"K9","OFF" },
     { 0xc410,"K10","ON" },
     { 0xc430,"K10","OFF" },
     { 0xc408,"K11","ON" },
     { 0xc428,"K11","OFF" },
     { 0xc418,"K12","ON" },
     { 0xc438,"K12","OFF" },
     { 0xc440,"K13","ON" },
     { 0xc460,"K13","OFF" },
     { 0xc450,"K14","ON" },
     { 0xc470,"K14","OFF" },
     { 0xc448,"K15","ON" },
     { 0xc468,"K15","OFF" },
     { 0xc458,"K16","ON" },
     { 0xc478,"K16","OFF" },
     { 0xc088,"K","BRIGHT" },
     { 0xc098,"K","DIM" },
     { 0xd000,"L1","ON" },
     { 0xd020,"L1","OFF" },
     { 0xd010,"L2","ON" },
     { 0xd030,"L2","OFF" },
     { 0xd008,"L3","ON" },
     { 0xd028,"L3","OFF" },
     { 0xd018,"L4","ON" },
     { 0xd038,"L4","OFF" },
     { 0xd040,"L5","ON" },
     { 0xd060,"L5","OFF" },
     { 0xd050,"L6","ON" },
     { 0xd070,"L6","OFF" },
     { 0xd048,"L7","ON" },
     { 0xd068,"L7","OFF" },
     { 0xd058,"L8","ON" },
     { 0xd078,"L8","OFF" },
     { 0xd400,"L9","ON" },
     { 0xd420,"L9","OFF" },
     { 0xd410,"L10","ON" },
     { 0xd430,"L10","OFF" },
     { 0xd408,"L11","ON" },
     { 0xd428,"L11","OFF" },
     { 0xd418,"L12","ON" },
     { 0xd438,"L12","OFF" },
     { 0xd440,"L13","ON" },
     { 0xd460,"L13","OFF" },
     { 0xd450,"L14","ON" },
     { 0xd470,"L14","OFF" },
     { 0xd448,"L15","ON" },
     { 0xd468,"L15","OFF" },
     { 0xd458,"L16","ON" },
     { 0xd478,"L16","OFF" },
     { 0xd088,"L","BRIGHT" },
     { 0xd098,"L","DIM" },
     { 0x0000,"M1","ON" },
     { 0x0020,"M1","OFF" },
     { 0x0010,"M2","ON" },
     { 0x0030,"M2","OFF" },
     { 0x0008,"M3","ON" },
     { 0x0028,"M3","OFF" },
     { 0x0018,"M4","ON" },
     { 0x0038,"M4","OFF" },
     { 0x0040,"M5","ON" },
     { 0x0060,"M5","OFF" },
     { 0x0050,"M6","ON" },
     { 0x0070,"M6","OFF" },
     { 0x0048,"M7","ON" },
     { 0x0068,"M7","OFF" },
     { 0x0058,"M8","ON" },
     { 0x0078,"M8","OFF" },
     { 0x0400,"M9","ON" },
     { 0x0420,"M9","OFF" },
     { 0x0410,"M10","ON" },
     { 0x0430,"M10","OFF" },
     { 0x0408,"M11","ON" },
     { 0x0428,"M11","OFF" },
     { 0x0418,"M12","ON" },
     { 0x0438,"M12","OFF" },
     { 0x0440,"M13","ON" },
     { 0x0460,"M13","OFF" },
     { 0x0450,"M14","ON" },
     { 0x0470,"M14","OFF" },
     { 0x0448,"M15","ON" },
     { 0x0468,"M15","OFF" },
     { 0x0458,"M16","ON" },
     { 0x0478,"M16","OFF" },
     { 0x0088,"M","BRIGHT" },
     { 0x0098,"M","DIM" },
     { 0x1000,"N1","ON" },
     { 0x1020,"N1","OFF" },
     { 0x1010,"N2","ON" },
     { 0x1030,"N2","OFF" },
     { 0x1008,"N3","ON" },
     { 0x1028,"N3","OFF" },
     { 0x1018,"N4","ON" },
     { 0x1038,"N4","OFF" },
     { 0x1040,"N5","ON" },
     { 0x1060,"N5","OFF" },
     { 0x1050,"N6","ON" },
     { 0x1070,"N6","OFF" },
     { 0x1048,"N7","ON" },
     { 0x1068,"N7","OFF" },
     { 0x1058,"N8","ON" },
     { 0x1078,"N8","OFF" },
     { 0x1400,"N9","ON" },
     { 0x1420,"N9","OFF" },
     { 0x1410,"N10","ON" },
     { 0x1430,"N10","OFF" },
     { 0x1408,"N11","ON" },
     { 0x1428,"N11","OFF" },
     { 0x1418,"N12","ON" },
     { 0x1438,"N12","OFF" },
     { 0x1440,"N13","ON" },
     { 0x1460,"N13","OFF" },
     { 0x1450,"N14","ON" },
     { 0x1470,"N14","OFF" },
     { 0x1448,"N15","ON" },
     { 0x1468,"N15","OFF" },
     { 0x1458,"N16","ON" },
     { 0x1478,"N16","OFF" },
     { 0x1088,"N","BRIGHT" },
     { 0x1098,"N","DIM" },
     { 0x2000,"O1","ON" },
     { 0x2020,"O1","OFF" },
     { 0x2010,"O2","ON" },
     { 0x2030,"O2","OFF" },
     { 0x2008,"O3","ON" },
     { 0x2028,"O3","OFF" },
     { 0x2018,"O4","ON" },
     { 0x2038,"O4","OFF" },
     { 0x2040,"O5","ON" },
     { 0x2060,"O5","OFF" },
     { 0x2050,"O6","ON" },
     { 0x2070,"O6","OFF" },
     { 0x2048,"O7","ON" },
     { 0x2068,"O7","OFF" },
     { 0x2058,"O8","ON" },
     { 0x2078,"O8","OFF" },
     { 0x2400,"O9","ON" },
     { 0x2420,"O9","OFF" },
     { 0x2410,"O10","ON" },
     { 0x2430,"O10","OFF" },
     { 0x2408,"O11","ON" },
     { 0x2428,"O11","OFF" },
     { 0x2418,"O12","ON" },
     { 0x2438,"O12","OFF" },
     { 0x2440,"O13","ON" },
     { 0x2460,"O13","OFF" },
     { 0x2450,"O14","ON" },
     { 0x2470,"O14","OFF" },
     { 0x2448,"O15","ON" },
     { 0x2468,"O15","OFF" },
     { 0x2458,"O16","ON" },
     { 0x2478,"O16","OFF" },
     { 0x2088,"O","BRIGHT" },
     { 0x2098,"O","DIM" },
     { 0x3000,"P1","ON" },
     { 0x3020,"P1","OFF" },
     { 0x3010,"P2","ON" },
     { 0x3030,"P2","OFF" },
     { 0x3008,"P3","ON" },
     { 0x3028,"P3","OFF" },
     { 0x3018,"P4","ON" },
     { 0x3038,"P4","OFF" },
     { 0x3040,"P5","ON" },
     { 0x3060,"P5","OFF" },
     { 0x3050,"P6","ON" },
     { 0x3070,"P6","OFF" },
     { 0x3048,"P7","ON" },
     { 0x3068,"P7","OFF" },
     { 0x3058,"P8","ON" },
     { 0x3078,"P8","OFF" },
     { 0x3400,"P9","ON" },
     { 0x3420,"P9","OFF" },
     { 0x3410,"P10","ON" },
     { 0x3430,"P10","OFF" },
     { 0x3408,"P11","ON" },
     { 0x3428,"P11","OFF" },
     { 0x3418,"P12","ON" },
     { 0x3438,"P12","OFF" },
     { 0x3440,"P13","ON" },
     { 0x3460,"P13","OFF" },
     { 0x3450,"P14","ON" },
     { 0x3470,"P14","OFF" },
     { 0x3448,"P15","ON" },
     { 0x3468,"P15","OFF" },
     { 0x3458,"P16","ON" },
     { 0x3478,"P16","OFF" },
     { 0x3088,"P","BRIGHT" },
     { 0x3098,"P","DIM" }
};

Message::Message() 
	: houseCode_(0), deviceCode_(0), functionCode_(0), dimmLevel_(0)
{
}

Message::~Message() {
}


void 
Message::setAddress(std::string address) {// adress in the form of A1, B2,...
	int len = address.length();
	if(len < 2 || len > 3) {
		return;
	}
	
	if(address[0] >= 'A' && address[0] <= 'P') {
		houseCode_ = s_MapHouseDeviceCodes[address[0] - 'A'];
	}
	
	int devcode = atoi(address.substr(1, len - 1).c_str());
	if(devcode >= 1 && devcode <= 16) {
		deviceCode_ = s_MapHouseDeviceCodes[devcode - 1];
	}
}

char* Message::getHouseCode(unsigned char x10addr)
{
	char house_code=(x10addr >> 4) & 0x0F;
	int i;
	static char buffer[2];

	for(i=0;i<16;i++)
	{
		if(house_code == s_MapHouseDeviceCodes[i])
		{
			sprintf(buffer, "%c", 'A'+i);
			break;
		}
	}
	return buffer;
}

char* Message::getAddress(unsigned char x10addr)
{
	char house_code=(x10addr >> 4) & 0x0F;
	char unit_code=x10addr & 0x0F;
	static char buffer[4];
	int i;
	for(i=0;i<16;i++)
	{
		if(house_code == s_MapHouseDeviceCodes[i])
		{
			house_code='A'+i;
			break;
		}
	}
	for(i=0;i<16;i++)
	{
		if(unit_code == s_MapHouseDeviceCodes[i])
		{
			unit_code=1+i;
			break;
		}
	}
	sprintf(buffer,"%c%d",house_code,unit_code);
	return buffer;
}

char* Message::getRfAddress(int rfcode)
{
	static char buffer[4];
        int idx;
        
        for (idx=0; idx < X10_RFCMDS; idx++) {
            if (rftable[idx].value == rfcode) {
	        sprintf(buffer,"%s",rftable[idx].addr);
	        return buffer;
            }
        }
	sprintf(buffer,"%s",rftable[1].addr);
        return buffer;
}

char* Message::getRfString(int rfcode)
{
	static char buffer[16];
        int idx;
        
        for (idx=0; idx < X10_RFCMDS; idx++) {
            if (rftable[idx].value == rfcode) {
	        sprintf(buffer,"%s %s",rftable[idx].addr, rftable[idx].cmd);
	        return buffer;
            }
        }
	sprintf(buffer,"%s %s",rftable[1].addr, rftable[1].cmd);
        return buffer;
}
};
