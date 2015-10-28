/***************************************************************************
 * Linux kernel device handling
 *
 * Copyright (C) 2004 David Zeuthen, <david@fubar.dk>
 * Copyright (C) 2005 Richard Hughes, <richard@hughsie.com>
 * Copyright (C) 2005 Danny Kukawka, <danny.kukawka@web.de>
 * Copyright (C) 2006 Kay Sievers <kay.sievers@vrfy.org>
 *
 * Copyright (C) 2005 David Zeuthen, <david@fubar.dk>
 * Copyright (C) 2005,2006 Kay Sievers, <kay.sievers@vrfy.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 **************************************************************************/

/*
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
*/

#include <libgen.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/types.h>
/*
#include <limits.h>
#include <sys/socket.h>   // for ifru_* has incomplete type
#include <linux/if_arp.h> // for ARPHRD_...
#include <asm/byteorder.h>
#include <fcntl.h>

#ifdef HAL_LINUX_INPUT_HEADER_H
  #include HAL_LINUX_INPUT_HEADER_H
#else
  #include <linux/input.h>
#endif

// for wireless extensions
#include <linux/if.h>
#include <linux/wireless.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#include "../device_info.h"
#include "../device_pm.h"
#include "../device_store.h"
#include "../hald.h"
#include "../hald_dbus.h"
#include "../hald_runner.h"
#include "../logger.h"
#include "../osspec.h"
#include "../util.h"
#include "../util_pm.h"
#include "../ids.h"

#include "coldplug.h"
#include "hotplug_helper.h"
#include "osspec_linux.h"
*/

#include <libudev.h>

#include "udi-helper.h"

/*
// this is kinda messy... but acpi.c + friends use this
gboolean _have_sysfs_lid_button = FALSE;
gboolean _have_sysfs_power_button = FALSE;
gboolean _have_sysfs_sleep_button = FALSE;
gboolean _have_sysfs_power_supply = FALSE; 
static gboolean battery_poll_running = FALSE;

#define POWER_SUPPLY_BATTERY_POLL_INTERVAL 30  // in seconds
#define DOCK_STATION_UNDOCK_POLL_INTERVAL 300  // in milliseconds
*/

/*
// we must use this kernel-compatible implementation
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)
*/


void
hal_util_compute_udi_valist (gchar *dst, gsize dstsize, const gchar *format, va_list args)
{
	g_vsnprintf (dst, dstsize, format, args);

	g_strcanon (dst,
		    "/_"
		    "abcdefghijklmnopqrstuvwxyz"
		    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		    "1234567890", '_');
}


void
hal_util_compute_udi (gchar *dst, gsize dstsize, const gchar *format, ...)
{
	va_list args;

	va_start (args, format);
	hal_util_compute_udi_valist (dst, dstsize, format, args);
	va_end (args);
}

void
hal_util_validate_udi (gchar *udi, gsize size) {

	char end[size];

	if (sscanf (udi, "/org/freedesktop/Hal/devices/%s", end) == 1) {
		if (strstr(end, "/") != NULL) {
			g_strcanon (end, "_"
		    			 "abcdefghijklmnopqrstuvwxyz"
		    			 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		   	 		 "1234567890", '_');
			g_snprintf (udi, size, "/org/freedesktop/Hal/devices/%s", end);
		}
	}
}

void
hald_compute_udi (gchar *dst, gsize dstsize, const gchar *format, ...)
{
	va_list args;

	va_start (args, format);
	hal_util_compute_udi_valist (dst, dstsize, format, args);
	va_end (args);

	hal_util_validate_udi (dst, dstsize);
}

int
udev_device_has_property (struct udev_device *device, const char *name)
{
	return(udev_device_get_property_value(device, name) != NULL);
}

int
udev_device_has_sysattr (struct udev_device *device, const char *name)
{
	return(udev_device_get_sysattr_value(device, name) != NULL);
}

static GHashTable *table = NULL;
static GHashTable *table2 = NULL;

int udi_helper_lookup(const char *udi, const char *syspath);

const char *
udi_helper_map_get_udev_syspath (const char *udi)
{
	return g_hash_table_lookup(table, udi);
}

void
print(gpointer key, gpointer value, gpointer user_data) {
	printf(user_data, key, value);
}

void
udi_helper_map_dump(void)
{
	if (!table)
		return;

	g_hash_table_foreach (table, (GHFunc)print, " #### Key = %s  ####  Value = %s\n");
}

const char *
udi_helper_get_key_by_value(const char *syspath)
{
	if (!table2)
		return NULL;

	return g_hash_table_lookup(table2, syspath);
}

int
udi_helper_map_table_new()
{
	if (table && table2)
		return 0;

	if (!table)
		table = g_hash_table_new(g_str_hash, g_str_equal);

	if (!table2)
		table2 = g_hash_table_new(g_str_hash, g_str_equal);


	return 1;
}

const char *
udi_helper_get_udi_by_syspath(const char *syspath)
{
	if (!table2)
		return NULL;

	return g_hash_table_lookup(table2, syspath);
}

const char *
udi_helper_udi_exists(const char *udi)
{
	if (!table)
		return NULL;

	const char *value = g_hash_table_lookup(table, udi);

	if (value != NULL) {
		//printf(" UDI_EXISTS-1.0- %s -- %s - EXISTS!\n", udi, value);
	} else {
		//printf(" UDI_EXISTS-1.0- %s - DOES NOT EXIST!\n", udi);
	}
	return g_hash_table_lookup(table, udi);
}

int
udi_helper_map_compare_value(const char *udi, const char *syspath)
{
	if (!table)
		return -1;

	char *value = g_hash_table_lookup(table, udi);

	if (value != NULL) {
		//printf (" COMPARE: %s ?? %s\n", syspath, value);
		if (g_strcmp0(syspath, value) == 0) {
			//printf (" COMPARE: %s == %s\n", syspath, value);
			return g_strcmp0(syspath, value);
		} else {
			//printf (" COMPARE: %s != %s\n", syspath, value);
		}
	}
	return -1;
}

/*
const char *
udi_helper_map_get_value(const char *udi)
{
	if (!table)
		return NULL;

	return (gchar *) g_hash_table_lookup (table, udi);
}
*/

int
udi_helper_remove_device(const char *udi, const char *syspath)
{
	if (udi != NULL)
	{
/*		const char *key, *value;
		g_hash_table_lookup_extended(table, udi, key, value);

		if ( g_hash_table_lookup(table2, value) != NULL )
			g_hash_table_remove(table2, syspath);

		free(key);
		free(value);

		g_hash_table_remove(table, udi);
*/
		return 1;
	}
	else if (syspath != NULL)
	{
		const char *key = g_hash_table_lookup(table2, syspath);
		const char *value = g_hash_table_lookup(table, key);

		if (key != NULL)
		{
			g_hash_table_remove(table, key);
			free( (void *) key);
		}
		if (value != NULL)
			free( (void *) value);

		g_hash_table_remove(table2, syspath);
		return 1;
	}
	else
	{
		//printf (" REMOVE: udi/syspath both NULL, nothing to remove");
	}
	return 0;
}

