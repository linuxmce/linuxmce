/*
 DCERouter_CreatePluginHardCoded_Impl
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the GNU Public License, available at: 
 http://www.fsf.org/licensing/licenses/gpl.html 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.
 
 */
 
#include "DCERouter.h"

#ifdef AUDIDEMO

#include "../Media_Plugin/Media_Plugin.h"
#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../File_Grids_Plugin/File_Grids_Plugin.h"
#include "../Orbiter_Plugin/Orbiter_Plugin.h"
#include "../Xine_Plugin/Xine_Plugin.h"
#include "../Media_Grids_Plugin/Media_Grids_Plugin.h"
#include "../General_Info_Plugin/General_Info_Plugin.h"
#include "../Mozilla_Plugin/Mozilla_Plugin.h"
#include "../Lighting_Plugin/Lighting_Plugin.h"
#include "../Climate_Plugin/Climate_Plugin.h"
#include "../Security_Plugin/Security_Plugin.h"
#include "../Telecom_Plugin/Telecom_Plugin.h"
#include "../MythTV_PlugIn/MythTV_PlugIn.h"

Command_Impl *Router::CreatePlugInHardCoded(int PK_Device, int PK_DeviceTemplate, string sCommandLine)
{
    typedef class Command_Impl * (* RAP_FType) (class Router *, int, string);
    RAP_FType RegisterAsPlugin;

    if (sCommandLine == "")
        return NULL;

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Loading plug-in device: %d master device: %d -- %s",PK_Device, PK_DeviceTemplate, sCommandLine.c_str());

    switch (PK_DeviceTemplate)
    {
        case DEVICETEMPLATE_Media_Plugin_CONST:
            return new Media_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Datagrid_Plugin_CONST:
            return new Datagrid_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_File_Grids_Plugin_CONST:
            return new File_Grids_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Orbiter_Plugin_CONST:
            return new Orbiter_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Xine_Plugin_CONST:
            return new Xine_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Media_Grids_Plugin_CONST:
            return new Media_Grids_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_General_Info_Plugin_CONST:
            return new General_Info_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Mozilla_Plugin_CONST:
            return new Mozilla_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Lighting_Plugin_CONST:
            return new Lighting_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Climate_Plugin_CONST:
            return new Climate_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Security_Plugin_CONST:
            return new Security_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_Telecom_Plugin_CONST:
            return new Telecom_Plugin(PK_Device, "localhost", true, false, this);

        case DEVICETEMPLATE_MythTV_PlugIn_CONST:
            return new MythTV_PlugIn(PK_Device, "localhost", true, false, this);

        default:
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Invalid plugin ID: %d. Returning NULL.", PK_Device);
            return NULL;
    }
}

#endif
