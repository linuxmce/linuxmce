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

import java.io.StringReader;
import java.util.HashMap;
import java.util.Map;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.*;
import org.xml.sax.InputSource;

public class Scenarios 
{
	Document m_doc = null;
	
	public Map<Integer, CommandGroup> m_mapCommandGroups = new HashMap<Integer, CommandGroup>();
	public Map<Integer, String> m_mapArrays = new HashMap<Integer, String>();
	public Map<Integer, String> m_mapRooms = new HashMap<Integer, String>();
	
	public Scenarios()
	{
		
	}
	
	public boolean Load(String sXML)
	{
		try
		{
			m_doc = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(
					new InputSource(new StringReader(sXML)));
			
			NodeList nodesPlutoXMLData = m_doc.getElementsByTagName("PlutoXmlData");
			if(nodesPlutoXMLData.getLength() != 0)
			{
				Element elemPlutoXMLData = (Element)nodesPlutoXMLData.item(0);

				//parsing status
				boolean bParsingStatusOK = true;

				//room list
				NodeList nodesRoomList = elemPlutoXMLData.getElementsByTagName("RoomList");
				if(nodesRoomList.getLength() != 0)
					bParsingStatusOK = bParsingStatusOK && ParseRoomList((Element)nodesRoomList.item(0));
				
				//command group list
				NodeList nodesCommandGroupList = elemPlutoXMLData.getElementsByTagName("CommandGroupList");
				if(nodesCommandGroupList.getLength() != 0)
					bParsingStatusOK = bParsingStatusOK && ParseCommandGroupList((Element)nodesCommandGroupList.item(0));
				
				//array list
				NodeList nodesArrayList = elemPlutoXMLData.getElementsByTagName("ArrayList");
				if(nodesArrayList.getLength() != 0)
					bParsingStatusOK = bParsingStatusOK && ParseArrayList((Element)nodesArrayList.item(0));
				
				return bParsingStatusOK;
			}
		}
		catch(Exception error)
		{
			System.out.println("Error parsing xml: " + error.getMessage());
			error.printStackTrace();
		}
		
		return false;		
	}
	
	private boolean ParseCommandGroupList(Element elem)
	{
		if(elem.getNodeName() == "CommandGroupList")
		{
			NodeList nodesCommandGroup = elem.getElementsByTagName("CommandGroup");
			for(int i = 0; i < nodesCommandGroup.getLength(); ++i)
			{
				Element elemCommandGroup = (Element)nodesCommandGroup.item(i);

				try
				{
					int nCommandGroupID = Integer.parseInt(elemCommandGroup.getAttribute("PK_CommandGroup")); 
					int nArrayID = Integer.parseInt(elemCommandGroup.getAttribute("PK_Array"));

					String sCommandGroupName = "Unknown";
					Node nodeCommandGroupName = elemCommandGroup.getFirstChild();
					if(null != nodeCommandGroupName)
						sCommandGroupName = nodeCommandGroupName.getNodeValue();
					
					int nRoomID = 0;
					NodeList nodesRoom = elemCommandGroup.getElementsByTagName("Room");
					if(0 != nodesRoom.getLength())
					{
						Element elemRoom = (Element)nodesRoom.item(0);
						nRoomID = Integer.parseInt(elemRoom.getAttribute("PK_Room"));
					}
					
					String sRoomName = "Unknown room";
					if(m_mapRooms.containsKey(nRoomID))
						sRoomName = (String)m_mapRooms.get(nRoomID);
					
					m_mapCommandGroups.put(nCommandGroupID, new CommandGroup(nCommandGroupID,
							nArrayID, nRoomID, sCommandGroupName, sRoomName));
				}
				catch(Exception error)
				{
					System.out.println("Error parsing xml: " + error.getMessage());
				}
			}
			
			return true;
		}
		
		return false;
	}

	private boolean ParseRoomList(Element elem)
	{
		if(elem.getNodeName() == "RoomList")
		{
			NodeList nodesRoom = elem.getElementsByTagName("Room");
			for(int i = 0; i < nodesRoom.getLength(); ++i)
			{
				Element elemRoom = (Element)nodesRoom.item(i);
				
				try
				{
					int nRoomID = Integer.parseInt(elemRoom.getAttribute("PK_Room"));

					String sRoomName = "Unknown";
					Node nodeRoomName = elemRoom.getFirstChild();
					if(null != elemRoom)
						sRoomName = nodeRoomName.getNodeValue();					
					
					m_mapRooms.put(nRoomID, sRoomName);
				}
				catch(Exception error)
				{
					System.out.println("Error parsing xml: " + error.getMessage());
				}				
			}
			
			return true;
		}
		
		return false;
	}
	
	private boolean ParseArrayList(Element elem)
	{
		if(elem.getNodeName() == "ArrayList")
		{
			NodeList nodesArray = elem.getElementsByTagName("Array");
			for(int i = 0; i < nodesArray.getLength(); ++i)
			{
				Element elemArray = (Element)nodesArray.item(i);
				
				try
				{
					int nArrayID = Integer.parseInt(elemArray.getAttribute("PK_Array"));
					
					String sArrayName = "Unknown";
					Node nodeArrayName = elemArray.getFirstChild();
					if(null != elemArray)
						sArrayName = nodeArrayName.getNodeValue();						
					
					m_mapArrays.put(nArrayID, sArrayName);
				}
				catch(Exception error)
				{
					System.out.println("Error parsing xml: " + error.getMessage());
				}				
			}
			
			return true;
		}
		
		return false;
	}	
	
	public void PrintInfo()
	{
		System.out.println("Scenarios information");
		System.out.println("=====================");
		System.out.println("Number of scenarios : " + (new Integer(m_mapCommandGroups.size()).toString()));
		System.out.println("Number of rooms     : " + (new Integer(m_mapRooms.size()).toString()));
		System.out.println("Number of arrays    : " + (new Integer(m_mapArrays.size()).toString()));
	}
}