const char *
udi_helper_add_to_map_inc(const char *udi, const char *syspath, int increment)
{
	int count = -1;

	char key[256];
	strcpy(key, udi); //= g_strdup(udi);

	while ( (increment != 0) && udi_helper_udi_exists(key) != NULL && udi_helper_map_compare_value(key, syspath) != 0)
	{
			count += increment;
			sprintf(key, "%s_%i", udi, count);
	}

	if ( udi_helper_udi_exists(key) == NULL )
	{

		char *key_dup = g_strdup(key);
		char *value = g_strdup(syspath);

		g_hash_table_insert(table, key_dup, value);
		g_hash_table_insert(table2, value, key_dup);

//		udi_helper_map_dump();

		return key_dup;
	} else {
		char* key_chk = NULL;
		char* value = NULL;
		char** key_chk_ptr = &key_chk;
		char** value_ptr = &value;
		gboolean result = g_hash_table_lookup_extended(table, key, (gpointer*)key_chk_ptr, (gpointer*)value_ptr);
		if (result) {
			return key_chk;
		}
	}
	return NULL;
}

const char *
udi_helper_add_to_map(const char *udi, const char *syspath)
{
	return udi_helper_add_to_map_inc(udi, syspath, 1);
}


/* ----------------------------------------------------- */

const char *
acpi_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	char *path = strdup(syspath);
	char *basepath = basename(path);

	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	if (uid != NULL) {
		hald_compute_udi (udi, sizeof (udi),
				  "%s_acpi_%s",
				  uid,
				  basepath);
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/acpi_%s",
				  basepath);
	}
	g_free(path);

	return	udi_helper_add_to_map(udi, syspath);
}

const char *
amba_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	if (uid != NULL) {
		hald_compute_udi (udi, sizeof (udi),
				  "%s_amba_%s",
				  uid,
				  udev_device_get_sysattr_value(d, "id"));
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/amba_%s",
				  udev_device_get_sysattr_value(d, "id"));
	}

	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
backlight_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "%s_backlight",
			  hal_device_property_get_string (d, "info.parent"));
	hal_device_set_udi (d, udi);
	return TRUE;
}
*/
const char *
blockdev_compute_udi (struct udev_device *d)
{
	gchar udi[256];
	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uuid = udev_device_get_property_value (d, "ID_FS_UUID");
	const char *label = udev_device_get_property_value (d, "volume.label"); // udisks: get this somehow? udisks ??
	const char *devtype = udev_device_get_devtype(d);

	if (devtype && strcmp(devtype, "partition") == 0) {
		char *volumelabel = NULL;

	        // replace '/'  to avoid trouble if the string gets part of the UDI see fd.o #11401
		if (label != NULL) {
			volumelabel = g_strdup(label);
		        volumelabel = g_strdelimit (volumelabel, "/", '_');
		}

		if (uuid != NULL && strlen (uuid) > 0) {
			printf(" >>>>>> %s\n", uuid);
			hald_compute_udi (udi, sizeof (udi),
					  "/org/freedesktop/Hal/devices/volume_uuid_%s", uuid);
		} else if (volumelabel != NULL && strlen (volumelabel) > 0) {
			hald_compute_udi (udi, sizeof (udi),
					  "/org/freedesktop/Hal/devices/volume_label_%s", volumelabel);
		} else if (udev_device_has_property(d, "volume.is_disc") && // udisks: get this somehow? udisks ??
			   udev_device_has_sysattr(d, "volume.disc.is_blank")) { // udisks: get this somehow? udisks ??
			// this should be a empty CD/DVD
			hald_compute_udi (udi, sizeof (udi),
                                          "/org/freedesktop/Hal/devices/volume_empty_%s",
					  udev_device_get_property_value (d, "ID_TYPE"));
		} else {
			// fallback to partition number, size
			const char *size = udev_device_get_sysattr_value (d, "size");
			int int_size = strtol(size, NULL, 10) * 512;

			hald_compute_udi (udi, sizeof (udi),
//					  "/org/freedesktop/Hal/devices/volume_part%s_size_%lld", // TODO: revisit if necessary
					  "/org/freedesktop/Hal/devices/volume_part%s_size_%i",
					  udev_device_get_sysattr_value (d, "partition"),
					  int_size);
		}
		g_free(volumelabel);
	} else {
		const char *model = udev_device_get_property_value (d, "ID_MODEL");
		const char *serial = udev_device_get_property_value (d, "ID_SERIAL");
		const char *bus = udev_device_get_property_value (d, "ID_BUS");
		const char *type = udev_device_get_property_value (d, "ID_TYPE");
		const char *uid = udi_helper_compute_parent_udi(d);
		const char *syspath = udev_device_get_syspath(d);

		if (strstr(syspath, "virtual"))
			return NULL;

		if (serial != NULL) {
			hald_compute_udi (udi, sizeof (udi),
					  "/org/freedesktop/Hal/devices/storage_serial_%s",
					  serial);
		} else if ((model != NULL) && (strlen(model) != 0) ) {
			hald_compute_udi (udi, sizeof (udi),
					  "/org/freedesktop/Hal/devices/storage_model_%s",
					  model);
		} else if ((bus != NULL) && (type != NULL)){
			hald_compute_udi (udi, sizeof (udi),
					  "%s_storage_%s_%s",
					  udi_helper_compute_parent_udi (d),
					  bus, type);
		} else if ((strstr(syspath, "virtual") != NULL) && (udev_device_has_sysattr(d, "dev"))) {
			hald_compute_udi (udi, sizeof (udi),
					  "%s_storage_%s_%s",
					  uid != NULL ? uid : "/org/freedesktop/Hal/devices/virtual",
					  bus != NULL ? bus : "unknown",
					  udev_device_get_sysattr_value (d, "dev"));
		} else {
			hald_compute_udi (udi, sizeof (udi),
					  "%s_storage",
					  uid != NULL ? uid : udi_helper_compute_parent_udi (d));
		}

		if (uuid != NULL && strlen (uuid) > 0) {
			if (udi_helper_add_to_map(udi, syspath)) {
				char syspath_fake[256];
				sprintf(syspath_fake, "%s/fakepath", syspath);
				printf(" >>>>>> %s\n", uuid);
				hald_compute_udi (udi, sizeof (udi),
						  "/org/freedesktop/Hal/devices/volume_uuid_%s", uuid);
				return	udi_helper_add_to_map(udi, syspath_fake);
			}
		}
	}

	return	udi_helper_add_to_map(udi, syspath);
}


