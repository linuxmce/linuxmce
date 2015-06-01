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
/****************************************************************************}
{ Program: GenerateRCx.cpp                                                   }
{ Author : M.Majoor                                                          }
{----------------------------------------------------------------------------}
{ Generate a raw learned code from a Pronto RC5/RC5x/RC6/RC6A code.          }
{                                                                            }
{ Command line parameters:                                                   }
{   1    2         3       4       5      6        7       8       9         }
{   5000 0000      0000    0001    ssss   cccc     Repeat                    }
{   5000 ffff      0000    0001    ssss   cccc     Repeat                    }
{   5001 0000      0000    0002    ssss   cccc     dddd    0000    Repeat    }
{   5001 ffff      0000    0002    ssss   cccc     dddd    0000    Repeat    }
{   6000 0000      0000    0001    ssss   cccc     Repeat                    }
{   6000 ffff      0000    0001    ssss   cccc     Repeat                    }
{   6001 0000      0000    0002    uuuu   ssss     cccc    0000    Repeat    }
{   6001 ffff      0000    0002    uuuu   ssss     cccc    0000    Repeat    }
{                                                                            }
{ were:                                                                      }
{   Repeat    == Repeat (optional)                                           }
{                0    generates a 'once' code                                }
{                <>0  generates a 'repeat' code                              }   
{                Default = 0                                                 }
{   ffff      == Carrier frequency in Pronto units, in hexadecimal           }
{   uuuu      == Customer, in hexadecimal                                    }
{   ssss      == System, in hexadecimal                                      }
{   cccc      == Command, in hexadecimal                                     }
{   dddd      == Data, in hexadecimal                                        }
{                                                                            }
{ Returned result code:                                                      }
{    0 = Success                                                             }
{   -1 = Number of parameters not correct                                    }
{  -1x = Error in parameter x, where '1' is the first parameter              }
{ If no error is detected then the result is being output on the console.    }
{                                                                            }
{ Version                                                                    }
{  1.00   20050904 - First release                                           }
{****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
using namespace ::std;

#ifdef WIN32
#include <tchar.h>
#else
#define _tcsninc(_pc, _sz) (((_pc)+(_sz)))  // Linux doesn't have this macro
void strupr(char *Result)
{
	while(*Result)
	{
		if( *Result>='a' && *Result<='z' )
			*Result = *Result-32;
		Result++;
	}
}

#endif

bool Toggle = false;                             // Defines the value of the toggle bit
char Result[1024];                               // Text being displayed as result
char BitString[128];                             // String holding bit data
char EncodeBitString[128];                       // Result of EncodeBits
char HalfBitString[128];                         // String holding half bit data
int  error;                                      // Result code to return
unsigned long DefaultFrequency = 36000;          // Default carrier frequency
int  BitTime = 889;                              // Bit time in us
int  Delay  = 69;                                // Default 69 ms delay after codes
char Zero[3] = "00";                             // Normal phase bits '0'
char One[3]  = "11";                             // Normal phase bits '1'
char BiphaseZero[3] = "10";                      // Bi-phase bits '0' (RC5)
char BiphaseOne[3]  = "01";                      // Bi-phase bits '1' (RC5)
char BiphaseZero2[3] = "01";                     // Bi-phase bits '0' (RC6)
char BiphaseOne2[3]  = "10";                     // Bi-phase bits '1' (RC6)

unsigned long Frequency = 0;
unsigned long System    = 0;
unsigned long Command   = 0;
unsigned long Data      = 0;
unsigned long Customer  = 0;
unsigned long Repeat    = 0;



/*-----------------------------------------------------------------------------
  Params  : <Data>       Data bit s to convert
            <FromBit>    Starting bit [0..15]
            <ToBit>      Ending bit to convert [0..15]
            <AZero>      How a '0' bit is to be converted to
            <AOne>       How a '1' bit is to be converted to
  Returns : -            Global result in 'EncodeBitString'

  Descript: Encode bits to a string representation
  Notes   : -
 -----------------------------------------------------------------------------*/
