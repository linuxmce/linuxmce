/*
 * Copyright (C) 2002 Intel Corp.
 *
 * This are the chipset specific routines for the Intel 7505 chipset.  
 * The chipset contains an AGP 3.0 compliant north bridge allowing 
 * for up to 8x speeds, hence we need to detect and configure 
 * accordingly.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * JEFF HARTMANN, OR ANY OTHER CONTRIBUTORS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * TODO: 
 * - Allocate more than order 0 pages to avoid too much linear map splitting.
 */

#include <linux/version.h>
#ifdef MODVERSIONS
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,71)
#include <linux/modversions.h>
#endif
#endif
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>

//#include <linux/agp_backend.h>
#include "agp_backend.h"

#include "agp.h"

/*
 *  Support for the Intel 7505 Chipset.  
 */

static struct gatt_mask intel_7505_masks[] =
{
	{mask: 0x00000017, type: 0}
};

static unsigned long intel_7505_mask_memory(unsigned long addr, int type)
{
	/* memory type is ignored */
	return addr | agp_bridge.masks[0].mask;
}

static int intel_7505_configure(void)
{
	u32 temp;
	struct aper_size_info_16 *current_size;
	
	current_size = A_SIZE_16(agp_bridge.current_size);

	/* aperture size */
	pci_write_config_word(agp_bridge.dev, INTEL_I7505_APSIZE,
			      current_size->size_value);

	/* address to map to */
	pci_read_config_dword(agp_bridge.dev, INTEL_I7505_NAPBASELO,
			     &temp);

	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

	/* attbase */ 
	pci_write_config_dword(agp_bridge.dev, INTEL_I7505_ATTBASE,
			       agp_bridge.gatt_bus_addr);

	/* agpctrl */
	pci_write_config_dword(agp_bridge.dev, INTEL_I7505_AGPCTRL, 0x0000);

	/* clear error registers */
	pci_write_config_byte(agp_bridge.dev, INTEL_I7505_ERRSTS, 0xff);

	return 0;
}

static struct aper_size_info_16 intel_7505_sizes[7] =
{
	{256, 65536, 6, 0xf00},
	{128, 32768, 5, 0xf20},
	{64, 16384, 4, 0xf30},
	{32, 8192, 3, 0xf38},
	{16, 4096, 2, 0xf3c},
	{8, 2048, 1, 0xf3e},
	{4, 1024, 0, 0xf3f}
};

static int intel_7505_fetch_size(void)
{
	int i;
	u16 tmp;
	struct aper_size_info_16 *values;

	/* 
	 * For AGP 3.0 APSIZE is now 16 bits
	 */
	pci_read_config_word(agp_bridge.dev, INTEL_I7505_APSIZE, &tmp);
	tmp = (tmp & 0xfff);

	values = A_SIZE_16(agp_bridge.aperture_sizes);

	for(i=0; i < agp_bridge.num_aperture_sizes; i++) {
		if(tmp == values[i].size_value) {
			agp_bridge.previous_size = 
				agp_bridge.current_size = (void *)(values + i);
			agp_bridge.aperture_size_idx = i;
			return values[i].size;
		}
	}
	return 0;
}
static void intel_7505_cleanup(void)
{
	struct aper_size_info_16 *previous_size;

	previous_size = A_SIZE_16(agp_bridge.previous_size);
	pci_write_config_byte(agp_bridge.dev, INTEL_I7505_APSIZE,
			      previous_size->size_value);
}

static void intel_7505_tlbflush(agp_memory * mem)
{
  	u32 temp;
  	pci_read_config_dword(agp_bridge.dev, INTEL_I7505_AGPCTRL, &temp);
  	pci_write_config_dword(agp_bridge.dev, INTEL_I7505_AGPCTRL, temp & ~(1 << 7));
  	pci_read_config_dword(agp_bridge.dev, INTEL_I7505_AGPCTRL, &temp);
  	pci_write_config_dword(agp_bridge.dev, INTEL_I7505_AGPCTRL, temp | (1 << 7));
}

int __init intel_7505_setup(struct pci_dev *pdev)
{
	agp_bridge.masks = intel_7505_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) intel_7505_sizes;
	agp_bridge.size_type = U16_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = intel_7505_configure;
	agp_bridge.fetch_size = intel_7505_fetch_size;
	agp_bridge.cleanup = intel_7505_cleanup;
	agp_bridge.tlb_flush = intel_7505_tlbflush;
	agp_bridge.mask_memory = intel_7505_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_3_0_enable;
	agp_bridge.cache_flush = global_cache_flush;
	agp_bridge.create_gatt_table = agp_generic_create_gatt_table;
	agp_bridge.free_gatt_table = agp_generic_free_gatt_table;
	agp_bridge.insert_memory = agp_generic_insert_memory;
	agp_bridge.remove_memory = agp_generic_remove_memory;
	agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
	agp_bridge.free_by_type = agp_generic_free_by_type;
	agp_bridge.agp_alloc_page = agp_generic_alloc_page;
	agp_bridge.agp_destroy_page = agp_generic_destroy_page;
	agp_bridge.suspend = agp_generic_suspend;
	agp_bridge.resume = agp_generic_resume;
	agp_bridge.cant_use_aperture = 0;

	return 0;
}