const char *
bluetooth_compute_udi (struct udev_device *d)
{
	gchar udi[256];
	const char *syspath = udev_device_get_syspath(d);
	const char *devtype = udev_device_get_devtype(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);

	if ( 0 == strcmp(devtype, "host") && strlen(devtype) == strlen("host") ) {
		hald_compute_udi (udi, sizeof (udi),
//				  "%s_bluetooth_hci_%0llx",
				  "%s_bluetooth_hci",
				  uid
				  );
	}

/*
	if (hal_device_has_capability (d, "bluetooth_acl")) {
		hald_compute_udi (udi, sizeof (udi),
                                  "/org/freedesktop/Hal/devices/bluetooth_acl_%0llx",
                                  hal_device_property_get_uint64 (d, "bluetooth_acl.address"));
	} else if (hal_device_has_capability (d, "bluetooth_sco")) {
		hald_compute_udi (udi, sizeof (udi),
                                  "/org/freedesktop/Hal/devices/bluetooth_acl_%0llx",
                                  hal_device_property_get_uint64 (d, "bluetooth_acl.address"));
	}
*/
	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
ccw_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/ccw_%s",
			  hal_device_property_get_string
			  (d, "ccw.bus_id"));
	hal_device_set_udi (d, udi);
	return TRUE;

}
*/
/*static gboolean
ccwgroup_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/ccwgroup_%s",
			  hal_device_property_get_string
			  (d, "ccwgroup.bus_id"));
	hal_device_set_udi (d, udi);
	return TRUE;

}
*/
/*
static gboolean
drm_compute_udi (HalDevice *d)
{
	gchar udi[256];
	const char *dir;
	const char *name;

	dir = hal_device_property_get_string (d, "linux.sysfs_path");

	name = hal_util_get_last_element(dir);

	// generate e.g.: /org/freedesktop/Hal/devices/pci_8086_2a02_drm_i915_card0
	hald_compute_udi (udi, sizeof (udi),
			  "%s_drm_%s_%s",
			  hal_device_property_get_string (d, "info.parent"),
			  hal_device_property_get_string (d, "drm.dri_library"),
			  name);

	hal_device_set_udi (d, udi);

	return TRUE;
}
*/

const char *
dvb_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
/*
	const char *uid = udi_helper_compute_parent_udi(d);
	printf(" ****** %s\n", uid);
*/

//	const char *adaptor = udev_device_get_property_value(d, "DVB_ADAPTER_NUM");
	const char *device = udev_device_get_property_value(d, "DVB_DEVICE_NUM");
	const char *type = udev_device_get_property_value(d, "DVB_DEVICE_TYPE");

	hald_compute_udi (udi, sizeof (udi), "%s_dvb_%s%s",
			  udi_helper_compute_parent_udi(d),
			  type != NULL ? type : "unknown",
			  device != NULL ? device : "X");

	return	udi_helper_add_to_map(udi, syspath);
}

const char *
firewire_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);

	hald_compute_udi (udi, sizeof (udi), "%s_firewire",
			  udi_helper_compute_parent_udi(d) );


	return	udi_helper_add_to_map(udi, syspath);
}

/*  old
static gboolean
firewire_compute_udi (HalDevice *d)
{
	gchar udi[256];

	if (hal_device_has_capability (d, "ieee1394")) {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/ieee1394_guid%0llx",
				  hal_device_property_get_uint64 (d, "ieee1394.guid"));
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  "%s_unit%d",
				  hal_device_property_get_string (d, "ieee1394_unit.originating_device"),
				  hal_device_property_get_int (d, "ieee1394_unit.unit_index"));
	}

	hal_device_set_udi (d, udi);
	return TRUE;

}
*/

const char *
hid_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	hald_compute_udi (udi, sizeof (udi), "%s_hid",
			  uid);

	return	udi_helper_add_to_map(udi, syspath);
}

const char *
hidraw_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	hald_compute_udi (udi, sizeof (udi), "%s_hidraw",
			  uid);

	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
ibmebus_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hal_util_compute_udi (hald_get_gdl (), udi, sizeof (udi),
			      "/org/freedesktop/Hal/devices/ibmebus%s",
			      hal_device_property_get_string (d, "ibmebus.devspec"));
	hal_device_set_udi (d, udi);
	return TRUE;
}
*/
/*
static gboolean
ide_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "%s_ide_%d_%d",
			  hal_device_property_get_string (d, "info.parent"),
			  hal_device_property_get_int (d, "ide.host"),
			  hal_device_property_get_int (d, "ide.channel"));
	hal_device_set_udi (d, udi);

	return TRUE;

}
*/
/*
static gboolean
ieee1394_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/ieee1394_guid_%0llx",
			  hal_device_property_get_uint64 (d, "ieee1394.guid"));
	hal_device_set_udi (d, udi);
	return TRUE;

}
*/

const char *
input_compute_udi (struct udev_device *d)
{
	gchar udi[256];
	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	struct udev_device *parent = udev_device_get_parent_with_subsystem_devtype(d, "input", NULL);
	if (parent != NULL) {
		return NULL;
	}

	hald_compute_udi (udi, sizeof (udi),
			  "%s_logicaldev_input",
			  uid);

	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
iucv_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/iucv_%s",
			  hal_device_property_get_string
			  (d, "iucv.bus_id"));
	hal_device_set_udi (d, udi);
	return TRUE;

}
*/
/*
static gboolean
leds_compute_udi (HalDevice *d)
{
	gchar udi[256];
	const char *name;
	const char *colour;
	const char *function;

        name = hal_device_property_get_string (d, "leds.device_name");
        colour = hal_device_property_get_string (d, "leds.colour");
        function = hal_device_property_get_string (d, "leds.function");

	if (name && function && colour) {
		hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/leds_%s_%s_%s", name, function, colour);
	} else if (name && function) {
		hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/leds_%s_%s", name, function);
	} else if (name) {
		hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/leds_%s", name);
	} else {
		hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/leds_unknown");
	}

	hal_device_set_udi (d, udi);
	return TRUE;
}
*/
/*
static gboolean
memstick_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hal_util_compute_udi (hald_get_gdl (), udi, sizeof (udi),
			      "%s_memstick_card",
			      hal_device_property_get_string (d, "info.parent"));
	hal_device_set_udi (d, udi);
	return TRUE;

}
*/
/*
static gboolean
memstick_host_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hal_util_compute_udi (hald_get_gdl (), udi, sizeof (udi),
			      "%s_memstick_host",
			      hal_device_property_get_string (d, "info.parent"));
	hal_device_set_udi (d, udi);
	return TRUE;
}
*/

const char *
mmc_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(udev_device_get_parent_with_subsystem_devtype(d, "mmc_host", NULL));
//	printf(" ****** %s\n", uid);

	// udisks: values do not match hal, need to get 'rca' value to match hald. udisks ??
	if (udev_device_has_sysattr(d, "manfid")) {
		hald_compute_udi (udi, sizeof (udi),
//				  "%s_mmc_card_rca%d",
				  "%s_mmc_card_%s_%s_%s_%s",
				  uid != NULL ? uid : udi_helper_compute_parent_udi(d),
				  udev_device_get_sysattr_value(d, "manfid"),
				  udev_device_get_sysattr_value(d, "oemid"),
				  udev_device_get_sysattr_value(d, "type"),
				  udev_device_get_sysattr_value(d, "serial"));
	} else if (udev_device_has_sysattr(d, "type")) {
		hald_compute_udi (udi, sizeof (udi),
//				  "%s_mmc_card_rca%d",
				  "%s_mmc_card_%s",
				  uid != NULL ? uid : udi_helper_compute_parent_udi(d),
				  udev_device_get_sysattr_value(d, "type"));
	}

	return	udi_helper_add_to_map(udi, syspath);
}