void EncodeBits(int Data, int FromBit, int ToBit, char AZero[3], char AOne[2])
{
  int Loop;

  strcpy(EncodeBitString, "");
  // Safety checks
  if (FromBit > 15)
    { return; }
  if (ToBit > 15)
    { return; }

  if (ToBit < FromBit)
  { 
    for (Loop = FromBit; Loop >= ToBit; Loop--)
      {
        if ((Data & (1<<Loop)) == 0)
          { strcat(EncodeBitString, AZero); }
        else 
          { strcat(EncodeBitString, AOne); }
      }
  }
  else
  {
    for (Loop = FromBit; Loop <= ToBit; Loop++)
      {
        if ((Data & (1<<Loop)) == 0)
          { strcat(EncodeBitString, AZero); }
        else 
          { strcat(EncodeBitString, AOne); }
      }
  }
}

/*-----------------------------------------------------------------------------
  Params  : <HalfBitTime>  Bit time (us)
  Returns : -              Result in Result


  Descript: Convert HalfBit into 1<>0 sequences (PRONTO format)
  Notes   : 
 -----------------------------------------------------------------------------*/
void ZeroOneSequences(int HalfBitTime)
{
  char TempString[1024];
  char Header[1024];
  unsigned long TimingUnit;
  unsigned long DelayUnit;
  unsigned long CarrierUnit;
  int  Items;
  int Count0;
  int Count1;
  int Len;
  int Index;

  // First we have some minor cleaning up to do. We must make sure we start
  // with a '1' and end with a '0'. A leading '0' can be removed and a
  // trailing '0' might be added.
  // Because we do this our string is always correct for the next stages.
  while (HalfBitString[0] == '0')
    { strcpy(TempString, _tcsninc(HalfBitString, 1));
      strcpy(HalfBitString, TempString);
    }
  Len = strlen(HalfBitString);
  if (HalfBitString[Len-1] == '1')
    { strcat(HalfBitString, "0"); }

  // Timing we use
  TimingUnit = ((HalfBitTime * Frequency) + 500000)/1E6; // 'half' bit - us -> units
  DelayUnit  = ((Delay * Frequency) + 500)/1000;         // delay        ms -> units
  CarrierUnit = 1E6/(Frequency  * 0.241246);     
  
  // Now count them and while we are at it translate it into the result:
  // Each 1->0 sequence is translated into two (4-digit hexadecimal) numbers.
  // These two numbers indicate the ON time (1) and the OFF time (0) of the
  // sequence. It is in units of the carrier frequency. 
  Items = 0;
  Index = 0;
  while (HalfBitString[Index] != 0)
  {
    Items++;
    Count0 = 0;
    Count1 = 0;
    while ((HalfBitString[Index] != 0) && (HalfBitString[Index] == '1'))
    {  Count1++; Index++; }
    while ((HalfBitString[Index] != 0) && (HalfBitString[Index] == '0'))
    {  Count0++; Index++; }
    // We now have counted the number of ones and zeroes in one sequence. Now
    // convert them into hexadecimal numbers.
    // A special case is if all data is processed; then we need to use the
    // requested delay.
    if (HalfBitString[Index] != 0)
      { sprintf(TempString, " %.4x %.4x", Count1 * TimingUnit, Count0 * TimingUnit); }
    else
      { sprintf(TempString, " %.4x %.4x", Count1 * TimingUnit, DelayUnit); }
    strcat(Result, TempString);
  }
  // Remove leading space
  while (Result[0] == ' ')
    { strcpy(TempString, _tcsninc(Result, 1));
      strcpy(Result, TempString);
    }
  // Now we have all our codes in a single string we only need to add the
  // header:
  // 0000 CCCC IIII 0000
  // CCCC = carrier frequency      Frequency = 1000000/(N * .241246)
  //                               1000000 / (Frequency * 0.241246)
  // IIII = total items
  //                         OR FOR REPEAT
  // 0000 CCCC 0000 IIII
  // CCCC = carrier frequency      Frequency = 1000000/(N * .241246)
  //                               1000000 / (Frequency * 0.241246)
  // IIII = total items
  // Again all values are 4-digits hexadecimal

  // First add '0000' header
  strcpy(Header, "0000 ");
  // Then the timing unit
  sprintf(TempString, "%.4x ", CarrierUnit);
  strcat(Header, TempString);
  // and then the 'once' and 'repeat' 'counters'
  if (Repeat != 0)
    { sprintf(TempString, "0000 %.4x ", Items); }
  else
    { sprintf(TempString, "%.4x 0000 ", Items); }
  strcat(Header, TempString);
  strcat(Header, Result);
  strcpy(Result, Header);
  strupr(Result);
}


