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

package com.tivo.hme.orbiter.UI;

import java.util.Iterator;
import java.util.Map;

import com.tivo.hme.bananas.*;
import com.tivo.hme.orbiter.PlutoProxy;
import com.tivo.hme.scenarios.Scenarios;
import com.tivo.hme.scenarios.CommandGroup;

public class SubscenariosScreen extends BasicScreen
{
	Integer m_nArrayID = 0;
    String m_sArrayName = new String();
    Scenarios m_scenarios = null;
	
    /**
     * Constructor 
     */
    public SubscenariosScreen(BApplication app, int nArrayID, String sArrayName, Scenarios scenarios, PlutoProxy proxy)
    {
        super(app);
        
        m_nArrayID = nArrayID;
        m_sArrayName = sArrayName;
        m_scenarios = scenarios;
        
        // 
        // create a standard list
        //
        
        StandardList standardList = new StandardList(getNormal(), proxy, 30, 150, 400, 280, 40);

        for (Iterator<Map.Entry<Integer, CommandGroup>> it = m_scenarios.m_mapCommandGroups.entrySet().iterator(); it.hasNext(); )
        {
	        Map.Entry<Integer, CommandGroup> entry = (Map.Entry<Integer, CommandGroup>)it.next();
	        CommandGroup pCommandGroup = (CommandGroup)entry.getValue();
	        
	        if(pCommandGroup.ArrayID() == m_nArrayID)
	        	standardList.add(pCommandGroup);
        }

        setFocusDefault(standardList);
    }
  
    /**
     * Title of the screen
     */
    public String toString() 
    {
        return m_sArrayName;
    }
    
    /**
     * A simple standard list that displays an icon and a string
     * 
     */
    static class StandardList extends BList 
    {
    	private PlutoProxy m_proxy;
    	
        /**
         * Constructor
         */
        public StandardList(BView parent, PlutoProxy proxy, int x, int y, int width, int height, int rowHeight)
        {
            super(parent, x, y, width, height, rowHeight);
            m_proxy = proxy;
            setBarAndArrows(BAR_HANG, BAR_DEFAULT, "pop", H_RIGHT);
        }

        /**
         * Create a row that has an icon and text.
         */
        protected void createRow(BView parent, int index)
        {
            BText text = new BText(parent, 60, 0, parent.getWidth()-70, parent.getHeight());
            text.setFlags(RSRC_HALIGN_LEFT);
            text.setShadow(true);
            
            text.setValue(get(index));
        }     
        
        /**
         * Transform SELECT key into a push action.
         */
        public boolean handleKeyPress(int code, long rawcode) 
        {
            switch (code) {
              case KEY_SELECT:
                //
                // Go to the screen currently selected in the list.
                //
                postEvent(new BEvent.Action(this, "push"));
                
                CommandGroup pCommandGroup = (CommandGroup)this.get(getFocus());
                m_proxy.ExecuteCommandGroup(pCommandGroup.CommandGroupID());
                //TODO: send the command
                                
                return true;
            }
            return super.handleKeyPress(code, rawcode);
        }
    }
}