const char *
mmc_host_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	hald_compute_udi (udi, sizeof (udi),
			  "%s_mmc_host",
			  uid);

	return	udi_helper_add_to_map(udi, syspath);
}


const char *
net_compute_udi (struct udev_device *d)
{
	gchar udi[256];
	const gchar *id;
	gboolean id_only = TRUE;

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	id = udev_device_get_sysattr_value (d, "address");

	if (id == NULL || (strcmp (id, "00:00:00:00:00:00") == 0)) {
		// Need to fall back to something else if mac not available.
		if (!udev_device_get_parent(d)) {
			hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/net_computer_loopback");
			id_only = FALSE;
		}
	}

	if (id_only)
		hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/net_%s", id);

	return	udi_helper_add_to_map(udi, syspath);
}

/*
const char *
of_platform_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/of_platform_%s",
			 hal_device_property_get_string (d, "of_platform.id"));
	hal_device_set_udi (d, udi);

	return TRUE;
}
*/

const char *
pci_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);

/*
	const char *uid = udi_helper_compute_parent_udi(d);
	printf(" ****** %s\n", uid);
*/
	const char *vendor = udev_device_get_sysattr_value (d, "vendor");
	const char *device = udev_device_get_sysattr_value (d, "device");
	int int_vendor = strtol(vendor, NULL, 16);
	int int_device = strtol(device, NULL, 16);

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/pci_%x_%x",
			  int_vendor,
			  int_device);

	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
pcmcia_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/pcmcia_%d_%d",
			  hal_device_property_get_int (d, "pcmcia.manfid1"),
			  hal_device_property_get_int (d, "pcmcia.manfid2"));
	hal_device_set_udi (d, udi);
	return TRUE;

}
*/

const char *
platform_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);

/*
	const char *uid = udi_helper_compute_parent_udi(d);
	printf(" ****** %s\n", uid);
*/
	char *path = strdup(syspath);
	char *basepath = basename(path);

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/platform_%s",
			  basepath);

	g_free(path);

	return	udi_helper_add_to_map(udi, syspath);
}


const char *
pnp_compute_udi (struct udev_device *d)
{
	gchar udi[256];
//	char key[256];

	const char *id =  udev_device_get_sysattr_value (d, "id");
	const char *syspath = udev_device_get_syspath(d);

//	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	hald_compute_udi (udi, sizeof (udi),
		  "/org/freedesktop/Hal/devices/pnp_%s",
		  id);

//	printf(" PNP_COMPUTE-1.0: %s -- %s\n", udi, syspath);

	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
power_supply_compute_udi (HalDevice *d)
{
	gchar udi[256];
	const char *dir;
	const char *name;

	dir = hal_device_property_get_string (d, "linux.sysfs_path");

	name = hal_util_get_last_element(dir);
	if (name)
		hald_compute_udi (udi, sizeof (udi),
				  "%s_power_supply_%s_%s",
				  hal_device_property_get_string (d, "info.parent"),
				  hal_device_property_get_string (d, "info.category"),
				  name);
	else
		hald_compute_udi (udi, sizeof (udi),
				  "%s_power_supply_%s",
				  hal_device_property_get_string (d, "info.parent"),
				  hal_device_property_get_string (d, "info.category"));

	hal_device_set_udi (d, udi);
	return TRUE;
}
*/
/*
static gboolean
ppdev_compute_udi (HalDevice *d)
{
	gchar udi[256];
	const char *name;

	name = hal_util_get_last_element( hal_device_property_get_string(d, "linux.device_file"));

	if (name) {
		hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/ppdev_%s", name);
	} else {
		hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/ppdev");
	}

	hal_device_set_udi (d, udi);

	return TRUE;
}
*/
/*
static gboolean
pseudo_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/pseudo",
			  hal_device_property_get_string (d, "platform.id"));
	hal_device_set_udi (d, udi);

	return TRUE;

}
*/
/*
static gboolean
rfkill_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "%s_rfkill_%s_%s",
			  hal_device_property_get_string (d, "info.parent"),
			  hal_device_property_get_string (d, "killswitch.name"),
			  hal_device_property_get_string (d, "killswitch.type"));
	hal_device_set_udi (d, udi);
	return TRUE;
}
*/

const char *scsi_device_compute_udi (struct udev_device *d);
const char *scsi_host_compute_udi (struct udev_device *d);
const char *scsi_target_compute_udi (struct udev_device *d);

const char *
scsi_compute_udi (struct udev_device *d)
{
	const char *devtype = udev_device_get_property_value(d, "DEVTYPE");
	if (strcmp(devtype, "scsi_host") == 0) {
		return scsi_host_compute_udi(d);
	}
	else if (strcmp(devtype, "scsi_target")) {
		return scsi_target_compute_udi(d);
	}
	else if (strcmp(devtype, "scsi_device")) {
		return scsi_device_compute_udi(d);
	}

	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	char *path = strdup(syspath);
	char *basepath = basename(path);

	if (uid != NULL && udev_device_has_sysattr(d, "unique_id")) {
		hald_compute_udi (udi, sizeof (udi),
				  "%s_scsi_device_lun%s",
				  uid,
				  udev_device_get_sysattr_value (d, "unique_id")); // scsi.lun ??
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  uid != NULL ? uid : "/org/freedesktop/Hal/devices/scsi_%s",
				  basepath);
	}

	g_free(path);
	return	udi_helper_add_to_map(udi, syspath);
}


const char *
scsi_device_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	hald_compute_udi (udi, sizeof (udi),
			  "%s_scsi_device",
			  uid);

	return	udi_helper_add_to_map(udi, syspath);
}

const char *
scsi_generic_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	hald_compute_udi (udi, sizeof (udi),
			  "%s_scsi_generic",
			  uid);

	return	udi_helper_add_to_map(udi, syspath);
}

const char *
scsi_host_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *devtype = udev_device_get_devtype(d);
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

//	const char *unique_id = udev_device_get_sysattr_value (d, "unique_id");

	if (!uid)
		uid = "/org/freedesktop/Hal/devices/computer";

/*
	if (devtype == NULL && unique_id != NULL && !strstr(uid, "scsi_host_")) {
		int int_id = strtol(unique_id, NULL, 10) - 1;
		hald_compute_udi (udi, sizeof (udi),
				  "%s_%i_scsi_host",
				  uid,
				  int_id);
	} else
*/
	if (devtype != NULL && (uid && !strstr(uid, "_if"))) {
		const char *syspath = udev_device_get_syspath(d);
		char shost[2];
		shost[0] = syspath[strlen(syspath)-1];
		shost[1] = '\0';
		int int_host = strtol ( shost, NULL, 10);

		hald_compute_udi (udi, sizeof (udi),
				  "%s_scsi_host_%i",
				  uid,
				  int_host);
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  "%s_scsi_host",
				  uid);
	}

	return	udi_helper_add_to_map(udi, syspath);
}

