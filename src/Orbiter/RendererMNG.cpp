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
#include "RendererMNG.h"

#include <zlib.h>

const char * PNGsignature = "\x89PNG\x0D\x0A\x1A\x0A";
const char * MNGsignature = "\x8AMNG\x0D\x0A\x1A\x0A";

unsigned long & ReverseEndian(unsigned long & N)
{
	ByteAccessLong X, Y;

	// TODO: Detect byte order
	X.ulValue = N;
	for (int i = 0; i <= 3; i++)
	{
		Y.ucBytes[i] = X.ucBytes[3-i];
	}

	N = Y.ulValue;

	return N;
}

void ParseMNGHeader(const char * RawData, MNGHeader * Header)
{
	const unsigned long * Data = (const unsigned long *) RawData;

	ReverseEndian(Header->frame_width = * Data++);
	ReverseEndian(Header->frame_height = * Data++);
	ReverseEndian(Header->ticks_per_second = * Data++);
	ReverseEndian(Header->nominal_layer_count = * Data++);
	ReverseEndian(Header->nominal_frame_count = * Data++);
	ReverseEndian(Header->nominal_play_time = * Data++);
	ReverseEndian(Header->simplicity_profile = * Data++);
}

bool ReadChunk(FILE * f, PNGChunk * chunk)
{
	size_t bytes;
	char buffer[1024];

	// Data length
	if (fread(&chunk->length, 1, 4, f) < 4)
		return false;

	ReverseEndian(chunk->length);

	// Chunk type code
	bytes = fread(buffer, 1, 4, f);
	if (bytes < 4)
		return false;
	buffer[bytes] = 0;
	chunk->type = buffer;

	// Chunk binary data
	chunk->data = new char[chunk->length];
	if (fread(chunk->data, 1, chunk->length, f) < chunk->length)
		return false;

	// Chunk CRC
	if (fread(&chunk->crc32, 1, 4, f) < 4)
		return false;
	ReverseEndian(chunk->crc32);

	return true;
}

size_t MNGHeader::BinaryForm(char * & MemoryZonePointer) const
{
	const int chunk_size = 40;

	MemoryZonePointer = new char[chunk_size];
	char * Position = MemoryZonePointer;

	unsigned long length = 28;
	ReverseEndian(length);

	unsigned long W, H, T, L, F, P, S;
	W = frame_width;
	H = frame_height;
	T = ticks_per_second;
	L = nominal_layer_count;
	F = nominal_frame_count;
	P = nominal_play_time;
	S = simplicity_profile;

	ReverseEndian(W);
	ReverseEndian(H);
	ReverseEndian(T);
	ReverseEndian(L);
	ReverseEndian(F);
	ReverseEndian(P);
	ReverseEndian(S);

	memcpy(Position, &length, 4); Position += 4;
	memcpy(Position, "MHDR", 4); Position += 4;
	memcpy(Position, &W, 4); Position += 4;
	memcpy(Position, &H, 4); Position += 4;
	memcpy(Position, &T, 4); Position += 4;
	memcpy(Position, &L, 4); Position += 4;
	memcpy(Position, &F, 4); Position += 4;
	memcpy(Position, &P, 4); Position += 4;
	memcpy(Position, &S, 4); Position += 4;

	unsigned long CRC32;
	CRC32 = crc32(0, Z_NULL, 0);
	CRC32 = crc32(CRC32, (const Bytef *) "MHDR", 4);
	CRC32 = crc32(CRC32, (const Bytef *) &W, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &H, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &T, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &L, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &F, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &P, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &S, 4);

	ReverseEndian(CRC32);

	memcpy(Position, &CRC32, 4); Position += 4;

	return chunk_size;
}

PNGCatChunks::~PNGCatChunks()
{
	for (size_t i = 0; i < count(); i++)
		if (m_Chunks[i])
			delete m_Chunks[i];
	m_Chunks.clear();
}

bool PNGCatChunks::AddChunk(const PNGChunk * Chunk)
{
	if (count() == 0 && Chunk->type != "IHDR")
		return false; // given chunk is not the first one

	if (count() > 0 && m_Chunks[count() - 1]->type == "IEND")
		return false; // PNG already complete

	m_Chunks.insert(m_Chunks.end(), Chunk);
	return true;
}

size_t PNGCatChunks::CatChunks(char * & MemoryZonePointer) const // it is the user's responsibility deleting this pointer
{
	size_t Size = 8;
	const int Chunk_Extra = 3 * sizeof(unsigned long);

	size_t i;
	for (i = 0; i < count(); i++)
	{
		Size += m_Chunks[i]->length + Chunk_Extra; // data length plus 3 32bit chunk values
	}

	MemoryZonePointer = new char[Size];
	char * Position = MemoryZonePointer;

	strncpy(Position, PNGsignature, 8); Position += 8;
	for (i = 0; i < count(); i++)
	{
		unsigned long ChunkLength = m_Chunks[i]->length;
		unsigned long ChunkCRC32 = m_Chunks[i]->crc32;

		ReverseEndian(ChunkLength);
		ReverseEndian(ChunkCRC32);

		memcpy(Position, &ChunkLength, 4); Position += 4;
		memcpy(Position, m_Chunks[i]->type.c_str(), 4); Position += 4;
		memcpy(Position, m_Chunks[i]->data, m_Chunks[i]->length); Position += m_Chunks[i]->length;
		memcpy(Position, &ChunkCRC32, 4); Position += 4;
	}

	return Size;
}

