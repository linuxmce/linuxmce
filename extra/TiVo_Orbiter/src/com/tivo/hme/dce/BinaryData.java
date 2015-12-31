/*  Java Orbiter
 *
 *  Copyright (C) 2007-2008 Pluto, Inc., a Delaware Corporation
 *
 *  http://www.plutohome.com
 *
 *  Phone: +1 (877) 758-8648
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

package com.tivo.hme.dce;

public class BinaryData 
{
	public static int DATA_BLOCK = 4096;
	byte[] m_pData = null;
	int m_nPos = 0;
	
	public BinaryData()
	{
		m_pData = new byte[DATA_BLOCK];
	}
	
	public BinaryData(byte[] pData)
	{
		m_pData = pData;
	}
	
	public byte[] Data()
	{
		return m_pData;
	}
	
	public int Position()
	{
		return m_nPos;
	}
	
	private void IncreaseSize()
	{
		byte[] pNewData = new byte[m_pData.length + DATA_BLOCK];
		
		for(int i = 0; i < m_pData.length; ++i)
			pNewData[i] = m_pData[i];
		
		m_pData = pNewData;
	}
	
	public BinaryData Write(byte[] pSmallData)
	{
		if(m_nPos + pSmallData.length > m_pData.length)
			IncreaseSize();
		
		for(int i = 0; i < pSmallData.length; ++i)
			m_pData[m_nPos + i] = pSmallData[i];
		
		m_nPos += pSmallData.length;
		
		return this;
	}

	public BinaryData WriteFront(byte[] pSmallData)
	{
		int nNewLength = m_pData.length;
		if(m_nPos + pSmallData.length > m_pData.length)
			nNewLength = m_nPos + pSmallData.length;
		
		byte[] pNewData = new byte[nNewLength];
		
		for(int i = 0; i < pSmallData.length; ++i)
			pNewData[i] = pSmallData[i];
		
		for(int j = 0; j < m_nPos; ++j)
			pNewData[pSmallData.length + j] = m_pData[j];
		
		m_pData = pNewData;
		m_nPos += pSmallData.length;
	
		return this;
	}
	
	public byte[] Read(Integer nBytes)
	{
		byte[] pSmallData = new byte[nBytes];
		
		if(m_nPos + nBytes < m_pData.length)
		{
			for(int i = 0; i < nBytes; ++i)
				pSmallData[i] = m_pData[m_nPos + i];
			
			m_nPos += nBytes;
		}
		else
		{
			System.out.println("Cannot serialize object from position " + 
					(new Integer(m_nPos)).toString() + " bytes to read " +
					(new Integer(nBytes)).toString());
		}
		
		return pSmallData;
	}
	
	public String ReadString()
	{
		String sResult = new String();
		
		for(int i = m_nPos; i < m_pData.length; i++)
		{
			if(m_pData[i] == '\0')
			{
				m_nPos = i + 1;
				break;
			}
			else
				sResult += (char)m_pData[i];
		}
		
		return sResult;
	}
}
