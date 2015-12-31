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

package com.tivo.hme.orbiter;

import com.tivo.hme.dce.*;

public class PlutoProxy 
{
	//device templates
	private final static int DEVICETEMPLATE_XML_Data_Handler_Plugin_CONST = 1868;
	private final static int DEVICETEMPLATE_Moxi_Orbiter_CONST = 1872;
	
	//virtual devices
	private final static int VIRTUALDEVICE_DCEROUTER_CONST = -1000;
	
	//commands
	private final static int COMMAND_Request_XML_Data_CONST = 869;
	private final static int COMMAND_Simulate_Keypress_CONST = 28;
	
	//command parameters
	private final static int COMMANDPARAMETER_DataGrid_ID_CONST = 15;
	private final static int COMMANDPARAMETER_Data_String_CONST = 109;
	private final static int COMMANDPARAMETER_Parameters_CONST = 202;
	private final static int COMMANDPARAMETER_PK_Button_CONST = 26;
	
	private int m_nDeviceID = 0;
	
	private Communication m_comm = null;
	
	public PlutoProxy(MessageProcessor pMessageProcessor, String sIPAddress, int nPort, int nDeviceID)
	{
		m_comm = new Communication(pMessageProcessor, sIPAddress, nPort, nDeviceID);
		
		m_nDeviceID = nDeviceID;
		
		if(!m_comm.Connect())
			System.out.println("Failed to connect to router:" + sIPAddress);
	}
	
	public void finalize()
	{
		m_comm.Disconnect();
	}
	
	public String GetScenarios()
	{
		if(m_comm.IsConnected())
		{
			Message message = new Message(
				  m_nDeviceID,  //from 
				  DEVICETEMPLATE_XML_Data_Handler_Plugin_CONST, //to template
				  Message.MessagePriority.PRIORITY_NORMAL, 
				  Message.MessageType.MESSAGETYPE_COMMAND, 
				  COMMAND_Request_XML_Data_CONST //message id
			);
			message.AddParameter(COMMANDPARAMETER_DataGrid_ID_CONST, "CommandGroups");
			message.AddParameter(COMMANDPARAMETER_Data_String_CONST, "");
			message.AddParameter(COMMANDPARAMETER_Parameters_CONST, "");
			message.ExpectedResponse(Message.MessageExpectedResponse.ER_ReplyMessage);
			message.TargetType(Message.MessageTargetType.TT_DeviceTemplate, "");
			message.Write();
			m_comm.EventHandler().SendMessage(message);
			  
			Message response = m_comm.EventHandler().ReceivedMessage();
			if(null != response)
			{
				String sXML = response.GetParameter(COMMANDPARAMETER_Data_String_CONST);
				System.out.println("Received xml:\n" + sXML);
				
				return sXML;
			 }
		}
		return null;
	}
	
	public void ExecuteCommandGroup(int nCommandGroupID)
	{
		System.out.println("Executing command group with id " + (new Integer(nCommandGroupID)).toString());
		
		if(m_comm.IsConnected())
		{
			Message message = new Message(
				  m_nDeviceID,  //from 
				  VIRTUALDEVICE_DCEROUTER_CONST, //to
				  Message.MessagePriority.PRIORITY_NORMAL, 
				  Message.MessageType.MESSAGETYPE_EXEC_COMMAND_GROUP, 
				  nCommandGroupID //message id
			);
			message.Write();
			m_comm.EventHandler().SendMessage(message);
		}
	}
	
	public void SendKey(int nPlutoKey)
	{
		System.out.println("Sending pluto key to proxy orbiter: " + (new Integer(nPlutoKey)).toString());
		
		if(m_comm.IsConnected())
		{
			Message message = new Message(
				  m_nDeviceID,  //from 
				  DEVICETEMPLATE_Moxi_Orbiter_CONST, 
				  Message.MessagePriority.PRIORITY_NORMAL, 
				  Message.MessageType.MESSAGETYPE_COMMAND, 
				  COMMAND_Simulate_Keypress_CONST //message id
			);
			message.AddParameter(COMMANDPARAMETER_PK_Button_CONST, (new Integer(nPlutoKey)).toString());
			message.TargetType(Message.MessageTargetType.TT_DeviceTemplate, "");
			message.Write();
			m_comm.EventHandler().SendMessage(message);
		}		
	}
}