/*-----------------------------------------------------------------------------
  Params   : <argc>  Number of parameters
             <argv>  Parameter 'list' ([1] = first parameter, e.g. 5000)
  Return   : -       Global result in 'result'

  Descript : Convert 5000 sequence to raw data (learned format)
  Notes    :   5000 0000 0000 0001 ssss cccc [Repeat]
               5000 ffff 0000 0001 ssss cccc [Repeat]

             Format of a RC5 sequence
               Bitstream:
                 ss T SSSSS CCCCCC

                 ss = 10   Add 64 to command
                 ss = 11   Use command as it is
                 T         Toggle bit
                 SSSSS     System bits (5)
                 CCCCCC    Command bits (6)

                 ALL bits are biphase encoded:
                 0 -> 10
                 1 -> 01

 -----------------------------------------------------------------------------*/
int Process5000(int argc, char* argv[])
{
  unsigned long TestParam;
  int Index;

  // Convert hexadecimal strings to numbers
  Frequency = 1000000 / (strtoul(argv[2], 0, 16) * 0.241246);
  if (Frequency == 0)
    { Frequency = DefaultFrequency; }
  System    = strtoul(argv[5], 0, 16);
  Command   = strtoul(argv[6], 0, 16);
  // If optional repeats also present, convert it
  if (argc >= 8)
    { Repeat = strtoul(argv[7], 0, 16); }

  // Check on valid data
  TestParam = strtoul(argv[3], 0, 16);
  if (TestParam != 0)
    { return -3; }
  TestParam = strtoul(argv[4], 0, 16);
  if (TestParam != 1)
    { return -4; }
  if (System > 31)
    { return -5; }
  if (Command > 127)
    { return -6; }
  
  // Convert data into binary data. Order is important because resulting
  // data is concatenated.
  strcpy(BitString, "");
  if (Command > 63)
    { EncodeBits(2, 1, 0, "0", "1");   strcat(BitString, EncodeBitString); }
  else
    { EncodeBits(3, 1, 0, "0", "1");   strcat(BitString, EncodeBitString); }
  if (Toggle)
    { EncodeBits(1, 0, 0, "0", "1");   strcat(BitString, EncodeBitString); }
  else
    { EncodeBits(0, 0, 0, "0", "1");   strcat(BitString, EncodeBitString); }

  EncodeBits(System,  4, 0, "0", "1"); strcat(BitString, EncodeBitString);
  EncodeBits(Command, 5, 0, "0", "1"); strcat(BitString, EncodeBitString);

  // Convert individual bits into two separate bits, based on bi-phase
  strcpy(HalfBitString, "");
  Index = 0;
  while (BitString[Index] != 0)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, BiphaseZero); }
    else
      { strcat(HalfBitString, BiphaseOne); }
    Index++;
  }

  // Convert bits into numerical values. This means counting consecutive 1's
  // and 0's
  ZeroOneSequences(BitTime);

  return 0;
}


