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

import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

public class Message
{
	BinaryData m_data;
	
	private static int MAGIC_START = 1234;
	private static int MAGIC_STOP = 6789;
	private static int DEVICEID_MASTERDEVICE = -2000;
	private static int DEVICEID_CATEGORY = -2001;
	private static int DEVICEID_LIST = -2002;
	
	private int m_nDeviceIDFrom = 0;
	private int m_nDeviceIDTo = 0;
	private MessagePriority m_Priority = MessagePriority.PRIORITY_NORMAL;
	private MessageType m_MessageType = MessageType.MESSAGETYPE_COMMAND;
	private int m_nID = 0;
	private int m_nGroupID = 0; 
	private int m_nCategoryID = 0;
	private int m_nTemplateID = 0;
	private boolean m_bIncludeChildren = false;
	private MessageBroadcastLevel m_BroadcastLevel = MessageBroadcastLevel.BL_SameHouse;
	private MessageRetry m_Retry = MessageRetry.MR_None;
	private boolean m_bRelativeToSender = true;
	private MessageExpectedResponse m_ExpectedResponse = MessageExpectedResponse.ER_None;
	private String m_sToDevicesList = new String();
	
	private Map<Integer, String> m_mapParameters = new HashMap<Integer, String>();
	private Map<Integer, byte[]> m_mapDataParameters = new HashMap<Integer, byte[]>();
	private List<Message> m_listExtraMessages = new ArrayList<Message>();
	
    public enum MessagePriority
    { 
    	PRIORITY_LOW(0), 
    	PRIORITY_NORMAL(1), 
    	PRIORITY_HIGH(2), 
    	PRIORITY_URGENT(3);
    	
    	private int m_nValue;
    	int value() { return m_nValue; }
    	MessagePriority(int nValue) { m_nValue = nValue; }
    	public static MessagePriority convert(int i) { return values()[i]; }
    }
    
    public enum MessageType 
    {
    	MESSAGETYPE_COMMAND(1), 
    	MESSAGETYPE_EVENT(2), 
    	MESSAGETYPE_DATAPARM_CHANGE(3), 
    	MESSAGETYPE_REPLY(4), 
		MESSAGETYPE_DATAPARM_REQUEST(5), 
		MESSAGETYPE_LOG(6), 
		MESSAGETYPE_SYSCOMMAND(7), 
		MESSAGETYPE_REGISTER_INTERCEPTOR(8), 
		MESSAGETYPE_MESSAGE_INTERCEPTED(9), 
		MESSAGETYPE_EXEC_COMMAND_GROUP(10),
		MESSAGETYPE_START_PING(11), 
		MESSAGETYPE_STOP_PING(12), 
		MESSAGETYPE_PURGE_INTERCEPTORS(13),
		MESSAGETYPE_PENDING_TASKS(14);
    	
    	private int m_nValue;
    	int value() { return m_nValue; }
    	MessageType(int nValue) { m_nValue = nValue; }    	
    	public static MessageType convert(int i) { return values()[i]; }
	}
	
    public enum MessageBroadcastLevel
    {
    	BL_None(0),
    	BL_DirectSiblings(1),
    	BL_SameComputer(2),
    	BL_SameRoom(3),
    	BL_SameHouse(4),
    	BL_AllHouses(5);
    	
    	private int m_nValue;
    	int value() { return m_nValue; }
    	MessageBroadcastLevel(int nValue) { m_nValue = nValue; }    
    	public static MessageBroadcastLevel convert(int i) { return values()[i]; }
    }
    
    public enum MessageRetry 
    { 
    	MR_None(0),
    	MR_Retry(1),
    	MR_Persist(2);
    	
    	private int m_nValue;
    	int value() { return m_nValue; }
    	MessageRetry(int nValue) { m_nValue = nValue; }       
    	public static MessageRetry convert(int i) { return values()[i]; }
    };
    
    public enum MessageExpectedResponse 
    { 
    	ER_None(0), 
    	ER_ReplyMessage(1), 
    	ER_ReplyString(2), 
    	ER_DeliveryConfirmation(3);
    	
    	private int m_nValue;
    	int value() { return m_nValue; }
    	MessageExpectedResponse(int nValue) { m_nValue = nValue; } 
    	public static MessageExpectedResponse convert(int i) { return values()[i]; }
    };
    
