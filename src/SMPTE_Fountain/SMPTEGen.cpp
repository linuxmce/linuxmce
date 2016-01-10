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
#include<stdio.h>
#include<string.h>
#include "SMPTEGen.h"
#include<stdlib.h>

SMPTEGen::SMPTEGen(int BufferCount, int SampleSize)
{
	m_pBuffer = NULL;
	m_BufferCount = BufferCount;
	m_SampleSize = SampleSize;
	m_bFlipFlop = false;
	ClearBuffer();
}

SMPTEGen::~SMPTEGen()
{
	delete m_pBuffer;
}

void SMPTEGen::AddPulse(bool b)
{
	for(int i=0;i<m_SampleSize;i++)
	{
		*(m_pCurBuffer++) = b ? 235 : 20;
		*(m_pCurBuffer++) = b ? 235 : 20;
	}
}

void SMPTEGen::AddBit(bool b)
{
	m_bFlipFlop = !m_bFlipFlop;
	if (!b)
	{
		AddPulse(m_bFlipFlop);
		AddPulse(m_bFlipFlop);
	}
	else
	{
		AddPulse(m_bFlipFlop);
		m_bFlipFlop = !m_bFlipFlop;
		AddPulse(m_bFlipFlop);
	}
}

void SMPTEGen::AddByte(int Byte)
{
	for(int i=0;i<8;i++)
	{
		AddBit((Byte & (1 << i))>0);
	}
}

bool SMPTEGen::AddSMPTE(long smpte)
{
	int ff = smpte % 25;
	smpte = smpte / 25;
	int ss = smpte % 60;
	smpte = smpte / 60;
	int mm = smpte % 60;
	smpte = smpte / 60;
	int hh = smpte;
	AddByte(ff % 10);
	AddByte(ff / 10);
	AddByte(ss % 10);
	AddByte(ss / 10);
	AddByte(mm % 10); 
	AddByte(mm / 10);
	AddByte(hh % 10);
	AddByte(hh / 10);
	AddByte(252);
	AddByte(255 - 64);
	return (m_pCurBuffer-m_pBuffer) >= m_BufferSize;
}

void SMPTEGen::ClearBuffer()
{
	// buffer is 10 characters per smtpe code, 8 bits, 4xSampleSize samples per bit.
	int m_BufferSize = m_BufferCount * m_SampleSize * 320;
	delete m_pBuffer;
	m_pBuffer = new char[m_BufferSize];
	m_pCurBuffer = m_pBuffer;
	memset(m_pBuffer, 0, m_BufferSize); 
}


char *SMPTEGen::GetAndClearBuffer(int &length)
{
	char *ReturnValue = m_pBuffer;
	length = m_pCurBuffer - m_pBuffer;
	m_pBuffer = NULL;
	ClearBuffer();
	return ReturnValue;
}

char *SMPTEGen::GetSMPTE(long SMPTE, int &length)
{
	AddSMPTE(SMPTE);
	return GetAndClearBuffer(length);
}

long SMPTEGen::FromTimecode(const char *s)
{
	int hh=0, mm=0, ss=0, ff=0;

	char *strbuf = strdup(s);

	char *p = strtok(strbuf, ":");
	if (p)
	{
		hh = atoi(p);
		p = strtok(NULL, ":");
		if (p)
		{
			mm = atoi(p);
			p = strtok(NULL, ":");
			if (p)
			{
				ss = atoi(p);
				p = strtok(NULL, ":.");
				if (p)
				{
					ff = atoi(p);
				}
			}
		}
	}
	long gm;

	gm = hh * 60; 
	gm = (gm + mm) * 60;
	gm = (gm + ss) * 25;
	gm = gm + ff;

	return gm;
}

const char *SMPTEGen::FromSMPTECode(long SMPTE)
{
	int hh, mm, ss, ff;

	ff = SMPTE % 25;
	SMPTE = SMPTE / 25;
	ss = SMPTE % 60;
	SMPTE = SMPTE / 60;
	mm = SMPTE % 60;
	SMPTE = SMPTE / 60;
	hh = SMPTE;

	static char s[25];
	sprintf(s, "%02d:%02d:%02d.%02d", hh, mm, ss, ff);
	return s;
}
	