/*-----------------------------------------------------------------------------
  Params   : <argc>  Number of parameters
             <argv>  Parameter 'list' ([1] = first parameter, e.g. 5001)
  Return   : -       Global result in 'result'

  Descript : Convert 5001 sequence to raw data (learned format)
  Notes    :   5001 0000 0000 0002 ssss cccc dddd 0000 [Repeat]
               5001 ffff 0000 0002 ssss cccc dddd 0000 [Repeat]

             Format of a RC5X sequence
               Bitstream:
                 ss T SSSSS dd CCCCCC DDDDDD

                 ss = 10   Add 64 to command      (To be biphase encoded)
                 ss = 11   Use command as it is   (To be biphase encoded)
                 T         Toggle bit             (To be biphase encoded)
                 SSSSS     System bits (5)        (To be biphase encoded)
                 dddd      Divider bits (2)       (NOT to be biphase encoded)
                           '00'
                 CCCCCC    Command bits (6)       (To be biphase encoded)
                 DDDDDD    Data bits (6)          (To be biphase encoded)

                 ALL bits are biphase encoded, except for the dddd bits:
                 0 -> 10
                 1 -> 01
             
                 dddd bits encoded:
                 0 -> 00
 -----------------------------------------------------------------------------*/
int Process5001(int argc, char* argv[])
{
  unsigned long TestParam;
  int Index;

  // Convert hexadecimal strings to numbers
  Frequency = 1000000 / (strtoul(argv[2], 0, 16) * 0.241246);
  if (Frequency == 0)
    { Frequency = DefaultFrequency; }
  System    = strtoul(argv[5], 0, 16);
  Command   = strtoul(argv[6], 0, 16);
  Data      = strtoul(argv[7], 0, 16);
  // If optional repeats also present, convert it
  if (argc >= 10)
    { Repeat = strtoul(argv[9], 0, 16); }

  // Check on valid data
  TestParam = strtoul(argv[3], 0, 16);
  if (TestParam != 0)
    { return -3; }
  TestParam = strtoul(argv[4], 0, 16);
  if (TestParam != 2)
    { return -4; }
  if (System > 31)
    { return -5; }
  if (Command > 127)
    { return -6; }
  if (Data > 63)
    { return -7; }
  TestParam = strtoul(argv[8], 0, 16);
  if (TestParam != 0)
    { return -8; }
  
  // Convert data into binary data. Order is important because resulting
  // data is concatenated.
  strcpy(BitString, "");
  if (Command > 63)
    { EncodeBits(2, 1, 0, "0", "1");   strcat(BitString, EncodeBitString); }
  else
    { EncodeBits(3, 1, 0, "0", "1");   strcat(BitString, EncodeBitString); }
  if (Toggle)
    { EncodeBits(1, 0, 0, "0", "1");   strcat(BitString, EncodeBitString); }
  else
    { EncodeBits(0, 0, 0, "0", "1");   strcat(BitString, EncodeBitString); }

  EncodeBits(System,  4, 0, "0", "1"); strcat(BitString, EncodeBitString);
  strcat(BitString, "00");
  EncodeBits(Command, 5, 0, "0", "1"); strcat(BitString, EncodeBitString);
  EncodeBits(Data,    5, 0, "0", "1"); strcat(BitString, EncodeBitString);

  // Convert individual bits into two separate bits, based on bi-phase
  strcpy(HalfBitString, "");
  for (Index=0; Index<=7; Index++)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, BiphaseZero); }
    else
      { strcat(HalfBitString, BiphaseOne); }
  }
  for (Index=8; Index<=9; Index++)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, Zero); }
    else
      { strcat(HalfBitString, One); }
  }
  for (Index=10; Index<=21; Index++)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, BiphaseZero); }
    else
      { strcat(HalfBitString, BiphaseOne); }
  }

  // Convert bits into numerical values. This means counting consecutive 1's
  // and 0's
  ZeroOneSequences(BitTime);

  return 0;
}