const char *
scsi_target_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	char slun[2];
	slun[0] = syspath[strlen(syspath)-1];
	slun[1] = '\0';
	int lun = strtol ( slun, NULL, 10);

	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	hald_compute_udi (udi, sizeof (udi),
			  "%s_lun%i",
			  uid,
			  lun);

	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
sdio_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "%s_sdio%d",
			  hal_device_property_get_string (d, "info.parent"),
			  hal_device_property_get_int (d, "sdio.card_id"));
	hal_device_set_udi (d, udi);
	return TRUE;
}
*/

const char *
serial_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	struct udev_device *dev2;
	dev2 = udev_device_get_parent_with_subsystem_devtype(
                       d,
                       "usb",
                       "usb_device");
	const char *uid;
	if (dev2)
		uid = udi_helper_compute_udi(dev2);
	else
		uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	const char *port;

	if (udev_device_has_sysattr(d, "port_number"))
		port = udev_device_get_sysattr_value (d, "port_number");
	else if (udev_device_has_sysattr(d, "line"))
		port = udev_device_get_sysattr_value (d, "line");
	else if (udev_device_has_sysattr (udev_device_get_parent(d), "port_number"))
		port = udev_device_get_sysattr_value (udev_device_get_parent(d), "port_number");
	else if (udev_device_has_sysattr (udev_device_get_parent(d), "line"))
		port = udev_device_get_sysattr_value (udev_device_get_parent(d), "line");
	else if (udev_device_has_property(d, "MINOR"))
		port = udev_device_get_property_value (d, "MINOR");
	else // if (udev_device_has_sysattr(d, "dev"))
		port = udev_device_get_sysattr_value (d, "dev");


	const char *bus;
	bus = udev_device_get_property_value (d, "ID_BUS"); // should be BUS (ie, usb, ..)
	if (bus == NULL) {
		bus = udev_device_get_sysattr_value(d, "iomem_base");
	}
	// do not report virtual devices as real hardware
	if (strstr(syspath, "virtual"))
		return NULL;

	const char *iface = udev_device_get_property_value (d, "ID_USB_INTERFACE_NUM");

	if (iface != NULL && dev2) {
		int int_iface = strtol(iface, NULL, 10);
		hald_compute_udi (udi, sizeof (udi),
//				  "%s_serial_%s_%d",
				  "%s_if%d_serial_%s_%s",
				  uid,
				  int_iface,
				  bus != NULL ? bus : "unknown",
				  port != NULL ? port : "0");
	} else if (dev2) {
		hald_compute_udi (udi, sizeof (udi),
//				  "%s_serial_%s_%d",
				  "%s_serial_%s_%s",
				  uid,
				  bus != NULL ? bus : "unknown",
				  port != NULL ? port : "0");
	} else if (strstr(syspath, "virtual") != NULL) {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/virtual_serial_%s",
				  port != NULL ? port : "0");
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/serial_%s_%s",
				  bus != NULL ? bus : "unknown",
				  port != NULL ? port : "0");
	}

	return	udi_helper_add_to_map_inc(udi, syspath, 0);
}

/* ************* old
static gboolean
serial_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "%s_serial_%s_%d",
			  hal_device_property_get_string (d, "info.parent"),
			  hal_device_property_get_string (d, "serial.type"),
			  hal_device_property_get_int (d, "serial.port"));
	hal_device_set_udi (d, udi);

	return TRUE;
}
*/

const char *
serio_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	const char *value;
	if (udev_device_has_sysattr(d, "description")) {
		value = udev_device_get_sysattr_value(d, "description");
	} else if (udev_device_has_sysattr(d, "driver")) {
		value = udev_device_get_sysattr_value(d, "driver");
	} else {
		value = udev_device_get_property_value(d, "SERIO_TYPE");
	}

	if (uid != NULL) {
		hald_compute_udi (udi, sizeof (udi),
				  "%s_%s",
				  uid,
				  value);
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/serio_%s",
				  value);
	}

	return	udi_helper_add_to_map(udi, syspath);
}


const char *
sound_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	if (udev_device_has_sysattr(d, "number")) {
		const char *number = udev_device_get_sysattr_value (d, "number");
		int int_number = strtol(number, NULL, 10);

		// don't include card number as it may not be persistent across reboots
		hald_compute_udi (udi, sizeof (udi),
				  "%s_sound_card_%i",
				  uid,
				  int_number);
	} else {
		const char *syspath = udev_device_get_syspath(d);
		char *path = strdup(syspath);
		char *basepath = basename(path);

		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/sound_%s",
				  basepath);
		g_free(path);
	}

/*
	if (udev_device_has_sysattr(d, "id")) {
		// don't include card number as it may not be persistent across reboots
		hald_compute_udi (udi, sizeof (udi),
//				  "%s_sound_card_%i",
				  "%s_sound_card_%s",
				  parent_udi,
				  udev_device_get_sysattr_value (d, "id"));
	} else if (hal_device_has_property(d, "alsa.card")) {
		// don't include card number as it may not be persistent across reboots
		hald_compute_udi (udi, sizeof (udi),
				  "%s_alsa_%s_%i",
				  parent,
				  hal_device_property_get_string (d, "alsa.type"),
				  hal_device_property_get_int (d, "alsa.device"));
	} else if (hal_device_has_property(d, "oss.card")) {
		// don't include card number as it may not be persistent across reboots
		hald_compute_udi (udi, sizeof (udi),
				  "%s_oss_%s_%i",
				  parent,
				  hal_device_property_get_string (d, "oss.type"),
				  hal_device_property_get_int (d, "oss.device"));
	} else if (hal_device_has_property(d, "alsa.type")) {
		// handle global ALSA devices
		hald_compute_udi (udi, sizeof (udi),
				  "%s_alsa_%s",
				  parent,
				  hal_device_property_get_string (d, "alsa.type"));
	} else if (hal_device_has_property(d, "oss.type")) {
		// handle global OSS devices
		hald_compute_udi (udi, sizeof (udi),
				  "%s_oss_%s",
				  info.parent,
				  hal_device_property_get_string (d, "oss.type"));
	} else {
		// fallback
		hald_compute_udi (udi, sizeof (udi), "%s_sound_unknown",
				  parent_udi);
	}
*/
	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