    public enum MessageTargetType
    {
    	TT_Device,
    	TT_DeviceTemplate,
    	TT_DeviceCategory,
    	TT_DeviceList
    }
    
	public Message()
	{
	
	}
	
	public Message(int nDeviceIDFrom, int nDeviceIDTo, MessagePriority Priority, MessageType MessageType, int nID)
	{
		m_nDeviceIDFrom = nDeviceIDFrom;
		m_nDeviceIDTo = nDeviceIDTo;
		m_Priority = Priority;
		m_MessageType = MessageType;
		m_nID = nID;		
	}
	
	public void TargetType(MessageTargetType targetType, String sDevicesList)
	{
		switch(targetType)
		{
			case TT_DeviceTemplate:
				m_nTemplateID = m_nDeviceIDTo;
				m_nDeviceIDTo = DEVICEID_MASTERDEVICE;
				break;
				
			case TT_DeviceCategory:
				m_nCategoryID = m_nDeviceIDTo;
				m_nDeviceIDTo = DEVICEID_CATEGORY;
				break;
				
			case TT_DeviceList:
				m_sToDevicesList = sDevicesList;
				m_nDeviceIDTo = DEVICEID_LIST;
				break;
		}
	}
	
	public Message(byte[] pData)
	{
		Clear();
		
		m_data = new BinaryData(pData);
		FromData();
	}
	
	private void Clear()
	{
		
	}
	
	public void Read(byte[] data)
	{
		m_data = new BinaryData(data);
		FromData();
	}
	
	public void Read(BinaryData bdata)
	{
		m_data = bdata;
		FromData();
	}
	
	public void FromData()
	{
		Integer nMagicStart = Serialize.ReadInteger(m_data);
		if(nMagicStart != MAGIC_START)
		{
			System.out.println("Malformated message: magic number for start is not respected!");
			return;
		}
		
		try
		{
			m_nDeviceIDFrom = Serialize.ReadInteger(m_data);
			m_nDeviceIDTo = Serialize.ReadInteger(m_data);
			m_nGroupID = Serialize.ReadInteger(m_data);
			m_nID = Serialize.ReadInteger(m_data);
			m_Priority = MessagePriority.convert(Serialize.ReadInteger(m_data));
			m_MessageType = MessageType.convert(Serialize.ReadInteger(m_data));
			m_nCategoryID = Serialize.ReadInteger(m_data);
			m_nTemplateID = Serialize.ReadInteger(m_data);
			m_bIncludeChildren = Serialize.ReadChar(m_data) != '0';
			m_BroadcastLevel = MessageBroadcastLevel.convert(Serialize.ReadInteger(m_data));
			m_Retry = MessageRetry.convert(Serialize.ReadInteger(m_data));
			m_bRelativeToSender = Serialize.ReadChar(m_data) != '0';
			m_ExpectedResponse = MessageExpectedResponse.convert(Serialize.ReadInteger(m_data));
			m_sToDevicesList = Serialize.ReadString(m_data);
			
			int nStringParameters = Serialize.ReadInteger(m_data);
			for(int i = 0; i < nStringParameters; ++i)
			{
				int nKey = Serialize.ReadInteger(m_data);
				String sValue = Serialize.ReadString(m_data);
				m_mapParameters.put(nKey, sValue);
			}
			
			int nDataParameters = Serialize.ReadInteger(m_data);
			for(int i = 0; i < nDataParameters; ++i)
			{
				int nKey = Serialize.ReadInteger(m_data);
				byte[] pValue = Serialize.ReadBlock(m_data);
				m_mapDataParameters.put(nKey, pValue);				
			}
			
			int nExtraMessages = Serialize.ReadInteger(m_data);
			for(int i = 0; i < nExtraMessages; ++i)
			{
				Message message = new Message();
				message.Read(m_data);
				m_listExtraMessages.add(message);
			}
		}
		catch (Exception e) 
		{
			System.out.println("Failed to parse message. Error: " + e.getMessage());
		}
		
		Integer nMagicEnd = Serialize.ReadInteger(m_data);
		if(nMagicEnd != MAGIC_STOP)
		{
			System.out.println("Malformated message: magic number for end message is not respected!");
			return;
		}		
	}

