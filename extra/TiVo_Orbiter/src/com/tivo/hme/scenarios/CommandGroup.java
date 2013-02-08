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

package com.tivo.hme.scenarios;

public class CommandGroup 
{
	private int m_nCommandGroupID = 0;
	private int m_nArrayID = 0;
	private int m_nRoomID = 0;
	private String m_sName = new String();
	private String m_sRoomName = new String();
	
	public CommandGroup(int nCommandGroupID, int nArrayID, int nRoomID, String sName, String sRoomName)
	{
		m_nCommandGroupID = nCommandGroupID;
		m_nArrayID = nArrayID;
		m_nRoomID = nRoomID;
		m_sName = sName;
		m_sRoomName = sRoomName;
	}
	
	public int CommandGroupID()
	{
		return m_nCommandGroupID;
	}
	
	public int ArrayID()
	{
		return m_nArrayID;
	}
	
	public int RoomID()
	{
		return m_nRoomID;
	}
	
	public String Name()
	{
		return m_sName;
	}
	
	public String toString()
	{
		return m_sName + " / " + m_sRoomName;
	}
}