/*-----------------------------------------------------------------------------
  Params   : <argc>  Number of parameters
             <argv>  Parameter 'list' ([1] = first parameter, e.g. 6000)
  Return   : -       Global result in 'result'

  Descript : Convert 6000 sequence to raw data (learned format)
  Notes    :   6000 0000 0000 0001 ssss cccc [Repeat]
               6000 ffff 0000 0001 ssss cccc [Repeat]


              Format of a RC6 sequence
                Bitstream:
                  hhhhhhhh TT SSSSSSSS CCCCCCCC

                  hhhhhhhh  Header data (16)
                            '1111110010010101' (NOT to be biphase encoded!)
                  TT        Toggle (4)
                            '0011' or          (NOT to be biphase encoded!)
                            '1100'             (NOT to be biphase encoded!)
                  SSSSSSSS  System bits  (8)   (To be biphase encoded)
                  CCCCCCCC  Command bits (8)   (To be biphase encoded)

                  ONLY system and command bits are biphase encoded:
                  0 -> 01
                  1 -> 10               Note the inverted nature in comparison with RC5
 -----------------------------------------------------------------------------*/
int Process6000(int argc, char* argv[])
{
  unsigned long TestParam;
  int Index;

  // Convert hexadecimal strings to numbers
  Frequency = 1000000 / (strtoul(argv[2], 0, 16) * 0.241246);
  if (Frequency == 0)
    { Frequency = DefaultFrequency; }
  System    = strtoul(argv[5], 0, 16);
  Command   = strtoul(argv[6], 0, 16);
  // If optional repeats also present, convert it
  if (argc >= 8)
    { Repeat = strtoul(argv[7], 0, 16); }

  // Check on valid data
  TestParam = strtoul(argv[3], 0, 16);
  if (TestParam != 0)
    { return -3; }
  TestParam = strtoul(argv[4], 0, 16);
  if (TestParam != 1)
    { return -4; }
  if (System > 255)
    { return -5; }
  if (Command > 255)
    { return -6; }
  
  // Convert data into binary data. Order is important because resulting
  // data is concatenated.
  strcpy(BitString, "00000000");
  if (Toggle)
    { strcat(BitString, "10"); }
  else
    { strcat(BitString, "01"); }

  EncodeBits(System,  7, 0, "0", "1"); strcat(BitString, EncodeBitString);
  EncodeBits(Command, 7, 0, "0", "1"); strcat(BitString, EncodeBitString);

  // Convert individual bits into two separate bits, based on bi-phase
  strcpy(HalfBitString, "1111110010010101");
  for (Index=8; Index<=9; Index++)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, Zero); }
    else
      { strcat(HalfBitString, One); }
  }
  for (Index=10; Index<=25; Index++)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, BiphaseZero2); }
    else
      { strcat(HalfBitString, BiphaseOne2); }
  }

  // Convert bits into numerical values. This means counting consecutive 1's
  // and 0's
  ZeroOneSequences(BitTime/2);

  return 0;
}


