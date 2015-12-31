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
class SMPTEGen
{
	int m_SampleSize, m_BufferCount, m_BufferSize;
	char *m_pBuffer, *m_pCurBuffer;
	bool m_bFlipFlop;

	void AddPulse(bool b);
	void AddBit(bool b);
	void AddByte(int Byte);

	void ClearBuffer();

public:
	SMPTEGen(int BufferCount=1 , int SampleSize=1); // Buffer 
	virtual ~SMPTEGen();

	bool AddSMPTE(long SMPTE); // Returns TRUE when buffer is full
	char *GetAndClearBuffer(int &length);  // Returns the current buffer and begins a new one.
	char *GetSMPTE(long SMTPE, int &length); // Most useful with single-slice buffer, just gets and returns one smpte frame

	static long FromTimecode(const char *s);
	static const char *FromSMPTECode(long SMPTE);
};

