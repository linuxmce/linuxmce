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

import java.awt.*;
import com.tivo.hme.bananas.*;

public class BasicScreen extends BScreen
{       
    /**
     * Constructor
     */
    public BasicScreen(BApplication app)
    {
        super(app);

        //
        // set the title of the screen
        //
        
        BText title = new BText(getNormal(), SAFE_TITLE_H+100, SAFE_TITLE_V, (getWidth()-(SAFE_TITLE_H*2))-100, 54);
        title.setValue(this.toString());
        title.setColor(Color.yellow);
        title.setShadow(Color.black, 3);
        title.setFlags(RSRC_VALIGN_TOP);
        title.setFont("default-48.font");
    }
}
