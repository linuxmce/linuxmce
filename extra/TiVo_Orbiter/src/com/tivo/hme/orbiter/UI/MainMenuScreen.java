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

public class MainMenuScreen extends BasicScreen
{
    SampleList list;   

	private Scenarios m_scenarios = null;    
    
    /**
     * Constructor
     * 
     * 
     * @param parent
     * @param x
     * @param y
     * @param width
     * @param height
     */
    public MainMenuScreen(BApplication app, Scenarios scenarios, PlutoProxy proxy)
    {
        super(app);
        
        m_scenarios = scenarios;
        
        //
        // create the list, each row in the list is a screen.
        //
        
        list = new SampleList(this.getNormal(), SAFE_TITLE_H+10, (getHeight()-SAFE_TITLE_V)-290, 300, 280, 35);
        
        for (Iterator<Map.Entry<Integer, String>> it = m_scenarios.m_mapArrays.entrySet().iterator(); it.hasNext(); )
        {
	        Map.Entry<Integer, String> entry = (Map.Entry<Integer, String>)it.next();
	        Integer nKey = (Integer)entry.getKey();
	        String sValue = (String)entry.getValue();        	
        	list.add(new SubscenariosScreen(getBApp(), nKey, sValue, scenarios, proxy));
        }
        
        //
        // Must set focus to the list so that bar and highlights will show up.
        //
        
        setFocusDefault(list);
    }
    
    /**
     * Hand an action
     */
    public boolean handleAction(BView view, Object action) {
        if (action.equals("push")) {
            //
            // Go to the screen currently selected in the list.
            //

            BScreen screen = (BScreen)(list.get(list.getFocus()));
            getBApp().push(screen, TRANSITION_LEFT);            
            return true;
        }        
       return super.handleAction(view, action);
    }
    
    /**
     * Listen for the left key.
     */
    public boolean handleKeyPress(int code, long rawcode) 
    {
        switch (code) {
          case KEY_LEFT:
            //
            // Kill the application.
            //
            
            getBApp().setActive(false);
            return true;
        }
    
        return super.handleKeyPress(code, rawcode);
    }
    
    /**
     * Screen title.
     */
    public String toString() 
    {
        return "LinuxMCE 1.1";
    }
    
    /**
     * A list which displays the text of the object passed in to each row.  In
     * this particular case each row is a screen and we know that each screen
     * has a toString() method so we use that as the title.
     *
     */
    static class SampleList extends BList 
    {
        /**
         * Constructor
         */
        public SampleList(BView parent, int x, int y, int width, int height, int rowHeight)
        {
            super(parent, x, y, width, height, rowHeight);
            
            //
            // Add a bar that will hang off the left edge of the screen and is
            // the width of the list.
            //
            // In addition, add a right arrow highlight with the action of
            // "push".
            //
            
            setBarAndArrows(BAR_HANG, BAR_DEFAULT, null, "push");
        }

        /**
         * create a row that displays text
         */
        protected void createRow(BView parent, int index)
        {   
            //
            // the row is a simple text item, aligned left, and is the name of
            // the screen
            //
            
            BText text = new BText(parent, 10, 4, parent.getWidth()-40, parent.getHeight() - 4);
            text.setShadow(true);
            text.setFlags(RSRC_HALIGN_LEFT);
            text.setValue(get(index).toString());
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
                return true;
            }
            return super.handleKeyPress(code, rawcode);
        }
    }
}
