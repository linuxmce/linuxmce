/***************************************************************************
 * Linux kernel device handling
 *
 * Copyright (C) 2004 David Zeuthen, <david@fubar.dk>
 * Copyright (C) 2006 Kay Sievers <kay.sievers@novell.com>
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UDI_HELPER_H
#define UDI_HELPER_H

#include <glib.h>

const char *udi_helper_compute_udi (struct udev_device *d);
const char *udi_helper_compute_parent_udi (struct udev_device *d);
const char *udi_helper_map_get_udev_syspath (const char *udi);
const char *udi_helper_get_key_by_value(const char *syspath);

int udi_helper_remove_device(const char *udi, const char *syspath);
int udi_helper_map_table_new();

gboolean udev_device_has_property (struct udev_device *device, const char *name);
gboolean udev_device_has_sysattr (struct udev_device *device, const char *name);



/*
#include "hotplug.h"

typedef enum {
	OSS_DEVICE_TYPE_DSP,
	OSS_DEVICE_TYPE_ADSP,
	OSS_DEVICE_TYPE_MIDI,
	OSS_DEVICE_TYPE_AMIDI,
	OSS_DEVICE_TYPE_AUDIO,
	OSS_DEVICE_TYPE_MIXER,
	OSS_DEVICE_TYPE_UNKNOWN
} ClassDevOSSDeviceTypes;

void hotplug_event_begin_add_dev (const gchar *subsystem, const gchar *sysfs_path, const gchar *device_file,
				  HalDevice *parent_dev, const gchar *parent_path,
				  void *end_token);

void hotplug_event_begin_remove_dev (const gchar *subsystem, const gchar *sysfs_path, void *end_token);
void hotplug_event_refresh_dev (const gchar *subsystem, const gchar *sysfs_path, HalDevice *d, void *end_token);

gboolean dev_rescan_device (HalDevice *d);

HotplugEvent *dev_generate_add_hotplug_event (HalDevice *d);

HotplugEvent *dev_generate_remove_hotplug_event (HalDevice *d);

extern gboolean _have_sysfs_lid_button;
extern gboolean _have_sysfs_power_button;
extern gboolean _have_sysfs_sleep_button;
extern gboolean _have_sysfs_power_supply;


// --------------------------------------------------
// blockdev.h


void hotplug_event_begin_add_blockdev (const gchar *sysfs_path, const char *device_file, gboolean is_partition, HalDevice *parent, void *end_token);

void hotplug_event_begin_remove_blockdev (const gchar *sysfs_path, void *end_token);

void hotplug_event_refresh_blockdev (gchar *sysfs_path, HalDevice *d, void *end_token);

gboolean blockdev_rescan_device (HalDevice *d);

HotplugEvent *blockdev_generate_add_hotplug_event (HalDevice *d);

HotplugEvent *blockdev_generate_remove_hotplug_event (HalDevice *d);

void blockdev_refresh_mount_state (HalDevice *d);

void blockdev_process_mdstat (void);
*/

#endif /* UDI_HELPER_H */

#ifdef __cplusplus
}
#endif