/*-----------------------------------------------------------------------------
  Params   : <argc>  Number of parameters
             <argv>  Parameter 'list' ([1] = first parameter, e.g. 6001)
  Return   : -       Global result in 'result'

  Descript : Convert 6001 sequence to raw data (learned format)
  Notes    :   6001 0000 0000 0002 uuuu ssss cccc 0000 [Repeat]
               6001 ffff 0000 0002 uuuu ssss cccc 0000 [Repeat]

             Format of a RC6A sequence
               Bitstream:
                 hhhhhhhh TT s UUUUUUU SSSSSSSS CCCCCCCC   or
	             hhhhhhhh TT s UUUUUUUUUUUUUUU SSSSSSSS CCCCCCCC

               hhhhhhhh        Header data (17)       (NOT to be biphase encoded)
                               ‘11111110010101001'
	           TT               Toggle bits (4)       (NOT to be biphase encoded)
                               ‘0011' or
                               ‘1100'
               s = 0           Customer range 0..127         (to be biphase encoded)
               s = 1           Customer range 32768..65535   (to be biphase encoded)
               UUUUUUU         CUSTOMER identifier (7 or 15) (to be biphase encoded)
                               7 or 15 bits depending on the ‘s' bit
               SSSSSSSS        System bits (8)               (to be biphase encoded)
               CCCCCCCC        Command bits (8)              (to be biphase encoded)

             All bits to be biphase encoded will have a single bit split up into two half bits:
             0 -> 01
             1 -> 10	Note: inverse from RC5/RC5X biphase format!

            Note: a RC6A bit transmission takes half the time of a RC5/RC5X bit transmission!
------------------------------------------------------------------*/
int Process6001(int argc, char* argv[])
{
  unsigned long TestParam;
  int Index;

  // Convert hexadecimal strings to numbers
  Frequency = 1000000 / (strtoul(argv[2], 0, 16) * 0.241246);
  if (Frequency == 0)
    { Frequency = DefaultFrequency; }
  System    = strtoul(argv[6], 0, 16);
  Command   = strtoul(argv[7], 0, 16);
  Customer  = strtoul(argv[5], 0, 16);
  // If optional repeats also present, convert it
  if (argc >= 10)
    { Repeat = strtoul(argv[9], 0, 16); }

  // Check on valid data
  TestParam = strtoul(argv[3], 0, 16);
  if (TestParam != 0)
    { return -3; }
  TestParam = strtoul(argv[4], 0, 16);
  if (TestParam != 2)
    { return -4; }
  if ((Customer > 127) && (Customer < 32768))
    { return -5; }
  if (System > 255)
    { return -6; }
  if (Command > 255)
    { return -7; }
  TestParam = strtoul(argv[8], 0, 16);
  if (TestParam != 0)
    { return -8; }
  
  // Convert data into binary data. Order is important because resulting
  // data is concatenated.
  strcpy(BitString, "000000000");
  if (Toggle)
    { strcat(BitString, "10"); }
  else
    { strcat(BitString, "01"); }
  if (Customer < 128)
  {
    strcat(BitString, "0");
    EncodeBits(Customer,  6, 0, "0", "1"); strcat(BitString, EncodeBitString);
  }
  else
  {
    strcat(BitString, "1");
    EncodeBits(Customer,  14, 0, "0", "1"); strcat(BitString, EncodeBitString);
  }

  EncodeBits(System,  7, 0, "0", "1"); strcat(BitString, EncodeBitString);
  EncodeBits(Command, 7, 0, "0", "1"); strcat(BitString, EncodeBitString);

  // Convert individual bits into two separate bits, based on bi-phase
  strcpy(HalfBitString, "011111110010101001");
  for (Index=9; Index<=10; Index++)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, Zero); }
    else
      { strcat(HalfBitString, One); }
  }
  if (BitString[11] == '0')
    { strcat(HalfBitString, BiphaseZero2); }
  else
    { strcat(HalfBitString, BiphaseOne2); }
  Index = 12;
  while (BitString[Index] != 0)
  {
    if (BitString[Index] == '0')
      { strcat(HalfBitString, BiphaseZero2); }
    else
      { strcat(HalfBitString, BiphaseOne2); }
    Index++;
  }

  // Convert bits into numerical values. This means counting consecutive 1's
  // and 0's
  ZeroOneSequences(BitTime/2);

  return 0;
}

string ConvertRC5_6(string sCode)
{
	Result[0]=0;
	char pCode[500]; // max size of an rc5/6
	strncpy(pCode,sCode.c_str(),500);
	char* argv[20];  // Never more than 20 times
	int argc=1;
	argv[0] = "command";  // Functions expect command line style, so first arg is the file name
	
	// These utilties want argc/argv format
	char *pPos = pCode,*pPos_Prior=pCode;
	while(*pPos && argc<20)
	{
		while(*pPos && *pPos!=' ')
			pPos++;
		argv[argc++]=pPos_Prior;
		if( *pPos==' ' )
		{
			*pPos=0;
			pPos++;
			pPos_Prior=pPos;
		}
	}

	if( sCode[0]=='5' )
	{
		if( sCode[1]=='0' )
			error = Process5000(argc, argv);
		else if( sCode[1]=='1' )
			error = Process5001(argc, argv);
	}
	else if( sCode[0]=='6' )
	{
		if( sCode[1]=='0' )
			error = Process6000(argc, argv);
		else if( sCode[1]=='1' )
			error = Process6001(argc, argv);
	}
	if (error >= 0)
		return Result;
    return "";
}