ssb_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hal_util_compute_udi (hald_get_gdl (), udi, sizeof (udi),
			      "/org/freedesktop/Hal/devices/ssb_%s",
			      hal_device_property_get_string (d, "ssb.bus_id"));
	hal_device_set_udi (d, udi);
	return TRUE;
}
*/
/*
static gboolean
tape_compute_udi (HalDevice *d)
{
	gchar udi[256];
	const gchar *sysfs_name;

	sysfs_name = hal_util_get_last_element (hal_device_property_get_string
						(d, "linux.sysfs_path"));
	if (!sysfs_name)
		return FALSE;
	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/tape_%s",
			  sysfs_name);
	hal_device_set_udi (d, udi);

	return TRUE;
}
*/

const char *
usb_compute_udi (struct udev_device *d)
{
	gchar udi[256];
	const char *uid;

	const char *syspath = udev_device_get_syspath(d);

/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	struct udev_device *dev2 = udev_device_get_parent_with_subsystem_devtype(
                       d,
                       "usb",
                       "usb_device");
	if (dev2)
		uid = udi_helper_compute_udi(dev2);
	else
		uid = udi_helper_compute_parent_udi(d);
//	printf(" ******> %s\n", uid);


	if (udev_device_has_property(d, "ID_USB_INTERFACE_NUM")) {
		const char *iface = udev_device_get_property_value (d, "ID_USB_INTERFACE_NUM");
		int int_iface = strtol(iface, NULL, 10);

		hald_compute_udi (udi, sizeof (udi),
				  "%s_if%i",
				  uid,
				  int_iface);
	} else if (udev_device_has_sysattr(d, "bInterfaceNumber")) {
		const char *iface = udev_device_get_sysattr_value (d, "bInterfaceNumber");
		int int_iface = strtol(iface, NULL, 10);

		hald_compute_udi (udi, sizeof (udi),
				  "%s_if%i",
				  uid,
				  int_iface);
	} else if (udev_device_get_devtype(d) != NULL) {
		const char *s_vendor = NULL;
		const char *s_product = NULL;
		const char *serial = NULL;

		if (udev_device_has_sysattr(d, "idVendor")) {
			s_vendor = udev_device_get_sysattr_value(d,"idVendor");
			s_product = udev_device_get_sysattr_value(d,"idProduct");
			serial = udev_device_get_sysattr_value(d, "serial");
			if (serial == NULL)
				serial = udev_device_get_property_value(d, "ID_SERIAL");
		} else if (dev2) {
			s_vendor = udev_device_get_sysattr_value(dev2,"idVendor");
			s_product = udev_device_get_sysattr_value(dev2,"idProduct");
			serial = udev_device_get_sysattr_value(dev2, "serial");
			if (serial == NULL)
				serial = udev_device_get_property_value(dev2, "ID_SERIAL");
		}


		int vendor = strtol(s_vendor, NULL, 16);
		int product = strtol(s_product, NULL, 16);

		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/usb_device_%x_%x_%s",
				  vendor,
				  product,
				  serial != NULL ? serial : "noserial");
	} else {
		return NULL;
	}

	return	udi_helper_add_to_map_inc(udi, syspath, 0);
}

/*
static gboolean
usbclass_compute_udi (HalDevice *d)
{
	gchar udi[256];

	if (hal_device_has_capability (d, "hiddev")) {
		hald_compute_udi (udi, sizeof (udi),
				  "%s_hiddev",
				  hal_device_property_get_string (d, "info.parent"));
		hal_device_set_udi (d, udi);
	} else if (hal_device_has_capability (d, "printer")) {
		const char *serial;

		serial = hal_device_property_get_string (d, "printer.serial");
		hald_compute_udi (udi, sizeof (udi),
				  "%s_printer_%s",
				  hal_device_property_get_string (d, "info.parent"),
				  serial != NULL ? serial : "noserial");
		hal_device_set_udi (d, udi);
	}

	return TRUE;
}
*/
/*
static gboolean
usbraw_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi), "%s_usbraw",
			  hal_device_property_get_string (d, "info.parent"));
	hal_device_set_udi (d, udi);

	return TRUE;
}
*/

const char *
video4linux_compute_udi (struct udev_device *d)
{
	gchar udi[256];

	const char *syspath = udev_device_get_syspath(d);
/*
	const char *udi_from_map = udi_helper_get_udi_by_syspath(syspath);
	if (udi_from_map != NULL)
		return udi_helper_add_to_map(udi_from_map, syspath);
*/
	const char *uid = udi_helper_compute_parent_udi(d);
//	printf(" ****** %s\n", uid);

	const char *s_index = udev_device_get_sysattr_value(d, "index");
	int index = strtol(s_index, NULL, 10);

	if (--index >= 0) {
		hald_compute_udi (udi, sizeof (udi), "%s_video4linux_%i",
				  uid,
				  index);
	} else {

		hald_compute_udi (udi, sizeof (udi), "%s_video4linux",
				  uid);
	}

	return	udi_helper_add_to_map(udi, syspath);
}

/*
static gboolean
vio_compute_udi (HalDevice *d)
{
	gchar udi[256];
	const char *type;

	type = hal_device_property_get_string (d, "vio.type");

	if (type) {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/vio_%s_%s",
				  type,
				  hal_device_property_get_string (d, "vio.id"));
	} else {
		hald_compute_udi (udi, sizeof (udi),
				  "/org/freedesktop/Hal/devices/vio_%s",
				  hal_device_property_get_string (d, "vio.id"));
	}

	hal_device_set_udi (d, udi);

	return TRUE;
}
*/
/*
static gboolean
virtio_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/virtio_%s",
			  hal_device_property_get_string (d, "virtio.id"));
	hal_device_set_udi (d, udi);

	return TRUE;
}
*/
/*
static gboolean
vmbus_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi), "/org/freedesktop/Hal/devices/_%s",
			 hal_device_property_get_string (d, "vmbus.bus_id"));
	hal_device_set_udi (d, udi);

	return TRUE;
}
*/
/*
static gboolean
xen_compute_udi (HalDevice *d)
{
	gchar udi[256];

	hald_compute_udi (udi, sizeof (udi),
			  "/org/freedesktop/Hal/devices/xen_%s",
			  hal_device_property_get_string (d, "xen.bus_id"));
	hal_device_set_udi (d, udi);
	return TRUE;
}
*/

/*--------------------------------------------------------------------------------------------------------------*/
struct DevHandler_s;
typedef struct DevHandler_s DevHandler;

struct DevHandler_s
{
	const gchar *subsystem;
//	HalDevice *(*add) (const gchar *sysfs_path, const gchar *device_file,
//			   HalDevice *parent_dev, const gchar *parent_path);
//	const gchar *(*get_prober)(HalDevice *d);
//	gboolean (*post_probing) (HalDevice *d);
	const char *(*compute_udi) (struct udev_device *d);
//	gboolean (*refresh) (HalDevice *d);
//	gboolean (*remove) (HalDevice *d);
};

/*--------------------------------------------------------------------------------------------------------------*/
/* 		 	PLEASE KEEP THE SUBSYSTEMS IN ALPHABETICAL ORDER !!!					*/
/*--------------------------------------------------------------------------------------------------------------*/

