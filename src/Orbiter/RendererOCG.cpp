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
#include "RendererOCG.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/minilzo.h"
#include "PlutoUtils/FileUtils.h"
#include <cstring>

//----------------------------------------------------------------------------------------------------------------
#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);
//----------------------------------------------------------------------------------------------------------------
static const int ciOCGMagicStart = 0x1234;
static const int ciOCGMagicEnd = 0x5678;
//----------------------------------------------------------------------------------------------------------------
RendererOCG::RendererOCG()
{
	Initialize();
}
//----------------------------------------------------------------------------------------------------------------
RendererOCG::RendererOCG(string sOCGFilename)
{
	Initialize();
	LoadOCG(sOCGFilename);
}
//----------------------------------------------------------------------------------------------------------------
RendererOCG::RendererOCG(char *pPixelsData, int iPixelsDataSize, 
						 char *pPixelFormatData, int iPixelFormatDataSize, 
						 int iWidth, int iHeight)
{
	Initialize();
	SetSurface(pPixelsData, iPixelsDataSize, pPixelFormatData, iPixelFormatDataSize, iWidth, iHeight);
}
//----------------------------------------------------------------------------------------------------------------
RendererOCG::~RendererOCG()
{
	Cleanup();
}
//----------------------------------------------------------------------------------------------------------------
void RendererOCG::Cleanup()
{
	m_sFilename = "";

	PLUTO_SAFE_DELETE_ARRAY(m_pOCGData);  
	m_iOCGSize = 0;
	PLUTO_SAFE_DELETE_ARRAY(m_pPixelsData);
	m_iPixelsDataSize = 0;
	PLUTO_SAFE_DELETE_ARRAY(m_pPixelFormatData);
	m_iPixelFormatDataSize = 0;
	m_iWidth = 0;
	m_iHeight = 0;
}
//----------------------------------------------------------------------------------------------------------------
void RendererOCG::Initialize()
{
	m_sFilename = "";
	m_pOCGData = NULL;
	m_iOCGSize = 0;
	m_pPixelsData = NULL;
	m_iPixelsDataSize = 0;	
	m_pPixelFormatData = NULL;
	m_iPixelFormatDataSize = 0;
	m_iWidth = 0;
	m_iHeight = 0;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::SaveOCG(string sOCGFilename)
{
	return FileUtils::WriteBufferIntoFile(sOCGFilename, m_pOCGData, m_iOCGSize);
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::LoadOCG(string sOCGFilename)
{
	Cleanup();

	size_t iOCGSize;
	char *pOCGData = FileUtils::ReadFileIntoBuffer(sOCGFilename, iOCGSize);

	if(!pOCGData)
		return false;

	return SetOCGData(pOCGData, (int)iOCGSize);

	PLUTO_SAFE_DELETE_ARRAY(pOCGData);
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::SetSurface(char *pPixelsData, int iPixelsDataSize, 
							 char *pPixelFormatData, int iPixelFormatDataSize, 
							 int iWidth, int iHeight)
{
	Cleanup();

	m_pPixelsData = pPixelsData;
	m_iPixelsDataSize = iPixelsDataSize;
	m_pPixelFormatData = pPixelFormatData;
	m_iPixelFormatDataSize = iPixelFormatDataSize;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	//compress pixels data
	char *pCompressedData = new char[3 * m_iPixelsDataSize];
	int iCompressedDataSize = 0;
	long iSize = 0;
	lzo1x_1_compress((lzo_byte *)m_pPixelsData, (lzo_uint)m_iPixelsDataSize, (lzo_byte *)pCompressedData, (lzo_uint *)(&iSize), wrkmem);

	iCompressedDataSize = (int)iSize;
	
	//serialize data
	m_pOCGData = new char[4 + 4 + 4 + 4 + m_iPixelFormatDataSize + 4 + iCompressedDataSize + 4 + 4];
	m_iOCGSize = 0;

	//magic start
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&ciOCGMagicStart), sizeof(ciOCGMagicStart));
	m_iOCGSize += sizeof(ciOCGMagicStart);
	//width
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iWidth), sizeof(m_iWidth));
	m_iOCGSize += sizeof(m_iWidth);
	//heigth
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iHeight), sizeof(m_iHeight));
	m_iOCGSize += sizeof(m_iHeight);

	//pixels format size
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iPixelFormatDataSize), sizeof(m_iPixelFormatDataSize));
	m_iOCGSize += sizeof(m_iPixelFormatDataSize);
	//pixels format data
	memcpy(m_pOCGData + m_iOCGSize, (char *)(m_pPixelFormatData), m_iPixelFormatDataSize);
	m_iOCGSize += m_iPixelFormatDataSize;

	//compressed data size
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&iCompressedDataSize), sizeof(iCompressedDataSize));
	m_iOCGSize += sizeof(iCompressedDataSize);
	//compressed data
	memcpy(m_pOCGData + m_iOCGSize, (char *)(pCompressedData), iCompressedDataSize);
	m_iOCGSize += iCompressedDataSize;

	//uncompressed data size
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&m_iPixelsDataSize), sizeof(m_iPixelsDataSize));
	m_iOCGSize += sizeof(m_iPixelsDataSize);

	//magic end
	memcpy(m_pOCGData + m_iOCGSize, (char *)(&ciOCGMagicEnd), sizeof(ciOCGMagicEnd));
	m_iOCGSize += sizeof(ciOCGMagicEnd);

	PLUTO_SAFE_DELETE_ARRAY(pCompressedData);
	return true;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::SetOCGData(char *pOCGData, int iOCGSize)//it's user responsability to delete pOCGData!!!
{
	Cleanup();

	m_iOCGSize = iOCGSize;
	m_pOCGData = new char[iOCGSize];
	memcpy(m_pOCGData, pOCGData, iOCGSize);

	//deserialize OCG data
	int iOffset = 0;

	//magic start
	int iMagicStart = *(int *)(m_pOCGData + iOffset);
	iOffset += sizeof(iMagicStart);
	if(iMagicStart != ciOCGMagicStart)
		return false;

	//width
	m_iWidth = *(int *)(m_pOCGData + iOffset);
	iOffset += sizeof(m_iWidth);
	//height
	m_iHeight = *(int *)(m_pOCGData + iOffset);
	iOffset += sizeof(m_iHeight);

	//pixel format data size
	m_iPixelFormatDataSize = *(int *)(m_pOCGData + iOffset);
	iOffset += sizeof(m_iPixelFormatDataSize);
	//pixel format data
	m_pPixelFormatData = new char[m_iPixelFormatDataSize];
	memcpy(m_pPixelFormatData, m_pOCGData + iOffset, m_iPixelFormatDataSize);
	iOffset += m_iPixelFormatDataSize;

	//compressed data size
	int iCompressedDataSize = *(int *)(m_pOCGData + iOffset);
	iOffset += sizeof(iCompressedDataSize);
	//compressed data
	char *pCompressedData = new char[iCompressedDataSize];
	memcpy(pCompressedData, m_pOCGData + iOffset, iCompressedDataSize);
	iOffset += iCompressedDataSize;

	//uncompressed data size
	//int iUncompressedSize = *(int *)(m_pOCGData + iOffset);


		//WinCE issue :(
		unsigned char v1 = ((unsigned char *)(m_pOCGData + iOffset))[0];
		unsigned char v2 = ((unsigned char *)(m_pOCGData + iOffset))[1];
		unsigned char v3 = ((unsigned char *)(m_pOCGData + iOffset))[2];
		unsigned char v4 = ((unsigned char *)(m_pOCGData + iOffset))[3];

		int iUncompressedSize = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;



	iOffset += sizeof(iUncompressedSize);

	//magic end
	//int iMagicEnd = *(int *)(m_pOCGData + iOffset);

		//WinCE issue :(
		v1 = ((unsigned char *)(m_pOCGData + iOffset))[0];
		v2 = ((unsigned char *)(m_pOCGData + iOffset))[1];
		v3 = ((unsigned char *)(m_pOCGData + iOffset))[2];
		v4 = ((unsigned char *)(m_pOCGData + iOffset))[3];

		int iMagicEnd = (v4 << 24) + (v3 << 16) + (v2 << 8) + v1;

	iOffset += sizeof(iMagicEnd);
	if(iMagicEnd != ciOCGMagicEnd || iOffset != m_iOCGSize)
	{
		PLUTO_SAFE_DELETE_ARRAY(pCompressedData);
		return false;
	}


	m_pPixelsData = new char[iUncompressedSize];

	long size = 0;
	lzo1x_decompress((lzo_byte *)pCompressedData, (lzo_uint)iCompressedDataSize, (lzo_byte *)m_pPixelsData, (lzo_uint *)(&size), wrkmem);
	m_iPixelsDataSize = (int)size;

	PLUTO_SAFE_DELETE_ARRAY(pCompressedData);
	return true;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::GetSurface(char *& pPixelsData, int& iPixelsDataSize, 
							 char *& pPixelFormatData, int& iPixelFormatDataSize,
							 int& iWidth, int& iHeight)
{
	iWidth = m_iWidth;
	iHeight = m_iHeight;

	iPixelsDataSize = m_iPixelsDataSize;
	pPixelsData = m_pPixelsData;
	iPixelFormatDataSize = m_iPixelFormatDataSize;
	pPixelFormatData = m_pPixelFormatData;

	return true;
}
//----------------------------------------------------------------------------------------------------------------
bool RendererOCG::GetOCGData(char *& pOCGData, int& iOCGSize)
{
	//TODO: not implemented

	return true;
}
//----------------------------------------------------------------------------------------------------------------
