/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "TiraAPI.h"
#include "TransAction.h"

#include <cstdio>
#include <cstring>

// Returns  0 -- success
//          1 -- invalid parameter
//          2 -- not enough space to store CCF string

const int PLADiffThresh = 5*32;

int ConvertToCCF(char* CCFOut, int CCFStringSize, const unsigned char* tta) {

   if (tta == 0 )
      return 1;
   if ( CCFOut == 0 )
      return 2;
   const TransmitActionNew* ta = (const TransmitActionNew*) tta;

   if ( ta->TimeBase != 8 && ta->TimeBase != 32 )
      return 1;
   if ( ta->StructureVersion != 0 )
      return 1;
   if (ta->SequenceLength < ta->RepeatBegin )
      return 1;



   // output header
   if ( CCFStringSize < 15 + 5 + 5*(ta->SequenceLength+1)  ) return 2;
   CCFOut[0] = 0;
   strcat(CCFOut, "0000 "); CCFStringSize -= 5;

   // output frequency
   unsigned int periodCCF = (ta->PulsePeriod/100)*41455/100000;
   if (periodCCF > 0xFFFF ) periodCCF = 0xFFFF;
   sprintf(CCFOut + strlen(CCFOut), "%04X ", periodCCF );

   // Output baseLen     (in CCF it is in pairs)
   int baseLen = (ta->RepeatBegin+1) / 2;
   if ( baseLen < 0 ) baseLen = 0;
   sprintf(CCFOut + strlen(CCFOut), "%04X ", baseLen );

   // Output repeatLen
   unsigned int repeatLen = (ta->SequenceLength+1) / 2 - baseLen;
   sprintf(CCFOut + strlen(CCFOut), "%04X ", repeatLen );


   // TranslatePLA

   unsigned int CCFPLA[20];
   for (int i = 0; i < ta->PLA_Size; i++ )
      CCFPLA[i] =  (ta->PLA[i] * 1000 * ta->TimeBase + ta->PulsePeriod/2) / ta->PulsePeriod;

   // Now output data
   const unsigned char* IrData = tta + sizeof(TransmitActionNew);
   int CurrLen = strlen(CCFOut);
   for (int i = 0; i < ta->SequenceLength; i++ ) {
      sprintf(CCFOut + CurrLen, "%04X ", CCFPLA[IrData[i]] );
      CurrLen += 5;
   };

   // make sure we output'ed even number
   if ( ta->SequenceLength % 2 == 0 )
      return 0;

   if ( repeatLen == 0 ) {
      sprintf(CCFOut + CurrLen, "%04X ", 5 ); // Could be any number, really
      CurrLen += 5;
   } else {
      sprintf(CCFOut + CurrLen, "%04X ", CCFPLA[IrData[ta->RepeatBegin]] );
      CurrLen += 5;
   };


   return 0;


};

 