	public BinaryData Write()
	{
		m_data = new BinaryData();		
		m_data = ToData(m_data, true);
		
		return m_data;
	}	
	
	public BinaryData ToData(BinaryData data, boolean bWithHeader)
	{
 		m_data = Serialize.WriteInteger(m_data, MAGIC_START);
		m_data = Serialize.WriteInteger(m_data, m_nDeviceIDFrom);
		m_data = Serialize.WriteInteger(m_data, m_nDeviceIDTo);
		m_data = Serialize.WriteInteger(m_data, m_nGroupID); 
		m_data = Serialize.WriteInteger(m_data, m_nID); 
		m_data = Serialize.WriteInteger(m_data, m_Priority.value());
		m_data = Serialize.WriteInteger(m_data, m_MessageType.value());
		m_data = Serialize.WriteInteger(m_data, m_nCategoryID); 
		m_data = Serialize.WriteInteger(m_data, m_nTemplateID); 
		m_data = Serialize.WriteChar(m_data, m_bIncludeChildren ? '1' : '0'); 
		m_data = Serialize.WriteInteger(m_data, m_BroadcastLevel.value());
		m_data = Serialize.WriteInteger(m_data, m_Retry.value());
		m_data = Serialize.WriteChar(m_data, m_bRelativeToSender ? '1' : '0'); 
		m_data = Serialize.WriteInteger(m_data, m_ExpectedResponse.value());
		m_data = Serialize.WriteString(m_data, m_sToDevicesList); 
		
		m_data = Serialize.WriteInteger(m_data, m_mapParameters.size()); 
	    for (Iterator<Map.Entry<Integer, String>> it = m_mapParameters.entrySet().iterator(); it.hasNext(); )
	    {
	        Map.Entry<Integer, String> entry = (Map.Entry<Integer, String>)it.next();
	        Integer nKey = (Integer)entry.getKey();
	        String sValue = (String)entry.getValue();
	        
	        m_data = Serialize.WriteInteger(m_data, nKey); 
	        m_data = Serialize.WriteString(m_data, sValue); 
	    }
	    
		m_data = Serialize.WriteInteger(m_data, m_mapDataParameters.size()); 
		for (Iterator<Map.Entry<Integer, byte[]>> it = m_mapDataParameters.entrySet().iterator(); it.hasNext(); )
		{
	        Map.Entry<Integer, byte[]> entry = (Map.Entry<Integer, byte[]>)it.next();
	        Integer nKey = (Integer)entry.getKey();
	        byte[] pValue = (byte[])entry.getValue();
	        
	        m_data = Serialize.WriteInteger(m_data, nKey);
	        m_data = Serialize.WriteBlock(m_data, pValue); 			
		}
		
		m_data = Serialize.WriteInteger(m_data, m_listExtraMessages.size());
		for(Message message : m_listExtraMessages)
		{	
			m_data = message.ToData(m_data, false);
		}
		
		m_data = Serialize.WriteInteger(m_data, MAGIC_STOP);
		
		if(bWithHeader)
		{
			String sHeader = "MESSAGE " + (new Integer(m_data.Position())).toString() + "\n";
			m_data = Serialize.WriteStringFront(m_data, sHeader);
		}
		
		return m_data;
	}
	
	public byte[] Data()
	{
		return m_data.Data();
	}
	
	public void AddParameter(int nKey, String sValue)
	{
		m_mapParameters.put(nKey, sValue);
	}
	
	public void AddDataParameter(int nKey, byte[] value)
	{
		m_mapDataParameters.put(nKey, value);
	}
	
	public void AddExtraMessage(Message message)
	{
		m_listExtraMessages.add(message);
	}
	
	public void ExpectedResponse(MessageExpectedResponse aExpectedResponse)
	{
		m_ExpectedResponse = aExpectedResponse;
	}
	
	public String GetParameter(int Key)
	{
		if(m_mapParameters.containsKey(Key))
			return (String)m_mapParameters.get(Key);
		
		return "";
	}
	
	public byte[] GetDataParameter(int Key)
	{
		if(m_mapDataParameters.containsKey(Key))
			return m_mapDataParameters.get(Key);
		
		return new byte[0];
	}
	
	public int MessageID()
	{
		return m_nID;
	}
}
