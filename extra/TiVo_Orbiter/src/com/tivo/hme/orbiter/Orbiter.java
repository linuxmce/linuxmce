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

import com.tivo.hme.bananas.BApplication;
import com.tivo.hme.bananas.BView;
import com.tivo.hme.interfaces.IContext;
import com.tivo.hme.orbiter.UI.*;

import com.tivo.hme.scenarios.*;
import com.tivo.hme.dce.*;

import java.util.*;
import java.io.*;

import com.tivo.hme.sdk.ImageResource;

public class Orbiter extends BApplication implements MessageProcessor
{
    //
    // This is the title of the application. The runtime environment will
    // discover the title here and use it as the name of the application.
    //
    
    public final static String TITLE = "LinuxMCE 1.1";

	private final int COMMAND_Update_Object_Image_CONST = 32;
	private final int COMMANDPARAMETER_Data_CONST = 19;    
	
	private final int COMMAND_MENU_CONST = 548;
	private final int COMMANDPARAMETER_Text_CONST = 9;
	
	private MainMenuScreen m_MainMenuScreen = null;
	private OrbiterScreen m_OrbiterScreen = null;
	
	private boolean m_bMenuOn = true;
	private final String m_sBackgroundPicture = "blue.jpg";
	
    /**
     * Show the main screen for our sample app, as this is the first screen
     * shown, we will not show a transition to this screen
     */
    public void init(IContext context) throws Exception 
    {
        super.init(context);
        
        String sIPAddress = "localhost";
        String sDeviceID = "0";
        
        //read configuration
		try 
		{
			Properties p = new Properties();
			p.load(new FileInputStream("orbiter_tivo.ini"));
			
			sIPAddress = p.getProperty("IPAddress");
			sDeviceID = p.getProperty("DeviceID");
			
			System.out.println("IPAddress = " + sIPAddress);
			System.out.println("DeviceID = " + sDeviceID);
			p.list(System.out);
		} 
		catch (Exception e) 
		{
			System.out.println(e);
		}         
        
		Scenarios scenarios = new Scenarios();        
		PlutoProxy proxy = new PlutoProxy(this, sIPAddress, 3450, Integer.parseInt(sDeviceID));
		String sXML = proxy.GetScenarios();
		if(null != sXML)
		{
			if(scenarios.Load(sXML))
			{
				System.out.println("Scenarios loaded successfully!");
				scenarios.PrintInfo();
			}
		}
		
		getRoot().setResource(m_sBackgroundPicture);
		
		m_OrbiterScreen = new OrbiterScreen(this, proxy);
		m_MainMenuScreen = new MainMenuScreen(this, scenarios, proxy); 
		m_bMenuOn = true;
		
		push(m_MainMenuScreen, TRANSITION_NONE);
    }
    
    /**
	 * Every screen handle the "pop" action in the same way, so we will handle
	 * this in the application.
	 */
    public boolean handleAction(BView view, Object action) 
    {
        if (action.equals("pop")) 
        {
            pop();
            return true;
        }        
        return super.handleAction(view, action);
    }
    
	public void ProcessMessage(Message message)
	{
		if(null == message)
		{
			//ignore empty messages
			return;
		}
		
		System.out.println("Processing message from router: " + message.Data());
		
		switch(message.MessageID())
		{
			case COMMAND_Update_Object_Image_CONST:
			{
				byte[] pImage = message.GetDataParameter(COMMANDPARAMETER_Data_CONST);
				
				System.out.println("Received image, size " +  (new Integer(pImage.length)).toString());

				ImageResource imgres = createImage(pImage);
				getRoot().setResource(imgres); 
	        	
	        	if(m_bMenuOn)
	        	{
	        		pop();
	        		push(m_OrbiterScreen, TRANSITION_NONE);
	        		m_bMenuOn = false;
	        	}		        	
	        	
				try
				{
					flush();		
				}
				catch (Exception e) 
				{
					System.out.println("Failed to flush. Error: " + e.getMessage());
				}	
				
				break;
			}
			
			case COMMAND_MENU_CONST:
			{
				String sValue = message.GetParameter(COMMANDPARAMETER_Text_CONST);
				System.out.println("Received CMD_Menu with value " + sValue);
				
				if(0 == sValue.compareTo("orbiter"))
				{
		        	if(m_bMenuOn)
		        	{
		        		pop();
		        		push(m_OrbiterScreen, TRANSITION_NONE);
		        		m_bMenuOn = false;
		        	}					
				}
				else
				{
		        	if(!m_bMenuOn)
		        	{
		        		getRoot().setResource(m_sBackgroundPicture);
		        		
		        		pop();
		        		push(m_MainMenuScreen, TRANSITION_NONE);
		        		m_bMenuOn = true;
		        	}								
				}
				
				try
				{
					flush();		
				}
				catch (Exception e) 
				{
					System.out.println("Failed to flush. Error: " + e.getMessage());
				}				
	        				
				break;
			}
		}
	}    
}