static DevHandler dev_handler_acpi =
{
	.subsystem    = "acpi",
//	.add          = acpi_add,
	.compute_udi  = acpi_compute_udi,
//	.remove       = dev_remove
};

static DevHandler dev_handler_amba =
{
	.subsystem    = "amba",
//	.add          = acpi_add,
	.compute_udi  = amba_compute_udi,
//	.remove       = dev_remove
};

/*
static DevHandler dev_handler_backlight =
{
	.subsystem    = "backlight",
	.add          = backlight_add,
	.compute_udi  = backlight_compute_udi,
	.remove       = dev_remove
};
*/

static DevHandler dev_handler_blockdev =
{
	.subsystem    = "block",
//	.add          = blockdev_add,
	.compute_udi  = blockdev_compute_udi,
//	.remove       = dev_remove
};

static DevHandler dev_handler_bluetooth =
{
	.subsystem    = "bluetooth",
//	.add          = bluetooth_add,
//	.get_prober   = NULL,
//	.post_probing = NULL,
	.compute_udi  = bluetooth_compute_udi,
//	.remove       = dev_remove
};

/*
// s390 specific busses
static DevHandler dev_handler_ccw = {
	.subsystem   = "ccw",
	.add         = ccw_add,
	.compute_udi = ccw_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_ccwgroup = {
	.subsystem   = "ccwgroup",
	.add         = ccwgroup_add,
	.compute_udi = ccwgroup_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_drm =
{
       .subsystem    = "drm",
       .add          = drm_add,
       .compute_udi  = drm_compute_udi,
       .remove       = dev_remove
};
*/

static DevHandler dev_handler_dvb =
{
	.subsystem    = "dvb",
//	.add          = dvb_add,
//	.get_prober   = NULL,
//	.post_probing = NULL,
	.compute_udi  = dvb_compute_udi,
//	.remove       = dev_remove
};


// krh's new firewire stack
static DevHandler dev_handler_firewire = {
	.subsystem    = "firewire",
//	.add          = firewire_add,
//	.get_prober   = firewire_get_prober,
//	.post_probing = firewire_post_probing,
	.compute_udi  = firewire_compute_udi,
//	.remove       = dev_remove
};


static DevHandler dev_handler_hid = {
	.subsystem   = "hid",
//	.add         = hidraw_add,
	.compute_udi = hid_compute_udi,
//	.remove      = dev_remove
};

static DevHandler dev_handler_hidraw = {
	.subsystem   = "hidraw",
//	.add         = hidraw_add,
	.compute_udi = hidraw_compute_udi,
//	.remove      = dev_remove
};

/*
static DevHandler dev_handler_ibmebus = {
	.subsystem   = "ibmebus",
	.add         = ibmebus_add,
	.compute_udi = ibmebus_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_ide = {
	.subsystem   = "ide",
	.add         = ide_add,
	.compute_udi = ide_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_ieee1394 = {
	.subsystem   = "ieee1394",
	.add         = ieee1394_add,
	.compute_udi = ieee1394_compute_udi,
	.remove      = dev_remove
};
*/

static DevHandler dev_handler_input =
{
	.subsystem    = "input",
//	.add          = input_add,
//	.get_prober   = input_get_prober,
//	.post_probing = input_post_probing,
	.compute_udi  = input_compute_udi,
//	.remove       = dev_remove
};

/*
static DevHandler dev_handler_iucv = {
	.subsystem   = "iucv",
	.add         = iucv_add,
	.compute_udi = iucv_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_leds = {
	.subsystem   = "leds",
	.add         = leds_add,
	.compute_udi = leds_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_memstick = {
	.subsystem   = "memstick",
	.add         = memstick_add,
	.compute_udi = memstick_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_memstick_host =
{
	.subsystem    = "memstick_host",
	.add          = memstick_host_add,
	.get_prober   = NULL,
	.post_probing = NULL,
	.compute_udi  = memstick_host_compute_udi,
	.remove       = dev_remove
};
*/

static DevHandler dev_handler_mmc = {
	.subsystem   = "mmc",
//	.add         = mmc_add,
	.compute_udi = mmc_compute_udi,
//	.remove      = dev_remove
};

static DevHandler dev_handler_mmc_host =
{
	.subsystem    = "mmc_host",
//	.add          = mmc_host_add,
//	.get_prober   = NULL,
//	.post_probing = NULL,
	.compute_udi  = mmc_host_compute_udi,
//	.remove       = dev_remove
};

static DevHandler dev_handler_net =
{
	.subsystem    = "net",
//	.add          = net_add,
//	.refresh      = net_refresh,
//	.get_prober   = net_get_prober,
//	.post_probing = net_post_probing,
	.compute_udi  = net_compute_udi,
//	.remove       = dev_remove
};

/*
static DevHandler dev_handler_of_platform =
{
	.subsystem   = "of_platform",
	.add         = of_platform_add,
	.compute_udi = of_platform_compute_udi,
	.remove      = dev_remove
};
*/

static DevHandler dev_handler_pci = {
	.subsystem   = "pci",
//	.add         = pci_add,
	.compute_udi = pci_compute_udi,
//	.remove      = dev_remove
};

/*
static DevHandler dev_handler_pcmcia = {
	.subsystem   = "pcmcia",
	.add         = pcmcia_add,
	.compute_udi = pcmcia_compute_udi,
	.remove      = dev_remove
};
*/

static DevHandler dev_handler_platform = {
	.subsystem   = "platform",
//	.add         = platform_add,
//	.refresh     = platform_refresh,
	.compute_udi = platform_compute_udi,
//	.remove      = dev_remove
};

static DevHandler dev_handler_pnp = {
	.subsystem   = "pnp",
//	.add         = pnp_add,
	.compute_udi = pnp_compute_udi,
//	.remove      = dev_remove
};

/*
static DevHandler dev_handler_ppdev = {
	.subsystem   = "ppdev",
	.add         = ppdev_add,
	.compute_udi = ppdev_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_ps3_system_bus =
{
	.subsystem   = "ps3_system_bus",
	.add         = ps3_system_bus_add,
	.compute_udi = ps3_system_bus_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_power_supply =
{
       .subsystem    = "power_supply",
       .add          = power_supply_add,
       .refresh      = power_supply_refresh,
       .compute_udi  = power_supply_compute_udi,
       .remove       = dev_remove
};

static DevHandler dev_handler_rfkill =
{
       .subsystem    = "rfkill",
       .add          = rfkill_add,
       .compute_udi  = rfkill_compute_udi,
       .refresh      = rfkill_refresh,
       .remove       = dev_remove
};

*/
static DevHandler dev_handler_scsi = {
	.subsystem   = "scsi",
//	.add         = scsi_add,
	.compute_udi = scsi_compute_udi,
//	.remove      = dev_remove
};

static DevHandler dev_handler_scsi_generic =
{
	.subsystem    = "scsi_generic",
//	.add          = scsi_generic_add,
//	.get_prober   = NULL,
//	.post_probing = NULL,
	.compute_udi  = scsi_generic_compute_udi,
//	.remove       = dev_remove
};

