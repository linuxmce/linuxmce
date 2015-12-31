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
#ifndef GenericIODevice_h
#define GenericIODevice_h

/*

This class is used to create strings of data to be sent to
a variety of devices like A/V equipment, home automation controllers, etc.
The purpose is to allow the user to specify standard strings in a database
that will translate DCE Commands into strings that can be sent to the device,
and receive strings from the device and convert them into DCE Events.  The 
actual communication layer can be anything, typically RS232 or Ethernet.

The input strings for the commands comes from the InfraredGroup_Command 
table, which primarily holds infrared codes for scores of i/r based a/v devices,
but also holds commands for rs232 and ethernet controlled devices.

This class is pre-populated with the command in "DCE Format", where
the command is m_dwPK_Command, and the parameters are m_mapCommandParameters,
and m_pDevice points to the target device including all it's data.  
CreateOutputString takes the command syntax as pulled from the InfraredGroup_Command
table, and returns a string in a suitable format to send the device.

The syntax for the input tokens is a string of characters with special tokens
mixed in.  The tokens are in the format:
<TOKEN;Parms:format?translation>

where TOKEN and Parms are as follows
TOKEN						Parms
C = The command				None
CP = Command Parameter		The PK_CommandParameter
STX = the STX character		None
ETX = the ETX character		None
ACK = The ACK character		None
NAK = The NAK character		None
NULL = the NULL character	None
CHK = A checksum			See the table for valid checksum paramaters
D = The PK_Device			None
T = The PK_DeviceTemplate	None
DP = Device Parameter		The PK_DeviceParameter
W = Wait x milliseconds		The number of milliseconds to wait before transmitting anything else

format indicates how the token should be formatted.  The format character
may be followed by an optional number indicating the width in number of bytes
c = ascii characters.  If a width is specified it will be padded with spaces.  If the width is shorter than the token it will be truncated.
x = a hexadecimal number as a character
i = an integer in binary format.  If the width is 1 it will be a byte, 2 a 16 bit number, etc.
n = number ascii characters.  If a width is specified it will be padded with leading 0's.  If the width is shorter than the token it will be truncated.

translation means that if the token resolves to 1 string, it will be replaced with another.  This is a sequence
of [original value]=[destination value] pairs.  If the last value is an *, that means any other value that hasn't already
been matched

examples:

<C?15=20,20=10,*=1>  if the PK_Command was 15, the token will resolve to 20.  If it was 20 it will resolve to 10.  If it is anything else it will be 1
<D,i4>  The device id as a 32-bit integer (ie 4 bytes)
<CP;14:c10?+100=1x,-100=2x,\=100=10x,\*=-1x,*=><NULL>
	means take the command parameter 14.  If it's "+100" replace that with "1x", and so on.
	if it's "=100" replace it with 10x, and if it's "*" replace it with -1x.  Note that the
	* and = needed to be escaped with a \.  If it is any other token, replace it with nothing.
	The response will be padded to 10 characters and NULL terminated


The following tokens are used to add some incoming strings (Events) to monitor
either to indicate success or failure or to listen for some temporary Events

OK = What follows is the event signalling the device received the command OK
	Parms can be: 0 or not specified means do nothing if we do not receive an OK event
				  1 means send the Retry string only
				  2 means send the Retry string and resend the command
				  3 means send the command again without the retry string
	Format determines how long to wait for the OK event -- See the event token
	Translation, if specified, is a message to be sent in response to an OK Event

FAIL = What follows is the event signalling the device did not received the command OK
	Parms can be: 0 or not specified means do nothing if we receive a FAIL event
				  1 means send the Retry string only
				  2 means send the Retry string and resend the command
				  3 means send the command again without the retry string
	Format determines how long to wait for the FAIL event -- See the event token
	Translation, if specified, is a message to be sent in response to a FAIL Event

RETRY = What follows is the string to send in response to a failure

EVENT = What follows is a temporary event to listen for, see Events for the Event syntax
	Parms is the event number
	Format determines how long to listen where it can be:
		[n]ms = wait n milliseconds
		[n]ev = must be within n events
		i or not specified means the next immediate character from the device must match

example:
<STX>VOLUP\n<ETX><OK;2:1000ms?<D> 0 2 195><ACK><FAIL;3:500ms><NAK><RETRY><NAK><W;100>

Send the device the STX character followed "VOLUP", a newline and ETX
If the device received the command correctly it will send the ACK character.  Wait 1000ms
for the ACK character.  If it's received in that time, fire the message Type 2, ID 195, to device 0.
If it's not received within 1000ms, send NAK (the retry string), wait 100ms and resend the command.
If the device response with a NAK, don't send NAK back (the retry string), just immediately re-transmit
the command.

Event syntax:

An incoming string is expected to match a given format for it to be an event.  This also accepts
the same tokens as the commands, except EP is used for EventParamter instead of CP for CommandParameter,
and CHAR means any number of characters will be considered a match and processed on their own.

For example, if there is an entry in DeviceTemplate_Event for Event #18 like this:

VOL<EP;33:c3?-10=0, 100=10><CHK;1>\n
Then a string that starts with VU followed by 10 characters and a valid checksum will fire the Event 18.
The 3 characters are used to determine the value of Event Parameter #33.  If the 3 chacters are "-10" then
the value 0 will be added as Event Parameter 13, if they are 100, then 10 will be used.  

If the Event Parameters will not be coming in at a known place, you can specify additional PARM tokens that 
will be matched.  For example:
VOL<CHAR><CHK;1>\n<PARM;13>LEVEL <EP:c3?-10=0, 100=10><PARM;15>

*/

#include "SerializeClass/SerializeClass.h"
#include "DeviceData_Impl.h"

using namespace DCE;

class IOEvent
{
public:
	string m_sEvent;
	int iPK_Event;
	clock_t tExpiration;
};

class GenericIODevice : public SerializeClass
{
public:
	DeviceData_Impl *m_pDevice;

	~GenericIODevice() {};

	// This takes the command and parameters that were passed in and creates an output string
	// It returns a negative value if there was an error processing the string, otherwise it 
	// returns the number of characters in the resulting string.  The resulting string
	// will be stored in m_??? inherited from ???? and freed in the destructor
	int TransmitCommand(string sCommand,Message *pMessage);

	void ProcessToken(string sToken,string sParms,string sFormat,string sTranslation,Message *pMessage);
	string FormatAndTranslate(string sToken,string sFormat,string sTranslation);

	virtual void Transmit(const char *pData,int iSize)=0;
	void ReceiveData(const char *pData,int iSize);
};

#endif
