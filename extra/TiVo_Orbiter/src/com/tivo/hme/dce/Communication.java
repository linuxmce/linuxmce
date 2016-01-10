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

public class Communication implements MessageReceiver
{
	private EventHandler m_pEventHandler = null;
	private CommandHandler m_pCommandHandler = null;
	private CommandHandlerAsync m_pCommandHandlerAsync = null;
	private MessageProcessor m_pMessageProcessor = null;
	
	public Communication(MessageProcessor pMessageProcessor, String sHost, int nPort, int nDevice)
	{
		m_pMessageProcessor = pMessageProcessor;
		m_pEventHandler = new EventHandler(sHost, nPort, nDevice);
		m_pCommandHandler = new CommandHandler(sHost, nPort, nDevice);
	}
	
	public boolean Connect()
	{
		boolean bConnected = m_pEventHandler.Connect() && m_pCommandHandler.Connect();
		
		if(bConnected)
		{
			m_pCommandHandlerAsync = new CommandHandlerAsync(this);
			new Thread(m_pCommandHandlerAsync).start();
	     }		
	     
	     return bConnected;
	}

	public void Disconnect()
	{
		m_pEventHandler.Disconnect();
		m_pCommandHandler.Disconnect();
	}	
	
	public boolean IsConnected()
	{
		return m_pEventHandler.IsConnected() && m_pCommandHandler.IsConnected();
	}
	
	public EventHandler EventHandler()
	{
		return m_pEventHandler;
	}
	
	public void WaitForMessages()
	{
		while(true)
		{
			if(IsConnected())
			{
				Message message = m_pCommandHandler.ReceivedMessage();
				if(null != message)
					m_pMessageProcessor.ProcessMessage(message);
			}
			else
			{
				try
				{
					Thread.sleep(10000);
				}
				catch (Exception e) 
				{
					System.out.println("Failed to sleep. Error: " + e.getMessage());
				}
			}
		}
	}
}