size_t PNGCatChunks::count() const
{
	return m_Chunks.size();
}

void PNGCatChunks::clear()
{
	for (size_t i = 0; i < count(); i++)
		if (m_Chunks[i])
			delete m_Chunks[i];
	m_Chunks.clear();
}

RendererMNG::RendererMNG()
{
	memset(&m_MNGHeader, 0, sizeof(m_MNGHeader));
}

RendererMNG::RendererMNG(int Width, int Height)
{
	memset(&m_MNGHeader, 0, sizeof(m_MNGHeader));
	m_MNGHeader.frame_height = Height;
	m_MNGHeader.frame_width = Width;
	m_MNGHeader.ticks_per_second = 1000;
}

RendererMNG::~RendererMNG()
{
	m_vectMNGframes.clear();
}

size_t RendererMNG::count() const
{
	return m_vectMNGframes.size();
}

RendererImage *RendererMNG::GetFrame(size_t number) const
{
	if (/*number < 0 ||*/ number >= count())
		return NULL;

	return m_vectMNGframes[number];
}

void RendererMNG::ReplaceFrame(size_t number, RendererImage * frame)
{
	if (/*number < 0 ||*/ number >= count())
		return;

	if (m_vectMNGframes[number])
		delete m_vectMNGframes[number];
	m_vectMNGframes[number] = frame;
}

void RendererMNG::InsertFrame(size_t number, RendererImage * frame)
{
	if (/*number < 0 ||*/ number >= count())
		return;

	vector<RendererImage>::iterator ivRI;
	//int i;

	// Linux G++ 3.3.4 doesn't like this one
	// m_vectMNGframes.insert(&m_vectMNGframes[number], frame);
	m_vectMNGframes.insert(m_vectMNGframes.begin() + number, frame);
}

void RendererMNG::AppendFrame(RendererImage * frame)
{
	m_vectMNGframes.insert(m_vectMNGframes.end(), frame);
}

void RendererMNG::DeleteFrame(size_t number)
{
	if (/*number < 0 ||*/ number >= count())
		return;

	// Linux G++ 3.3.4 doesn't like this one
	// m_vectMNGframes.erase(&m_vectMNGframes[number]);
	m_vectMNGframes.erase(m_vectMNGframes.begin() + number);
}

const MNGHeader & RendererMNG::GetHeader() const
{
	return m_MNGHeader;
}

void RendererMNG::SetHeader(const MNGHeader & Header)
{
	m_MNGHeader = Header;
}

InMemoryMNG::InMemoryMNG()
{
	m_pMNGHeader = NULL;
}

InMemoryMNG::~InMemoryMNG()
{
	size_t iSize = m_vectMNGframes.size();

	for(size_t i = 0; i < iSize; i++)
	{
		delete m_vectMNGframes[i];
	}
	m_vectMNGframes.clear();

	if(NULL != m_pMNGHeader)
	{
		delete m_pMNGHeader;
		m_pMNGHeader = NULL;
	}
}

size_t InMemoryMNG::GetFrame(int iIndex, char *&pData)
{
	PNGCatChunks *pPNGCatChunks = m_vectMNGframes[iIndex];
	return pPNGCatChunks->CatChunks(pData);
}

/*static*/ InMemoryMNG *InMemoryMNG::CreateInMemoryMNGFromFile(string sFileName, PlutoSize Size)
{
	FILE * File;
	int RealW, RealH;
	MNGHeader *pHeader = new MNGHeader;

	File = fopen(sFileName.c_str(), "rb");
	if (! File)
		return NULL;

	char buffer[1024];
	fread(buffer, 1, 8, File); // strip signature

	PNGChunk * chunk;
	bool BuildingPNG = false;

	InMemoryMNG *pInMemoryMNG = new InMemoryMNG;
	PNGCatChunks * InMemoryPNG;

	while (ReadChunk(File, chunk = new PNGChunk))
	{
		if (chunk->type == "MHDR")
		{
			ParseMNGHeader(chunk->data, pHeader);
			RealW = Size.Width == 0 ? pHeader->frame_width : Size.Width;
			RealH = Size.Height == 0 ? pHeader->frame_height : Size.Height;
		}

		if (chunk->type == "IHDR")
		{
			InMemoryPNG = new PNGCatChunks;
			BuildingPNG = true;
		}

		if (BuildingPNG)
		{
			InMemoryPNG->AddChunk(chunk);
			if (chunk->type == "IEND")
			{
				BuildingPNG = false;
				pInMemoryMNG->m_vectMNGframes.push_back(InMemoryPNG);
			}
		}
		else
		{
			delete chunk;
		}
	}

	delete chunk; //(for Radu) If ReadCheck returns false, who will delete the chunk? BUBU!

	fclose(File);

	pHeader->frame_height = RealH;
	pHeader->frame_width = RealW;
	pInMemoryMNG->m_pMNGHeader = pHeader;

	return pInMemoryMNG;
}
