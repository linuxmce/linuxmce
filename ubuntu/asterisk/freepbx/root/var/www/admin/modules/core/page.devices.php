<?php /* $Id: page.devices.php 2617 2006-09-26 11:43:51Z qldrob $ */
//Copyright (C) 2004 Coalescent Systems Inc. (info@coalescentsystems.ca)
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

?>
<div class="rnav">
<?php 
$devices = core_devices_list();
drawListMenu($devices, $skip, $type, $display, $extdisplay, _("Device"));
?>
</div>