static DevHandler dev_handler_scsi_host =
{
	.subsystem    = "scsi_host",
//	.add          = scsi_host_add,
//	.get_prober   = NULL,
//	.post_probing = NULL,
	.compute_udi  = scsi_host_compute_udi,
//	.remove       = dev_remove
};

/*
static DevHandler dev_handler_sdio = {
	.subsystem   = "sdio",
	.add         = sdio_add,
	.compute_udi = sdio_compute_udi,
	.remove      = dev_remove
};
*/
static DevHandler dev_handler_serial =
{
	.subsystem    = "tty",
//	.add          = serial_add,
//	.get_prober   = serial_get_prober,
//	.post_probing = NULL,
	.compute_udi  = serial_compute_udi,
//	.remove       = dev_remove
};

static DevHandler dev_handler_serio = {
	.subsystem   = "serio",
//	.add         = serio_add,
	.compute_udi = serio_compute_udi,
//	.remove      = dev_remove
};

static DevHandler dev_handler_sound =
{
	.subsystem    = "sound",
//	.add          = sound_add,
//	.get_prober   = NULL,
//	.post_probing = NULL,
	.compute_udi  = sound_compute_udi,
//	.remove       = dev_remove
};

/*
static DevHandler dev_handler_ssb = {
	.subsystem   = "ssb",
	.add         = ssb_add,
	.compute_udi = ssb_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_tape =
{
	.subsystem    = "tape",
	.add          = tape_add,
	.get_prober   = NULL,
	.post_probing = NULL,
	.compute_udi  = tape_compute_udi,
	.remove       = dev_remove
};

static DevHandler dev_handler_tape390 =
{
	.subsystem    = "tape390",
	.add          = tape_add,
	.get_prober   = NULL,
	.post_probing = NULL,
	.compute_udi  = tape_compute_udi,
	.remove       = dev_remove
};
*/

static DevHandler dev_handler_usb = {
	.subsystem   = "usb",
//	.add         = usb_add,
	.compute_udi = usb_compute_udi,
//	.remove      = dev_remove
};

//static DevHandler dev_handler_usb_serial = {
//	.subsystem   = "usb-serial",
//	.add         = usb_add,
//	.compute_udi = usb-serial_compute_udi,
//	.remove      = dev_remove
//};

/*
static DevHandler dev_handler_usbclass =
{
	.subsystem    = "usb",
	.add          = usbclass_add,
	.get_prober   = usbclass_get_prober,
	.post_probing = NULL,
	.compute_udi  = usbclass_compute_udi,
	.remove       = dev_remove
};

static DevHandler dev_handler_usbraw =
{
	.subsystem    = "usb_device",
	.add          = usbraw_add,
	.get_prober   = NULL,
	.post_probing = NULL,
	.compute_udi  = usbraw_compute_udi,
	.remove       = dev_remove
};
*/

static DevHandler dev_handler_video4linux =
{
	.subsystem    = "video4linux",
//	.add          = video4linux_add,
//	.get_prober   = video4linux_get_prober,
//	.post_probing = NULL,
	.compute_udi  = video4linux_compute_udi,
//	.remove       = dev_remove
};

/*
static DevHandler dev_handler_vio =
{
	.subsystem   = "vio",
	.add         = vio_add,
	.compute_udi = vio_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_virtio =
{
	.subsystem   = "virtio",
	.add         = virtio_add,
	.compute_udi = virtio_compute_udi,
	.remove      = dev_remove
};

static DevHandler dev_handler_vmbus =
{
	.subsystem   = "vmbus",
	.add         = vmbus_add,
	.compute_udi = vmbus_compute_udi,
	.remove      = dev_remove
};


static DevHandler dev_handler_xen = {
	.subsystem   = "xen",
	.add         = xen_add,
	.compute_udi = xen_compute_udi,
	.remove      = dev_remove
};
*/

/*--------------------------------------------------------------------------------------------------------------*/
/* 		 	PLEASE KEEP THE SUBSYSTEMS IN ALPHABETICAL ORDER !!!					*/
/*--------------------------------------------------------------------------------------------------------------*/

static DevHandler *dev_handlers[] = {
	&dev_handler_acpi,
	&dev_handler_amba,
//	&dev_handler_backlight,
	&dev_handler_blockdev,
	&dev_handler_bluetooth,
//	&dev_handler_ccw,
//	&dev_handler_ccwgroup,
//	&dev_handler_drm,
	&dev_handler_dvb,
	&dev_handler_firewire,
	&dev_handler_hid,
	&dev_handler_hidraw,
//	&dev_handler_ibmebus,
//	&dev_handler_ide,
//	&dev_handler_ieee1394,
	&dev_handler_input,
//	&dev_handler_iucv,
//	&dev_handler_leds,
	&dev_handler_mmc,
//	&dev_handler_memstick,
//	&dev_handler_memstick_host,
	&dev_handler_mmc_host,
	&dev_handler_net,
//	&dev_handler_of_platform,
	&dev_handler_pci,
//	&dev_handler_pcmcia,
	&dev_handler_platform,
	&dev_handler_pnp,
//	&dev_handler_power_supply,
//	&dev_handler_ppdev,
//	&dev_handler_ps3_system_bus,
//	&dev_handler_pseudo,
//	&dev_handler_rfkill,
	&dev_handler_scsi,
	&dev_handler_scsi_generic,
	&dev_handler_scsi_host,
//	&dev_handler_sdio,
	&dev_handler_serial,
	&dev_handler_serio,
	&dev_handler_sound,
//	&dev_handler_ssb,
//	&dev_handler_tape,
//	&dev_handler_tape390,
	// Don't change order of usbclass and usb
//	&dev_handler_usbclass,
	&dev_handler_usb,
//	&dev_handler_usb_serial,
	// --------------------------------------
//	&dev_handler_usbraw,
	&dev_handler_video4linux,
//	&dev_handler_vio,
//	&dev_handler_virtio,
//	&dev_handler_vmbus,
//	&dev_handler_xen,

	NULL
};

/*--------------------------------------------------------------------------------------------------------------*/


const char *
udi_helper_compute_udi (struct udev_device *device)
{
	guint i;

	const char *subsystem = udev_device_get_subsystem(device);
	if (subsystem != NULL)
		for (i = 0; dev_handlers [i] != NULL; i++) {
			DevHandler *handler;

			handler = dev_handlers[i];
			if (strcmp (handler->subsystem, subsystem) == 0) {
				//printf("HANDLER FOUND FOR :%s:\n", subsystem);
				return (handler->compute_udi(device));
			}
		}
	//printf("HANDLER **NOT** FOUND FOR :%s:\n", subsystem);
	return NULL;
}

const char *
udi_helper_compute_parent_udi (struct udev_device *device)
{
	struct udev_device *d = udev_device_get_parent(device);

	if (d)
		 return (udi_helper_compute_udi (d));

	return "/org/freedesktop/Hal/devices/computer";
}

