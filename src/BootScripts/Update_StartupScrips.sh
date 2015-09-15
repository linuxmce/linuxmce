#!/bin/bash
. /usr/pluto/bin/pluto.func

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Updating startup scripts"

Q="REPLACE INTO Device_StartupScript(FK_Device,FK_StartupScript,Boot_Order,Background,Enabled,Parameter)
SELECT FK_Device_ControlledVia, PK_StartupScript,Hybrid_Boot_Order,Hybrid_Background,Hybrid_Enabled,Hybrid_Parameter FROM Device
JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
JOIN StartupScript ON 1=1
LEFT JOIN Device_StartupScript ON Device_StartupScript.FK_Device=PK_Device AND Device_StartupScript.FK_StartupScript=PK_StartupScript
WHERE FK_DeviceCategory=8 AND FK_Device_ControlledVia IS NOT NULL
AND Device_StartupScript.FK_Device IS NULL"

RunSQL "$Q;"

Q="REPLACE INTO Device_StartupScript(FK_Device,FK_StartupScript,Boot_Order,Background,Enabled,Parameter)
SELECT PK_Device, PK_StartupScript,MD_Boot_Order,MD_Background,MD_Enabled,MD_Parameter FROM Device
JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
JOIN StartupScript ON 1=1
LEFT JOIN Device_StartupScript ON Device_StartupScript.FK_Device=PK_Device AND Device_StartupScript.FK_StartupScript=PK_StartupScript
WHERE FK_DeviceCategory=8 AND FK_Device_ControlledVia IS NULL
AND Device_StartupScript.FK_Device IS NULL"

RunSQL "$Q;"

Q="REPLACE INTO Device_StartupScript(FK_Device,FK_StartupScript,Boot_Order,Background,Enabled,Parameter)
SELECT PK_Device, PK_StartupScript,Core_Boot_Order,Core_Background,Core_Enabled,Core_Parameter FROM Device
JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
JOIN StartupScript ON 1=1
LEFT JOIN Device_StartupScript ON Device_StartupScript.FK_Device=PK_Device AND Device_StartupScript.FK_StartupScript=PK_StartupScript
WHERE FK_DeviceCategory=7
AND Device_StartupScript.FK_Device IS NULL"

RunSQL "$Q;"

Q="UPDATE Device_StartupScript 
JOIN Device ON FK_Device=PK_Device
JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
SET Boot_Order = StartupScript.Core_Boot_Order 
WHERE FK_DeviceCategory=7"

RunSQL "$Q;"

Q="UPDATE Device_StartupScript 
JOIN Device As Device_Co ON FK_Device=Device_Co.PK_Device
JOIN Device As Device_Hy ON Device_Hy.FK_Device_ControlledVia=Device_Co.PK_Device
JOIN DeviceTemplate AS DeviceTemplate_Co ON Device_Co.FK_DeviceTemplate=DeviceTemplate_Co.PK_DeviceTemplate
JOIN DeviceTemplate AS DeviceTemplate_Hy ON Device_Hy.FK_DeviceTemplate=DeviceTemplate_Hy.PK_DeviceTemplate
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
SET Boot_Order = StartupScript.Hybrid_Boot_Order 
WHERE DeviceTemplate_Hy.FK_DeviceCategory=8 AND DeviceTemplate_Co.FK_DeviceCategory=7"

RunSQL "$Q;"

Q="UPDATE Device_StartupScript 
JOIN Device ON FK_Device=PK_Device
JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
JOIN StartupScript ON FK_StartupScript=PK_StartupScript
SET Boot_Order = StartupScript.MD_Boot_Order 
WHERE FK_DeviceCategory=8 AND FK_Device_ControlledVia IS NULL"

RunSQL "$Q;"

