/*
 * AGPGART module version 0.99
 * Copyright (C) 1999 Jeff Hartmann
 * Copyright (C) 1999 Precision Insight, Inc.
 * Copyright (C) 1999 Xi Graphics, Inc.
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
 */

/* further general & copyright notes, for e.g. VIA KT400/AGP3, 
 * compareable to Linux kernel 2.5/2.6 tree */
/*
 * AGPGART backend implementation.
 *
 * Copyright (C) 2002-2003 Dave Jones
 * Copyright (C) 1999 Jeff Hartmann
 * Copyright (C) 1999 Precision Insight, Inc.
 * Copyright (C) 1999 Xi Graphics, Inc.
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
 */

/*
 *
 * Modified by FGL Graphics, a division of SONICblue Inc.
 * Copyright (C) 2000 FGL Graphics, a division of SONICblue Inc.
 *
 */

/*
 *
 * Modified by Jason Barto (jpbarto@graffiti.net) to provide
 * support for the VIA P4X400 Northbridge.
 * Modifications made on 04/30/2003
 *
 */

/*
 * Update Status
 * =============
 * This source file is synced with code in:
 * - linux-2.4.16 (Linus Torvalds/Marcelo Tosatti)
 * - linux-2.4.8-ac7 (Alan Cox)
 * see www.kernel.org and related ftp sites for reference.
 * and
 * - http://innominate.org/cgi-bin/lksr/linux/drivers/char/agp/agpgart_be.c
 *   version 1.1.1.8, 2001-10-09, CVS-Tag: LINUX_2_4_13
 */

// dual boards problem:
// MGA G450 (PCI) looks like an AGP board
#define FGL_FIX

#if defined(FIREGL1) || defined(FIREGL23)
/* AGP module will be staticly linked to FireGL kernel module */
#define __NO_VERSION__
#else /* Original AGPGART module */
#define EXPORT_SYMTAB 
#endif /* Original AGPGART module */

#include <linux/version.h>
#ifdef MODVERSIONS
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,71)
#include <linux/modversions.h>
#endif
#endif

#include <linux/config.h>


#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/miscdevice.h>

#if (LINUX_VERSION_CODE >= 0x020400)
#define FGL_PM_PRESENT
#else
// some recent SuSE or RedHat kernels from the 2.2.x series do have PM as well.
// if in need then enable this on your own risk.
//#define FGL_PM_PRESENT
#endif

#ifdef FGL_PM_PRESENT
#include <linux/pm.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,15))
#include <linux/pm_legacy.h>
#endif
#endif
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/page.h>

#include "agp_backend.h"
#include "agp.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 22)
#define pci_name(pdev) ((pdev)->slot_name)
#endif

#ifdef FGL
//#define FGL_MORE_FLUSH    // performs exhaustive cpu cache flushes
#define FGL_PATCH_IRONGATE_FLUSH    // code like published from Jeff Hartmann
                                        // at the dri-developer mailing list
    // its supposed to be a work around for tlb_flush with certain revisions of AMD 751
    
#endif

#ifdef FGL
#if defined(put_page) && defined(UnlockPage)
#define AGPGART_2_4_19
#endif /* put_page && UnlockPage */
#endif /* FGL */

#ifdef STANDALONE_AGPGART
MODULE_AUTHOR("Jeff Hartmann <jhartmann@precisioninsight.com>");
MODULE_PARM(agp_try_unsupported, "1i");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL and additional rights");
#endif
#ifdef FGL
EXPORT_SYMBOL(agp_allocate_memory_phys_list);
#endif
EXPORT_SYMBOL(agp_free_memory);
EXPORT_SYMBOL(agp_allocate_memory);
EXPORT_SYMBOL(agp_copy_info);
EXPORT_SYMBOL(agp_bind_memory);
EXPORT_SYMBOL(agp_unbind_memory);
EXPORT_SYMBOL(agp_enable);
EXPORT_SYMBOL(agp_backend_acquire);
EXPORT_SYMBOL(agp_backend_release);
#endif /* STANDALONE_AGPGART */

struct agp_bridge_data agp_bridge;
struct agp_bridge_data agp_bridge_pci2pci;
int agp_graphics_device_bus = 0; 

#ifdef STANDALONE_AGPGART
static int agp_try_unsupported __initdata = 0;
#else /* !STANDALONE_AGPGART */
int agp_try_unsupported = 0;
#endif /* !STANDALONE_AGPGART */

int agp_memory_reserved;


/* inline */void simple_flush_cache(void)
{
#if defined(__i386__) || defined(__x86_64__)
	asm volatile ("wbinvd":::"memory");
#elif defined(__alpha__) || defined(__ia64__) || defined(__sparc__)
	/* ??? I wonder if we'll really need to flush caches, or if the
	   core logic can manage to keep the system coherent.  The ARM
	   speaks only of using `cflush' to get things in memory in
	   preparation for power failure.

	   If we do need to call `cflush', we'll need a target page,
	   as we can only flush one page at a time.

	   Ditto for IA-64. --davidm 00/08/07 */
	mb();
#else
#error "Please define simple_flush_cache."
#endif
}

#if LINUX_VERSION_CODE > 0x020500
#ifdef CONFIG_SMP
static void ipi_handler(void *null)
{
        flush_agp_cache();
}

void __attribute__((unused)) global_cache_flush(void)
{
        if (smp_call_function(ipi_handler, NULL, 1, 1) != 0)
                panic(PFX "timed out waiting for the other CPUs!\n");
        flush_agp_cache();
}
#else
void global_cache_flush(void)
{
        flush_agp_cache();
}
#endif  /* !CONFIG_SMP */
#else /* LINUX_VERSION_CODE > 0x020500 */
#ifdef CONFIG_SMP
static atomic_t cpus_waiting;

static void ipi_handler(void *null)
{
	simple_flush_cache();
	atomic_dec(&cpus_waiting);
	while (atomic_read(&cpus_waiting) > 0)
		barrier();
}

void smp_flush_cache(void)
{
	atomic_set(&cpus_waiting, smp_num_cpus - 1);
	if (smp_call_function(ipi_handler, NULL, 1, 0) != 0)
		panic(PFX "timed out waiting for the other CPUs!\n");
	simple_flush_cache();
	while (atomic_read(&cpus_waiting) > 0)
		barrier();
}
#define global_cache_flush smp_flush_cache
#else				/* CONFIG_SMP */
#define global_cache_flush simple_flush_cache
#endif				/* CONFIG_SMP */
#endif /* LINUX_VERSION_CODE > 0x020500 */

#ifdef FIREGL_AGP_HOOK
void firegl_agp_flush_cache(void)
{
    simple_flush_cache();
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10) 
#define firegl_pci_find_class(class,from) pci_get_class(class,from)
#else
#define firegl_pci_find_class(class,from) pci_find_class(class,from)
#endif

int agp_backend_acquire(void)
{
	if (agp_bridge.type == NOT_SUPPORTED) {
		return -EINVAL;
	}
	atomic_inc(&agp_bridge.agp_in_use);

	if (atomic_read(&agp_bridge.agp_in_use) != 1) {
		atomic_dec(&agp_bridge.agp_in_use);
		return -EBUSY;
	}
#ifdef STANDALONE_AGPGART
    // don't increment our own module usage count, cause this
    // would prevent the firegl kernel module from ever beeing unloaded.
    // (agp_backend_release is called during module unload)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
    __module_get(THIS_MODULE);
#else
	MOD_INC_USE_COUNT;
#endif
#endif // STANDALONE_AGPGART
	return 0;
}

void agp_backend_release(void)
{
	if (agp_bridge.type == NOT_SUPPORTED) {
		return;
	}
	atomic_dec(&agp_bridge.agp_in_use);
#ifdef STANDALONE_AGPGART
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
    module_put(THIS_MODULE);
#else
	MOD_DEC_USE_COUNT;
#endif
#endif // STANDALONE_AGPGART
}

/* 
 * Generic routines for handling agp_memory structures -
 * They use the basic page allocation routines to do the
 * brunt of the work.
 */


static void agp_free_key(int key)
{

	if (key < 0) {
		return;
	}
	if (key < MAXKEY) {
		clear_bit(key, agp_bridge.key_list);
	}
}

static int agp_get_key(void)
{
	int bit;

	bit = find_first_zero_bit(agp_bridge.key_list, MAXKEY);
	if (bit < MAXKEY) {
		set_bit(bit, agp_bridge.key_list);
		return bit;
	}
	return -1;
}

static agp_memory *agp_create_memory(int scratch_pages)
{
	agp_memory *new;

    // allocate management header for the memory
	new = kmalloc(sizeof(agp_memory), GFP_KERNEL);
	if (new == NULL) {
		return NULL;
	}
	memset(new, 0, sizeof(agp_memory));

    // allocate an agp memory handle for the memory
	new->key = agp_get_key();
	if (new->key < 0) {
		kfree(new);
		return NULL;
	}

    // allocate the memory itself
	new->memory = vmalloc(PAGE_SIZE * scratch_pages);
	if (new->memory == NULL) {
		agp_free_key(new->key);
		kfree(new);
		return NULL;
	}
	new->num_scratch_pages = scratch_pages;

#ifdef FGL_xxx
    {   // enter some magic value at start of each page
#define PAGE_GOOD   0xba6e600d
        int n;
        unsigned long *ptr;
        
        ptr = new->memory;
        for(n=0; n<scratch_pages; n++)
        {
            *ptr = PAGE_GOOD;
            ptr += PAGE_SIZE/sizeof(*ptr);
        }
    }
#endif

	return new;
}

#define ENTRIES_PER_PAGE		(PAGE_SIZE / sizeof(unsigned long))

#ifdef FGL
// flag values for agp_memory.alloc_type
#define AGP_ALLOC_TYPE_REGULAR     0x00000000
#define AGP_ALLOC_TYPE_PHYS_LIST   0x00000001
#endif

#ifdef FGL
static void agp_free_memory_phys_list(agp_memory * curr)
{
	int i;

	if ((agp_bridge.type == NOT_SUPPORTED) || (curr == NULL)) {
		return;
	}

	if (curr->is_bound == TRUE) {
		agp_unbind_memory(curr);
	}

	if (curr->type != 0) {
#if 0
		agp_bridge.free_by_type_phys_list(curr);
#endif
		return;
	}

	if (curr->page_count != 0) {
		for (i = 0; i < curr->page_count; i++) {
			curr->memory[i] &= ~(0x00000fff);
		}
	}

	agp_free_key(curr->key);    // release the agp handle
	vfree(curr->memory);        // release the table with the phys adresses
	kfree(curr);                // release the agp_memory structure itself
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
    module_put(THIS_MODULE);
#else
	MOD_DEC_USE_COUNT;
#endif
}

agp_memory *agp_allocate_memory_phys_list(
    size_t page_count, u32 type, u64 *phys_addr)
{
	int scratch_pages;
	agp_memory *new;
	int i;

	if (agp_bridge.type == NOT_SUPPORTED) {
		return NULL;
	}
	if ((atomic_read(&agp_bridge.current_memory_agp) + page_count) >
	    agp_bridge.max_memory_agp) {
		return NULL;
	}

	if (type != 0) {
#if 0        
		new = agp_bridge.alloc_by_type_phys_list(page_count, type, phys_addr);
		return new;
#else
        return NULL;    // types are not yet supported for this call
#endif
	}
    /* We always increase the module count, since free auto-decrements
	 * it
	 */
 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
    __module_get(THIS_MODULE);
#else
    MOD_INC_USE_COUNT;
#endif

    /* allocate a "new" agp_memory structure for the requested pages */
	scratch_pages = (page_count + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
	new = agp_create_memory(scratch_pages);
	if (new == NULL) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
        module_put(THIS_MODULE);
#else
        MOD_DEC_USE_COUNT;
#endif
		return NULL;
	}

    /* convert/copy the pysical adresses into the "new" agp_memory table */
	for (i = 0; i < page_count; i++) {
		/* current driver can't really handle physical address over 4G 
		   on 32-bit systems (with PAE enabled). We have to fail it here.
		*/
		if (phys_addr[i] >= ((u64)1<<32)) {
			agp_free_memory(new);
			return NULL;
		}
		new->memory[i] = agp_bridge.mask_memory( phys_addr[i], type);
	}
    new->page_count += page_count;
    new->alloc_type = AGP_ALLOC_TYPE_PHYS_LIST;

	return new;
}
#endif /* FGL */

void agp_free_memory(agp_memory * curr)
{
	int i;

	if ((agp_bridge.type == NOT_SUPPORTED) || (curr == NULL)) {
		return;
	}
	if (curr->is_bound == TRUE) {
		agp_unbind_memory(curr);
	}
    
#ifdef FGL
    // does this memory need release trough the alternate slot?
    if (curr->alloc_type == AGP_ALLOC_TYPE_PHYS_LIST)
    {
        agp_free_memory_phys_list(curr);
        return;
    }
#endif /* FGL */

	if (curr->type != 0) {
		agp_bridge.free_by_type(curr);
		return;
	}
	if (curr->page_count != 0) {
		for (i = 0; i < curr->page_count; i++) {
			curr->memory[i] &= ~(0x00000fff);
			agp_bridge.agp_destroy_page((unsigned long)
					 phys_to_virt(curr->memory[i]));
		}
	}
	agp_free_key(curr->key);
	vfree(curr->memory);
	kfree(curr);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
    module_put(THIS_MODULE);
#else
	MOD_DEC_USE_COUNT;
#endif
}

agp_memory *agp_allocate_memory(size_t page_count, u32 type)
{
	int scratch_pages;
	agp_memory *new;
	int i;

	if (agp_bridge.type == NOT_SUPPORTED) {
		return NULL;
	}
	if ((atomic_read(&agp_bridge.current_memory_agp) + page_count) >
	    agp_bridge.max_memory_agp) {
		return NULL;
	}

	if (type != 0) {
		new = agp_bridge.alloc_by_type(page_count, type);
		return new;
	}
      	/* We always increase the module count, since free auto-decrements
	 * it
	 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
    __module_get(THIS_MODULE);
#else
    MOD_INC_USE_COUNT;
#endif

	scratch_pages = (page_count + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;

	new = agp_create_memory(scratch_pages);

	if (new == NULL) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
        module_put(THIS_MODULE);
#else
        MOD_DEC_USE_COUNT;
#endif
		return NULL;
	}
#ifdef FGL
    new->alloc_type = AGP_ALLOC_TYPE_REGULAR;
#endif
	for (i = 0; i < page_count; i++) {
		new->memory[i] = agp_bridge.agp_alloc_page();

		if (new->memory[i] == 0) {
			/* Free this structure */
			agp_free_memory(new);
			return NULL;
		}
		new->memory[i] =
		    agp_bridge.mask_memory(
				   virt_to_phys((void *) new->memory[i]),
						  type);
		new->page_count++;
	}

	return new;
}

/* End - Generic routines for handling agp_memory structures */

static int agp_return_size(void)
{
	int current_size;
	void *temp;

	temp = agp_bridge.current_size;

	switch (agp_bridge.size_type) {
	case U8_APER_SIZE:
		current_size = A_SIZE_8(temp)->size;
		break;
	case U16_APER_SIZE:
		current_size = A_SIZE_16(temp)->size;
		break;
	case U32_APER_SIZE:
		current_size = A_SIZE_32(temp)->size;
		break;
	case LVL2_APER_SIZE:
		current_size = A_SIZE_LVL2(temp)->size;
		break;
	case FIXED_APER_SIZE:
		current_size = A_SIZE_FIX(temp)->size;
		break;
	default:
		current_size = 0;
		break;
	}

	return current_size;
}

/* Routine to copy over information structure */

void agp_copy_info(agp_kern_info * info)
{
    unsigned long page_mask = 0;
    int i;

	memset(info, 0, sizeof(agp_kern_info));
	if (agp_bridge.type == NOT_SUPPORTED) {
		info->chipset = agp_bridge.type;
		return;
	}
	info->version.major = agp_bridge.version->major;
	info->version.minor = agp_bridge.version->minor;
	info->device = agp_bridge.dev;
	info->chipset = agp_bridge.type;
	info->mode = agp_bridge.mode;
	info->aper_base = agp_bridge.gart_bus_addr;
	info->aper_size = agp_return_size();
	info->max_memory = agp_bridge.max_memory_agp;
	info->current_memory = atomic_read(&agp_bridge.current_memory_agp);
    info->cant_use_aperture = agp_bridge.cant_use_aperture;

    for(i = 0; i < agp_bridge.num_of_masks; i++)
        page_mask |= agp_bridge.mask_memory(page_mask, i);

    info->page_mask = ~page_mask;
}

/* End - Routine to copy over information structure */

/*
 * Routines for handling swapping of agp_memory into the GATT -
 * These routines take agp_memory and insert them into the GATT.
 * They call device specific routines to actually write to the GATT.
 */

int agp_bind_memory(agp_memory * curr, off_t pg_start)
{
	int ret_val;

	if ((agp_bridge.type == NOT_SUPPORTED) ||
	    (curr == NULL) || (curr->is_bound == TRUE)) {
		return -EINVAL;
	}

#ifndef AGPGART_2_2
	if (curr->is_flushed == FALSE) {
		CACHE_FLUSH();
		curr->is_flushed = TRUE;
	}
#endif /* !AGPGART_2_2 */
    ret_val = agp_bridge.insert_memory(curr, pg_start, curr->type);

	if (ret_val != 0) {
		return ret_val;
	}
	curr->is_bound = TRUE;
	curr->pg_start = pg_start;
	return 0;
}

int agp_unbind_memory(agp_memory * curr)
{
	int ret_val;

	if ((agp_bridge.type == NOT_SUPPORTED) || (curr == NULL)) {
		return -EINVAL;
	}
	if (curr->is_bound != TRUE) {
		return -EINVAL;
	}
	ret_val = agp_bridge.remove_memory(curr, curr->pg_start, curr->type);

	if (ret_val != 0) {
		return ret_val;
	}
	curr->is_bound = FALSE;
	curr->pg_start = 0;
	return 0;
}

/* End - Routines for handling swapping of agp_memory into the GATT */

/* 
 * Driver routines - start
 * Currently this module supports the following chipsets:
 * i810, i815, 440lx, 440bx, 440gx, i830, i840, i845, i845g, i850, i860, via vp3,
 * via mvp3, via kx133, via kt133, amd irongate, amd 761, amd 762, ALi M1541,
 * and generic support for the SiS chipsets.
 */




/* Generic Agp routines - Start */

//static
int agp_generic_agp_v2_enable(u32 mode)
{
	struct pci_dev *device = NULL;
	u32 command, scratch, cap_id;
	u8 cap_ptr;

    // command = read from AGPstatus reg of bridge (PCI device 0:0.0). 
	pci_read_config_dword(agp_bridge.dev,
			      agp_bridge.capndx + 4,
			      &command);

	/*
	 * PASS1: go through all devices that claim to be
	 *        AGP devices and collect their data.
	 */

	while ((device = firegl_pci_find_class(PCI_CLASS_DISPLAY_VGA << 8,
					device)) != NULL) {
		pci_read_config_dword(device, 0x04, &scratch);

        /* check in STATUS reg if a list of capabilities is present */
        if (!(scratch & 0x00100000))
			continue;

		/* find out if AGP device and -if so- address of AGP regs */
        pci_read_config_byte(device, 0x34, &cap_ptr);

		if (cap_ptr != 0x00) {
			do {
				pci_read_config_dword(device,
						      cap_ptr, &cap_id);

				if ((cap_id & 0xff) != 0x02)
					cap_ptr = (cap_id >> 8) & 0xff;
			}
			while (((cap_id & 0xff) != 0x02) && (cap_ptr != 0x00));
		}
		if (cap_ptr != 0x00) {
			/*
			 * Ok, here we have a AGP device. Disable impossible 
			 * settings, and adjust the readqueue to the minimum.
			 */

			pci_read_config_dword(device, cap_ptr + 4, &scratch);
            /* scratch = AGPStatus reg */

#ifdef FGL_FIX
            /* AGP 1x or 2x or 4x - at least one of this list */
            /* mga g450 pci can be uncovered this way */
            if (!(scratch & 7))
                continue;
#endif /* FGL_FIX */


			/* adjust RQ depth */
            /* max num of pipeline commands */ 
            /* is minimum of Bridge or Graphics AGP device */
			command =
                ((command & ~0xff000000) |
                 min((mode & 0xff000000), 
                     min((command & 0xff000000),
                         (scratch & 0xff000000))
                    ));

			/* disable SBA (side banding) if it's not supported */
			if (!((command & 0x00000200) &&
			      (scratch & 0x00000200) &&
			      (mode & 0x00000200)))
				command &= ~0x00000200;

			/* disable FW (fast write) if it's not supported */
			if (!((command & 0x00000010) &&
			      (scratch & 0x00000010) &&
			      (mode & 0x00000010)))
				command &= ~0x00000010;

			if (!((command & 4) &&
			      (scratch & 4) &&
			      (mode & 4)))
				command &= ~0x00000004;

			if (!((command & 2) &&
			      (scratch & 2) &&
			      (mode & 2)))
				command &= ~0x00000002;

			if (!((command & 1) &&
			      (scratch & 1) &&
			      (mode & 1)))
				command &= ~0x00000001;
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10) 
            // the reference count has been increased in agp_backend_initialize.
        if (device)
            pci_dev_put(device); 
#endif

	/*
	 * PASS2: Figure out the 4X/2X/1X setting and enable the
	 *        target (our motherboard chipset).
	 */

	if (command & 4) {
		command &= 0xfffffff8 + 4;	/* 4X */
	}
	if (command & 2) {
		command &= 0xfffffff8 + 2;	/* 2X */
	}
	if (command & 1) {
		command &= 0xfffffff8 + 1;	/* 1X */
	}


#ifdef FGL_xxx
    // there is no AGP mode that matches all requirements
    if (!(command & 7))
	{
        // some older AGP chipsets or graphics adapters might revolt 
		// if they are in AGP mode and AGP mode gets disabled
        return(-1); /* error return */
	}
#endif
#ifdef FGL_FIX
    /* set AGP enable bit - only if a valid mode was determined */
    /* (a way to unhide mga g450 pci) */
    if (command & 7)
#endif
        command |= 0x00000100;
#ifdef __x86_64__
		// TODO: FIX it
        command |= 0x00000104;
#endif // #ifdef __x86_64__

	pci_write_config_dword(agp_bridge.dev,
			       agp_bridge.capndx + 8,
			       command);

	/*
	 * PASS3: Go throu all AGP devices and update the
	 *        command registers.
	 */

	while ((device = firegl_pci_find_class(PCI_CLASS_DISPLAY_VGA << 8,
					device)) != NULL) {

		pci_read_config_dword(device, 0x04, &scratch);

		if (!(scratch & 0x00100000))
			continue;

		pci_read_config_byte(device, 0x34, &cap_ptr);

		if (cap_ptr != 0x00) {
			do {
				pci_read_config_dword(device,
						      cap_ptr, &cap_id);

				if ((cap_id & 0xff) != 0x02)
					cap_ptr = (cap_id >> 8) & 0xff;
			}
			while (((cap_id & 0xff) != 0x02) && (cap_ptr != 0x00));
		}
		if (cap_ptr != 0x00)
        {
#ifdef FGL_FIX
            /* don't enable anything on a device that can not do */
            /* AGP 1x or 2x or 4x; ultimate check */
			pci_read_config_dword(device, cap_ptr + 4, &scratch);
            if (!(scratch & 7))
                continue;
#endif
			pci_write_config_dword(device, cap_ptr + 8, command);
        }
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10) 
            // the reference count has been increased in agp_backend_initialize.
        if (device)
            pci_dev_put(device); 
#endif

    return 0; /* success */
}

#ifndef FGL_USE_AGPV3_OFFICIAL_ENABLE
/*
 * Try to configure an AGP v3 capable setup.
 */
static int agp_generic_agp_v3_enable(u32 mode)
{
	struct pci_dev *device = NULL;
	u32 command;
	u8 cap_ptr;

    /* take the mode mask as the start value for the command */
    command = mode;

	/* go through all devices that claim to be AGP devices */
    /* and determine the best possible command value */
	pci_for_each_dev(device) {
		cap_ptr = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (cap_ptr != 0x00) {
			u32 dev_caps, dev_rq, cmd_rq;

			printk (KERN_INFO "AGP: Setting up AGPv3 capable device at %d:%d:%d\n",
					device->bus->number, PCI_FUNC(device->devfn), PCI_SLOT(device->devfn));

			pci_read_config_dword(device, cap_ptr + 4, &dev_caps); /* read supported caps */

			/* adjust RQ depth */
			cmd_rq = command & 0xff000000; /* currend RQ caps value */
			dev_rq = dev_caps & 0xff000000; /* client RQ caps value */
			cmd_rq = min(cmd_rq, dev_rq); /* take the lower of both */
			command =
			    ((command & ~0xff000000) | cmd_rq);

			/* disable SBA if it's not supported */
			if (!(dev_caps & 0x200))
				command &= ~0x200;

			/* disable FW if it's not supported */
			if (!(dev_caps & 0x10))
				command &= ~0x10;

            /* disable AGPv3 mode 8x if it's not supported */
			if (!(dev_caps & 2))
				command &= ~2;		

            /* disable AGPv3 mode 4x if it's not supported */
			if (!(dev_caps & 1))
				command &= ~1;
		}
	}

	/* take the overall device capabilities and */
    /* build a valid command word out of it */
	if (command & 2)
    {
		command = (command & ~7) | 2; /* set AGPv3 mode 8x */
    }
    else
	if (command & 1)
    {
		command = (command & ~7) | 1; /* AGPv3 mode 4x */
    }
    else {
		printk (KERN_INFO "AGP: Devices do not present a common mode\n");
        return(-1); /* failed */
    }

	command |= 0x100; /* set AGP_ENABLE bit */

	printk (KERN_INFO "AGP: determined command value = %08x\n", command);

	/* Go through all AGP devices and update the command registers. */
	pci_for_each_dev(device) {
		cap_ptr = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (cap_ptr != 0x00)
			pci_write_config_dword(device, cap_ptr + 8, command);
	}

    return 0; /* success */
}
#endif

// please call the generic agp routine, even if your chipset only supports one
// AGP specification. other devices might support just the opposite and here
// are the checks that prevent you from enabling AGP in a screwed setup.
int agp_generic_agp_enable(u32 mode)
{
	struct pci_dev *device = NULL;
	u8 cap_ptr;
	u8 v2_devs=0;       // AGP 2.0 device in standard mode (4x/2x/1x)
	u8 v3comp_devs=0;   // AGP 3.0 device in AGP 2.0 compatibility mode (4x/2x/1x)
	u8 v3_devs=0;       // AGP 3.0 device in standard mode (8x/4x)

    /* determine the number of AGPv2 and AGPv3 devices, including the bridge */
	pci_for_each_dev(device) {
		cap_ptr = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (cap_ptr != 0x00) {
            u32 cap_id, dev_caps;

			pci_read_config_dword(device, cap_ptr, &cap_id);
			cap_id = (cap_id>>16) & 0xff; /* extract version code */
			if (cap_id >= 0x30) {
    			/* conforms to AGP v3 specification */
    			pci_read_config_dword(device, cap_ptr+4, &dev_caps); /* agp_status = device capabilities */
                if (dev_caps & (1<<3)) { /* check AGP3.0_MODE bit */
                    /* in AGP v3 standard mode */
    				v3_devs++;
                } else {
                    /* in AGP v3 compatibility mode */
    				v3comp_devs++;
                }
			} else {
    		    v2_devs++;
			}

            /* this will serve us as a fallback if bridge device is not identical
               to the device where AGP_CAPS does reside. this might fail if there
               are multiple AGP busmaster devices on multiple busses. */
            agp_graphics_device_bus = device->bus->number; /* remember graphic's bus number */
		}
	}

    /* report status of detection */
    if( v2_devs>0 )
        printk (KERN_INFO "AGP: Found %d AGPv2 devices\n", v2_devs);
    if( v3comp_devs>0 )
        printk (KERN_INFO "AGP: Found %d AGPv3 devices in compatibility mode\n", v3comp_devs);
    if( v3_devs>0 )
        printk (KERN_INFO "AGP: Found %d AGPv3 devices\n", v3_devs);

    /* call respective init routine or deny operation */
    if( (v2_devs+v3comp_devs+v3_devs==0) ) {
		printk (KERN_INFO "AGP: No AGP capable devices found\n");
		return(-1);
	} else
    if( (v2_devs+v3comp_devs>0) && (v3_devs>0) ) {
        /* AGP v3 4x standard mode is incompatible to AGP v2 4x mode !!! */
        /* so there is nothing we can do to concat these two worlds */
		printk (KERN_INFO "AGP: Device modes are incompatible\n");
		return(-1);
	} else
	if (v2_devs+v3comp_devs>1) {
        printk (KERN_INFO "AGP: Doing enable for AGPv2\n");
		return agp_generic_agp_v2_enable(mode);
	} else 
	if (v3_devs>1) {
		printk (KERN_INFO "AGP: Doing enable for AGPv3\n");
#ifdef FGL_USE_AGPV3_OFFICIAL_ENABLE
        return agp_generic_agp_3_0_enable(mode);
#else
		return agp_generic_agp_v3_enable(mode); // FGL preliminary coding
#endif
    } else {
		printk (KERN_INFO "AGP: Insufficent amount of AGP devices found\n");
		return(-1);
	}
}

int agp_generic_create_gatt_table(void)
{
	char *table;
	char *table_end;
	int size;
	int page_order;
	int num_entries;
	int i;
	void *temp;
#ifndef AGPGART_2_2
	struct page *page;
#endif /* !AGPGART_2_2 */
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    int err;
#endif

	/* The generic routines can't handle 2 level gatt's */
	if (agp_bridge.size_type == LVL2_APER_SIZE) {
		return -EINVAL;
	}

	table = NULL;
	i = agp_bridge.aperture_size_idx;
	temp = agp_bridge.current_size;
	size = page_order = num_entries = 0;

	if (agp_bridge.size_type != FIXED_APER_SIZE) {
		do {
			switch (agp_bridge.size_type) {
			case U8_APER_SIZE:
				size = A_SIZE_8(temp)->size;
				page_order =
				    A_SIZE_8(temp)->page_order;
				num_entries =
				    A_SIZE_8(temp)->num_entries;
				break;
			case U16_APER_SIZE:
				size = A_SIZE_16(temp)->size;
				page_order = A_SIZE_16(temp)->page_order;
				num_entries = A_SIZE_16(temp)->num_entries;
				break;
			case U32_APER_SIZE:
				size = A_SIZE_32(temp)->size;
				page_order = A_SIZE_32(temp)->page_order;
				num_entries = A_SIZE_32(temp)->num_entries;
				break;
				/* This case will never really happen. */
			case FIXED_APER_SIZE:
			case LVL2_APER_SIZE:
			default:
				size = page_order = num_entries = 0;
				break;
			}

			table = (char *) __get_free_pages(GFP_KERNEL|__GFP_COMP,
							  page_order);	

			if (table == NULL) {
				i++;
				switch (agp_bridge.size_type) {
				case U8_APER_SIZE:
					agp_bridge.current_size = A_IDX8();
					break;
				case U16_APER_SIZE:
					agp_bridge.current_size = A_IDX16();
					break;
				case U32_APER_SIZE:
					agp_bridge.current_size = A_IDX32();
					break;
					/* This case will never really 
					 * happen. 
					 */
				case FIXED_APER_SIZE:
				case LVL2_APER_SIZE:
				default:
					agp_bridge.current_size =
					    agp_bridge.current_size;
					break;
				}
			} else {
				agp_bridge.aperture_size_idx = i;
			}
		} while ((table == NULL) &&
			 (i < agp_bridge.num_aperture_sizes));
	} else {
		size = ((struct aper_size_info_fixed *) temp)->size;
		page_order = ((struct aper_size_info_fixed *) temp)->page_order;
		num_entries = ((struct aper_size_info_fixed *) temp)->num_entries;
		table = (char *) __get_free_pages(GFP_KERNEL|__GFP_COMP, page_order);
	}

	if (table == NULL) {
		return -ENOMEM;
	}
	table_end = table + ((PAGE_SIZE * (1 << page_order)) - 1);

#ifdef AGPGART_2_2
	for (i = MAP_NR(table); i <= MAP_NR(table_end); i++) {
		set_bit(PG_reserved, &mem_map[i].flags);
	}

   	agp_bridge.gatt_table = (u32 *) table;
	agp_bridge.gatt_table_real = (u32 *) table;
	CACHE_FLUSH();

	if (agp_bridge.gatt_table == NULL) {
		for (i = MAP_NR(table); i <= MAP_NR(table_end); i++) {
			clear_bit(PG_reserved, &mem_map[i].flags);
		}

		free_pages((unsigned long) table, page_order);

		return -ENOMEM;
	}
#else /* !AGPGART_2_2 */
	for (page = virt_to_page(table); 
		page <= virt_to_page(table_end); page++)
		set_bit(PG_reserved, &page->flags);

	agp_bridge.gatt_table_real = (u32 *) table;
	CACHE_FLUSH();
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    err = change_page_attr(virt_to_page(table), 1<<page_order, PAGE_KERNEL_NOCACHE);
    if (!err)    
#endif
    {
    	agp_bridge.gatt_table = ioremap_nocache(virt_to_phys(table),
	    				(PAGE_SIZE * (1 << page_order)));
    }
	CACHE_FLUSH();

	if (agp_bridge.gatt_table == NULL
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
        || (err)
#endif
        )
    {
		for (page = virt_to_page(table);
			 page <= virt_to_page(table_end); page++)
			clear_bit(PG_reserved, &page->flags);

		free_pages((unsigned long) table, page_order);

		return -ENOMEM;
	}
#endif /* !AGPGART_2_2 */

	agp_bridge.gatt_bus_addr = virt_to_phys(agp_bridge.gatt_table_real);

	for (i = 0; i < num_entries; i++) {
		agp_bridge.gatt_table[i] =
		    (unsigned long) agp_bridge.scratch_page;
	}

	return 0;
}

int agp_generic_suspend(void)
{
    return 0;
}

void agp_generic_resume(void)
{
    return;
}

int agp_generic_free_gatt_table(void)
{
	int page_order;
	char *table, *table_end;
	void *temp;
#ifdef AGPGART_2_2
    int i;
#else /* !AGPGART_2_2 */
	struct page *page;
#endif /* !AGPGART_2_2 */

	temp = agp_bridge.current_size;

	switch (agp_bridge.size_type) {
	case U8_APER_SIZE:
		page_order = A_SIZE_8(temp)->page_order;
		break;
	case U16_APER_SIZE:
		page_order = A_SIZE_16(temp)->page_order;
		break;
	case U32_APER_SIZE:
		page_order = A_SIZE_32(temp)->page_order;
		break;
	case FIXED_APER_SIZE:
		page_order = A_SIZE_FIX(temp)->page_order;
		break;
	case LVL2_APER_SIZE:
		/* The generic routines can't deal with 2 level gatt's */
		return -EINVAL;
		break;
	default:
		page_order = 0;
		break;
	}

	/* Do not worry about freeing memory, because if this is
	 * called, then all agp memory is deallocated and removed
	 * from the table.
	 */

#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
   change_page_attr(virt_to_page(agp_bridge.gatt_table_real), 1<<page_order, 
            PAGE_KERNEL);
#endif
 
#ifndef AGPGART_2_2
	iounmap(agp_bridge.gatt_table);
#endif /* !AGPGART_2_2 */
	table = (char *) agp_bridge.gatt_table_real;
	table_end = table + ((PAGE_SIZE * (1 << page_order)) - 1);

#ifdef AGPGART_2_2
	for (i = MAP_NR(table); i <= MAP_NR(table_end); i++) {
		clear_bit(PG_reserved, &mem_map[i].flags);
	}
#else /* !AGPGART_2_2 */
	for (page = virt_to_page((unsigned long)table); 
			page <= virt_to_page((unsigned long)table_end); page++)
			clear_bit(PG_reserved, &page->flags);
#endif /* !AGPGART_2_2 */

	free_pages((unsigned long) agp_bridge.gatt_table_real, page_order);
	return 0;
}

int agp_generic_insert_memory(agp_memory * mem,
				     off_t pg_start, int type)
{
	int i, j, num_entries;
	void *temp;

	temp = agp_bridge.current_size;

	switch (agp_bridge.size_type) {
	case U8_APER_SIZE:
		num_entries = A_SIZE_8(temp)->num_entries;
		break;
	case U16_APER_SIZE:
		num_entries = A_SIZE_16(temp)->num_entries;
		break;
	case U32_APER_SIZE:
		num_entries = A_SIZE_32(temp)->num_entries;
		break;
	case FIXED_APER_SIZE:
		num_entries = A_SIZE_FIX(temp)->num_entries;
		break;
	case LVL2_APER_SIZE:
		/* The generic routines can't deal with 2 level gatt's */
		return -EINVAL;
		break;
	default:
		num_entries = 0;
		break;
	}

	if (type != 0 || mem->type != 0) {
		/* The generic routines know nothing of memory types */
		return -EINVAL;
	}
	if ((pg_start + mem->page_count) > num_entries) {
		return -EINVAL;
	}
	j = pg_start;

	while (j < (pg_start + mem->page_count)) {
		if (!PGE_EMPTY(agp_bridge.gatt_table[j])) {
			return -EBUSY;
		}
		j++;
	}

#ifndef AGPGART_2_2
	if (mem->is_flushed == FALSE) {
		CACHE_FLUSH();
		mem->is_flushed = TRUE;
	}
#endif /* !AGPGART_2_2 */
	for (i = 0, j = pg_start; i < mem->page_count; i++, j++) {
		agp_bridge.gatt_table[j] = mem->memory[i];
	}
#ifdef AGPGART_2_2
	/* Do cache flush all the time since the table is not 
	 * mapped uncached */
    CACHE_FLUSH();
#endif /* AGPGART_2_2 */
	agp_bridge.tlb_flush(mem);
	return 0;
}

int agp_generic_remove_memory(agp_memory * mem, off_t pg_start,
				     int type)
{
	int i;

	if (type != 0 || mem->type != 0) {
		/* The generic routines know nothing of memory types */
		return -EINVAL;
	}
	for (i = pg_start; i < (mem->page_count + pg_start); i++) {
		agp_bridge.gatt_table[i] =
		    (unsigned long) agp_bridge.scratch_page;
	}

	agp_bridge.tlb_flush(mem);
	return 0;
}

agp_memory *agp_generic_alloc_by_type(size_t page_count, int type)
{
	return NULL;
}

void agp_generic_free_by_type(agp_memory * curr)
{
	if (curr->memory != NULL) {
		vfree(curr->memory);
	}
	agp_free_key(curr->key);
	kfree(curr);
}

/* 
 * Basic Page Allocation Routines -
 * These routines handle page allocation
 * and by default they reserve the allocated 
 * memory.  They also handle incrementing the
 * current_memory_agp value, Which is checked
 * against a maximum value.
 */

unsigned long agp_generic_alloc_page(void)
{
#ifdef AGPGART_2_2
	void *pt;

	pt = (void *) __get_free_page(GFP_KERNEL);
	if (pt == NULL) {
		return 0;
	}
#ifdef AGPGART_2_2
	atomic_inc(&mem_map[MAP_NR(pt)].count);
	set_bit(PG_locked, &mem_map[MAP_NR(pt)].flags);
#else /* !AGPGART_2_2 */
	get_page(&virt_to_page(pt));
	set_bit(PG_locked, &virt_to_page(pt)->flags);
#endif /* !AGPGART_2_2 */
	atomic_inc(&agp_bridge.current_memory_agp);
	return (unsigned long) pt;
#else
    // 2.4.8-ac7 and 2.4.13
    struct page * page;

    page = alloc_page(GFP_KERNEL);
    if (page == NULL) {
        return 0;
    }

#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    if (change_page_attr(page, 1, PAGE_KERNEL_NOCACHE) < 0) {
        __free_page(page); 
        return 0;
    }
#endif

    get_page(page);
    set_bit(PG_locked, &page->flags);
    atomic_inc(&agp_bridge.current_memory_agp);

    return (unsigned long)page_address(page);
#endif
}

#ifdef AGPGART_2_2
void agp_generic_destroy_page(unsigned long page)
{
	void *pt = (void *) page;

	if (pt == NULL) {
		return;
	}
	atomic_dec(&mem_map[MAP_NR(pt)].count);
	clear_bit(PG_locked, &mem_map[MAP_NR(pt)].flags);
	wake_up(&mem_map[MAP_NR(pt)].wait);
	free_page((unsigned long) pt);
	atomic_dec(&agp_bridge.current_memory_agp);
}
#else // ! AGPGART_2_2
void agp_generic_destroy_page(unsigned long addr)
{
    // 2.4.8-ac7 and 2.4.13
    void *pt = (void *) addr;                                                 
    struct page *page;                                                        

    if (pt == NULL) {                                                         
        return;                                                           
    }                                                                         
    page = virt_to_page((unsigned long)pt);                                                  

#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    change_page_attr(page, 1, PAGE_KERNEL); 
#endif

#if LINUX_VERSION_CODE > 0x020500
//    unmap_page_from_agp(page); // LINUX_TODO
    put_page(page);
    unlock_page(page);
#else /* LINUX_VERSION_CODE > 0x020500 */
#ifdef AGPGART_2_4_19
    put_page(page);
    UnlockPage(page);
#else /* AGPGART_2_4_19 */
    __put_page(page);                                                 
    clear_bit(PG_locked, &page->flags);                                       
    wake_up(&page->wait);                                                     
#endif /* AGPGART_2_4_19 */
#endif /* LINUX_VERSION_CODE > 0x020500 */
    free_page((unsigned long) pt);                                            
    atomic_dec(&agp_bridge.current_memory_agp);                               
}
#endif // AGPGART_2_2

/* End Basic Page Allocation Routines */

void agp_enable(u32 mode)
{
	if (agp_bridge.type == NOT_SUPPORTED) return;
	agp_bridge.agp_enable(mode);
}

/* End - Generic Agp routines */

static struct aper_size_info_fixed intel_i810_sizes[] =
{
	{64, 16384, 4},
     /* The 32M mode still requires a 64k gatt */
	{32, 8192, 4}
};

// flag values for agp_memory.type
#define AGP_DCACHE_MEMORY 1
#define AGP_PHYS_MEMORY   2

static struct gatt_mask intel_i810_masks[] =
{
	{I810_PTE_VALID, 0},
	{(I810_PTE_VALID | I810_PTE_LOCAL), AGP_DCACHE_MEMORY},
	{I810_PTE_VALID, 0}
};

static struct _intel_i810_private {
	struct pci_dev *i810_dev;	/* device one */
	volatile u8 *registers;
	int num_dcache_entries;
} intel_i810_private;

static int intel_i810_fetch_size(void)
{
	u32 smram_miscc;
	struct aper_size_info_fixed *values;

	pci_read_config_dword(agp_bridge.dev, I810_SMRAM_MISCC, &smram_miscc);
	values = A_SIZE_FIX(agp_bridge.aperture_sizes);

	if ((smram_miscc & I810_GMS) == I810_GMS_DISABLE) {
		printk(KERN_WARNING PFX "i810 is disabled\n");
		return 0;
	}
	if ((smram_miscc & I810_GFX_MEM_WIN_SIZE) == I810_GFX_MEM_WIN_32M) {
		agp_bridge.previous_size =
		    agp_bridge.current_size = (void *) (values + 1);
		agp_bridge.aperture_size_idx = 1;
		return values[1].size;
	} else {
		agp_bridge.previous_size =
		    agp_bridge.current_size = (void *) (values);
		agp_bridge.aperture_size_idx = 0;
		return values[0].size;
	}

	return 0;
}

static int intel_i810_configure(void)
{
	struct aper_size_info_fixed *current_size;
	u32 temp;
	int i;

	current_size = A_SIZE_FIX(agp_bridge.current_size);

	pci_read_config_dword(intel_i810_private.i810_dev, I810_MMADDR, &temp);
	temp &= 0xfff80000;

	intel_i810_private.registers =
	    (volatile u8 *) ioremap(temp, 128 * 4096);

	if ((INREG32(intel_i810_private.registers, I810_DRAM_CTL)
	     & I810_DRAM_ROW_0) == I810_DRAM_ROW_0_SDRAM) {
		/* This will need to be dynamically assigned */
		printk(KERN_INFO PFX "detected 4MB dedicated video ram.\n");
		intel_i810_private.num_dcache_entries = 1024;
	}
	pci_read_config_dword(intel_i810_private.i810_dev, I810_GMADDR, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);
	OUTREG32(intel_i810_private.registers, I810_PGETBL_CTL,
		 agp_bridge.gatt_bus_addr | I810_PGETBL_ENABLED);
	CACHE_FLUSH();

	if (agp_bridge.needs_scratch_page == TRUE) {
		for (i = 0; i < current_size->num_entries; i++) {
			OUTREG32(intel_i810_private.registers,
				 I810_PTE_BASE + (i * 4),
				 agp_bridge.scratch_page);
		}
	}
	return 0;
}

static void intel_i810_cleanup(void)
{
	OUTREG32(intel_i810_private.registers, I810_PGETBL_CTL, 0);
	iounmap((void *) intel_i810_private.registers);
}

static void intel_i810_tlbflush(agp_memory * mem)
{
	return;
}

static int intel_i810_agp_enable(u32 mode)
{
	return(0); /* always succeeds */
}

static int intel_i810_insert_entries(agp_memory * mem, off_t pg_start,
				     int type)
{
	int i, j, num_entries;
	void *temp;

	temp = agp_bridge.current_size;
	num_entries = A_SIZE_FIX(temp)->num_entries;

	if ((pg_start + mem->page_count) > num_entries) {
		return -EINVAL;
	}
	for (j = pg_start; j < (pg_start + mem->page_count); j++) {
		if (!PGE_EMPTY(agp_bridge.gatt_table[j])) {
			return -EBUSY;
		}
	}

	if (type != 0 || mem->type != 0) {
		if ((type == AGP_DCACHE_MEMORY) &&
		    (mem->type == AGP_DCACHE_MEMORY)) {
			/* special insert */
			CACHE_FLUSH();
			for (i = pg_start;
			     i < (pg_start + mem->page_count); i++) {
				OUTREG32(intel_i810_private.registers,
					 I810_PTE_BASE + (i * 4),
					 (i * 4096) | I810_PTE_LOCAL |
					 I810_PTE_VALID);
			}
			CACHE_FLUSH();
			agp_bridge.tlb_flush(mem);
			return 0;
		}
	        if((type == AGP_PHYS_MEMORY) &&
		   (mem->type == AGP_PHYS_MEMORY)) {
		   goto insert;
		}
		return -EINVAL;
	}

insert:
   	CACHE_FLUSH();
	for (i = 0, j = pg_start; i < mem->page_count; i++, j++) {
		OUTREG32(intel_i810_private.registers,
			 I810_PTE_BASE + (j * 4), mem->memory[i]);
	}
	CACHE_FLUSH();

	agp_bridge.tlb_flush(mem);
	return 0;
}

static int intel_i810_remove_entries(agp_memory * mem, off_t pg_start,
				     int type)
{
	int i;

#ifdef AGPGART_2_2
   	CACHE_FLUSH();
#endif /* AGPGART_2_2 */
	for (i = pg_start; i < (mem->page_count + pg_start); i++) {
		OUTREG32(intel_i810_private.registers,
			 I810_PTE_BASE + (i * 4),
			 agp_bridge.scratch_page);
	}
	CACHE_FLUSH();

	agp_bridge.tlb_flush(mem);
	return 0;
}

static agp_memory *intel_i810_alloc_by_type(size_t pg_count, int type)
{
	agp_memory *new;

	if (type == AGP_DCACHE_MEMORY) {
		if (pg_count != intel_i810_private.num_dcache_entries) {
			return NULL;
		}
		new = agp_create_memory(1);

		if (new == NULL) {
			return NULL;
		}
		new->type = AGP_DCACHE_MEMORY;
		new->page_count = pg_count;
		new->num_scratch_pages = 0;
		vfree(new->memory);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
        __module_get(THIS_MODULE);
#else
	   	MOD_INC_USE_COUNT;
#endif
		return new;
	}
	if(type == AGP_PHYS_MEMORY) {
		/* The I810 requires a physical address to program
		 * it's mouse pointer into hardware.  However the
		 * Xserver still writes to it through the agp
		 * aperture
		 */
	   	if (pg_count != 1) {
		   	return NULL;
		}
	   	new = agp_create_memory(1);

		if (new == NULL) {
			return NULL;
		}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
        __module_get(THIS_MODULE);
#else
        MOD_INC_USE_COUNT;
#endif
		new->memory[0] = agp_bridge.agp_alloc_page();

		if (new->memory[0] == 0) {
			/* Free this structure */
			agp_free_memory(new);
			return NULL;
		}
		new->memory[0] =
		    agp_bridge.mask_memory(
				   virt_to_phys((void *) new->memory[0]),
						  type);
		new->page_count = 1;
	   	new->num_scratch_pages = 1;
	   	new->type = AGP_PHYS_MEMORY;
	        new->physical = virt_to_phys((void *) new->memory[0]);
	   	return new;
	}
   
	return NULL;
}

static void intel_i810_free_by_type(agp_memory * curr)
{
	agp_free_key(curr->key);
   	if(curr->type == AGP_PHYS_MEMORY) {
	   	agp_bridge.agp_destroy_page((unsigned long)
				 phys_to_virt(curr->memory[0]));
		vfree(curr->memory);
	}
	kfree(curr);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
    module_put(THIS_MODULE);
#else
   	MOD_DEC_USE_COUNT;
#endif
}

static unsigned long intel_i810_mask_memory(unsigned long addr, int type)
{
	/* Type checking must be done elsewhere */
	return addr | agp_bridge.masks[type].mask;
}

static int __init intel_i810_setup(struct pci_dev *i810_dev)
{
    intel_i810_private.i810_dev = i810_dev;

    agp_bridge.masks = intel_i810_masks;
    agp_bridge.num_of_masks = 2;
    agp_bridge.aperture_sizes = (void *) intel_i810_sizes;
    agp_bridge.size_type = FIXED_APER_SIZE;
    agp_bridge.num_aperture_sizes = 2;
    agp_bridge.dev_private_data = (void *) &intel_i810_private;
    agp_bridge.needs_scratch_page = TRUE;
    agp_bridge.configure = intel_i810_configure;
    agp_bridge.fetch_size = intel_i810_fetch_size;
    agp_bridge.cleanup = intel_i810_cleanup;
    agp_bridge.tlb_flush = intel_i810_tlbflush;
    agp_bridge.mask_memory = intel_i810_mask_memory;
    agp_bridge.agp_enable = intel_i810_agp_enable;
    agp_bridge.cache_flush = global_cache_flush;
    agp_bridge.create_gatt_table = agp_generic_create_gatt_table;
    agp_bridge.free_gatt_table = agp_generic_free_gatt_table;
    agp_bridge.insert_memory = intel_i810_insert_entries;
    agp_bridge.remove_memory = intel_i810_remove_entries;
    agp_bridge.alloc_by_type = intel_i810_alloc_by_type;
    agp_bridge.free_by_type = intel_i810_free_by_type;
    agp_bridge.agp_alloc_page = agp_generic_alloc_page;
    agp_bridge.agp_destroy_page = agp_generic_destroy_page;
    agp_bridge.suspend = agp_generic_suspend;
    agp_bridge.resume = agp_generic_resume;
    agp_bridge.cant_use_aperture = 0;

    return 0;
}

static struct aper_size_info_fixed intel_i830_sizes[] = 
{ 
    {128, 32768, 5}, 
    /* The 64M mode still requires a 128k gatt */ 
    {64, 16384, 5} 
}; 
    
static struct _intel_i830_private { 
    struct pci_dev *i830_dev;   /* device one */ 
    volatile u8 *registers; 
    int gtt_entries; 
} intel_i830_private; 
    
static void intel_i830_init_gtt_entries(void) { 
    u16 gmch_ctrl; 
    int gtt_entries; 
    u8 rdct; 
    int local = 0;
    static const int ddt[4] = { 0, 16, 32, 64 }; 
    
    pci_read_config_word(agp_bridge.dev,I830_GMCH_CTRL,&gmch_ctrl); 
    
    if ((agp_bridge.dev->device != PCI_DEVICE_ID_INTEL_830_M_0) &&
        (agp_bridge.dev->device != PCI_DEVICE_ID_INTEL_845_G_0))
    {
        // decoding for anything but 830 M and 845 G
        switch (gmch_ctrl & I855_GMCH_GMS_MASK)
        {
        case I855_GMCH_GMS_STOLEN_1M:
            gtt_entries = MB(1) - KB(132);
            break;
        case I855_GMCH_GMS_STOLEN_4M:
            gtt_entries = MB(4) - KB(132);
            break;
        case I855_GMCH_GMS_STOLEN_8M:
            gtt_entries = MB(8) - KB(132);
            break;
        case I855_GMCH_GMS_STOLEN_16M:
            gtt_entries = MB(16) - KB(132);
            break;
        case I855_GMCH_GMS_STOLEN_32M:
            gtt_entries = MB(32) - KB(132);
            break;
        default:
            gtt_entries = 0;
            break;
        }
    } else {
        // decoding for 830 M and 845 G
        switch (gmch_ctrl & I830_GMCH_GMS_MASK)
        {
        case I830_GMCH_GMS_STOLEN_512:
            gtt_entries = KB(512) - KB(132);
            break;
        case I830_GMCH_GMS_STOLEN_1024:
            gtt_entries = MB(1) - KB(132);
            break;
        case I830_GMCH_GMS_STOLEN_8192:
            gtt_entries = MB(8) - KB(132);
            break;
        case I830_GMCH_GMS_LOCAL:
            rdct = INREG8(intel_i830_private.registers, I830_RDRAM_CHANNEL_TYPE);
            gtt_entries = (I830_RDRAM_ND(rdct) + 1) * MB(ddt[I830_RDRAM_DDT(rdct)]);
            local = 1;
            break;
        default:
            gtt_entries = 0;
            break;
        }
    }

    if (gtt_entries > 0)
    {
        printk(KERN_INFO PFX "Detected %dK %s memory.\n",
            gtt_entries / KB(1), local ? "local" : "stolen");
    }
    else
    {
        printk(KERN_INFO PFX "No pre-allocated video memory detected.\n");
    }

    gtt_entries /= KB(4);
    intel_i830_private.gtt_entries = gtt_entries; 
} 
    
/* The intel i830 automatically initializes the agp aperture during POST. 
 * Use the memory already set aside for in the GTT. 
 */ 
static int intel_i830_create_gatt_table(void) 
{ 
    int page_order; 
    struct aper_size_info_fixed *size; 
    int num_entries; 
    u32 temp; 
    
    size = agp_bridge.current_size; 
    page_order = size->page_order; 
    num_entries = size->num_entries; 
    agp_bridge.gatt_table_real = 0; 
    
    pci_read_config_dword(intel_i830_private.i830_dev,I810_MMADDR,&temp); 
    temp &= 0xfff80000; 
    
    intel_i830_private.registers = (volatile u8 *) ioremap(temp,128 * 4096); 
    if (!intel_i830_private.registers) return (-ENOMEM); 
    
    temp = INREG32(intel_i830_private.registers,I810_PGETBL_CTL) & 0xfffff000; 
    CACHE_FLUSH(); 
    
    /* we have to call this as early as possible after the MMIO base address is known */ 
    intel_i830_init_gtt_entries(); 
    
    agp_bridge.gatt_table = NULL; 
    
    agp_bridge.gatt_bus_addr = temp; 
    
    return(0); 
} 
    
/* Return the gatt table to a sane state. Use the top of stolen 
 * memory for the GTT. 
 */ 
static int intel_i830_free_gatt_table(void) 
{ 
    return(0); 
} 
    
static int intel_i830_fetch_size(void) 
{ 
    u16 gmch_ctrl; 
    struct aper_size_info_fixed *values; 
    
    values = A_SIZE_FIX(agp_bridge.aperture_sizes); 
    
    if ((agp_bridge.dev->device != PCI_DEVICE_ID_INTEL_830_M_0) &&
        (agp_bridge.dev->device != PCI_DEVICE_ID_INTEL_845_G_0))
    {
        // handling for anything but 830 M and 845 G
        agp_bridge.previous_size = agp_bridge.current_size = (void *) values;
        agp_bridge.aperture_size_idx = 0;
        return(values[0].size);
    }

    pci_read_config_word(agp_bridge.dev,I830_GMCH_CTRL,&gmch_ctrl);

    if ((gmch_ctrl & I830_GMCH_MEM_MASK) == I830_GMCH_MEM_128M) { 
        agp_bridge.previous_size = agp_bridge.current_size = (void *) values; 
        agp_bridge.aperture_size_idx = 0; 
        return(values[0].size); 
    } else { 
        agp_bridge.previous_size = agp_bridge.current_size = (void *) values; 
        agp_bridge.aperture_size_idx = 1; 
        return(values[1].size); 
    } 
    
    return(0); 
} 
    
static int intel_i830_configure(void) 
{ 
    struct aper_size_info_fixed *current_size; 
    u32 temp; 
    u16 gmch_ctrl; 
    int i; 
    
    current_size = A_SIZE_FIX(agp_bridge.current_size); 
    
    pci_read_config_dword(intel_i830_private.i830_dev,I810_GMADDR,&temp); 
    agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK); 
    
    pci_read_config_word(agp_bridge.dev,I830_GMCH_CTRL,&gmch_ctrl); 
    gmch_ctrl |= I830_GMCH_ENABLED; 
    pci_write_config_word(agp_bridge.dev,I830_GMCH_CTRL,gmch_ctrl); 
    
    OUTREG32(intel_i830_private.registers,I810_PGETBL_CTL,agp_bridge.gatt_bus_addr | I810_PGETBL_ENABLED); 
    CACHE_FLUSH(); 
    
    if (agp_bridge.needs_scratch_page == TRUE) 
        for (i = intel_i830_private.gtt_entries; i < current_size->num_entries; i++) 
            OUTREG32(intel_i830_private.registers,I810_PTE_BASE + (i * 4),agp_bridge.scratch_page); 
    
    return (0); 
} 
    
static void intel_i830_cleanup(void) 
{ 
    iounmap((void *) intel_i830_private.registers); 
} 
    
static int intel_i830_insert_entries(agp_memory *mem,off_t pg_start,int type) 
{ 
    int i,j,num_entries; 
    void *temp; 
    
    temp = agp_bridge.current_size; 
    num_entries = A_SIZE_FIX(temp)->num_entries; 
    
    if (pg_start < intel_i830_private.gtt_entries) { 
        printk (KERN_DEBUG "pg_start == 0x%.8lx,intel_i830_private.gtt_entries == 0x%.8x\n", 
            pg_start,intel_i830_private.gtt_entries); 
    
        printk ("Trying to insert into local/stolen memory\n"); 
        return (-EINVAL); 
    } 
    
    if ((pg_start + mem->page_count) > num_entries) 
        return (-EINVAL); 
    
    /* The i830 can't check the GTT for entries since its read only, 
     * depend on the caller to make the correct offset decisions. 
     */ 
    
    if ((type != 0 && type != AGP_PHYS_MEMORY) || 
        (mem->type != 0 && mem->type != AGP_PHYS_MEMORY)) 
        return (-EINVAL); 
    
    CACHE_FLUSH(); 
    
    for (i = 0, j = pg_start; i < mem->page_count; i++, j++) 
        OUTREG32(intel_i830_private.registers,I810_PTE_BASE + (j * 4),mem->memory[i]); 
    
    CACHE_FLUSH(); 
    
    agp_bridge.tlb_flush(mem); 
    
    return(0); 
} 
    
static int intel_i830_remove_entries(agp_memory *mem,off_t pg_start,int type) 
{ 
    int i; 
    
    CACHE_FLUSH (); 
    
    if (pg_start < intel_i830_private.gtt_entries) { 
        printk ("Trying to disable local/stolen memory\n"); 
        return (-EINVAL); 
    } 
    
    for (i = pg_start; i < (mem->page_count + pg_start); i++) 
        OUTREG32(intel_i830_private.registers,I810_PTE_BASE + (i * 4),agp_bridge.scratch_page); 
    
    CACHE_FLUSH(); 
    
    agp_bridge.tlb_flush(mem); 
    
    return (0); 
} 
    
static agp_memory *intel_i830_alloc_by_type(size_t pg_count,int type) 
{ 
    agp_memory *nw; 
    
    /* always return NULL for now */ 
    if (type == AGP_DCACHE_MEMORY) return(NULL); 
    
    if (type == AGP_PHYS_MEMORY) { 
        unsigned long physical; 
    
        /* The i830 requires a physical address to program 
         * it's mouse pointer into hardware. However the 
         * Xserver still writes to it through the agp 
         * aperture 
         */ 
    
        if (pg_count != 1) return(NULL); 
    
        nw = agp_create_memory(1); 
    
        if (nw == NULL) return(NULL); 
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) 
        __module_get(THIS_MODULE);
#else
        MOD_INC_USE_COUNT; 
#endif
        nw->memory[0] = agp_bridge.agp_alloc_page(); 
        physical = nw->memory[0]; 
        if (nw->memory[0] == 0) { 
            /* free this structure */ 
            agp_free_memory(nw); 
            return(NULL); 
        } 
    
        nw->memory[0] = agp_bridge.mask_memory(virt_to_phys((void *) nw->memory[0]),type); 
        nw->page_count = 1; 
        nw->num_scratch_pages = 1; 
        nw->type = AGP_PHYS_MEMORY; 
        nw->physical = virt_to_phys((void *) physical); 
        return(nw); 
    } 
    
    return(NULL); 
} 
    
static int __init intel_i830_setup(struct pci_dev *i830_dev) 
{ 
    intel_i830_private.i830_dev = i830_dev; 
    
    agp_bridge.masks = intel_i810_masks; 
    agp_bridge.num_of_masks = 3; 
    agp_bridge.aperture_sizes = (void *) intel_i830_sizes; 
    agp_bridge.size_type = FIXED_APER_SIZE; 
    agp_bridge.num_aperture_sizes = 2; 
    
    agp_bridge.dev_private_data = (void *) &intel_i830_private; 
    agp_bridge.needs_scratch_page = TRUE; 
    
    agp_bridge.configure = intel_i830_configure; 
    agp_bridge.fetch_size = intel_i830_fetch_size; 
    agp_bridge.cleanup = intel_i830_cleanup; 
    agp_bridge.tlb_flush = intel_i810_tlbflush; 
    agp_bridge.mask_memory = intel_i810_mask_memory; 
    agp_bridge.agp_enable = intel_i810_agp_enable; 
    agp_bridge.cache_flush = global_cache_flush; 
    
    agp_bridge.create_gatt_table = intel_i830_create_gatt_table; 
    agp_bridge.free_gatt_table = intel_i830_free_gatt_table; 
    
    agp_bridge.insert_memory = intel_i830_insert_entries; 
    agp_bridge.remove_memory = intel_i830_remove_entries; 
    agp_bridge.alloc_by_type = intel_i830_alloc_by_type; 
    agp_bridge.free_by_type = intel_i810_free_by_type; 
    agp_bridge.agp_alloc_page = agp_generic_alloc_page; 
    agp_bridge.agp_destroy_page = agp_generic_destroy_page; 
    
    agp_bridge.suspend = agp_generic_suspend; 
    agp_bridge.resume = agp_generic_resume; 
    agp_bridge.cant_use_aperture = 0; 
    
    return(0); 
} 


static int intel_fetch_size(void)
{
	int i;
	u16 temp;
	struct aper_size_info_16 *values;

	pci_read_config_word(agp_bridge.dev, INTEL_APSIZE, &temp);
	values = A_SIZE_16(agp_bridge.aperture_sizes);

	for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
		if (temp == values[i].size_value) {
			agp_bridge.previous_size =
			    agp_bridge.current_size = (void *) (values + i);
			agp_bridge.aperture_size_idx = i;
			return values[i].size;
		}
	}

	return 0;
}

static int intel_8xx_fetch_size(void)
{
       int i;
       u8 temp;
       struct aper_size_info_8 *values;

       pci_read_config_byte(agp_bridge.dev, INTEL_APSIZE, &temp);
       values = A_SIZE_8(agp_bridge.aperture_sizes);

       for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
               if (temp == values[i].size_value) {
                       agp_bridge.previous_size =
                           agp_bridge.current_size = (void *) (values + i);
                       agp_bridge.aperture_size_idx = i;
                       return values[i].size;
               }
       }

       return 0;
}

static void intel_tlbflush(agp_memory * mem)
{
	pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x2200); // AGPRSE, GTLB disable/flush
	pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x2280); // AGPRSE, GTLB enable
}

static void intel_8xx_tlbflush(agp_memory * mem)
{
  u32 temp;
  pci_read_config_dword(agp_bridge.dev, INTEL_AGPCTRL, &temp);
  pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, temp & ~(1 << 7));
  pci_read_config_dword(agp_bridge.dev, INTEL_AGPCTRL, &temp);
  pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, temp | (1 << 7));
}

static void intel_cleanup(void)
{
	u16 temp;
	struct aper_size_info_16 *previous_size;

	previous_size = A_SIZE_16(agp_bridge.previous_size);
	pci_read_config_word(agp_bridge.dev, INTEL_NBXCFG, &temp);
	pci_write_config_word(agp_bridge.dev, INTEL_NBXCFG, temp & ~(1 << 9)); // global aperture access disable
	pci_write_config_word(agp_bridge.dev, INTEL_APSIZE,
			      previous_size->size_value);
}

static void intel_8xx_cleanup(void)
{
       u16 temp;
       struct aper_size_info_8 *previous_size;

       previous_size = A_SIZE_8(agp_bridge.previous_size);
       pci_read_config_word(agp_bridge.dev, INTEL_NBXCFG, &temp);
       pci_write_config_word(agp_bridge.dev, INTEL_NBXCFG, temp & ~(1 << 9));
       pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
                             previous_size->size_value);
}

static int intel_configure(void)
{
	u32 temp;
	u16 temp2;
	struct aper_size_info_16 *current_size;

	current_size = A_SIZE_16(agp_bridge.current_size);

	/* aperture size */
	pci_write_config_word(agp_bridge.dev, INTEL_APSIZE,
			      current_size->size_value);

	/* address to map to */
	pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

	/* attbase - aperture base */
	pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
			       agp_bridge.gatt_bus_addr);

	/* agpctrl */
	pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x2280); // AGPRSE, GTLB enable

	/* paccfg/nbxcfg */
	pci_read_config_word(agp_bridge.dev, INTEL_NBXCFG, &temp2);
	pci_write_config_word(agp_bridge.dev, INTEL_NBXCFG,
			      (temp2 & ~(1 << 10)) | (1 << 9)); // PCI Agent to Aperture Access enable, global aperture access enable
	/* clear any possible error conditions */
	pci_write_config_byte(agp_bridge.dev, INTEL_ERRSTS + 1, 7); // OOGF, IAAF, AIGATT
	return 0;
}

static void intel_820_tlbflush(agp_memory * mem)
{
    return;
}

static void intel_820_cleanup(void)
{
    u8 temp;
    struct aper_size_info_8 *previous_size;

    previous_size = A_SIZE_8(agp_bridge.previous_size);
    pci_read_config_byte(agp_bridge.dev, INTEL_I820_RDCR, &temp);
    pci_write_config_byte(agp_bridge.dev, INTEL_I820_RDCR, 
                          temp & ~(1 << 1));
    pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
                          previous_size->size_value);
}

#if 0
#ifdef FIREGL_AGP_HOOK
static int intel_820_configure(void)
{
	u32 temp;
	u16 temp2;
	struct aper_size_info_16 *current_size;

	current_size = A_SIZE_16(agp_bridge.current_size);

	/* aperture size */
	pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
			      (char)current_size->size_value); 

	/* address to map to */
	pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

	/* attbase - aperture base */
	pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
			       agp_bridge.gatt_bus_addr); 

	/* agpctrl */
	pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x0000); // GTLB flush/disable

	/* mcgcfg */
	pci_read_config_word(agp_bridge.dev, INTEL_I820_MCHCFG, &temp2);
	pci_write_config_word(agp_bridge.dev, INTEL_I820_MCHCFG,
			      temp2 | (1 << 9)); // global aperture access enable
	/* clear any possible error conditions */
	pci_write_config_word(agp_bridge.dev, INTEL_I820_ERRSTS, 0xc000); // FSBPAR, AGPERR
	return 0;
}
#endif /* FIREGL_AGP_HOOK */
#else
static int intel_820_configure(void)
{
    u32 temp;
    u8 temp2; 
    struct aper_size_info_8 *current_size;

    current_size = A_SIZE_8(agp_bridge.current_size);

    /* aperture size */
    pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
                          current_size->size_value); 

    /* address to map to */
    pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
    agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

    /* attbase - aperture base */
    pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
                           agp_bridge.gatt_bus_addr); 

    /* agpctrl */
    pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x0000); 

    /* global enable aperture access */
    /* This flag is not accessed through MCHCFG register as in */
    /* i850 chipset. */
    pci_read_config_byte(agp_bridge.dev, INTEL_I820_RDCR, &temp2);
    pci_write_config_byte(agp_bridge.dev, INTEL_I820_RDCR, 
                          temp2 | (1 << 1));
    /* clear any possible AGP-related error conditions */
    pci_write_config_word(agp_bridge.dev, INTEL_I820_ERRSTS, 0x001c); // OOGF, IAAF, ITTEF
    return 0;
}
#endif

static int intel_830mp_configure(void)
{
    u32 temp;
    u16 temp2;
    struct aper_size_info_8 *current_size;
 
    current_size = A_SIZE_8(agp_bridge.current_size);
 
    /* aperture size */
    pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
                          current_size->size_value);
 
    /* address to map to */
    pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
    agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);
 
    /* attbase - aperture base */
    pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
                           agp_bridge.gatt_bus_addr);
 
    /* agpctrl */
    pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x0000);
 
    /* gmch */
    pci_read_config_word(agp_bridge.dev, INTEL_NBXCFG, &temp2);
    pci_write_config_word(agp_bridge.dev, INTEL_NBXCFG,
                          temp2 | (1 << 9));

    /* clear any possible AGP-related error conditions */
    pci_write_config_word(agp_bridge.dev, INTEL_I830_ERRSTS, 0x1c);

    return 0;
}

static int intel_840_configure(void)
{
	u32 temp;
	u16 temp2;
	struct aper_size_info_8 *current_size;

	current_size = A_SIZE_8(agp_bridge.current_size);

	/* aperture size */
	pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
			      current_size->size_value); 

	/* address to map to */
	pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

	/* attbase - aperture base */
	pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
			       agp_bridge.gatt_bus_addr); 

	/* agpctrl */
	pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x0000); // GTLB flush/disable

	/* mcgcfg */
	pci_read_config_word(agp_bridge.dev, INTEL_I840_MCHCFG, &temp2);
	pci_write_config_word(agp_bridge.dev, INTEL_I840_MCHCFG,
			      temp2 | (1 << 9)); // global aperture access enable
	/* clear any possible error conditions */
	pci_write_config_word(agp_bridge.dev, INTEL_I840_ERRSTS, 0xc000); // FSBPAR, AGPERR
	return 0;
}

static int intel_845_configure(void)
{
    u32 temp;
    u8 temp2;
    struct aper_size_info_8 *current_size;

    current_size = A_SIZE_8(agp_bridge.current_size);

    /* aperture size */
    pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
                          current_size->size_value); 

    /* address to map to */
    pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
    agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

    /* attbase - aperture base */
    pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
                           agp_bridge.gatt_bus_addr); 

    /* agpctrl */
    pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x0000); 

    /* agpm */
    pci_read_config_byte(agp_bridge.dev, INTEL_I845_AGPM, &temp2);
    pci_write_config_byte(agp_bridge.dev, INTEL_I845_AGPM,
                          temp2 | (1 << 1));
    /* clear any possible error conditions */
    pci_write_config_word(agp_bridge.dev, INTEL_I845_ERRSTS, 0x001c); 
    return 0;
}

static void intel_845_resume(void)
{
   intel_845_configure();
}

static int intel_850_configure(void)
{
	u32 temp;
	u16 temp2;
	struct aper_size_info_8 *current_size;

	current_size = A_SIZE_8(agp_bridge.current_size);

	/* aperture size */
	pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
			      current_size->size_value); 

	/* address to map to */
	pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

	/* attbase - aperture base */
	pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
			       agp_bridge.gatt_bus_addr); 

	/* agpctrl */
	pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x0000); // GTLB flush/disable

	/* mcgcfg */
	pci_read_config_word(agp_bridge.dev, INTEL_I850_MCHCFG, &temp2);
	pci_write_config_word(agp_bridge.dev, INTEL_I850_MCHCFG,
			      temp2 | (1 << 9)); // global aperture access enable
	/* clear any possible AGP-related error conditions */
	pci_write_config_word(agp_bridge.dev, INTEL_I850_ERRSTS, 0x001c); // OOGF, IAAF, ITTEF
	return 0;
}

static int intel_860_configure(void)
{
    u32 temp;
    u16 temp2;
    struct aper_size_info_8 *current_size;

    current_size = A_SIZE_8(agp_bridge.current_size);

    /* aperture size */
    pci_write_config_byte(agp_bridge.dev, INTEL_APSIZE,
                          current_size->size_value);

    /* address to map to */
    pci_read_config_dword(agp_bridge.dev, INTEL_APBASE, &temp);
    agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

    /* attbase - aperture base */
    pci_write_config_dword(agp_bridge.dev, INTEL_ATTBASE,
                           agp_bridge.gatt_bus_addr);

    /* agpctrl */
    pci_write_config_dword(agp_bridge.dev, INTEL_AGPCTRL, 0x0000);

    /* mcgcfg */
    pci_read_config_word(agp_bridge.dev, INTEL_I860_MCHCFG, &temp2);
    pci_write_config_word(agp_bridge.dev, INTEL_I860_MCHCFG,
                          temp2 | (1 << 9));
    /* clear any possible AGP-related error conditions */
    pci_write_config_word(agp_bridge.dev, INTEL_I860_ERRSTS, 0xf700);
    return 0;
}

static unsigned long intel_mask_memory(unsigned long addr, int type)
{
	/* Memory type is ignored */

	return addr | agp_bridge.masks[0].mask;
}

static void intel_resume(void)
{
    intel_configure();
}

/* Setup function */
static struct gatt_mask intel_generic_masks[] =
{
	{0x00000017, 0}
};

static struct aper_size_info_8 intel_8xx_sizes[7] =
{
    {256, 65536, 6, 0},
    {128, 32768, 5, 32},
    {64, 16384, 4, 48},
    {32, 8192, 3, 56},
    {16, 4096, 2, 60},
    {8, 2048, 1, 62},
    {4, 1024, 0, 63}
};

static struct aper_size_info_16 intel_generic_sizes[7] =
{
	{256, 65536, 6, 0},
	{128, 32768, 5, 32},
	{64, 16384, 4, 48},
	{32, 8192, 3, 56},
	{16, 4096, 2, 60},
	{8, 2048, 1, 62},
	{4, 1024, 0, 63}
};

static struct aper_size_info_8 intel_830mp_sizes[4] =
{
  {256, 65536, 6, 0},
  {128, 32768, 5, 32},
  {64, 16384, 4, 48},
  {32, 8192, 3, 56}
};

int __init intel_generic_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = intel_generic_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) intel_generic_sizes;
	agp_bridge.size_type = U16_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = intel_configure;
	agp_bridge.fetch_size = intel_fetch_size;
	agp_bridge.cleanup = intel_cleanup;
	agp_bridge.tlb_flush = intel_tlbflush;
	agp_bridge.mask_memory = intel_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
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
    agp_bridge.resume = intel_resume;
    agp_bridge.cant_use_aperture = 0;

    /* try determining the matching AGP bus bridge device - not really needed */
    {
        struct pci_dev *sub_dev;

        sub_dev =
            pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443GX_AGP_0, NULL);
		if (sub_dev != NULL)
            agp_bridge_pci2pci.dev = sub_dev;
#if 0
        else
        {
        sub_dev =
            pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_xxx_AGP_0, NULL);
        if /* ... LINUX_TODO */
            agp_bridge_pci2pci.dev = sub_dev;
        }
#endif
    }

	return 0;
	
	(void) pdev; /* unused */
}

#if 0
#ifdef FIREGL_AGP_HOOK
static int __init intel_820_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = intel_generic_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) intel_generic_sizes;
	agp_bridge.size_type = U16_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = intel_820_configure;
	agp_bridge.fetch_size = intel_fetch_size;
	agp_bridge.cleanup = intel_820_cleanup; // bugfix? intel_cleanup;
	agp_bridge.tlb_flush = intel_tlbflush;
	agp_bridge.mask_memory = intel_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
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
	
	(void) pdev; /* unused */
}
#endif // FIREGL_AGP_HOOK
#else
static int __init intel_820_setup (struct pci_dev *pdev)
{
    agp_bridge.masks = intel_generic_masks;
    agp_bridge.num_of_masks = 1;
    agp_bridge.aperture_sizes = (void *) intel_8xx_sizes;
    agp_bridge.size_type = U8_APER_SIZE;
    agp_bridge.num_aperture_sizes = 7;
    agp_bridge.dev_private_data = NULL;
    agp_bridge.needs_scratch_page = FALSE;
    agp_bridge.configure = intel_820_configure;
    agp_bridge.fetch_size = intel_8xx_fetch_size;
	agp_bridge.cleanup = intel_820_cleanup; // bugfix? intel_cleanup;
    agp_bridge.tlb_flush = intel_820_tlbflush;
    agp_bridge.mask_memory = intel_mask_memory;
    agp_bridge.agp_enable = agp_generic_agp_enable;
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

    (void) pdev; /* unused */
}
#endif

static int __init intel_830mp_setup (struct pci_dev *pdev)
{
    agp_bridge.masks = intel_generic_masks;
    agp_bridge.num_of_masks = 1;
    agp_bridge.aperture_sizes = (void *) intel_830mp_sizes;
    agp_bridge.size_type = U8_APER_SIZE;
    agp_bridge.num_aperture_sizes = 4;
    agp_bridge.dev_private_data = NULL;
    agp_bridge.needs_scratch_page = FALSE;
    agp_bridge.configure = intel_830mp_configure;
    agp_bridge.fetch_size = intel_8xx_fetch_size;
    agp_bridge.cleanup = intel_8xx_cleanup;
    agp_bridge.tlb_flush = intel_8xx_tlbflush;
    agp_bridge.mask_memory = intel_mask_memory;
    agp_bridge.agp_enable = agp_generic_agp_enable;
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

    (void) pdev; /* unused */
}

static int __init intel_840_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = intel_generic_masks;
	agp_bridge.num_of_masks = 1;
    agp_bridge.aperture_sizes = (void *) intel_8xx_sizes;
    agp_bridge.size_type = U8_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = intel_840_configure;
    agp_bridge.fetch_size = intel_8xx_fetch_size;
    agp_bridge.cleanup = intel_8xx_cleanup;
    agp_bridge.tlb_flush = intel_8xx_tlbflush;
    agp_bridge.mask_memory = intel_mask_memory;
    agp_bridge.agp_enable = agp_generic_agp_enable;
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

    /* try determining the matching AGP bus bridge device - not really needed */
    {
        struct pci_dev *sub_dev;

        sub_dev =
            pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_840_AGP_0, NULL);
		if (sub_dev != NULL)
            agp_bridge_pci2pci.dev = sub_dev;
    }

    return 0;
       
    (void) pdev; /* unused */
}

static int __init intel_845_setup (struct pci_dev *pdev)
{
    agp_bridge.masks = intel_generic_masks;
    agp_bridge.num_of_masks = 1;
    agp_bridge.aperture_sizes = (void *) intel_8xx_sizes;
    agp_bridge.size_type = U8_APER_SIZE;
    agp_bridge.num_aperture_sizes = 7;
    agp_bridge.dev_private_data = NULL;
    agp_bridge.needs_scratch_page = FALSE;
    agp_bridge.configure = intel_845_configure;
    agp_bridge.fetch_size = intel_8xx_fetch_size;
    agp_bridge.cleanup = intel_8xx_cleanup;
    agp_bridge.tlb_flush = intel_8xx_tlbflush;
	agp_bridge.mask_memory = intel_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
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
    agp_bridge.resume = intel_845_resume;
    agp_bridge.cant_use_aperture = 0;

	return 0;
	
	(void) pdev; /* unused */
}

static int __init intel_850_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = intel_generic_masks;
	agp_bridge.num_of_masks = 1;
    agp_bridge.aperture_sizes = (void *) intel_8xx_sizes;
    agp_bridge.size_type = U8_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = intel_850_configure;
    agp_bridge.fetch_size = intel_8xx_fetch_size;
    agp_bridge.cleanup = intel_8xx_cleanup;
    agp_bridge.tlb_flush = intel_8xx_tlbflush;
	agp_bridge.mask_memory = intel_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
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
	
	(void) pdev; /* unused */
}

static int __init intel_860_setup (struct pci_dev *pdev)
{
    agp_bridge.masks = intel_generic_masks;
    agp_bridge.num_of_masks = 1;
    agp_bridge.aperture_sizes = (void *) intel_8xx_sizes;
    agp_bridge.size_type = U8_APER_SIZE;
    agp_bridge.num_aperture_sizes = 7;
    agp_bridge.dev_private_data = NULL;
    agp_bridge.needs_scratch_page = FALSE;
    agp_bridge.configure = intel_860_configure;
    agp_bridge.fetch_size = intel_8xx_fetch_size;
    agp_bridge.cleanup = intel_8xx_cleanup;
    agp_bridge.tlb_flush = intel_8xx_tlbflush;
    agp_bridge.mask_memory = intel_mask_memory;
    agp_bridge.agp_enable = agp_generic_agp_enable;
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

    (void) pdev; /* unused */
}

//================================================================
// VIA

// forward declarions:
static int via_configure(void);

// functions
static int via_fetch_size(void)
{
	int i;
	u8 temp;
	struct aper_size_info_8 *values;

	values = A_SIZE_8(agp_bridge.aperture_sizes);
	pci_read_config_byte(agp_bridge.dev, VIA_APSIZE, &temp);
	for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
		if (temp == values[i].size_value) {
			agp_bridge.previous_size =
			    agp_bridge.current_size = (void *) (values + i);
			agp_bridge.aperture_size_idx = i;
			return values[i].size;
		}
	}

	return 0;
}

static void via_resume(void)
{
    via_configure();
}

static int via_configure(void)
{
	u32 temp;
	struct aper_size_info_8 *current_size;

	current_size = A_SIZE_8(agp_bridge.current_size);
	/* aperture size */
	pci_write_config_byte(agp_bridge.dev, VIA_APSIZE,
			      current_size->size_value);
	/* address to map too */
	pci_read_config_dword(agp_bridge.dev, VIA_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

	/* GART control register */
	pci_write_config_dword(agp_bridge.dev, VIA_GARTCTRL, 0x0000000f);

	/* attbase - aperture GATT base */
	pci_write_config_dword(agp_bridge.dev, VIA_ATTBASE,
			    (agp_bridge.gatt_bus_addr & 0xfffff000) | 3);
	return 0;
}

static void via_cleanup(void)
{
	struct aper_size_info_8 *previous_size;

	previous_size = A_SIZE_8(agp_bridge.previous_size);
	pci_write_config_dword(agp_bridge.dev, VIA_ATTBASE, 0);
	pci_write_config_byte(agp_bridge.dev, VIA_APSIZE,
			      previous_size->size_value);
}

static void via_tlbflush(agp_memory * mem)
{
	pci_write_config_dword(agp_bridge.dev, VIA_GARTCTRL, 0x0000008f);
	pci_write_config_dword(agp_bridge.dev, VIA_GARTCTRL, 0x0000000f);
}

static unsigned long via_mask_memory(unsigned long addr, int type)
{
	/* Memory type is ignored */

	return addr | agp_bridge.masks[0].mask;
}

static struct aper_size_info_8 via_generic_sizes[7] =
{
	{256, 65536, 6, 0},
	{128, 32768, 5, 128},
	{64, 16384, 4, 192},
	{32, 8192, 3, 224},
	{16, 4096, 2, 240},
	{8, 2048, 1, 248},
	{4, 1024, 0, 252}
};

static struct gatt_mask via_generic_masks[] =
{
	{0x00000000, 0}
};

static int __init via_generic_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = via_generic_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) via_generic_sizes;
	agp_bridge.size_type = U8_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = via_configure;
	agp_bridge.fetch_size = via_fetch_size;
	agp_bridge.cleanup = via_cleanup;
	agp_bridge.tlb_flush = via_tlbflush;
	agp_bridge.mask_memory = via_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
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
    agp_bridge.resume = via_resume;
    agp_bridge.cant_use_aperture = 0;

	return 0;
	
	(void) pdev; /* unused */
}

static int via_kt400_fetch_size(void)
{
       int i;
       u16 temp;
       struct aper_size_info_16 *values;

       values = A_SIZE_16(agp_bridge.aperture_sizes);
       pci_read_config_word(agp_bridge.dev, VIA_AGP3_APSIZE, &temp);
       for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
               if (temp == values[i].size_value) {
                       agp_bridge.previous_size =
                       agp_bridge.current_size = (void *) (values + i);
                       agp_bridge.aperture_size_idx = i;
                       return values[i].size;
               }
       }
       return 0;
}

static int via_kt400_configure(void)
{
       u32 temp;
       struct aper_size_info_16 *current_size;

       current_size = A_SIZE_16(agp_bridge.current_size);

       /* address to map too */
       pci_read_config_dword(agp_bridge.dev, VIA_APBASE, &temp);
       agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

       /* attbase - aperture GATT base */
       pci_write_config_dword(agp_bridge.dev, VIA_AGP3_ATTBASE,
               agp_bridge.gatt_bus_addr & 0xfffff000);
       return 0;
}

static void via_kt400_cleanup(void)
{
       struct aper_size_info_16 *previous_size;

       previous_size = A_SIZE_16(agp_bridge.previous_size);
       pci_write_config_word(agp_bridge.dev, VIA_AGP3_APSIZE, previous_size->size_value);
}
       
static void via_kt400_tlbflush(agp_memory * mem)
{
       u32 temp;

       pci_read_config_dword(agp_bridge.dev, VIA_AGP3_GARTCTRL, &temp);
       pci_write_config_dword(agp_bridge.dev, VIA_AGP3_GARTCTRL, temp & ~(1<<7));
       pci_write_config_dword(agp_bridge.dev, VIA_AGP3_GARTCTRL, temp);
}

static struct aper_size_info_16 via_kt400_sizes[11] =
{
       { 4,     1024,  0, 1<<11|1<<10|1<<9|1<<8|1<<5|1<<4|1<<3|1<<2|1<<1|1<<0 },
       { 8,     2048,  1, 1<<11|1<<10|1<<9|1<<8|1<<5|1<<4|1<<3|1<<2|1<<1},
       { 16,    4096,  2, 1<<11|1<<10|1<<9|1<<8|1<<5|1<<4|1<<3|1<<2},
       { 32,    8192,  3, 1<<11|1<<10|1<<9|1<<8|1<<5|1<<4|1<<3},
       { 64,   16384,  4, 1<<11|1<<10|1<<9|1<<8|1<<5|1<<4},
       { 128,  32768,  5, 1<<11|1<<10|1<<9|1<<8|1<<5},
       { 256,  65536,  6, 1<<11|1<<10|1<<9|1<<8},
       { 512,  131072, 7, 1<<11|1<<10|1<<9},
       { 1024, 262144, 8, 1<<11|1<<10},
       { 2048, 524288, 9, 1<<11}       /* 2GB <- Max supported */
};

static int __init via_kt400_setup (struct pci_dev *pdev)
{
    agp_bridge.masks = via_generic_masks;
    agp_bridge.num_of_masks = 1;
    agp_bridge.aperture_sizes = (void *) via_kt400_sizes;
    agp_bridge.size_type = U16_APER_SIZE; /* note: KT400 has 16 bit aperture size coding - this differs from via generic with 8 bit */
    agp_bridge.num_aperture_sizes = 10;
    agp_bridge.dev_private_data = NULL;
    agp_bridge.needs_scratch_page = FALSE;
    agp_bridge.configure = via_kt400_configure;
    agp_bridge.fetch_size = via_kt400_fetch_size;
    agp_bridge.cleanup = via_kt400_cleanup;
    agp_bridge.tlb_flush = via_kt400_tlbflush;
    agp_bridge.mask_memory = via_mask_memory;
    agp_bridge.agp_enable = agp_generic_agp_enable;
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
	
    (void) pdev; /* unused */
}


static int sis_fetch_size(void)
{
	u8 temp_size;
	int i;
	struct aper_size_info_8 *values;

	pci_read_config_byte(agp_bridge.dev, SIS_APSIZE, &temp_size);
	values = A_SIZE_8(agp_bridge.aperture_sizes);
	for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
		if ((temp_size == values[i].size_value) ||
		    ((temp_size & ~(0x03)) ==
		     (values[i].size_value & ~(0x03)))) {
			agp_bridge.previous_size =
			    agp_bridge.current_size = (void *) (values + i);

			agp_bridge.aperture_size_idx = i;
			return values[i].size;
		}
	}

	return 0;
}


static void sis_tlbflush(agp_memory * mem)
{
	pci_write_config_byte(agp_bridge.dev, SIS_TLBFLUSH, 0x02);
}

static int sis_configure(void)
{
	u32 temp;
	struct aper_size_info_8 *current_size;

	current_size = A_SIZE_8(agp_bridge.current_size);
	pci_write_config_byte(agp_bridge.dev, SIS_TLBCNTRL, 0x05);
	pci_read_config_dword(agp_bridge.dev, SIS_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);
	pci_write_config_dword(agp_bridge.dev, SIS_ATTBASE,
			       agp_bridge.gatt_bus_addr);
	pci_write_config_byte(agp_bridge.dev, SIS_APSIZE,
			      current_size->size_value);
	return 0;
}

static void sis_cleanup(void)
{
	struct aper_size_info_8 *previous_size;

	previous_size = A_SIZE_8(agp_bridge.previous_size);
	pci_write_config_byte(agp_bridge.dev, SIS_APSIZE,
			      (previous_size->size_value & ~(0x03)));
}

static unsigned long sis_mask_memory(unsigned long addr, int type)
{
	/* Memory type is ignored */

	return addr | agp_bridge.masks[0].mask;
}

static struct aper_size_info_8 sis_generic_sizes[7] =
{
	{256, 65536, 6, 99},
	{128, 32768, 5, 83},
	{64, 16384, 4, 67},
	{32, 8192, 3, 51},
	{16, 4096, 2, 35},
	{8, 2048, 1, 19},
	{4, 1024, 0, 3}
};

static struct gatt_mask sis_generic_masks[] =
{
	{0x00000000, 0}
};

static int __init sis_generic_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = sis_generic_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) sis_generic_sizes;
	agp_bridge.size_type = U8_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = sis_configure;
	agp_bridge.fetch_size = sis_fetch_size;
	agp_bridge.cleanup = sis_cleanup;
	agp_bridge.tlb_flush = sis_tlbflush;
	agp_bridge.mask_memory = sis_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
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

typedef struct _amd_page_map {
	unsigned long *real;
	unsigned long *remapped;
} amd_page_map;

static struct _amd_irongate_private {
	volatile u8 *registers;
	amd_page_map **gatt_pages;
	int num_tables;
#ifdef FGL
    u16 saved_gartenable_reg;
    u32 saved_apsize_reg;
#endif
} amd_irongate_private;

static int amd_create_page_map(amd_page_map *page_map)
{
    int i;
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    int err;
#endif

    page_map->real = (unsigned long *) __get_free_page(GFP_KERNEL);
    if (page_map->real == NULL) {
        return -ENOMEM;
    }
#ifdef AGPGART_2_2
    set_bit(PG_reserved, &mem_map[MAP_NR(page_map->real)].flags);
    page_map->remapped = page_map->real;

    for(i = 0; i < PAGE_SIZE / sizeof(unsigned long); i++) {
        page_map->remapped[i] = agp_bridge.scratch_page;
    }
    CACHE_FLUSH();
#else /* !AGPGART_2_2 */
    set_bit(PG_reserved, &virt_to_page((unsigned long)page_map->real)->flags);

    CACHE_FLUSH();
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    err = change_page_attr(virt_to_page(page_map->real), 1, PAGE_KERNEL_NOCACHE);
    if (!err)
#endif
    {
        page_map->remapped = ioremap_nocache(virt_to_phys(page_map->real), 
            PAGE_SIZE);
    }

    if ( (page_map->remapped == NULL)
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
        || (err)
#endif
        )
    {
        clear_bit(PG_reserved, 
            &virt_to_page((unsigned long)page_map->real)->flags);
        free_page((unsigned long) page_map->real);
        page_map->real = NULL;
        return -ENOMEM;
    }
    CACHE_FLUSH();

    for(i = 0; i < PAGE_SIZE / sizeof(unsigned long); i++)
    {
        page_map->remapped[i] = agp_bridge.scratch_page;
	}
#endif /* !AGPGART_2_2 */

    return 0;
}

static void amd_free_page_map(amd_page_map *page_map)
{
#ifdef AGPGART_2_2
	clear_bit(PG_reserved, 
		  &mem_map[MAP_NR(page_map->real)].flags);
#else /* !AGPGART_2_2 */
	iounmap(page_map->remapped);
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    change_page_attr(virt_to_page(page_map->real), 1, PAGE_KERNEL);
#endif
 	clear_bit(PG_reserved, 
		  &virt_to_page((unsigned long)page_map->real)->flags);
#endif /* !AGPGART_2_2 */
	free_page((unsigned long) page_map->real);

#ifdef FGL
    page_map->remapped = 0;
    page_map->real = 0;
#endif
}

static void amd_free_gatt_pages(void)
{
	int i;
	amd_page_map **tables;
	amd_page_map *entry;

	tables = amd_irongate_private.gatt_pages;
	for(i = 0; i < amd_irongate_private.num_tables; i++) {
		entry = tables[i];
		if (entry != NULL) {
			if (entry->real != NULL) {
				amd_free_page_map(entry);
			}
			kfree(entry);
		}
	}
	kfree(tables);
#ifdef FGL
	amd_irongate_private.num_tables = 0;
	amd_irongate_private.gatt_pages = 0;
#endif

}

static int amd_create_gatt_pages(int nr_tables)
{
	amd_page_map **tables;
	amd_page_map *entry;
	int retval = 0;
	int i;

	tables = kmalloc((nr_tables + 1) * sizeof(amd_page_map *), 
			 GFP_KERNEL);
	if (tables == NULL) {
		return -ENOMEM;
	}
	memset(tables, 0, sizeof(amd_page_map *) * (nr_tables + 1));
	for (i = 0; i < nr_tables; i++) {
		entry = kmalloc(sizeof(amd_page_map), GFP_KERNEL);
		if (entry == NULL) {
			retval = -ENOMEM;
			break;
		}
		memset(entry, 0, sizeof(amd_page_map));
		tables[i] = entry;
		retval = amd_create_page_map(entry);
		if (retval != 0) break;
	}
    // even partial allocations require private setup for a propper call to free
	amd_irongate_private.num_tables = nr_tables;
	amd_irongate_private.gatt_pages = tables;

	if (retval != 0) amd_free_gatt_pages();

	return retval;
}

/* Since we don't need contigious memory we just try
 * to get the gatt table once
 */

#define GET_PAGE_DIR_OFF(addr) (addr >> 22)
#define GET_PAGE_DIR_IDX(addr) (GET_PAGE_DIR_OFF(addr) - \
	GET_PAGE_DIR_OFF(agp_bridge.gart_bus_addr))
#define GET_GATT_OFF(addr) ((addr & 0x003ff000) >> 12) 
#define GET_GATT(addr) (amd_irongate_private.gatt_pages[\
	GET_PAGE_DIR_IDX(addr)]->remapped)

static int amd_create_gatt_table(void)
{
	struct aper_size_info_lvl2 *value;
	amd_page_map page_dir;
	unsigned long addr;
	int retval;
	u32 temp;
	int i;

	value = A_SIZE_LVL2(agp_bridge.current_size);

    // reserve a single page directory
	retval = amd_create_page_map(&page_dir);
	if (retval != 0) {
		return retval;
	}

    // reserve sufficient gatt (L2) page directories (each with 1024 entries)
    // (entry value is not fractional, so wen dont need to add anything)
	retval = amd_create_gatt_pages(value->num_entries / 1024);
	if (retval != 0) {
		amd_free_page_map(&page_dir);
		return retval;
	}

	agp_bridge.gatt_table_real = (u32*)page_dir.real;
	agp_bridge.gatt_table = (u32*)page_dir.remapped;
	agp_bridge.gatt_bus_addr = virt_to_bus(page_dir.real);

	/* Get the address for the gart region.
	 * This is a bus address even on the alpha, b/c its
	 * used to program the agp master not the cpu
	 */

	pci_read_config_dword(agp_bridge.dev, AMD_APBASE, &temp);
	addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);
	agp_bridge.gart_bus_addr = addr;

	/* For each gatt page table calculate its physical address 
       and enter this value into the respective entry of the page directory */
	for(i = 0; i < value->num_entries / 1024; i++, addr += 0x00400000) {
		page_dir.remapped[GET_PAGE_DIR_OFF(addr)] =
			virt_to_bus(amd_irongate_private.gatt_pages[i]->real);
		page_dir.remapped[GET_PAGE_DIR_OFF(addr)] |= 0x00000001; // set valid bit
	}
#ifdef FGL_MORE_FLUSH
    // better flush CPU caches once more so that above data goes to memory
	CACHE_FLUSH();
#endif

	return 0;
}

static int amd_free_gatt_table(void)
{
	amd_page_map page_dir;
   
	page_dir.real = (unsigned long *) agp_bridge.gatt_table_real;
	page_dir.remapped = (unsigned long *) agp_bridge.gatt_table;

	amd_free_gatt_pages();
	amd_free_page_map(&page_dir);
#ifdef FGL
	agp_bridge.gatt_table_real = 0;
	agp_bridge.gatt_table = 0;
#endif
	return 0;
}

static int amd_irongate_fetch_size(void)
{
	int i;
	u32 temp;
	struct aper_size_info_lvl2 *values;

	pci_read_config_dword(agp_bridge.dev, AMD_APSIZE, &temp);
	temp = (temp & 0x0000000e); // keep only the encoded size bits
	values = A_SIZE_LVL2(agp_bridge.aperture_sizes);
	for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
		if (temp == values[i].size_value) {
			agp_bridge.previous_size =
			    agp_bridge.current_size = (void *) (values + i);

			agp_bridge.aperture_size_idx = i;
			return values[i].size;
		}
	}

	return 0;
}

static int amd_irongate_configure(void)
{
	struct aper_size_info_lvl2 *current_size;
	u32 temp;
	u16 enable_reg;

	current_size = A_SIZE_LVL2(agp_bridge.current_size);

#ifdef DEBUG
	pci_read_config_dword(agp_bridge.dev, AMD_APBASE, &temp);
    printk(KERN_DEBUG PFX "AMD_APBASE=%08x\n",temp);
#endif
	/* Get the memory mapped registers and map them into kernel address space */
	pci_read_config_dword(agp_bridge.dev, AMD_MMBASE, &temp);
#ifdef DEBUG
    printk(KERN_DEBUG PFX "AMD_MMBASE=%08x\n",temp);
#endif
	temp = (temp & PCI_BASE_ADDRESS_MEM_MASK);
	amd_irongate_private.registers = (volatile u8 *) ioremap(temp, 4096);
#ifdef FGL
    if( !amd_irongate_private.registers )
        return -1;
#endif
#ifdef DEBUG
    {
        u32 i;
        printk(KERN_DEBUG PFX "AMD irongate registerdump at configure (ptr=%p)\n",amd_irongate_private.registers);
        for(i=0; i<0x40; i+=0x10)
            printk(KERN_DEBUG PFX "%02x: %02x%02x%02x%02x, %02x%02x%02x%02x, %02x%02x%02x%02x, %02x%02x%02x%02x\n",
                i,
                amd_irongate_private.registers[i+0x3],
                amd_irongate_private.registers[i+0x2],
                amd_irongate_private.registers[i+0x1],
                amd_irongate_private.registers[i+0x0],
                amd_irongate_private.registers[i+0x7],
                amd_irongate_private.registers[i+0x6],
                amd_irongate_private.registers[i+0x5],
                amd_irongate_private.registers[i+0x4],
                amd_irongate_private.registers[i+0xb],
                amd_irongate_private.registers[i+0x1],
                amd_irongate_private.registers[i+0x9],
                amd_irongate_private.registers[i+0x8],
                amd_irongate_private.registers[i+0xf],
                amd_irongate_private.registers[i+0xe],
                amd_irongate_private.registers[i+0xd],
                amd_irongate_private.registers[i+0xc]
                );
    }
#endif

#ifdef FGL

    temp = INREG32(amd_irongate_private.registers, 0x00) 
         & INREG32(amd_irongate_private.registers, 0x04) 
         & INREG32(amd_irongate_private.registers, 0x08) 
         & INREG32(amd_irongate_private.registers, 0x0c);

    if( temp == 0xffffffff ) /* no device present on CPU bus */
    {
        printk(KERN_DEBUG PFX "MMIO register access failed - values not plausible\n");
        iounmap((void *) amd_irongate_private.registers);
        return -1;
    }
#endif

#ifdef FGL
    pci_read_config_dword(agp_bridge.dev, AMD_APSIZE,
        &amd_irongate_private.saved_apsize_reg);
#endif

	/* Write out the address of the gatt table */
	OUTREG32(amd_irongate_private.registers, AMD_ATTBASE,
		 agp_bridge.gatt_bus_addr);

	/* Write the Sync register */
	pci_write_config_byte(agp_bridge.dev, AMD_MODECNTL, 0x80);
   
   	/* Set indexing mode */
   	pci_write_config_byte(agp_bridge.dev, AMD_MODECNTL2, 0x00);

#ifdef FGL
	amd_irongate_private.saved_gartenable_reg = 
        INREG16(amd_irongate_private.registers, AMD_GARTENABLE);
#endif
	/* Write the enable register */
	enable_reg = INREG16(amd_irongate_private.registers, AMD_GARTENABLE);
	enable_reg = (enable_reg | 0x0004);
    OUTREG16(amd_irongate_private.registers, AMD_GARTENABLE, enable_reg);

	/* Write out the size register (its rather a PC-BIOS task) */
	pci_read_config_dword(agp_bridge.dev, AMD_APSIZE, &temp);
	temp = (temp & ~(0x0000000e)) // clear original GART address space size bits
        | current_size->size_value // set new GART address space size bits
		| 0x00000001; // set GART Enable bit
	pci_write_config_dword(agp_bridge.dev, AMD_APSIZE, temp);

	/* Flush the tlb */
	OUTREG32(amd_irongate_private.registers, AMD_TLBFLUSH, 0x00000001);

	return 0;
}

static void amd_irongate_cleanup(void)
{
#ifdef FGL
    /* disable GART by setting the one and only invalid base address */
	OUTREG32(amd_irongate_private.registers, AMD_ATTBASE, 0);

    /* restore previousely present enable state in MMIO registers */    
	OUTREG16(amd_irongate_private.registers, AMD_GARTENABLE,
        amd_irongate_private.saved_gartenable_reg);
        
	/* Write back previous size and set BAR enable state to former value */
	pci_write_config_dword(agp_bridge.dev, AMD_APSIZE,
        amd_irongate_private.saved_apsize_reg);
#else
	struct aper_size_info_lvl2 *previous_size;
	u32 temp;
	u16 enable_reg;

	previous_size = A_SIZE_LVL2(agp_bridge.previous_size);
    
	enable_reg = INREG16(amd_irongate_private.registers, AMD_GARTENABLE);
	enable_reg = (enable_reg & ~(0x0004));
	OUTREG16(amd_irongate_private.registers, AMD_GARTENABLE, enable_reg);
    
	/* Write back the previous size and disable gart translation */
	pci_read_config_dword(agp_bridge.dev, AMD_APSIZE, &temp);
	temp = (temp & ~(0x0000000f)) // clear used GART size, clear GART enable
        | previous_size->size_value; // restore previous GART size, NO 'GART enable'!
	pci_write_config_dword(agp_bridge.dev, AMD_APSIZE, temp);
#endif

#ifdef DEBUG
    {
        u32 i;
        printk(KERN_DEBUG PFX "AMD irongate registerdump at cleanup (ptr=%p)\n",amd_irongate_private.registers);
        for(i=0; i<0x40; i+=0x10)
            printk(KERN_DEBUG PFX "%02x: %02x%02x%02x%02x, %02x%02x%02x%02x, %02x%02x%02x%02x, %02x%02x%02x%02x\n",
                i,
                amd_irongate_private.registers[i+0x3],
                amd_irongate_private.registers[i+0x2],
                amd_irongate_private.registers[i+0x1],
                amd_irongate_private.registers[i+0x0],
                amd_irongate_private.registers[i+0x7],
                amd_irongate_private.registers[i+0x6],
                amd_irongate_private.registers[i+0x5],
                amd_irongate_private.registers[i+0x4],
                amd_irongate_private.registers[i+0xb],
                amd_irongate_private.registers[i+0xa],
                amd_irongate_private.registers[i+0x9],
                amd_irongate_private.registers[i+0x8],
                amd_irongate_private.registers[i+0xf],
                amd_irongate_private.registers[i+0xe],
                amd_irongate_private.registers[i+0xd],
                amd_irongate_private.registers[i+0xc]
                );
    }
#endif

	iounmap((void *) amd_irongate_private.registers);
#ifdef FGL
    amd_irongate_private.registers = 0;
#endif
}

/*
 * This routine could be implemented by taking the addresses
 * written to the GATT, and flushing them individually.  However
 * currently it just flushes the whole table.  Which is probably
 * more efficent, since agp_memory blocks can be a large number of
 * entries.
 */

static void amd_irongate_tlbflush(agp_memory * temp)
{
#ifdef FGL_PATCH_IRONGATE_FLUSH
    u32 uValue;
    unsigned long end;
    int i;

    end = jiffies + (HZ*3);
#endif
    OUTREG32(amd_irongate_private.registers, AMD_TLBFLUSH, 0x00000001);
#ifdef FGL_PATCH_IRONGATE_FLUSH
    do {
        uValue = INREG32(amd_irongate_private.registers, AMD_TLBFLUSH);
        for(i = 0; i < 4096; i++)
            /* empty */;
    } while((uValue & 0x00000001) || ((signed)(end - jiffies) <= 0));
#endif
}

static unsigned long amd_irongate_mask_memory(unsigned long addr, int type)
{
	/* Only type 0 is supported by the irongate */

	return addr | agp_bridge.masks[0].mask;
}

static int amd_insert_memory(agp_memory * mem,
			     off_t pg_start, int type)
{
	int i, j, num_entries;
	unsigned long *cur_gatt;
	unsigned long addr;

	num_entries = A_SIZE_LVL2(agp_bridge.current_size)->num_entries;

	if (type != 0 || mem->type != 0) {
		return -EINVAL;
	}
	if ((pg_start + mem->page_count) > num_entries) {
		return -EINVAL;
	}

    // verify by meance of page tables if any page in the specified range is availabel
	j = pg_start;
	while (j < (pg_start + mem->page_count)) {
		addr = (j * PAGE_SIZE) + agp_bridge.gart_bus_addr;
		cur_gatt = GET_GATT(addr);
		if (!PGE_EMPTY(cur_gatt[GET_GATT_OFF(addr)])) {
			return -EBUSY;
		}
		j++;
	}

#ifndef AGPGART_2_2
	if (mem->is_flushed == FALSE) {
		CACHE_FLUSH();
		mem->is_flushed = TRUE;
	}
#endif /* !AGPGART_2_2 */

    // allocate the previousely verified page range
	for (i = 0, j = pg_start; i < mem->page_count; i++, j++) {
		addr = (j * PAGE_SIZE) + agp_bridge.gart_bus_addr;
		cur_gatt = GET_GATT(addr);
		cur_gatt[GET_GATT_OFF(addr)] = mem->memory[i];
	}
	
#ifdef FGL_MORE_FLUSH
    // we have modified the page dirs - we better flush anything from caches
    // even if we are somewhat sure by our allocation sheme that the
    // CPU already does no chaching on the page tables.
    CACHE_FLUSH();
#else
#ifdef AGPGART_2_2
    CACHE_FLUSH();
#endif /* AGPGART_2_2 */
#endif
	agp_bridge.tlb_flush(mem);
	return 0;
}

static int amd_remove_memory(agp_memory * mem, off_t pg_start,
			     int type)
{
	int i;
	unsigned long *cur_gatt;
	unsigned long addr;

	if (type != 0 || mem->type != 0) {
		return -EINVAL;
	}

    // free the specified range of pages
	for (i = pg_start; i < (mem->page_count + pg_start); i++) {
		addr = (i * PAGE_SIZE) + agp_bridge.gart_bus_addr;
		cur_gatt = GET_GATT(addr);
		cur_gatt[GET_GATT_OFF(addr)] = 
			(unsigned long) agp_bridge.scratch_page;
	}

#ifdef FGL_MORE_FLUSH
    // we have modified the page dirs - we better flush anything from caches
    // even if we are somewhat sure by our allocation sheme that the
    // CPU already does no chaching on the page tables.
    CACHE_FLUSH();
#else
#ifdef AGPGART_2_2
    CACHE_FLUSH();
#endif /* AGPGART_2_2 */
#endif
	agp_bridge.tlb_flush(mem);
	return 0;
}

static struct aper_size_info_lvl2 amd_irongate_sizes[7] =
{
//   MByte pages   bits on chipset
	{2048, 524288, 0x0000000c},
	{1024, 262144, 0x0000000a},
	{512, 131072, 0x00000008},
	{256, 65536, 0x00000006},
	{128, 32768, 0x00000004},
	{64, 16384, 0x00000002},
	{32, 8192, 0x00000000}
};

static struct gatt_mask amd_irongate_masks[] =
{
	{0x00000001, 0}
};

static int __init amd_irongate_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = amd_irongate_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) amd_irongate_sizes;
	agp_bridge.size_type = LVL2_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = (void *) &amd_irongate_private;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = amd_irongate_configure;
	agp_bridge.fetch_size = amd_irongate_fetch_size;
	agp_bridge.cleanup = amd_irongate_cleanup;
	agp_bridge.tlb_flush = amd_irongate_tlbflush;
	agp_bridge.mask_memory = amd_irongate_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
	agp_bridge.cache_flush = global_cache_flush;
	agp_bridge.create_gatt_table = amd_create_gatt_table;
	agp_bridge.free_gatt_table = amd_free_gatt_table;
	agp_bridge.insert_memory = amd_insert_memory;
	agp_bridge.remove_memory = amd_remove_memory;
	agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
	agp_bridge.free_by_type = agp_generic_free_by_type;
	agp_bridge.agp_alloc_page = agp_generic_alloc_page;
	agp_bridge.agp_destroy_page = agp_generic_destroy_page;
    agp_bridge.suspend = agp_generic_suspend;
    agp_bridge.resume = agp_generic_resume;
    agp_bridge.cant_use_aperture = 0;

	return 0;
	
	(void) pdev; /* unused */
}

/* Begin AMD-8151 support */
/* This is misnamed. It is really a generic AMD K8 Northbridge AMD driver. */

static int x86_64_insert_memory(agp_memory * mem, off_t pg_start, int type)
{
	int i, j, num_entries;
	void *temp;
	long tmp;
	u32 pte;

	temp = agp_bridge.current_size;

	num_entries = A_SIZE_32(temp)->num_entries;

	num_entries -= agp_memory_reserved>>PAGE_SHIFT;

	if (type != 0 || mem->type != 0)
		return -EINVAL;

	/* Make sure we can fit the range in the gatt table. */
	if ((pg_start + mem->page_count) > num_entries)
		return -EINVAL;

	j = pg_start;

	/* gatt table should be empty. */
	while (j < (pg_start + mem->page_count)) {
		if (!PGE_EMPTY(agp_bridge.gatt_table[j]))
			return -EBUSY;
		j++;
	}

	if (mem->is_flushed == FALSE) {
		CACHE_FLUSH();
		mem->is_flushed = TRUE;
	}

	for (i = 0, j = pg_start; i < mem->page_count; i++, j++) {
		tmp = agp_bridge.mask_memory(mem->memory[i], mem->type);

        // TODO: Johannes must find this BUG_ON routine/macro
		//BUG_ON(tmp & 0xffffff0000000ffc);
		pte = (tmp & 0x000000ff00000000LL) >> 28;
		pte |=(tmp & 0x00000000fffff000LL);
		pte |= 1<<1|1<<0;

		agp_bridge.gatt_table[j] = pte;
	}
	printk (KERN_INFO "AGP: inserted %d mappings into gatt_table\n", j);

	agp_bridge.tlb_flush(mem);
	return 0;
}

/*
 * This hack alters the order element according
 * to the size of a long. It sucks. I totally disown this, even
 * though it does appear to work for the most part.
 */
static struct aper_size_info_32 x86_64_aperture_sizes[7] =
{
	{32,   8192,   3+(sizeof(long)/8), 0 },
	{64,   16384,  4+(sizeof(long)/8), 1<<1 },
	{128,  32768,  5+(sizeof(long)/8), 1<<2 },
	{256,  65536,  6+(sizeof(long)/8), 1<<1 | 1<<2 },
	{512,  131072, 7+(sizeof(long)/8), 1<<3 },
	{1024, 262144, 8+(sizeof(long)/8), 1<<1 | 1<<3},
	{2048, 524288, 9+(sizeof(long)/8), 1<<2 | 1<<3}
};


/*
 * Get the current Aperture size from the x86-64.
 * Note, that there may be multiple x86-64's, but we just return
 * the value from the first one we find. The set_size functions
 * keep the rest coherent anyway. Or at least should do.
 */
static int amd_x86_64_fetch_size(void)
{
	struct pci_dev *dev;
	int i;
	u32 temp;
	struct aper_size_info_32 *values;

	pci_for_each_dev(dev) {
		if (dev->bus->number==0 &&
			PCI_FUNC(dev->devfn)==3 &&
			PCI_SLOT(dev->devfn)>=24 && PCI_SLOT(dev->devfn)<=31) {

			pci_read_config_dword(dev, AMD_X86_64_GARTAPERTURECTL, &temp);
			temp = (temp & 0xe);
			values = A_SIZE_32(x86_64_aperture_sizes);

			for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
				if (temp == values[i].size_value) {
					agp_bridge.previous_size =
					    agp_bridge.current_size = (void *) (values + i);

					agp_bridge.aperture_size_idx = i;
					return values[i].size;
				}
			}
		}
	}
	/* erk, couldn't find an x86-64 ? */
	return 0;
}


static void inline flush_x86_64_tlb(struct pci_dev *dev)
{
	u32 tmp;

	pci_read_config_dword (dev, AMD_X86_64_GARTCACHECTL, &tmp);
	tmp |= 1<<0;
	pci_write_config_dword (dev, AMD_X86_64_GARTCACHECTL, tmp);
}


void amd_x86_64_tlbflush(agp_memory * temp)
{
	struct pci_dev *dev;

	pci_for_each_dev(dev) {
		if (dev->bus->number==0 && PCI_FUNC(dev->devfn)==3 &&
		    PCI_SLOT(dev->devfn) >=24 && PCI_SLOT(dev->devfn) <=31) {
			flush_x86_64_tlb (dev);
		}
	}
}


/*
 * In a multiprocessor x86-64 system, this function gets
 * called once for each CPU.
 */
u64 amd_x86_64_configure (struct pci_dev *hammer, u64 gatt_table)
{
	u64 aperturebase;
	u32 tmp;
	u64 addr, aper_base;

	/* Address to map to */
	pci_read_config_dword (hammer, AMD_X86_64_GARTAPERTUREBASE, &tmp);
	aperturebase = tmp << 25;
	aper_base = (aperturebase & PCI_BASE_ADDRESS_MEM_MASK);

	/* address of the mappings table */
	addr = (u64) gatt_table;
	addr >>= 12;
	tmp = (u32) addr<<4;
	tmp &= ~0xf;
	pci_write_config_dword (hammer, AMD_X86_64_GARTTABLEBASE, tmp);

	/* Enable GART translation for this hammer. */
	pci_read_config_dword(hammer, AMD_X86_64_GARTAPERTURECTL, &tmp);
	tmp |= GARTEN;
	tmp &= ~(DISGARTCPU | DISGARTIO);
	pci_write_config_dword(hammer, AMD_X86_64_GARTAPERTURECTL, tmp);

	/* keep CPU's coherent. */
	flush_x86_64_tlb (hammer);
	
	return aper_base;
}


static struct aper_size_info_32 amd_8151_sizes[7] =
{
	{2048, 524288, 9, 0x00000000 },	/* 0 0 0 0 0 0 */
	{1024, 262144, 8, 0x00000400 },	/* 1 0 0 0 0 0 */
	{512,  131072, 7, 0x00000600 },	/* 1 1 0 0 0 0 */
	{256,  65536,  6, 0x00000700 },	/* 1 1 1 0 0 0 */
	{128,  32768,  5, 0x00000720 },	/* 1 1 1 1 0 0 */
	{64,   16384,  4, 0x00000730 },	/* 1 1 1 1 1 0 */
	{32,   8192,   3, 0x00000738 } 	/* 1 1 1 1 1 1 */
};

static int amd_8151_configure(void)
{
	struct pci_dev *dev;
	unsigned long gatt_bus = virt_to_phys(agp_bridge.gatt_table_real);

	/* Configure AGP regs in each x86-64 host bridge. */
	pci_for_each_dev(dev) {
		if (dev->bus->number==0 &&
			PCI_FUNC(dev->devfn)==3 &&
			PCI_SLOT(dev->devfn)>=24 && PCI_SLOT(dev->devfn)<=31) {
			agp_bridge.gart_bus_addr = amd_x86_64_configure(dev,gatt_bus);
	}
	}
	return 0;
}


static void amd_8151_cleanup(void)
{
	struct pci_dev *dev;
	u32 tmp;

	pci_for_each_dev(dev) {
		/* disable gart translation */
		if (dev->bus->number==0 && PCI_FUNC(dev->devfn)==3 &&
		    (PCI_SLOT(dev->devfn) >=24) && (PCI_SLOT(dev->devfn) <=31)) {

			pci_read_config_dword (dev, AMD_X86_64_GARTAPERTURECTL, &tmp);
			tmp &= ~(AMD_X86_64_GARTEN);
			pci_write_config_dword (dev, AMD_X86_64_GARTAPERTURECTL, tmp);
		}
	}
}



static unsigned long amd_8151_mask_memory(unsigned long addr, int type)
{
	return addr | agp_bridge.masks[0].mask;
}


static struct gatt_mask amd_8151_masks[] =
{
	{0, 0}
};


/*
 * Try to configure an AGP v3 capable setup.
 * If we fail (typically because we don't have an AGP v3
 * card in the system) we fall back to the generic AGP v2
 * routines.
 */
static int agp_x86_64_agp_enable(u32 mode)
{
	struct pci_dev *device = NULL;
	u32 command, scratch; 
	u8 cap_ptr;
	u8 agp_v3;
	u8 v3_devs=0;

	/* FIXME: If 'mode' is x1/x2/x4 should we call the AGPv2 routines directly ?
	 * Messy, as some AGPv3 cards can only do x4 as a minimum.
	 */

	/* PASS1: Count # of devs capable of AGPv3 mode. */
	pci_for_each_dev(device) {
		cap_ptr = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (cap_ptr != 0x00) {
			pci_read_config_dword(device, cap_ptr, &scratch);
			scratch &= (1<<20|1<<21|1<<22|1<<23);
			scratch = scratch>>20;
			/* AGP v3 capable ? */
			if (scratch>=3) {
				v3_devs++;
				printk (KERN_INFO "AGP: Found AGPv3 capable device at %d:%d:%d\n",
					device->bus->number, PCI_FUNC(device->devfn), PCI_SLOT(device->devfn));
			} else {
				printk (KERN_INFO "AGP: Version %x AGP device found.\n", scratch);
			}
		}
	}
	/* If not enough, go to AGP v2 setup */
	if (v3_devs<2) {
		printk (KERN_INFO "AGP: Only %d devices found, not enough, trying AGPv2\n", v3_devs);
		return agp_generic_agp_enable(mode);
	} else {
		printk (KERN_INFO "AGP: Enough AGPv3 devices found, setting up...\n");
	}


	pci_read_config_dword(agp_bridge.dev, agp_bridge.capndx + 4, &command);

	/*
	 * PASS2: go through all devices that claim to be
	 *        AGP devices and collect their data.
	 */

	pci_for_each_dev(device) {
		cap_ptr = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (cap_ptr != 0x00) {
			/*
			 * Ok, here we have a AGP device. Disable impossible 
			 * settings, and adjust the readqueue to the minimum.
			 */

			printk (KERN_INFO "AGP: Setting up AGPv3 capable device at %d:%d:%d\n",
					device->bus->number, PCI_FUNC(device->devfn), PCI_SLOT(device->devfn));
			pci_read_config_dword(device, cap_ptr + 4, &scratch);
			agp_v3 = (scratch & (1<<3) ) >>3;

			/* adjust RQ depth */
			command =
			    ((command & ~0xff000000) |
			    min((mode & 0xff000000),
			    min((command & 0xff000000),
			    (scratch & 0xff000000))));

			/* disable SBA if it's not supported */
			if (!((command & 0x200) && (scratch & 0x200) && (mode & 0x200)))
				command &= ~0x200;

			/* disable FW if it's not supported */
			if (!((command & 0x10) && (scratch & 0x10) && (mode & 0x10)))
				command &= ~0x10;

			if (!((command & 2) && (scratch & 2) && (mode & 2))) {
				command &= ~2;		/* 8x */
				printk (KERN_INFO "AGP: Putting device into 8x mode\n");
			}

			if (!((command & 1) && (scratch & 1) && (mode & 1))) {
				command &= ~1;		/* 4x */
				printk (KERN_INFO "AGP: Putting device into 4x mode\n");
			}
		}
	}
	/*
	 * PASS3: Figure out the 8X/4X setting and enable the
	 *        target (our motherboard chipset).
	 */

	if (command & 2)
		command &= ~5;	/* 8X */

	if (command & 1)
		command &= ~6;	/* 4X */

	command |= 0x100;

	pci_write_config_dword(agp_bridge.dev, agp_bridge.capndx + 8, command);

	/*
	 * PASS4: Go through all AGP devices and update the
	 *        command registers.
	 */

	pci_for_each_dev(device) {
		cap_ptr = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (cap_ptr != 0x00)
			pci_write_config_dword(device, cap_ptr + 8, command);
	}

    return 0;
}


static int __init amd_8151_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = amd_8151_masks;
	agp_bridge.aperture_sizes = (void *) amd_8151_sizes;
	agp_bridge.size_type = U32_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = amd_8151_configure;
	agp_bridge.fetch_size = amd_x86_64_fetch_size;
	agp_bridge.cleanup = amd_8151_cleanup;
	agp_bridge.tlb_flush = amd_x86_64_tlbflush;
	agp_bridge.mask_memory = amd_8151_mask_memory;
	agp_bridge.agp_enable = agp_x86_64_agp_enable;
	agp_bridge.cache_flush = global_cache_flush;
	agp_bridge.create_gatt_table = agp_generic_create_gatt_table;
	agp_bridge.free_gatt_table = agp_generic_free_gatt_table;
	agp_bridge.insert_memory = x86_64_insert_memory;
	agp_bridge.remove_memory = agp_generic_remove_memory;
	agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
	agp_bridge.free_by_type = agp_generic_free_by_type;
	agp_bridge.agp_alloc_page = agp_generic_alloc_page;
	agp_bridge.agp_destroy_page = agp_generic_destroy_page;
	agp_bridge.suspend = agp_generic_suspend;
	agp_bridge.resume = agp_generic_resume;
	agp_bridge.cant_use_aperture = 0;

	return 0;
	
	(void) pdev; /* unused */
}

/* NVIDIA x86-64 chipset support */
 

static struct _nvidia_x86_64_private {
	struct pci_dev *dev_1;
} nvidia_x86_64_private;


static struct aper_size_info_32 nvidia_x86_64_sizes[5] =
{
	{512,  131072, 7, 0x00000000 },
	{256,  65536,  6, 0x00000008 },
	{128,  32768,  5, 0x0000000C },
	{64,   16384,  4, 0x0000000E },
	{32,   8192,   3, 0x0000000F }
};


static int nvidia_x86_64_configure(void)
{
	struct pci_dev *dev, *hammer=NULL;
	int i, current_size;
	u32 tmp, apbase, apbar, aplimit;
	unsigned long gatt_bus = virt_to_phys(agp_bridge.gatt_table_real);

	if (!agp_bridge.dev) 
		return -ENODEV;

	/* configure AGP regs in each x86-64 host bridge */
	pci_for_each_dev(dev) {
		if (dev->bus->number==0 &&
			PCI_FUNC(dev->devfn)==3 &&
			PCI_SLOT(dev->devfn)>=24 && PCI_SLOT(dev->devfn)<=31) {
			agp_bridge.gart_bus_addr = amd_x86_64_configure(dev,gatt_bus);
			hammer = dev;
		}
	}
	if (hammer == NULL)
		return -ENODEV;

	/* translate x86-64 aperture size to NVIDIA aperture size */
	current_size = amd_x86_64_fetch_size();
	for (i = 0 ; i < agp_bridge.num_aperture_sizes; i++) {
		if (nvidia_x86_64_sizes[i].size == current_size)
			break;
	}
	/* if x86-64 size does not match any NVIDIA size, exit here */
	if (i == agp_bridge.num_aperture_sizes)
		return -ENODEV;
	pci_read_config_dword(nvidia_x86_64_private.dev_1, NVIDIA_X86_64_1_APSIZE, &tmp);
	tmp &= ~(0xf);
	tmp |= nvidia_x86_64_sizes[i].size_value;
	pci_write_config_dword(nvidia_x86_64_private.dev_1, NVIDIA_X86_64_1_APSIZE, tmp);

	/* shadow x86-64 registers into NVIDIA registers */
	pci_read_config_dword (hammer, AMD_X86_64_GARTAPERTUREBASE, &apbase);
	/* if x86-64 aperture base is beyond 4G, exit here */
	if ( (apbase & 0x7fff) >> (32 - 25) )
		 return -ENODEV;
	apbase = (apbase & 0x7fff) << 25;

	pci_read_config_dword(agp_bridge.dev, NVIDIA_X86_64_0_APBASE, &apbar);
	apbar &= ~PCI_BASE_ADDRESS_MEM_MASK;
	apbar |= apbase;
	pci_write_config_dword(agp_bridge.dev, NVIDIA_X86_64_0_APBASE, apbar);

	/* Shadow into secondary device looks dubious, but we keep it for now.
	   If these two could be dropped then the NForce3 code path could
	   be just folded into the generic functions above. */

	aplimit = apbase + (current_size * 1024 * 1024) - 1;
	pci_write_config_dword(nvidia_x86_64_private.dev_1, NVIDIA_X86_64_1_APBASE1, apbase);
	pci_write_config_dword(nvidia_x86_64_private.dev_1, NVIDIA_X86_64_1_APLIMIT1, aplimit);
	pci_write_config_dword(nvidia_x86_64_private.dev_1, NVIDIA_X86_64_1_APBASE2, apbase);
	pci_write_config_dword(nvidia_x86_64_private.dev_1, NVIDIA_X86_64_1_APLIMIT2, aplimit);

	/* Original driver updated the IORR here, but AMD documentation
	   explicitely discourages this for something already covered by the GART. */
	
	return 0;
}


static void nvidia_x86_64_cleanup(void)
{
	struct pci_dev *dev;
	u32 tmp;

	pci_for_each_dev(dev) {
		/* disable gart translation */
		if (dev->bus->number==0 && PCI_FUNC(dev->devfn)==3 &&
		    (PCI_SLOT(dev->devfn) >=24) && (PCI_SLOT(dev->devfn) <=31)) {

			pci_read_config_dword (dev, AMD_X86_64_GARTAPERTURECTL, &tmp);
			tmp &= ~(AMD_X86_64_GARTEN);
			pci_write_config_dword (dev, AMD_X86_64_GARTAPERTURECTL, tmp);
		}
	}
}


static unsigned long nvidia_x86_64_mask_memory(unsigned long addr, int type)
{
	return addr | agp_bridge.masks[0].mask;
}


static struct gatt_mask nvidia_x86_64_masks[] =
{
	{0x00000001, 0}
};


static int __init nvidia_x86_64_setup (struct pci_dev *pdev)
{
	nvidia_x86_64_private.dev_1 =
		pci_find_slot((unsigned int)pdev->bus->number, PCI_DEVFN(11, 0));

	if (nvidia_x86_64_private.dev_1 == NULL) {
		printk(KERN_INFO PFX "agpgart: Detected an NVIDIA "
			"nForce3 chipset, but could not find "
			"the secondary device.\n");
		agp_bridge.type = NOT_SUPPORTED;
		return -ENODEV;
	}

	agp_bridge.masks = nvidia_x86_64_masks;
	agp_bridge.aperture_sizes = (void *) nvidia_x86_64_sizes;
	agp_bridge.size_type = U32_APER_SIZE;
	agp_bridge.num_aperture_sizes = 5;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = nvidia_x86_64_configure;
	agp_bridge.fetch_size = amd_x86_64_fetch_size;
	agp_bridge.cleanup = nvidia_x86_64_cleanup;
	agp_bridge.tlb_flush = amd_x86_64_tlbflush;
	agp_bridge.mask_memory = nvidia_x86_64_mask_memory;
	agp_bridge.agp_enable = agp_x86_64_agp_enable;
	agp_bridge.cache_flush = global_cache_flush;
	agp_bridge.create_gatt_table = agp_generic_create_gatt_table;
	agp_bridge.free_gatt_table = agp_generic_free_gatt_table;
	agp_bridge.insert_memory = x86_64_insert_memory;
	agp_bridge.remove_memory = agp_generic_remove_memory;
	agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
	agp_bridge.free_by_type = agp_generic_free_by_type;
	agp_bridge.agp_alloc_page = agp_generic_alloc_page;
	agp_bridge.agp_destroy_page = agp_generic_destroy_page;
	agp_bridge.suspend = agp_generic_suspend;
	agp_bridge.resume = agp_generic_resume;
	agp_bridge.cant_use_aperture = 0;

	return 0;
	
	(void) pdev; /* unused */
}

static int ali_fetch_size(void)
{
	int i;
	u32 temp;
	struct aper_size_info_32 *values;

	pci_read_config_dword(agp_bridge.dev, ALI_ATTBASE, &temp);
	temp &= ~(0xfffffff0);
	values = A_SIZE_32(agp_bridge.aperture_sizes);

	for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
		if (temp == values[i].size_value) {
			agp_bridge.previous_size =
			    agp_bridge.current_size = (void *) (values + i);
			agp_bridge.aperture_size_idx = i;
			return values[i].size;
		}
	}

	return 0;
}

static void ali_tlbflush(agp_memory * mem)
{
	u32 temp;

	pci_read_config_dword(agp_bridge.dev, ALI_TLBCTRL, &temp);
// clear tag
	pci_write_config_dword(agp_bridge.dev, ALI_TAGCTRL,
			((temp & 0xfffffff0) | 0x00000001|0x00000002));
}

static void ali_cleanup(void)
{
	struct aper_size_info_32 *previous_size;
	u32 temp;

	previous_size = A_SIZE_32(agp_bridge.previous_size);

	pci_read_config_dword(agp_bridge.dev, ALI_TLBCTRL, &temp);
// clear tag
	pci_write_config_dword(agp_bridge.dev, ALI_TAGCTRL,
			((temp & 0xffffff00) | 0x00000001|0x00000002));

	pci_read_config_dword(agp_bridge.dev,  ALI_ATTBASE, &temp);
	pci_write_config_dword(agp_bridge.dev, ALI_ATTBASE,
			((temp & 0x00000ff0) | previous_size->size_value));
}

static int ali_configure(void)
{
	u32 temp;
	struct aper_size_info_32 *current_size;

	current_size = A_SIZE_32(agp_bridge.current_size);

	/* aperture size and gatt addr */
	pci_read_config_dword(agp_bridge.dev, ALI_ATTBASE, &temp);
	temp = (((temp & 0x00000ff0) | (agp_bridge.gatt_bus_addr & 0xfffff000))
			| (current_size->size_value & 0xf));
	pci_write_config_dword(agp_bridge.dev, ALI_ATTBASE, temp);

	/* tlb control */

	/*
	 *	Question: Jeff, ALi's patch deletes this:
	 *
	 *	pci_read_config_dword(agp_bridge.dev, ALI_TLBCTRL, &temp);
	 *	pci_write_config_dword(agp_bridge.dev, ALI_TLBCTRL,
	 *			       ((temp & 0xffffff00) | 0x00000010));
	 *
	 *	and replaces it with the following, which seems to duplicate the
	 *	next couple of lines below it. I suspect this was an oversight,
	 *	but you might want to check up on this?
	 */
	
	pci_read_config_dword(agp_bridge.dev, ALI_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

	/* address to map to */
	pci_read_config_dword(agp_bridge.dev, ALI_APBASE, &temp);
	agp_bridge.gart_bus_addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);

#if 0
	if (agp_bridge.type == ALI_M1541) {
		u32 nlvm_addr = 0;

		switch (current_size->size_value) {
			case 0:  break;
			case 1:  nlvm_addr = 0x100000;break;
			case 2:  nlvm_addr = 0x200000;break;
			case 3:  nlvm_addr = 0x400000;break;
			case 4:  nlvm_addr = 0x800000;break;
			case 6:  nlvm_addr = 0x1000000;break;
			case 7:  nlvm_addr = 0x2000000;break;
			case 8:  nlvm_addr = 0x4000000;break;
			case 9:  nlvm_addr = 0x8000000;break;
			case 10: nlvm_addr = 0x10000000;break;
			default: break;
		}
		nlvm_addr--;
		nlvm_addr&=0xfff00000;

		nlvm_addr+= agp_bridge.gart_bus_addr;
		nlvm_addr|=(agp_bridge.gart_bus_addr>>12);
		printk(KERN_INFO PFX "nlvm top &base = %8x\n",nlvm_addr);
	}
#endif

	pci_read_config_dword(agp_bridge.dev, ALI_TLBCTRL, &temp);
	temp &= 0xffffff7f;		//enable TLB
	pci_write_config_dword(agp_bridge.dev, ALI_TLBCTRL, temp);

	return 0;
}

static unsigned long ali_mask_memory(unsigned long addr, int type)
{
	/* Memory type is ignored */

	return addr | agp_bridge.masks[0].mask;
}

static void ali_cache_flush(void)
{
	global_cache_flush();

	if (agp_bridge.type == ALI_M1541) {
		int i, page_count;
		u32 temp;

		page_count = 1 << A_SIZE_32(agp_bridge.current_size)->page_order;
		for (i = 0; i < PAGE_SIZE * page_count; i += PAGE_SIZE) {
			pci_read_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL, &temp);
			pci_write_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL,
					(((temp & ALI_CACHE_FLUSH_ADDR_MASK) |
					  (agp_bridge.gatt_bus_addr + i)) |
					    ALI_CACHE_FLUSH_EN));
		}
	}
}

static unsigned long ali_alloc_page(void)
{
#ifdef AGPGART_2_2
	void *pt;
	u32 temp;

	pt = (void *) __get_free_page(GFP_KERNEL);
	if (pt == NULL)
		return 0;

#ifdef AGPGART_2_2
	atomic_inc(&mem_map[MAP_NR(pt)].count);
	set_bit(PG_locked, &mem_map[MAP_NR(pt)].flags);
#else /* !AGPGART_2_2 */
	get_page(&virt_to_page(pt));
	set_bit(PG_locked, &virt_to_page(pt)->flags);
#endif /* !AGPGART_2_2 */
	atomic_inc(&agp_bridge.current_memory_agp);

	global_cache_flush();

	if (agp_bridge.type == ALI_M1541) {
		pci_read_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL, &temp);
		pci_write_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL,
				(((temp & ALI_CACHE_FLUSH_ADDR_MASK) |
				  virt_to_phys((void *)pt)) |
				    ALI_CACHE_FLUSH_EN ));
	}
	return (unsigned long) pt;
#else /* AGPGART_2_2 */
    // 2.4.8-ac7 and 2.4.13
    unsigned long page_addr;

#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    page_addr = agp_generic_alloc_page();
    if (page_addr == 0)
        return 0;

    /* the page attribute setup got already handled with the above call */

    /* probably not needed anymore */
    global_cache_flush();
#else
    struct page *page;

    page = alloc_page(GFP_KERNEL);
    if (page == NULL)
        return 0;

    get_page(page);
    set_bit(PG_locked, &page->flags);
    atomic_inc(&agp_bridge.current_memory_agp);

    global_cache_flush();

    page_addr = (unsigned long)page_address(page);
#endif

	if (agp_bridge.type == ALI_M1541)
    {
        u32 temp;

        pci_read_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL, &temp);
        pci_write_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL,
            (   (   (temp & ALI_CACHE_FLUSH_ADDR_MASK) |
                    virt_to_phys((void*)page_addr)
                ) |
                ALI_CACHE_FLUSH_EN
            ));
    }

    return page_addr;
#endif /* AGPGART_2_2 */
}

#ifdef AGPGART_2_2
static void ali_destroy_page(unsigned long page)
{
	u32 temp;
	void *pt = (void *) page;

	if (pt == NULL)
		return;

	global_cache_flush();

	if (agp_bridge.type == ALI_M1541) {
		pci_read_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL, &temp);
		pci_write_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL,
				(((temp & ALI_CACHE_FLUSH_ADDR_MASK) |
				  virt_to_phys((void *)pt)) |
				    ALI_CACHE_FLUSH_EN));
	}

#ifdef AGPGART_2_2
	atomic_dec(&mem_map[MAP_NR(pt)].count);
	clear_bit(PG_locked, &mem_map[MAP_NR(pt)].flags);
	wake_up(&mem_map[MAP_NR(pt)].wait);
#else /* !AGPGART_2_2 */
	put_page(&virt_to_page(pt));
	clear_bit(PG_locked, &virt_to_page(pt)->flags);
	wake_up(&virt_to_page(pt)->wait);
#endif /* !AGPGART_2_2 */
	free_page((unsigned long) pt);
	atomic_dec(&agp_bridge.current_memory_agp);
}
#else
// 2.4.8-ac7 and 2.4.13
static void ali_destroy_page(unsigned long addr)
{
    void *pt = (void *) addr;
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    /* no more vars needed */
#else
    struct page *page;
#endif

    if (pt == NULL)
        return;

	global_cache_flush();

	if (agp_bridge.type == ALI_M1541) {
        u32 temp;

		pci_read_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL, &temp);
		pci_write_config_dword(agp_bridge.dev, ALI_CACHE_FLUSH_CTRL,
				(((temp & ALI_CACHE_FLUSH_ADDR_MASK) |
				  virt_to_phys(pt)) |
                    ALI_CACHE_FLUSH_EN));
    }

#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    agp_generic_destroy_page(addr);
#else
    page = virt_to_page((unsigned long)pt);
#if LINUX_VERSION_CODE > 0x020500
//    unmap_page_from_agp(page); // LINUX_TODO
    put_page(page);
    unlock_page(page);
#else /* LINUX_VERSION_CODE > 0x020500 */
#ifdef AGPGART_2_4_19
    put_page(page);
    UnlockPage(page);
#else /* AGPGART_2_4_19 */
    __put_page(page);
    clear_bit(PG_locked, &page->flags);
    wake_up(&page->wait);
#endif /* AGPGART_2_4_19 */
#endif /* LINUX_VERSION_CODE > 0x020500 */
    free_page((unsigned long) pt);
    atomic_dec(&agp_bridge.current_memory_agp);
#endif
}                                                                                
#endif

/* Setup function */
static struct gatt_mask ali_generic_masks[] =
{
	{0x00000000, 0}
};

static struct aper_size_info_32 ali_generic_sizes[7] =
{
	{256, 65536, 6, 10},
	{128, 32768, 5, 9},
	{64, 16384, 4, 8},
	{32, 8192, 3, 7},
	{16, 4096, 2, 6},
	{8, 2048, 1, 4},
	{4, 1024, 0, 3}
};

static int __init ali_generic_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = ali_generic_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) ali_generic_sizes;
	agp_bridge.size_type = U32_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = ali_configure;
	agp_bridge.fetch_size = ali_fetch_size;
	agp_bridge.cleanup = ali_cleanup;
	agp_bridge.tlb_flush = ali_tlbflush;
	agp_bridge.mask_memory = ali_mask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
	agp_bridge.cache_flush = ali_cache_flush;
	agp_bridge.create_gatt_table = agp_generic_create_gatt_table;
	agp_bridge.free_gatt_table = agp_generic_free_gatt_table;
	agp_bridge.insert_memory = agp_generic_insert_memory;
	agp_bridge.remove_memory = agp_generic_remove_memory;
	agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
	agp_bridge.free_by_type = agp_generic_free_by_type;
	agp_bridge.agp_alloc_page = ali_alloc_page;
	agp_bridge.agp_destroy_page = ali_destroy_page;
    agp_bridge.suspend = agp_generic_suspend;
    agp_bridge.resume = agp_generic_resume;
    agp_bridge.cant_use_aperture = 0;

	return 0;
	
	(void) pdev; /* unused */
}

typedef struct _serverworks_page_map {
	unsigned long *real;
	unsigned long *remapped;
} serverworks_page_map;

static struct _serverworks_private {
	struct pci_dev *svrwrks_dev;	/* device one */
	volatile u8 *registers;
	serverworks_page_map **gatt_pages;
	int num_tables;
	serverworks_page_map scratch_page_table;
#define scratch_dir scratch_page_table 

	int gart_addr_ofs;
	int mm_addr_ofs;
} serverworks_private;

static int serverworks_create_page_map(serverworks_page_map *page_map)
{
	int i;
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    int err;
#endif

	page_map->real = (unsigned long *) __get_free_page(GFP_KERNEL);
	if (page_map->real == NULL) {
		return -ENOMEM;
	}

#ifdef AGPGART_2_2
    set_bit(PG_reserved, &mem_map[MAP_NR(page_map->real)].flags);
	page_map->remapped = page_map->real;

	for(i = 0; i < PAGE_SIZE / sizeof(unsigned long); i++) {
		page_map->remapped[i] = agp_bridge.scratch_page;
	}
	CACHE_FLUSH();
#else /* !AGPGART_2_2 */
	set_bit(PG_reserved, &virt_to_page((unsigned long)page_map->real)->flags);

	CACHE_FLUSH();
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    err = change_page_attr(virt_to_page(page_map->real), 1, PAGE_KERNEL_NOCACHE);
    if (!err)
#endif
    {
    	page_map->remapped = ioremap_nocache(virt_to_phys(page_map->real), 
					    PAGE_SIZE);
    }

	if ( (page_map->remapped == NULL)
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
        || (err)
#endif
        )
    {
		clear_bit(PG_reserved, 
			  &virt_to_page((unsigned long)page_map->real)->flags);
		free_page((unsigned long) page_map->real);
		page_map->real = NULL;
		return -ENOMEM;
	}
	CACHE_FLUSH();

	for(i = 0; i < PAGE_SIZE / sizeof(unsigned long); i++) {
		page_map->remapped[i] = agp_bridge.scratch_page;
	}
#endif /* !AGPGART_2_2 */

	return 0;
}

static void serverworks_free_page_map(serverworks_page_map *page_map)
{
#ifdef AGPGART_2_2
	clear_bit(PG_reserved, 
		  &mem_map[MAP_NR(page_map->real)].flags);
#else /* !AGPGART_2_2 */
#if defined(CONFIG_X86) && (PAGE_ATTR_FIX != 0)
    change_page_attr(virt_to_page(page_map->real),1,PAGE_KERNEL); 
#endif
	iounmap(page_map->remapped);
	clear_bit(PG_reserved, 
		  &virt_to_page((unsigned long)page_map->real)->flags);
#endif /* !AGPGART_2_2 */
	free_page((unsigned long) page_map->real);

#ifdef FGL
    page_map->remapped = 0;
    page_map->real = 0;
#endif
}

static void serverworks_free_gatt_pages(void)
{
	int i;
	serverworks_page_map **tables;
	serverworks_page_map *entry;

	tables = serverworks_private.gatt_pages;
	for(i = 0; i < serverworks_private.num_tables; i++) {
		entry = tables[i];
		if (entry != NULL) {
			if (entry->real != NULL) {
				serverworks_free_page_map(entry);
			}
			kfree(entry);
		}
	}
#ifdef FGL
    serverworks_private.num_tables = 0;
#endif
	kfree(tables);

#ifdef FGL
    // free this single scratch table
    if( serverworks_private.scratch_page_table.real )
    {
        serverworks_free_page_map(&serverworks_private.scratch_page_table);
    }
#endif
}

static int serverworks_create_gatt_pages(int nr_tables)
{
	serverworks_page_map **tables;
	serverworks_page_map *entry;
	int retval = 0;
	int i;

	tables = kmalloc((nr_tables + 1) * sizeof(serverworks_page_map *), 
			 GFP_KERNEL);
	if (tables == NULL) {
		return -ENOMEM;
	}
	memset(tables, 0, sizeof(serverworks_page_map *) * (nr_tables + 1));
	for (i = 0; i < nr_tables; i++) {
		entry = kmalloc(sizeof(serverworks_page_map), GFP_KERNEL);
		if (entry == NULL) {
			retval = -ENOMEM;
			break;
		}
		memset(entry, 0, sizeof(serverworks_page_map));
		tables[i] = entry;
		retval = serverworks_create_page_map(entry);
		if (retval != 0) break;
	}
	serverworks_private.num_tables = nr_tables;
	serverworks_private.gatt_pages = tables;

	if (retval != 0) serverworks_free_gatt_pages();

	return retval;
}

#define SVRWRKS_GET_GATT(addr) (serverworks_private.gatt_pages[\
	GET_PAGE_DIR_IDX(addr)]->remapped)

#ifndef GET_PAGE_DIR_OFF
#define GET_PAGE_DIR_OFF(addr) (addr >> 22)
#endif

#ifndef GET_PAGE_DIR_IDX
#define GET_PAGE_DIR_IDX(addr) (GET_PAGE_DIR_OFF(addr) - \
	GET_PAGE_DIR_OFF(agp_bridge.gart_bus_addr))
#endif

#ifndef GET_GATT_OFF
#define GET_GATT_OFF(addr) ((addr & 0x003ff000) >> 12)
#endif

static int serverworks_create_gatt_table(void)
{
	struct aper_size_info_lvl2 *value;
	serverworks_page_map page_dir;
	unsigned long addr;
	int retval;
	u32 temp;
	int i;

	value = A_SIZE_LVL2(agp_bridge.current_size);
	retval = serverworks_create_page_map(&page_dir);
	if (retval != 0) {
		return retval;
	}
	retval = serverworks_create_page_map(&serverworks_private.scratch_page_table);
	if (retval != 0) {
		serverworks_free_page_map(&page_dir);
        serverworks_free_page_map(&serverworks_private.scratch_page_table); /* 2.4.8-ac7 */
		return retval;
	}

	/* Create a single scratch page table */
	for(i = 0; i < 1024; i++) {
		serverworks_private.scratch_page_table.remapped[i] = (unsigned long) agp_bridge.scratch_page;
    }
    /* attach this single scratch page table to any entry of the page table directory */
	for(i = 0; i < 1024; i++) {
		page_dir.remapped[i] =
			virt_to_bus(serverworks_private.scratch_page_table.real);
		page_dir.remapped[i] |= 0x00000001;
	}

	retval = serverworks_create_gatt_pages(value->num_entries / 1024);
	if (retval != 0) {
		serverworks_free_page_map(&page_dir);
        serverworks_free_page_map(&serverworks_private.scratch_page_table); /* 2.4.9 */
		return retval;
	}

	agp_bridge.gatt_table_real = (u32*)page_dir.real;
	agp_bridge.gatt_table = (u32*)page_dir.remapped;
	agp_bridge.gatt_bus_addr = virt_to_bus(page_dir.real);

	/* Get the address for the gart region.
	 * This is a bus address even on the alpha, b/c its
	 * used to program the agp master not the cpu
	 */

	pci_read_config_dword(agp_bridge.dev,
			      serverworks_private.gart_addr_ofs,
			      &temp);

	addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);
	agp_bridge.gart_bus_addr = addr;

	/* Calculate the agp offset */	
	for(i = 0; i < value->num_entries / 1024; i++, addr += 0x00400000) {
		page_dir.remapped[i] =
			virt_to_bus(serverworks_private.gatt_pages[i]->real);
		page_dir.remapped[i] |= 0x00000001;
	}

	return 0;
}

static int serverworks_free_gatt_table(void)
{
	serverworks_page_map page_dir;
   
	page_dir.real = (unsigned long *)agp_bridge.gatt_table_real;
	page_dir.remapped = (unsigned long *)agp_bridge.gatt_table;

	serverworks_free_gatt_pages();          // free all page tables
	serverworks_free_page_map(&page_dir);   // free the single page with the page directory
#ifndef FGL /* linux-2.4.8 */
    serverworks_free_page_map(&serverworks_private.scratch_page_table);
#endif
	return 0;
}

static int serverworks_fetch_size(void)
{
	int i;
	u32 temp;
	u32 temp2;
	struct aper_size_info_lvl2 *values;

	values = A_SIZE_LVL2(agp_bridge.aperture_sizes);
	pci_read_config_dword(agp_bridge.dev,
			        serverworks_private.gart_addr_ofs,
			        &temp);
	pci_write_config_dword(agp_bridge.dev,
			        serverworks_private.gart_addr_ofs,
                    SVWRKS_SIZE_MASK);
	pci_read_config_dword(agp_bridge.dev,
			        serverworks_private.gart_addr_ofs,
			        &temp2);
	pci_write_config_dword(agp_bridge.dev,
			        serverworks_private.gart_addr_ofs,
			        temp);
	temp2 &= SVWRKS_SIZE_MASK;

	for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
		if (temp2 == values[i].size_value) {
			agp_bridge.previous_size =
			    agp_bridge.current_size = (void *) (values + i);

			agp_bridge.aperture_size_idx = i;
			return values[i].size;
		}
	}

	return 0;
}

static int serverworks_configure(void)
{
	struct aper_size_info_lvl2 *current_size;
	u32 temp;
	u8 enable_reg;
	u8 cap_ptr;
	u32 cap_id;
	u16 cap_reg;

	current_size = A_SIZE_LVL2(agp_bridge.current_size);

	/* Get the memory mapped registers */
	pci_read_config_dword(agp_bridge.dev,
			      serverworks_private.mm_addr_ofs,
			      &temp);
	temp = (temp & PCI_BASE_ADDRESS_MEM_MASK);
	serverworks_private.registers = (volatile u8 *) ioremap(temp, 4096);

	OUTREG8(serverworks_private.registers, SVWRKS_GART_CACHE, 0x0a);

	OUTREG32(serverworks_private.registers, SVWRKS_GATTBASE, 
		 agp_bridge.gatt_bus_addr);

	cap_reg = INREG16(serverworks_private.registers, SVWRKS_COMMAND);
	cap_reg &= ~0x0007;
	cap_reg |= 0x4;
	OUTREG16(serverworks_private.registers, SVWRKS_COMMAND, cap_reg);

	pci_read_config_byte(serverworks_private.svrwrks_dev,
			     SVWRKS_AGP_ENABLE, &enable_reg);
	enable_reg |= 0x1; /* Agp Enable bit */
	pci_write_config_byte(serverworks_private.svrwrks_dev,
			      SVWRKS_AGP_ENABLE, enable_reg);
	agp_bridge.tlb_flush(NULL);

	pci_read_config_byte(serverworks_private.svrwrks_dev, 0x34, &cap_ptr);
	if (cap_ptr != 0x00) {
		do {
			pci_read_config_dword(serverworks_private.svrwrks_dev,
					      cap_ptr, &cap_id);

			if ((cap_id & 0xff) != 0x02)
				cap_ptr = (cap_id >> 8) & 0xff;
		}
		while (((cap_id & 0xff) != 0x02) && (cap_ptr != 0x00));
	}
	agp_bridge.capndx = cap_ptr;

	/* Fill in the mode register */
	pci_read_config_dword(serverworks_private.svrwrks_dev,
			      agp_bridge.capndx + 4,
			      &agp_bridge.mode);

	pci_read_config_byte(agp_bridge.dev,
			     SVWRKS_CACHING,
			     &enable_reg);
	enable_reg &= ~0x3;
	pci_write_config_byte(agp_bridge.dev,
			      SVWRKS_CACHING,
			      enable_reg);

	pci_read_config_byte(agp_bridge.dev,
			     SVWRKS_FEATURE,
			     &enable_reg);
	enable_reg |= (1<<6);
	pci_write_config_byte(agp_bridge.dev,
			      SVWRKS_FEATURE,
			      enable_reg);

	return 0;
}

static void serverworks_cleanup(void)
{
	iounmap((void *) serverworks_private.registers);
}

/*
 * This routine could be implemented by taking the addresses
 * written to the GATT, and flushing them individually.  However
 * currently it just flushes the whole table.  Which is probably
 * more efficent, since agp_memory blocks can be a large number of
 * entries.
 */

static void serverworks_tlbflush(agp_memory * temp)
{
	unsigned long end;

	OUTREG8(serverworks_private.registers, SVWRKS_POSTFLUSH, 0x01);
	end = jiffies + 3*HZ;
	while(INREG8(serverworks_private.registers, 
		     SVWRKS_POSTFLUSH) == 0x01) {
		if((signed)(end - jiffies) <= 0) {
			printk(KERN_ERR "Posted write buffer flush took more"
			       "then 3 seconds\n");
		}
	}
	OUTREG32(serverworks_private.registers, SVWRKS_DIRFLUSH, 0x00000001);
	end = jiffies + 3*HZ;
	while(INREG32(serverworks_private.registers, 
		     SVWRKS_DIRFLUSH) == 0x00000001) {
		if((signed)(end - jiffies) <= 0) {
			printk(KERN_ERR "TLB flush took more"
			       "then 3 seconds\n");
		}
	}
}

static unsigned long serverworks_mask_memory(unsigned long addr, int type)
{
	/* Only type 0 is supported by the serverworks chipsets */

	return addr | agp_bridge.masks[0].mask;
}

static int serverworks_insert_memory(agp_memory * mem,
			     off_t pg_start, int type)
{
	int i, j, num_entries;
	unsigned long *cur_gatt;
	unsigned long addr;

	num_entries = A_SIZE_LVL2(agp_bridge.current_size)->num_entries;

	if (type != 0 || mem->type != 0) {
		return -EINVAL;
	}
	if ((pg_start + mem->page_count) > num_entries) {
		return -EINVAL;
	}

	j = pg_start;
	while (j < (pg_start + mem->page_count)) {
		addr = (j * PAGE_SIZE) + agp_bridge.gart_bus_addr;
		cur_gatt = SVRWRKS_GET_GATT(addr);
		if (!PGE_EMPTY(cur_gatt[GET_GATT_OFF(addr)])) {
			return -EBUSY;
		}
		j++;
	}

	if (mem->is_flushed == FALSE) {
		CACHE_FLUSH();
		mem->is_flushed = TRUE;
	}

	for (i = 0, j = pg_start; i < mem->page_count; i++, j++) {
		addr = (j * PAGE_SIZE) + agp_bridge.gart_bus_addr;
		cur_gatt = SVRWRKS_GET_GATT(addr);
		cur_gatt[GET_GATT_OFF(addr)] = mem->memory[i];
	}
	agp_bridge.tlb_flush(mem);
	return 0;
}

static int serverworks_remove_memory(agp_memory * mem, off_t pg_start,
			     int type)
{
	int i;
	unsigned long *cur_gatt;
	unsigned long addr;

	if (type != 0 || mem->type != 0) {
		return -EINVAL;
	}

	CACHE_FLUSH();
	agp_bridge.tlb_flush(mem);

	for (i = pg_start; i < (mem->page_count + pg_start); i++) {
		addr = (i * PAGE_SIZE) + agp_bridge.gart_bus_addr;
		cur_gatt = SVRWRKS_GET_GATT(addr);
		cur_gatt[GET_GATT_OFF(addr)] = 
			(unsigned long) agp_bridge.scratch_page;
	}

	agp_bridge.tlb_flush(mem);
	return 0;
}

static struct gatt_mask serverworks_masks[] =
{
	{0x00000001, 0}
};

static struct aper_size_info_lvl2 serverworks_sizes[7] =
{
	{2048, 524288, 0x80000000},
	{1024, 262144, 0xc0000000},
	{512, 131072, 0xe0000000},
	{256, 65536, 0xf0000000},
	{128, 32768, 0xf8000000},
	{64, 16384, 0xfc000000},
	{32, 8192, 0xfe000000}
};

static int serverworks_agp_enable(u32 mode)
{
	struct pci_dev *device = NULL;
	u32 command, scratch, cap_id;
	u8 cap_ptr;

	pci_read_config_dword(serverworks_private.svrwrks_dev,
			      agp_bridge.capndx + 4,
			      &command);

	/*
	 * PASS1: go throu all devices that claim to be
	 *        AGP devices and collect their data.
	 */

	while ((device = firegl_pci_find_class(PCI_CLASS_DISPLAY_VGA << 8,
					device)) != NULL) {

		pci_read_config_dword(device, 0x04, &scratch);

		if (!(scratch & 0x00100000))
			continue;

		pci_read_config_byte(device, 0x34, &cap_ptr);

		if (cap_ptr != 0x00) {
			do {
				pci_read_config_dword(device,
						      cap_ptr, &cap_id);

				if ((cap_id & 0xff) != 0x02)
					cap_ptr = (cap_id >> 8) & 0xff;
			}
			while (((cap_id & 0xff) != 0x02) && (cap_ptr != 0x00));
		}
		if (cap_ptr != 0x00) {
			/*
			 * Ok, here we have a AGP device. Disable impossible 
			 * settings, and adjust the readqueue to the minimum.
			 */

			pci_read_config_dword(device, cap_ptr + 4, &scratch);
#ifdef FGL_FIX
            /* AGP 1x or 2x or 4x - at least one of this list */
            /* mga g450 pci can be uncovered this way */
            if (!(scratch & 7))
                continue;
#endif /* FGL_FIX */

			/* adjust RQ depth */
			command =
                ((command & ~0xff000000) |
                 min((mode & 0xff000000),
                     min((command & 0xff000000),
                         (scratch & 0xff000000))
                    ));

			/* disable SBA if it's not supported */
			if (!((command & 0x00000200) &&
			      (scratch & 0x00000200) &&
			      (mode & 0x00000200)))
				command &= ~0x00000200;

			/* disable FW */
			command &= ~0x00000010;

			command &= ~0x00000008;

			if (!((command & 4) &&
			      (scratch & 4) &&
			      (mode & 4)))
				command &= ~0x00000004;

			if (!((command & 2) &&
			      (scratch & 2) &&
			      (mode & 2)))
				command &= ~0x00000002;

			if (!((command & 1) &&
			      (scratch & 1) &&
			      (mode & 1)))
				command &= ~0x00000001;
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10) 
            // the reference count has been increased in agp_backend_initialize.
        if (device)
            pci_dev_put(device); 
#endif

	/*
	 * PASS2: Figure out the 4X/2X/1X setting and enable the
	 *        target (our motherboard chipset).
	 */

	if (command & 4) {
		command &= ~3;	/* 4X */
	}
	if (command & 2) {
		command &= ~5;	/* 2X */
	}
	if (command & 1) {
		command &= ~6;	/* 1X */
	}
#ifdef FGL_FIX
    /* set AGP enable bit - only if a valid mode was determined */
    /* (a way to unhide mga g450 pci) */
    if (command & 7)
#endif
    	command |= 0x00000100;

	pci_write_config_dword(serverworks_private.svrwrks_dev,
			       agp_bridge.capndx + 8,
			       command);

	/*
	 * PASS3: Go throu all AGP devices and update the
	 *        command registers.
	 */

	while ((device = firegl_pci_find_class(PCI_CLASS_DISPLAY_VGA << 8,
					device)) != NULL) {
		pci_read_config_dword(device, 0x04, &scratch);

		if (!(scratch & 0x00100000))
			continue;

		pci_read_config_byte(device, 0x34, &cap_ptr);

		if (cap_ptr != 0x00) {
			do {
				pci_read_config_dword(device,
						      cap_ptr, &cap_id);

				if ((cap_id & 0xff) != 0x02)
					cap_ptr = (cap_id >> 8) & 0xff;
			}
			while (((cap_id & 0xff) != 0x02) && (cap_ptr != 0x00));
		}
		if (cap_ptr != 0x00)
        {
#ifdef FGL_FIX
            /* don't enable anything on a device that can not do */
            /* AGP 1x or 2x or 4x; ultimate check */
			pci_read_config_dword(device, cap_ptr + 4, &scratch);
            if (!(scratch & 7))
                continue;
#endif
			pci_write_config_dword(device, cap_ptr + 8, command);
        }
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10) 
            // the reference count has been increased in agp_backend_initialize.
        if (device)
            pci_dev_put(device); 
#endif

    return(0); /* success */
}

static int __init serverworks_setup (struct pci_dev *pdev)
{
	u32 temp;
	u32 temp2;

	serverworks_private.svrwrks_dev = pdev;

	agp_bridge.masks = serverworks_masks;
	agp_bridge.num_of_masks = 1;
	agp_bridge.aperture_sizes = (void *) serverworks_sizes;
	agp_bridge.size_type = LVL2_APER_SIZE;
	agp_bridge.num_aperture_sizes = 7;
	agp_bridge.dev_private_data = (void *) &serverworks_private;
	agp_bridge.needs_scratch_page = TRUE;
	agp_bridge.configure = serverworks_configure;
	agp_bridge.fetch_size = serverworks_fetch_size;
	agp_bridge.cleanup = serverworks_cleanup;
	agp_bridge.tlb_flush = serverworks_tlbflush;
	agp_bridge.mask_memory = serverworks_mask_memory;
	agp_bridge.agp_enable = serverworks_agp_enable;
	agp_bridge.cache_flush = global_cache_flush;
	agp_bridge.create_gatt_table = serverworks_create_gatt_table;
	agp_bridge.free_gatt_table = serverworks_free_gatt_table;
	agp_bridge.insert_memory = serverworks_insert_memory;
	agp_bridge.remove_memory = serverworks_remove_memory;
	agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
	agp_bridge.free_by_type = agp_generic_free_by_type;
	agp_bridge.agp_alloc_page = agp_generic_alloc_page;
	agp_bridge.agp_destroy_page = agp_generic_destroy_page;
    agp_bridge.suspend = agp_generic_suspend;
    agp_bridge.resume = agp_generic_resume;
    agp_bridge.cant_use_aperture = 0;

	pci_read_config_dword(agp_bridge.dev,
			      SVWRKS_APSIZE,
			      &temp);

	serverworks_private.gart_addr_ofs = 0x10;

	if(temp & PCI_BASE_ADDRESS_MEM_TYPE_64) {
		pci_read_config_dword(agp_bridge.dev,
				      SVWRKS_APSIZE + 4,
				      &temp2);
		if(temp2 != 0) {
			printk("Detected 64 bit aperture address, but top "
			       "bits are not zero.  Disabling agp\n");
			return -ENODEV;
		}
		serverworks_private.mm_addr_ofs = 0x18;
	} else {
		serverworks_private.mm_addr_ofs = 0x14;
	}

	pci_read_config_dword(agp_bridge.dev,
			      serverworks_private.mm_addr_ofs,
			      &temp);
	if(temp & PCI_BASE_ADDRESS_MEM_TYPE_64) {
		pci_read_config_dword(agp_bridge.dev,
				      serverworks_private.mm_addr_ofs + 4,
				      &temp2);
		if(temp2 != 0) {
			printk("Detected 64 bit MMIO address, but top "
			       "bits are not zero.  Disabling agp\n");
			return -ENODEV;
		}
	}

	return 0;
}


#ifdef __ia64__

#ifndef log2
#define log2(x)		ffz(~(x))
#endif

#define HP_ZX1_IOVA_BASE	GB(1UL)
#define HP_ZX1_IOVA_SIZE	GB(1UL)
#define HP_ZX1_GART_SIZE	(HP_ZX1_IOVA_SIZE / 2)
#define HP_ZX1_SBA_IOMMU_COOKIE	0x0000badbadc0ffeeUL

#define HP_ZX1_PDIR_VALID_BIT	0x8000000000000000UL
#define HP_ZX1_IOVA_TO_PDIR(va)	((va - hp_private.iova_base) >> \
					hp_private.io_tlb_shift)

static struct aper_size_info_fixed hp_zx1_sizes[] =
{
	{0, 0, 0},		/* filled in by hp_zx1_fetch_size() */
};

static struct gatt_mask hp_zx1_masks[] =
{
	{HP_ZX1_PDIR_VALID_BIT, 0}
};

static struct _hp_private {
	struct pci_dev *ioc;
	volatile u8 *registers;
	u64 *io_pdir;		// PDIR for entire IOVA
	u64 *gatt;		// PDIR just for GART (subset of above)
	u64 gatt_entries;
	u64 iova_base;
	u64 gart_base;
	u64 gart_size;
	u64 io_pdir_size;
	int io_pdir_owner;	// do we own it, or share it with sba_iommu?
	int io_page_size;
	int io_tlb_shift;
	int io_tlb_ps;		// IOC ps config
	int io_pages_per_kpage;
} hp_private;

static int __init hp_zx1_ioc_shared(void)
{
	struct _hp_private *hp = &hp_private;

	printk(KERN_INFO PFX "HP ZX1 IOC: IOPDIR shared with sba_iommu\n");

	/*
	 * IOC already configured by sba_iommu module; just use
	 * its setup.  We assume:
	 * 	- IOVA space is 1Gb in size
	 * 	- first 512Mb is IOMMU, second 512Mb is GART
	 */
	hp->io_tlb_ps = INREG64(hp->registers, HP_ZX1_TCNFG);
	switch (hp->io_tlb_ps) {
		case 0: hp->io_tlb_shift = 12; break;
		case 1: hp->io_tlb_shift = 13; break;
		case 2: hp->io_tlb_shift = 14; break;
		case 3: hp->io_tlb_shift = 16; break;
		default:
			printk(KERN_ERR PFX "Invalid IOTLB page size "
			       "configuration 0x%x\n", hp->io_tlb_ps);
			hp->gatt = 0;
			hp->gatt_entries = 0;
			return -ENODEV;
	}
	hp->io_page_size = 1 << hp->io_tlb_shift;
	hp->io_pages_per_kpage = PAGE_SIZE / hp->io_page_size;

	hp->iova_base = INREG64(hp->registers, HP_ZX1_IBASE) & ~0x1;
	hp->gart_base = hp->iova_base + HP_ZX1_IOVA_SIZE - HP_ZX1_GART_SIZE;

	hp->gart_size = HP_ZX1_GART_SIZE;
	hp->gatt_entries = hp->gart_size / hp->io_page_size;

	hp->io_pdir = phys_to_virt(INREG64(hp->registers, HP_ZX1_PDIR_BASE));
	hp->gatt = &hp->io_pdir[HP_ZX1_IOVA_TO_PDIR(hp->gart_base)];

	if (hp->gatt[0] != HP_ZX1_SBA_IOMMU_COOKIE) {
	    	hp->gatt = 0;
		hp->gatt_entries = 0;
		printk(KERN_ERR PFX "No reserved IO PDIR entry found; "
		       "GART disabled\n");
		return -ENODEV;
	}

	return 0;
}

static int __init hp_zx1_ioc_owner(u8 ioc_rev)
{
	struct _hp_private *hp = &hp_private;

	printk(KERN_INFO PFX "HP ZX1 IOC: IOPDIR dedicated to GART\n");

	/*
	 * Select an IOV page size no larger than system page size.
	 */
	if (PAGE_SIZE >= KB(64)) {
		hp->io_tlb_shift = 16;
		hp->io_tlb_ps = 3;
	} else if (PAGE_SIZE >= KB(16)) {
		hp->io_tlb_shift = 14;
		hp->io_tlb_ps = 2;
	} else if (PAGE_SIZE >= KB(8)) {
		hp->io_tlb_shift = 13;
		hp->io_tlb_ps = 1;
	} else {
		hp->io_tlb_shift = 12;
		hp->io_tlb_ps = 0;
	}
	hp->io_page_size = 1 << hp->io_tlb_shift;
	hp->io_pages_per_kpage = PAGE_SIZE / hp->io_page_size;

	hp->iova_base = HP_ZX1_IOVA_BASE;
	hp->gart_size = HP_ZX1_GART_SIZE;
	hp->gart_base = hp->iova_base + HP_ZX1_IOVA_SIZE - hp->gart_size;

	hp->gatt_entries = hp->gart_size / hp->io_page_size;
	hp->io_pdir_size = (HP_ZX1_IOVA_SIZE / hp->io_page_size) * sizeof(u64);

	return 0;
}

static int __init hp_zx1_ioc_init(void)
{
	struct _hp_private *hp = &hp_private;
	struct pci_dev *ioc;
	int i;
	u8 ioc_rev;

	ioc = pci_find_device(PCI_VENDOR_ID_HP, PCI_DEVICE_ID_HP_ZX1_IOC, NULL);
	if (!ioc) {
		printk(KERN_ERR PFX "Detected HP ZX1 AGP bridge but no IOC\n");
		return -ENODEV;
	}
	hp->ioc = ioc;

	pci_read_config_byte(ioc, PCI_REVISION_ID, &ioc_rev);

	for (i = 0; i < PCI_NUM_RESOURCES; i++) {
		if (pci_resource_flags(ioc, i) == IORESOURCE_MEM) {
			hp->registers = (u8 *) ioremap(pci_resource_start(ioc,
									    i),
						    pci_resource_len(ioc, i));
			break;
		}
	}
	if (!hp->registers) {
		printk(KERN_ERR PFX "Detected HP ZX1 AGP bridge but no CSRs\n");

		return -ENODEV;
	}

	/*
	 * If the IOTLB is currently disabled, we can take it over.
	 * Otherwise, we have to share with sba_iommu.
	 */
	hp->io_pdir_owner = (INREG64(hp->registers, HP_ZX1_IBASE) & 0x1) == 0;

	if (hp->io_pdir_owner)
		return hp_zx1_ioc_owner(ioc_rev);

	return hp_zx1_ioc_shared();
}

static int hp_zx1_fetch_size(void)
{
	int size;

	size = hp_private.gart_size / MB(1);
	hp_zx1_sizes[0].size = size;
	agp_bridge.current_size = (void *) &hp_zx1_sizes[0];
	return size;
}

static int hp_zx1_configure(void)
{
	struct _hp_private *hp = &hp_private;

	agp_bridge.gart_bus_addr = hp->gart_base;
	agp_bridge.capndx = pci_find_capability(agp_bridge.dev, PCI_CAP_ID_AGP);
	pci_read_config_dword(agp_bridge.dev,
		agp_bridge.capndx + PCI_AGP_STATUS, &agp_bridge.mode);

	if (hp->io_pdir_owner) {
		OUTREG64(hp->registers, HP_ZX1_PDIR_BASE,
			virt_to_phys(hp->io_pdir));
		OUTREG64(hp->registers, HP_ZX1_TCNFG, hp->io_tlb_ps);
		OUTREG64(hp->registers, HP_ZX1_IMASK, ~(HP_ZX1_IOVA_SIZE - 1));
		OUTREG64(hp->registers, HP_ZX1_IBASE, hp->iova_base | 0x1);
		OUTREG64(hp->registers, HP_ZX1_PCOM,
			hp->iova_base | log2(HP_ZX1_IOVA_SIZE));
		INREG64(hp->registers, HP_ZX1_PCOM);
	}

	return 0;
}

static void hp_zx1_cleanup(void)
{
	struct _hp_private *hp = &hp_private;

	if (hp->io_pdir_owner)
		OUTREG64(hp->registers, HP_ZX1_IBASE, 0);
	iounmap((void *) hp->registers);
}

static void hp_zx1_tlbflush(agp_memory * mem)
{
	struct _hp_private *hp = &hp_private;

	OUTREG64(hp->registers, HP_ZX1_PCOM, 
		hp->gart_base | log2(hp->gart_size));
	INREG64(hp->registers, HP_ZX1_PCOM);
}

static int hp_zx1_create_gatt_table(void)
{
	struct _hp_private *hp = &hp_private;
	int i;

	if (hp->io_pdir_owner) {
		hp->io_pdir = (u64 *) __get_free_pages(GFP_KERNEL|__GFP_COMP,
						get_order(hp->io_pdir_size));
		if (!hp->io_pdir) {
			printk(KERN_ERR PFX "Couldn't allocate contiguous "
				"memory for I/O PDIR\n");
			hp->gatt = 0;
			hp->gatt_entries = 0;
			return -ENOMEM;
		}
		memset(hp->io_pdir, 0, hp->io_pdir_size);

		hp->gatt = &hp->io_pdir[HP_ZX1_IOVA_TO_PDIR(hp->gart_base)];
	}

	for (i = 0; i < hp->gatt_entries; i++) {
		hp->gatt[i] = (unsigned long) agp_bridge.scratch_page;
	}

	return 0;
}

static int hp_zx1_free_gatt_table(void)
{
	struct _hp_private *hp = &hp_private;
	
	if (hp->io_pdir_owner)
		free_pages((unsigned long) hp->io_pdir,
			    get_order(hp->io_pdir_size));
	else
		hp->gatt[0] = HP_ZX1_SBA_IOMMU_COOKIE;
	return 0;
}

static int hp_zx1_insert_memory(agp_memory * mem, off_t pg_start, int type)
{
	struct _hp_private *hp = &hp_private;
	int i, k;
	off_t j, io_pg_start;
	int io_pg_count;

	if (type != 0 || mem->type != 0) {
		return -EINVAL;
	}

	io_pg_start = hp->io_pages_per_kpage * pg_start;
	io_pg_count = hp->io_pages_per_kpage * mem->page_count;
	if ((io_pg_start + io_pg_count) > hp->gatt_entries) {
		return -EINVAL;
	}

	j = io_pg_start;
	while (j < (io_pg_start + io_pg_count)) {
		if (hp->gatt[j]) {
			return -EBUSY;
		}
		j++;
	}

	if (mem->is_flushed == FALSE) {
		CACHE_FLUSH();
		mem->is_flushed = TRUE;
	}

	for (i = 0, j = io_pg_start; i < mem->page_count; i++) {
		unsigned long paddr;

		paddr = mem->memory[i];
		for (k = 0;
		     k < hp->io_pages_per_kpage;
		     k++, j++, paddr += hp->io_page_size) {
			hp->gatt[j] = agp_bridge.mask_memory(paddr, type);
		}
	}

	agp_bridge.tlb_flush(mem);
	return 0;
}

static int hp_zx1_remove_memory(agp_memory * mem, off_t pg_start, int type)
{
	struct _hp_private *hp = &hp_private;
	int i, io_pg_start, io_pg_count;

	if (type != 0 || mem->type != 0) {
		return -EINVAL;
	}

	io_pg_start = hp->io_pages_per_kpage * pg_start;
	io_pg_count = hp->io_pages_per_kpage * mem->page_count;
	for (i = io_pg_start; i < io_pg_count + io_pg_start; i++) {
		hp->gatt[i] = agp_bridge.scratch_page;
	}

	agp_bridge.tlb_flush(mem);
	return 0;
}

static unsigned long hp_zx1_mask_memory(unsigned long addr, int type)
{
	return HP_ZX1_PDIR_VALID_BIT | addr;
}

#if 0 
// not used
static unsigned long hp_zx1_unmask_memory(unsigned long addr)
{
	return addr & ~(HP_ZX1_PDIR_VALID_BIT);
}
#endif

static int __init hp_zx1_setup (struct pci_dev *pdev)
{
	agp_bridge.masks = hp_zx1_masks;
	agp_bridge.dev_private_data = NULL;
	agp_bridge.size_type = FIXED_APER_SIZE;
	agp_bridge.needs_scratch_page = FALSE;
	agp_bridge.configure = hp_zx1_configure;
	agp_bridge.fetch_size = hp_zx1_fetch_size;
	agp_bridge.cleanup = hp_zx1_cleanup;
	agp_bridge.tlb_flush = hp_zx1_tlbflush;
	agp_bridge.mask_memory = hp_zx1_mask_memory;
//	agp_bridge.unmask_memory = hp_zx1_unmask_memory;
	agp_bridge.agp_enable = agp_generic_agp_enable;
	agp_bridge.cache_flush = global_cache_flush;
	agp_bridge.create_gatt_table = hp_zx1_create_gatt_table;
	agp_bridge.free_gatt_table = hp_zx1_free_gatt_table;
	agp_bridge.insert_memory = hp_zx1_insert_memory;
	agp_bridge.remove_memory = hp_zx1_remove_memory;
	agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
	agp_bridge.free_by_type = agp_generic_free_by_type;
	agp_bridge.agp_alloc_page = agp_generic_alloc_page;
	agp_bridge.agp_destroy_page = agp_generic_destroy_page;
	agp_bridge.cant_use_aperture = 1;

	return hp_zx1_ioc_init();

	(void) pdev; /* unused */
}

#endif /* __ia64__ */


//#ifdef CONFIG_AGP_ATI
static struct aper_size_info_lvl2 ati_generic_sizes[7] =
{
        {2048, 524288, 0x0000000c},
        {1024, 262144, 0x0000000a},
        {512, 131072, 0x00000008},
        {256, 65536, 0x00000006},
        {128, 32768, 0x00000004},
        {64, 16384, 0x00000002},
        {32, 8192, 0x00000000}
};

static struct gatt_mask ati_generic_masks[] =
{
        {0x00000001, 0}
};

typedef struct _ati_page_map {
        u32 *real;
    u32 *remapped;
} ati_page_map;

static struct _ati_generic_private {
        volatile u8 *registers;
        ati_page_map **gatt_pages;
        int num_tables;
} ati_generic_private;


/* This helper function converts a 40bits address
 * into a 32bits GART table entry.
 */
static u32 addr2entry(unsigned long addr)
{
    unsigned long ul_temp=0;
    u32 u32_temp=0;
    
    ul_temp = addr;
    u32_temp = (u32)(ul_temp >> 28) & 0x00000FF0;
    u32_temp |= (u32)(ul_temp & 0xFFFFF000);
    
    return u32_temp;
}

static int ati_create_page_map(ati_page_map *page_map)
{
        int i, err = 0;

        page_map->real = (u32 *) __get_free_page(GFP_KERNEL);
        if (page_map->real == NULL)
                return -ENOMEM;

        SetPageReserved(virt_to_page(page_map->real));
        /*
         * fredi - WARNING: added looking at the changes during
         * 2.4.20. I dont know if it's needed though.
         */
#ifdef CONFIG_X86
        err = change_page_attr(virt_to_page(page_map->real), 1, PAGE_KERNEL_NOCACHE);
#endif
        CACHE_FLUSH();
        page_map->remapped = ioremap_nocache(virt_to_phys(page_map->real),
                                            PAGE_SIZE);
        if (page_map->remapped == NULL || err) {
                ClearPageReserved(virt_to_page(page_map->real));
                free_page((unsigned long) page_map->real);
                page_map->real = NULL;
                return -ENOMEM;
        }
        CACHE_FLUSH();


        for(i = 0; i < PAGE_SIZE / sizeof(u32); i++)
    {
                page_map->remapped[i] = addr2entry(agp_bridge.scratch_page);
    }

        return 0;
}

static void ati_free_page_map(ati_page_map *page_map)
{
        /*
         * fredi - WARNING: added looking at the changes during
         * 2.4.20. I dont know if it's needed though.
         */
#ifdef CONFIG_X86
        change_page_attr(virt_to_page(page_map->real), 1, PAGE_KERNEL);
#endif
        iounmap(page_map->remapped);
        ClearPageReserved(virt_to_page(page_map->real));
        free_page((unsigned long) page_map->real);
}

static void ati_free_gatt_pages(void)
{
        int i;
        ati_page_map **tables;
        ati_page_map *entry;

        tables = ati_generic_private.gatt_pages;
        for(i = 0; i < ati_generic_private.num_tables; i++) {
                entry = tables[i];
                if (entry != NULL) {
                        if (entry->real != NULL)
                                ati_free_page_map(entry);
                        kfree(entry);
                }
        }
        kfree(tables);
}

static int ati_create_gatt_pages(int nr_tables)
{
        ati_page_map **tables;
        ati_page_map *entry;
        int retval = 0;
        int i;

        tables = kmalloc((nr_tables + 1) * sizeof(ati_page_map *),
                         GFP_KERNEL);
        if (tables == NULL)
                return -ENOMEM;

        memset(tables, 0, sizeof(ati_page_map *) * (nr_tables + 1));
        for (i = 0; i < nr_tables; i++) {
                entry = kmalloc(sizeof(ati_page_map), GFP_KERNEL);
                if (entry == NULL) {
                        retval = -ENOMEM;
                        break;
                }
                memset(entry, 0, sizeof(ati_page_map));
                tables[i] = entry;
                retval = ati_create_page_map(entry);
                if (retval != 0) break;
        }
        ati_generic_private.num_tables = nr_tables;
        ati_generic_private.gatt_pages = tables;

        if (retval != 0) ati_free_gatt_pages();

        return retval;
}

/*
 *Since we don't need contigious memory we just try
 * to get the gatt table once
 */

#define GET_PAGE_DIR_OFF(addr) (addr >> 22)
#define GET_PAGE_DIR_IDX(addr) (GET_PAGE_DIR_OFF(addr) - \
        GET_PAGE_DIR_OFF(agp_bridge.gart_bus_addr))
#define GET_GATT_OFF(addr) ((addr & 0x003ff000) >> 12)
#undef  GET_GATT /* (addr) */
#define GET_GATT(addr) (ati_generic_private.gatt_pages[\
        GET_PAGE_DIR_IDX(addr)]->remapped)

static int ati_insert_memory(agp_memory * mem,
                             off_t pg_start, int type)
{
        int i, j, num_entries;
    u32 *cur_gatt;
        unsigned long addr;

#ifdef DEBUG    
        printk(KERN_INFO PFX "Enter ati_insert_memory()! \n");
#endif

        num_entries = A_SIZE_LVL2(agp_bridge.current_size)->num_entries;

        if (type != 0 || mem->type != 0)
                return -EINVAL;

        if ((pg_start + mem->page_count) > num_entries)
                return -EINVAL;

        j = pg_start;
        while (j < (pg_start + mem->page_count)) {
                addr = (j * PAGE_SIZE) + agp_bridge.gart_bus_addr;
                cur_gatt = GET_GATT(addr);
        if (!PGE_EMPTY_64(cur_gatt[GET_GATT_OFF(addr)]))
                        return -EBUSY;
                j++;
        }

        if (mem->is_flushed == FALSE) {
                CACHE_FLUSH();
                mem->is_flushed = TRUE;
        }

        for (i = 0, j = pg_start; i < mem->page_count; i++, j++) 
    {
                addr = (j * PAGE_SIZE) + agp_bridge.gart_bus_addr;
                cur_gatt = GET_GATT(addr);

        /* Hack! Assume it snoopable! Try it! If not working, 
         * change it later! Should use mem->type to do it!!!!
         */
                cur_gatt[GET_GATT_OFF(addr)] =
                   addr2entry(((agp_bridge.mask_memory(mem->memory[i], mem->type)) & ~0x1L)) | 0x0D;
        }
        agp_bridge.tlb_flush(mem);
        return 0;
}

static int ati_remove_memory(agp_memory * mem, off_t pg_start,
                             int type)
{
        int i;
    u32 *cur_gatt;

        unsigned long addr;

        if (type != 0 || mem->type != 0) {
                return -EINVAL;
        }
        for (i = pg_start; i < (mem->page_count + pg_start); i++) {
                addr = (i * PAGE_SIZE) + agp_bridge.gart_bus_addr;
                cur_gatt = GET_GATT(addr);
                cur_gatt[GET_GATT_OFF(addr)] =
            addr2entry((unsigned long) agp_bridge.scratch_page);

        }

        agp_bridge.tlb_flush(mem);
        return 0;
}

static int ati_create_gatt_table(void)
{
        struct aper_size_info_lvl2 *value;
        ati_page_map page_dir;
        unsigned long addr;
        int retval;
        u32 temp;
        int i;
        struct aper_size_info_lvl2 *current_size;

        value = A_SIZE_LVL2(agp_bridge.current_size);
        retval = ati_create_page_map(&page_dir);
        if (retval != 0)
                return retval;

        retval = ati_create_gatt_pages(value->num_entries / 1024);
        if (retval != 0) {
                ati_free_page_map(&page_dir);
                return retval;
        }

        agp_bridge.gatt_table_real = (u32 *)page_dir.real;
        agp_bridge.gatt_table = (u32 *)page_dir.remapped;
        agp_bridge.gatt_bus_addr = virt_to_bus(page_dir.real);

        /* Write out the size register */
        current_size = A_SIZE_LVL2(agp_bridge.current_size);

        if ((agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS100) ||
            (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS200) ||
            (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS250)) {
                pci_read_config_dword(agp_bridge.dev, ATI_RS100_APSIZE, &temp);
                temp = (((temp & ~(0x0000000e)) | current_size->size_value)
                        | 0x00000001);
                pci_write_config_dword(agp_bridge.dev, ATI_RS100_APSIZE, temp);
                pci_read_config_dword(agp_bridge.dev, ATI_RS100_APSIZE, &temp);
        } else {
                pci_read_config_dword(agp_bridge.dev, ATI_RS300_APSIZE, &temp);
                temp = (((temp & ~(0x0000000e)) | current_size->size_value)
                        | 0x00000001);
                pci_write_config_dword(agp_bridge.dev, ATI_RS300_APSIZE, temp);
                pci_read_config_dword(agp_bridge.dev, ATI_RS300_APSIZE, &temp);
        }

        /*
         * Get the address for the gart region.
         * This is a bus address even on the alpha, b/c its
         * used to program the agp master not the cpu
         */
        pci_read_config_dword(agp_bridge.dev, ATI_APBASE, &temp);
        addr = (temp & PCI_BASE_ADDRESS_MEM_MASK);
        agp_bridge.gart_bus_addr = addr;

        /* Calculate the agp offset */
        for(i = 0; i < value->num_entries / 1024; i++, addr += 0x00400000) {
                page_dir.remapped[GET_PAGE_DIR_OFF(addr)] =
            addr2entry(virt_to_bus(ati_generic_private.gatt_pages[i]->real));
                page_dir.remapped[GET_PAGE_DIR_OFF(addr)] |= 0x00000001;
        }

        return 0;
}

static int ati_free_gatt_table(void)
{
        ati_page_map page_dir;

        page_dir.real = (u32 *)agp_bridge.gatt_table_real;
        page_dir.remapped = (u32 *)agp_bridge.gatt_table;

        ati_free_gatt_pages();
        ati_free_page_map(&page_dir);
        return 0;
}

static int ati_fetch_size(void)
{
        int i;
        u32 temp;
        struct aper_size_info_lvl2 *values;

        if ((agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS100) ||
            (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS200) ||
            (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS250)) {
                pci_read_config_dword(agp_bridge.dev, ATI_RS100_APSIZE, &temp);
        } else {
                pci_read_config_dword(agp_bridge.dev, ATI_RS300_APSIZE, &temp);
        }

        temp = (temp & 0x0000000e);
        values = A_SIZE_LVL2(agp_bridge.aperture_sizes);
        for (i = 0; i < agp_bridge.num_aperture_sizes; i++) {
                if (temp == values[i].size_value) {
                        agp_bridge.previous_size =
                            agp_bridge.current_size = (void *) (values + i);

                        agp_bridge.aperture_size_idx = i;
                        return values[i].size;
                }
        }

        return 0;
}

#define ATI_GART_VA_SIZE_64M_MSK   0x2   
#define ATI_GART_VA_SIZE_128M_MSK  0x4   
#define ATI_GART_VA_SIZE_64M       0x4000000   
#define ATI_GART_VA_SIZE_128M      0x8000000   

/* Try to remember gart base because when server shutdown
 * it should get reassigned.
 */
static unsigned int ati_gart_base = 0;

static int ati_configure(void)
{
    u32 temp;

    printk(KERN_INFO PFX "Enter ati_configure()! \n");

    /* Get the memory mapped registers */
    pci_read_config_dword(agp_bridge.dev, 
                          ATI_GART_MMBASE_ADDR, 
                          &temp);
    temp = (temp & 0xfffff000);
    ati_generic_private.registers = (volatile u8 *) ioremap(temp, 4096);

    if ((agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS100) ||
        (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS200) ||
        (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS250)) {
        pci_write_config_dword(agp_bridge.dev, ATI_RS100_IG_AGPMODE, 0x20000);
    } else {
        pci_write_config_dword(agp_bridge.dev, ATI_RS300_IG_AGPMODE, 0x20000);
    }

    OUTREG32(ati_generic_private.registers, 
             ATI_GART_FEATURE_ID, 
             0x60000);

    /* SIGNALED_SYSTEM_ERROR @ NB_STATUS */
    pci_read_config_dword(agp_bridge.dev, 4, &temp);
    /* Only clear it when there is SERR#line is asserted */
    if( temp & (1<<14) )
    {
        pci_write_config_dword(agp_bridge.dev, 4, temp | (1<<14));
    }

    /* Write out the address of the gatt table */
    OUTREG32(ati_generic_private.registers, ATI_GART_BASE,
             agp_bridge.gatt_bus_addr);

#ifdef DEBUG
    printk(KERN_INFO PFX " with ATI_GART_BASE = 0x%08lx \n", 
           agp_bridge.gatt_bus_addr);
    printk(KERN_INFO PFX " Leaving ati_configure()!\n");
#endif

    /* Flush the tlb */
    /* OUTREG32(ati_generic_private.registers, ATI_GART_CACHE_CNTRL, 1); */
    return 0;
}

static void ati_cleanup(void)
{
        struct aper_size_info_lvl2 *previous_size;
        u32 temp;

        previous_size = A_SIZE_LVL2(agp_bridge.previous_size);

        /* Write back the previous size and disable gart translation */
        if ((agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS100) ||
            (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS200) ||
            (agp_bridge.dev->device == PCI_DEVICE_ID_ATI_RS250)) {
                pci_read_config_dword(agp_bridge.dev, ATI_RS100_APSIZE, &temp);
                temp = ((temp & ~(0x0000000f)) | previous_size->size_value);
                pci_write_config_dword(agp_bridge.dev, ATI_RS100_APSIZE, temp);
        } else {
                pci_read_config_dword(agp_bridge.dev, ATI_RS300_APSIZE, &temp);
                temp = ((temp & ~(0x0000000f)) | previous_size->size_value);
                pci_write_config_dword(agp_bridge.dev, ATI_RS300_APSIZE, temp);
        }

        iounmap((void *) ati_generic_private.registers);
}

static void ati_tlbflush(agp_memory * mem)
{
    OUTREG32(ati_generic_private.registers, ATI_GART_CACHE_CNTRL, 1);
}

static unsigned long ati_mask_memory(unsigned long addr, int type)
{
        /* Memory type is ignored */
        return addr | agp_bridge.masks[0].mask;
}

static int __init ati_generic_setup (struct pci_dev *pdev)
{
        agp_bridge.masks = ati_generic_masks;
        agp_bridge.aperture_sizes = (void *) ati_generic_sizes;
        agp_bridge.size_type = LVL2_APER_SIZE;
        agp_bridge.num_aperture_sizes = 7;
        agp_bridge.dev_private_data = (void *) &ati_generic_private;
        agp_bridge.needs_scratch_page = TRUE;
        agp_bridge.configure = ati_configure;
        agp_bridge.fetch_size = ati_fetch_size;
        agp_bridge.cleanup = ati_cleanup;
        agp_bridge.tlb_flush = ati_tlbflush;
        agp_bridge.mask_memory = ati_mask_memory;
        agp_bridge.agp_enable = agp_generic_agp_enable;
        agp_bridge.cache_flush = global_cache_flush;
        agp_bridge.create_gatt_table = ati_create_gatt_table;
        agp_bridge.free_gatt_table = ati_free_gatt_table;
        agp_bridge.insert_memory = ati_insert_memory;
        agp_bridge.remove_memory = ati_remove_memory;
        agp_bridge.alloc_by_type = agp_generic_alloc_by_type;
        agp_bridge.free_by_type = agp_generic_free_by_type;
        agp_bridge.agp_alloc_page = agp_generic_alloc_page;
        agp_bridge.agp_destroy_page = agp_generic_destroy_page;
        agp_bridge.suspend = agp_generic_suspend;
        agp_bridge.resume = agp_generic_resume;
        agp_bridge.cant_use_aperture = 0;

        return 0;

        (void) pdev; /* unused */
}
//#endif /* CONFIG_AGP_ATI */

/* per-chipset initialization data.
 * note -- all chipsets for a single vendor MUST be grouped together
 */
static struct {
	unsigned short device_id; /* first, to make table easier to read */
	unsigned short vendor_id;
	enum chipset_type chipset;
	const char *vendor_name;
	const char *chipset_name;
	int (*chipset_setup) (struct pci_dev *pdev);
} agp_bridge_info[] __initdata = {

	{ PCI_DEVICE_ID_AL_M1541_0,
		PCI_VENDOR_ID_AL,
		ALI_M1541,
		"Ali",
		"M1541",
		ali_generic_setup },
	{ PCI_DEVICE_ID_AL_M1621_0,  
		PCI_VENDOR_ID_AL,
		ALI_M1621,
		"Ali",
		"M1621",
		ali_generic_setup },
	{ PCI_DEVICE_ID_AL_M1631_0,
		PCI_VENDOR_ID_AL,
		ALI_M1631,
		"Ali",
		"M1631",
		ali_generic_setup },
	{ PCI_DEVICE_ID_AL_M1632_0,
		PCI_VENDOR_ID_AL,
		ALI_M1632,
		"Ali",
		"M1632",
		ali_generic_setup },
	{ PCI_DEVICE_ID_AL_M1641_0,
		PCI_VENDOR_ID_AL,
		ALI_M1641,
		"Ali",
		"M1641",
		ali_generic_setup },
	{ PCI_DEVICE_ID_AL_M1647_0,
		PCI_VENDOR_ID_AL,
		ALI_M1647,
		"Ali",
		"M1647",
		ali_generic_setup },
	{ PCI_DEVICE_ID_AL_M1651_0,
		PCI_VENDOR_ID_AL,
		ALI_M1651,
		"Ali",
		"M1651",
		ali_generic_setup },  
	{ 0,
		PCI_VENDOR_ID_AL,
		ALI_GENERIC,
		"Ali",
		"Generic",
		ali_generic_setup },

    { PCI_DEVICE_ID_AMD_IRONGATE_0,
		PCI_VENDOR_ID_AMD,
		AMD_IRONGATE,
		"AMD",
		"751 (Irongate)",
		amd_irongate_setup },
    { PCI_DEVICE_ID_AMD_761_0, 
        PCI_VENDOR_ID_AMD, 
        AMD_761, 
        "AMD", 
        "761", 
        amd_irongate_setup }, 
    { PCI_DEVICE_ID_AMD_762_0, 
        PCI_VENDOR_ID_AMD, 
        AMD_762, // was: AMD_IRONGATE - suspected BUG
        "AMD", 
        "AMD 760MP", 
        amd_irongate_setup }, 
#if 0
    { PCI_DEVICE_ID_AMD_IGD4_0,
		PCI_VENDOR_ID_AMD,
		AMD_IGD4,
		"AMD",
		"IGD4",
		amd_irongate_setup },
    { PCI_DEVICE_ID_AMD_IGD4_2P_0,
		PCI_VENDOR_ID_AMD,
		AMD_IGD4_2P,
		"AMD",
		"IGD4-2P",
		amd_irongate_setup },
#endif
	{ 0,
		PCI_VENDOR_ID_AMD,
		AMD_GENERIC,
		"AMD",
		"Generic",
		amd_irongate_setup },

	{ PCI_DEVICE_ID_AMD_8151_0,
		PCI_VENDOR_ID_AMD,
		AMD_8151,
		"AMD",
		"8151",
		amd_8151_setup },

	{ PCI_DEVICE_ID_INTEL_82443LX_0,
		PCI_VENDOR_ID_INTEL,
		INTEL_LX,
		"Intel",
		"440LX",
		intel_generic_setup },
	{ PCI_DEVICE_ID_INTEL_82443BX_0,
		PCI_VENDOR_ID_INTEL,
		INTEL_BX,
		"Intel",
		"440BX",
		intel_generic_setup },
	{ PCI_DEVICE_ID_INTEL_82443GX_0,
		PCI_VENDOR_ID_INTEL,
		INTEL_GX,
		"Intel",
		"440GX",
		intel_generic_setup },
#ifdef FIREGL_AGP_HOOK
	{ PCI_DEVICE_ID_INTEL_815_MCH,
		PCI_VENDOR_ID_INTEL,
		INTEL_I815,
		"Intel",
		"i815",
		intel_generic_setup },
#else // FIREGL_AGP_HOOK
	{ PCI_DEVICE_ID_INTEL_815_0,
		PCI_VENDOR_ID_INTEL,
		INTEL_I815,
		"Intel",
		"i815",
		intel_generic_setup },
#endif // FIREGL_AGP_HOOK
	{ PCI_DEVICE_ID_INTEL_820_0,
		PCI_VENDOR_ID_INTEL,
		INTEL_I820,
		"Intel",
		"i820",
		intel_820_setup },
    { PCI_DEVICE_ID_INTEL_830_M_0, 
        PCI_VENDOR_ID_INTEL, 
        INTEL_I830_M, 
        "Intel", 
        "i830M", 
        intel_generic_setup },
#ifdef FIREGL_AGP_HOOK
    { PCI_DEVICE_ID_INTEL_82865_IG,
        PCI_VENDOR_ID_INTEL,
        INTEL_I865_G,
        "Intel",
        "i865G",
        intel_845_setup },
    { PCI_DEVICE_ID_INTEL_875_G_1,
        PCI_VENDOR_ID_INTEL,
        INTEL_I875_G,
        "Intel",
        "875G",
        intel_845_setup },
	{ PCI_DEVICE_ID_INTEL_840_1,
		PCI_VENDOR_ID_INTEL,
		INTEL_I840,
		"Intel",
		"i840",
		intel_840_setup },
#else
    { PCI_DEVICE_ID_INTEL_82865_HB,
        PCI_VENDOR_ID_INTEL,
        INTEL_I865_G,
        "Intel",
        "i865G",
        intel_845_setup },
    { PCI_DEVICE_ID_INTEL_875_G_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I875_G,
        "Intel",
        "875G",
        intel_845_setup },
	{ PCI_DEVICE_ID_INTEL_840_0,
		PCI_VENDOR_ID_INTEL,
		INTEL_I840,
		"Intel",
		"i840",
		intel_840_setup },
#endif // FIREGL_AGP_HOOK
    { PCI_DEVICE_ID_INTEL_845_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I845,
        "Intel",
        "i845 E/MP/MZ",
        intel_845_setup },
    { PCI_DEVICE_ID_INTEL_82845G_HB,
        PCI_VENDOR_ID_INTEL,
        INTEL_I845_G,
        "Intel",
        "i845 G/GL/GV/GE/PE",
        intel_830mp_setup },
    { PCI_DEVICE_ID_INTEL_850_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I850,
        "Intel",
        "i850",
        intel_850_setup },
    { PCI_DEVICE_ID_INTEL_855_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I855_PM,
        "Intel",
        "i855PM",
        intel_845_setup },
    { PCI_DEVICE_ID_INTEL_855_GM_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I855_PM,
        "Intel(R)",
        "i855PM",
        intel_845_setup },
    { PCI_DEVICE_ID_INTEL_855_PM_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I855_PM,
        "Intel(R)",
        "i855PM",
        intel_845_setup },
    { PCI_DEVICE_ID_INTEL_865_G_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I865_G,
        "Intel(R)",
        "i865G",
        intel_845_setup },
    { PCI_DEVICE_ID_INTEL_860_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I860,
        "Intel",
        "i860",
        intel_860_setup },
    { PCI_DEVICE_ID_INTEL_860_DP_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I860,
        "Intel",
        "i860",
        intel_860_setup },
    { PCI_DEVICE_ID_INTEL_860_QP_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I860,
        "Intel",
        "i860",
        intel_860_setup },
    { PCI_DEVICE_ID_INTEL_7205_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I7205,
        "Intel",
        "i7205",
//        intel_7505_setup },
        intel_860_setup }, /* semms to be i860 compatible in compatibility mode? */
    { PCI_DEVICE_ID_INTEL_7505_0,
        PCI_VENDOR_ID_INTEL,
        INTEL_I7505,
        "Intel",
        "i7505",
//        intel_7505_setup },
        intel_860_setup }, /* semms to be i860 compatible in compatibility mode? */
    { 0,
        PCI_VENDOR_ID_INTEL,
        INTEL_GENERIC,
        "Intel",
        "Generic",
        intel_generic_setup },

    { PCI_DEVICE_ID_SI_530,
		PCI_VENDOR_ID_SI,
		SIS_GENERIC,
		"SiS",
		"530",
		sis_generic_setup },
	{ PCI_DEVICE_ID_SI_540,
		PCI_VENDOR_ID_SI,
		SIS_GENERIC,
		"SiS",
		"540",
		sis_generic_setup },
	{ PCI_DEVICE_ID_SI_550,
		PCI_VENDOR_ID_SI,
		SIS_GENERIC,
		"SiS",
		"550",
		sis_generic_setup },
	{ PCI_DEVICE_ID_SI_620,
		PCI_VENDOR_ID_SI,
		SIS_GENERIC,
		"SiS",
		"620",
		sis_generic_setup },
	{ PCI_DEVICE_ID_SI_630,
		PCI_VENDOR_ID_SI,
		SIS_GENERIC,
		"SiS",
		"630",
		sis_generic_setup },
    { PCI_DEVICE_ID_SI_645,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "645",
        sis_generic_setup },
#ifndef __ia64__
    { PCI_DEVICE_ID_SI_646,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "646",
        sis_generic_setup },
    { PCI_DEVICE_ID_SI_645DX, /* this might add support for the Medion PCs series with the Radeon 9600 TX board */
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "645dx", /* 648 ? */
        sis_generic_setup },
    { PCI_DEVICE_ID_SI_648,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "648",
        sis_generic_setup },
    { PCI_DEVICE_ID_SI_650,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "650",
        sis_generic_setup },
    { PCI_DEVICE_ID_SI_651,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "651",
        sis_generic_setup },
    { PCI_DEVICE_ID_SI_730,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "730",
        sis_generic_setup },
	{ PCI_DEVICE_ID_SI_735,
		PCI_VENDOR_ID_SI,
		SIS_GENERIC,
		"SiS",
		"735",
		sis_generic_setup },
    { PCI_DEVICE_ID_SI_740,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "740",
        sis_generic_setup },
    { PCI_DEVICE_ID_SI_746,
        PCI_VENDOR_ID_SI,
        SIS_GENERIC,
        "SiS",
        "746",
        sis_generic_setup },
#endif // !__ia64__
	{ 0,
		PCI_VENDOR_ID_SI,
		SIS_GENERIC,
		"SiS",
		"Generic",
		sis_generic_setup },

    { PCI_DEVICE_ID_VIA_8501_0,
        PCI_VENDOR_ID_VIA,
        VIA_MVP4,
        "Via",
        "MVP4",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_82C597_0,
        PCI_VENDOR_ID_VIA,
        VIA_VP3,
        "Via",
        "VP3",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_82C598_0,
        PCI_VENDOR_ID_VIA,
        VIA_MVP3,
        "Via",
        "MVP3",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_8601_0,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_PLE133,
        "Via",
        "Apollo PLE133",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_82C691_0,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_PRO,
        "Via",
        "Apollo Pro/133/133A",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_8371_0,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_KX133,
        "Via",
        "Apollo KX133",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_8363_0,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_KT133,
        "Via",
        "Apollo KT133/KM133",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_8367_0, 
        PCI_VENDOR_ID_VIA, 
        VIA_APOLLO_KT266, 
        "Via", 
        "Apollo KT266/KT266A/KT333",
        via_generic_setup }, 
    { PCI_DEVICE_ID_VIA_8375,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_KM266,
        "Via",
        "Apollo KM266/KL266",
        via_generic_setup },
    { PCI_DEVICE_ID_VIA_8377_0,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_KT400,
        "Via",
        "Apollo KT400",
        via_kt400_setup }, // AGP v2
    { PCI_DEVICE_ID_VIA_8754, // == PCI_DEVICE_ID_VIA_P4X333,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_P4X400,
        "Via",
        "Apollo P4X400",
        via_generic_setup }, // AGP v2
    { PCI_DEVICE_ID_VIA_P4M266,
        PCI_VENDOR_ID_VIA,
        VIA_APOLLO_P4M266,
        "Via",
        "Apollo P4M266",
        via_generic_setup },
    { 0,
        PCI_VENDOR_ID_VIA,
        VIA_GENERIC,
        "Via",
        "Generic",
        via_generic_setup },

#ifdef FIREGL_AGP_SVRWKS
    // BroadCom-ServerWorks aka Fujitsu - Reliance Computer Corp. (RCC)
	{ PCI_DEVICE_ID_SERVERWORKS_CNB20LE_MCH_0,
		PCI_VENDOR_ID_SERVERWORKS,
		SERVERWORKS_CNB20LE,
		"ServerWorks",
		"CNB20LE",
		fgl_svrwks_cnb20le_setup },
	{ PCI_DEVICE_ID_SERVERWORKS_CNB20HE_MCH_0,
		PCI_VENDOR_ID_SERVERWORKS,
		SERVERWORKS_CNB20HE,
		"ServerWorks",
		"CNB20HE",
		fgl_svrwks_cnb20he_setup },
#if 0
    // no generic for now
	{ 0,
		PCI_VENDOR_ID_SERVERWORKS,
		SERVERWROKS_GENERIC,
		"ServerWorks",
		"Generic",
		fgl_svrwks_generic_setup },
#endif
#endif // FIREGL_AGP_SVRWKS

    { PCI_DEVICE_ID_NVIDIA_NFORCE,
        PCI_VENDOR_ID_NVIDIA,
        NVIDIA_NFORCE,
        "nVidia",
        "nForce",
        nvidia_nforce_setup },
    { PCI_DEVICE_ID_NVIDIA_NFORCE2,
        PCI_VENDOR_ID_NVIDIA,
        NVIDIA_NFORCE2,
        "nVidia",
        "nForce2",
        nvidia_nforce_setup },
    { PCI_DEVICE_ID_NVIDIA_NFORCE3,
        PCI_VENDOR_ID_NVIDIA,
        NVIDIA_NFORCE3,
        "NVIDIA",
        "nForce3/K8 On-CPU GART",
        nvidia_x86_64_setup },
    { PCI_DEVICE_ID_NVIDIA_NFORCE3S,
        PCI_VENDOR_ID_NVIDIA,
        NVIDIA_NFORCE3,
        "NVIDIA",
        "nForce3S/K8 On-CPU GART",
        nvidia_x86_64_setup },
    { 0, /* generic */
        PCI_VENDOR_ID_NVIDIA,
        NVIDIA_NFORCE,
        "nVidia",
        "Generic",
        nvidia_nforce_setup },

#ifdef __ia64__
	{ PCI_DEVICE_ID_HP_ZX1_LBA,
		PCI_VENDOR_ID_HP,
		HP_ZX1,
		"HP",
		"ZX1",
		hp_zx1_setup },
#endif // __ia64__



//#ifdef CONFIG_AGP_ATI
    { PCI_DEVICE_ID_ATI_RS100,
      PCI_VENDOR_ID_ATI,
      ATI_RS100,
      "ATI",
      "IGP320/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS200,
      PCI_VENDOR_ID_ATI,
      ATI_RS200,
      "ATI",
      "IGP330/340/345/350/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS250,
      PCI_VENDOR_ID_ATI,
      ATI_RS250,
      "ATI",
      "IGP7000/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS300_100,
      PCI_VENDOR_ID_ATI,
      ATI_RS300_100,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS300_133,
      PCI_VENDOR_ID_ATI,
      ATI_RS300_133,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS300_166,
      PCI_VENDOR_ID_ATI,
      ATI_RS300_166,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS300_200,
      PCI_VENDOR_ID_ATI,
      ATI_RS300_200,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS350_100,
      PCI_VENDOR_ID_ATI,
      ATI_RS350_100,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS350_133,
      PCI_VENDOR_ID_ATI,
      ATI_RS350_133,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS350_166,
      PCI_VENDOR_ID_ATI,
      ATI_RS350_166,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
    { PCI_DEVICE_ID_ATI_RS350_200,
      PCI_VENDOR_ID_ATI,
      ATI_RS350_200,
      "ATI",
      "IGP9100/M",
      ati_generic_setup },
//#endif /* CONFIG_AGP_ATI */

	{ 0, }, /* dummy final entry, always present */
};


/* scan table above for supported devices */
static int __init agp_lookup_host_bridge (struct pci_dev *pdev)
{
	int i;
	
    // scan for vendor first 
	for (i = 0; i < ARRAY_SIZE (agp_bridge_info); i++)
		if (pdev->vendor == agp_bridge_info[i].vendor_id)
			break;

	if (i >= ARRAY_SIZE (agp_bridge_info)) {
		printk (KERN_DEBUG PFX "unsupported bridge\n");
		return -ENODEV;
	}

    // scan for device in vendor section
	while ((i < ARRAY_SIZE (agp_bridge_info)) &&
	       (agp_bridge_info[i].vendor_id == pdev->vendor))
    {
		if (pdev->device == agp_bridge_info[i].device_id) 
        {
            // special case handling for AL_M1626
			if (pdev->device == PCI_DEVICE_ID_AL_M1621_0)
            {
				u8 hidden_1621_id;

				pci_read_config_byte(pdev, 0xFB, &hidden_1621_id);
				switch (hidden_1621_id) {
				case 0x31:
					agp_bridge_info[i].chipset_name="M1631";
					break;
				case 0x32:
					agp_bridge_info[i].chipset_name="M1632";
					break;
				case 0x41:
					agp_bridge_info[i].chipset_name="M1641";
					break;
				case 0x43:
                    // TODO ???
					break;
				case 0x47:
					agp_bridge_info[i].chipset_name="M1647";
					break;
				case 0x51:
					agp_bridge_info[i].chipset_name="M1651";
					break;
				default:
					break;
				}
			}
			printk (KERN_INFO PFX "Detected %s %s chipset\n",
				agp_bridge_info[i].vendor_name,
				agp_bridge_info[i].chipset_name);
			agp_bridge.type = agp_bridge_info[i].chipset;
			return agp_bridge_info[i].chipset_setup (pdev);
		}
		
		i++;
	}

	i--; /* point to vendor generic entry (device_id == 0) */

	/* try init anyway, if user requests it AND
	 * there is a 'generic' bridge entry for this vendor */
	if (agp_try_unsupported)
    {
      if (agp_bridge_info[i].device_id == 0) 
      {
		printk(KERN_WARNING PFX "Trying generic %s routines"
		       " for device id: %04x\n",
		       agp_bridge_info[i].vendor_name, pdev->device);
		agp_bridge.type = agp_bridge_info[i].chipset;
		return agp_bridge_info[i].chipset_setup (pdev);
      }
      else
      {   
    	printk(KERN_ERR PFX "Unsupported %s chipset (device id: %04x),"
	       " no generic code availabel for that chip vendor.\n",
	       agp_bridge_info[i].vendor_name, pdev->device);
      }
	}
    else
    {
    	printk(KERN_ERR PFX "Unsupported %s chipset (device id: %04x),"
	       " you might want to try agp_try_unsupported=1.\n",
	       agp_bridge_info[i].vendor_name, pdev->device);
    }
	return -ENODEV;
}

// FGL - begin
// capabilities helper routines

static u8 get_pcidev_capptr(struct pci_dev *dev)
{
    u16 pcists;
    u8 cap_ptr;
    
    pci_read_config_word(dev, PCI_STATUS, &pcists);
    if( pcists & PCI_STATUS_CAP_LIST )
        pci_read_config_byte(dev, PCI_CAPABILITY_LIST, &cap_ptr);
    else
        cap_ptr = 0;
        
    return cap_ptr;
}

static u8 find_agpv2_capptr(struct pci_dev *dev, u8 cap_ptr)
{
    while( cap_ptr ) /* browse all pcidevice cap entries */
    {
        u32 acapid;
        u8 capid;
        
        pci_read_config_dword(dev, cap_ptr, &acapid);
        capid = acapid & 0xff;
        if( capid == PCI_CAP_ID_AGP ) // are these AGP caps?
        {
            u8 agp_version_maj;

            agp_version_maj = (acapid >> 20) & 0x0f;
            if( agp_version_maj > 3 )
            {
                // unknown standard
                // not yet supported
            }
            else
            if( agp_version_maj == 3 )
            {
                u32 agpstat;
                
                pci_read_config_dword(dev, cap_ptr+PCI_AGP_STATUS, &agpstat);
                if( agpstat & (1<<3) ) // check signalling mode bit
                {
                    // device is in AGPv3 mode
                    // not a topic of our search
                }            
                else
                {
                    // device is in AGPv3 compatibility mode (=AGPv2)
                    return cap_ptr; /* found */
                }
            }
            else
            {
                // AGP v1 (maj==0 || maj==1) and AGP v2 (maj==2)
                return cap_ptr; /* found */
            }
        }

        // advance to next caps entry        
        cap_ptr = (acapid >> 8) & 0xff;
    }
    
    return 0; /* not found */
}

static u8 find_agpv3_agpv2compat_capptr(struct pci_dev *dev, u8 cap_ptr)
{
    while( cap_ptr ) /* browse all pcidevice cap entries */
    {
        u32 acapid;
        u8 capid;
        
        pci_read_config_dword(dev, cap_ptr, &acapid);
        capid = acapid & 0xff;
        if( capid == PCI_CAP_ID_AGP ) // are these AGP caps?
        {
            u8 agp_version_maj;

            agp_version_maj = (acapid >> 20) & 0x0f;
            if( agp_version_maj > 3 )
            {
                // unknown standard
                // not yet supported
            }
            else
            if( agp_version_maj == 3 )
            {
                u32 agpstat;
                
                pci_read_config_dword(dev, cap_ptr+PCI_AGP_STATUS, &agpstat);
                if( agpstat & (1<<3) ) // check signalling mode bit
                {
                    // device is in AGPv3 mode
                    // not topic of our search
                }            
                else
                {
                    // device is in AGPv3 compatibility mode (=AGPv2)
                    return cap_ptr; /* found */
                }
            }
            else
            {
                // AGP v1 (maj==0 || maj==1) and AGP v2 (maj==2)
                // old standard, not topic of our search
            }
        }

        // advance to next caps entry        
        cap_ptr = (acapid >> 8) & 0xff;
    }
    
    return 0; /* not found */
}

static u8 find_agpv3_capptr(struct pci_dev *dev, u8 cap_ptr)
{
    while( cap_ptr ) /* browse all pcidevice cap entries */
    {
        u32 acapid;
        u8 capid;
        
        pci_read_config_dword(dev, cap_ptr, &acapid);
        capid = acapid & 0xff;
        if( capid == PCI_CAP_ID_AGP ) // are these AGP caps?
        {
            u8 agp_version_maj;

            agp_version_maj = (acapid >> 20) & 0x0f;
            if( agp_version_maj > 3 )
            {
                // unknown standard
                // not yet supported
            }
            else
            if( agp_version_maj == 3 )
            {
                u32 agpstat;
                
                pci_read_config_dword(dev, cap_ptr+PCI_AGP_STATUS, &agpstat);
                if( agpstat & (1<<3) ) // check signalling mode bit
                {
                    // device is in AGPv3 mode
                    return cap_ptr; /* found */
                }            
                else
                {
                    // device is in AGPv3 compatibility mode (=AGPv2)
                    // not topic of our search
                }
            }
            else
            {
                // AGP v1 (maj==0 || maj==1) and AGP v2 (maj==2)
                // old standard, not topic of our search
            }
        }

        // advance to next caps entry        
        cap_ptr = (acapid >> 8) & 0xff;
    }
    
    return 0; /* not found */
}

// FGL - end

#ifdef __x86_64__
static int agp_check_supported_device(struct pci_dev *dev) 
{

  int i;

  for(i = 0; i < ARRAY_SIZE (agp_bridge_info); i++) 
  {
    if(dev->vendor == agp_bridge_info[i].vendor_id &&
       dev->device == agp_bridge_info[i].device_id)
      return 1;
  }

  return 0;
}
#endif

/* Supported Device Scanning routine */

static int __init agp_find_supported_device(void)
{
	struct pci_dev *dev = NULL;
    u8 cap_ptr = 0x00;

    // locate host bridge device
#ifdef __x86_64__
    do {
        dev = firegl_pci_find_class(PCI_CLASS_BRIDGE_HOST << 8, dev);
    } while((dev) && !agp_check_supported_device(dev));
#else
    if ((dev = firegl_pci_find_class(PCI_CLASS_BRIDGE_HOST << 8, NULL)) == NULL)
        return -ENODEV;
#endif

#ifdef __x86_64__
    /* If there is any K8 northbridge in the system always use the K8 driver */
    if (agp_try_unsupported
        && pci_find_device(PCI_VENDOR_ID_AMD, 0x1103, NULL)
        && !pci_find_device(PCI_VENDOR_ID_NVIDIA,
                            PCI_DEVICE_ID_NVIDIA_NFORCE3,
                            NULL)) {

      u8 cap_ptr = 0x00;
      /* find capndx */
      cap_ptr = pci_find_capability(dev, PCI_CAP_ID_AGP);
      if (cap_ptr == 0x00)
        return -ENODEV;
      agp_bridge.capndx = cap_ptr;

      /* Fill in the mode register */
      pci_read_config_dword(agp_bridge.dev,
                            agp_bridge.capndx + 4,
                            &agp_bridge.mode);

      printk(KERN_INFO PFX "Detected GART in AMD K8 Northbridge\n");
      agp_bridge.type = AMD_8151;
      return amd_8151_setup(dev);
    }
#endif

	agp_bridge.dev = dev;
    agp_bridge_pci2pci.dev = dev;

	/* need to test for I810 here - this is a special exit path */
	if (dev->vendor == PCI_VENDOR_ID_INTEL) {
		struct pci_dev *sub_dev;

		switch (dev->device) {
		case PCI_DEVICE_ID_INTEL_810_0:
			sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
					       PCI_DEVICE_ID_INTEL_810_1,
						   NULL);
			if (sub_dev == NULL) {
				printk(KERN_ERR PFX "Detected an Intel i810,"
				       " but could not find the secondary"
				       " device.\n");
				return -ENODEV;
			}
			printk(KERN_INFO PFX "Detected an Intel "
			       "i810 Chipset.\n");
			agp_bridge.type = INTEL_I810;
			return intel_i810_setup (sub_dev);

		case PCI_DEVICE_ID_INTEL_810_DC100_0:
			sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
					 PCI_DEVICE_ID_INTEL_810_DC100_1,
						   NULL);
			if (sub_dev == NULL) {
				printk(KERN_ERR PFX "Detected an Intel i810 "
				       "DC100, but could not find the "
				       "secondary device.\n");
				return -ENODEV;
			}
			printk(KERN_INFO PFX "Detected an Intel i810 "
			       "DC100 Chipset.\n");
			agp_bridge.type = INTEL_I810;
			return intel_i810_setup(sub_dev);

		case PCI_DEVICE_ID_INTEL_810_E_0:
			sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
					     PCI_DEVICE_ID_INTEL_810_E_1,
						   NULL);
			if (sub_dev == NULL) {
				printk(KERN_ERR PFX "Detected an Intel i810 E"
				    ", but could not find the secondary "
				       "device.\n");
				return -ENODEV;
			}
			printk(KERN_INFO PFX "Detected an Intel i810 E "
			       "Chipset.\n");
			agp_bridge.type = INTEL_I810;
			return intel_i810_setup(sub_dev);

#ifdef FIREGL_AGP_HOOK
		 case PCI_DEVICE_ID_INTEL_815_MCH:
		   /* The i815 can operate either as an i810 style
		    * integrated device, or as an AGP4X motherboard.
		    */
			sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
						   PCI_DEVICE_ID_INTEL_815_GFX,
						   NULL);
            if (sub_dev != NULL)
            {
                printk(KERN_INFO PFX "Detected an Intel i815 "
                    "chipset in graphics mode.\n");
                agp_bridge.type = INTEL_I815;
                return intel_i810_setup(sub_dev);
            }
            
			sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
						   PCI_DEVICE_ID_INTEL_815_AGP,
						   NULL);
            if (sub_dev != NULL)
            {
                printk(KERN_INFO PFX "Detected an Intel i815 "
                    "chipset in AGP-bridge mode.\n");
                agp_bridge.type = INTEL_I815;
                
                /* determine AGP register set offset */
			    agp_bridge.capndx = 0xA0; /* fixed value for this chipset config - skip browsing linked list */

			    /* fill in the mode register */
			    pci_read_config_dword(agp_bridge.dev,
			        agp_bridge.capndx + 4, // AGP status register
			        &agp_bridge.mode);

                return fgl_intel_i815_AGP_setup(sub_dev);
            }
			printk(KERN_ERR PFX "Detected an "
				       "Intel i815, but could not find the"
				       " secondary device.\n");
			agp_bridge.type = NOT_SUPPORTED;
			return -ENODEV;
#else
		 case PCI_DEVICE_ID_INTEL_815_0:
		   /* The i815 can operate either as an i810 style
		    * integrated device, or as an AGP4X motherboard.
		    *
		    * This only addresses the first mode:
		    */
			sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
						   PCI_DEVICE_ID_INTEL_815_1,
						   NULL);
			if (sub_dev == NULL) {
				printk(KERN_ERR PFX "Detected an "
				       "Intel i815, but could not find the"
				       " secondary device.\n");
				agp_bridge.type = NOT_SUPPORTED;
				return -ENODEV;
			}
			printk(KERN_INFO PFX "Detected an Intel i815 "
			       "chipset in AGP mode.\n");
			agp_bridge.type = INTEL_I810; // ??? is i810 compatible to i815 ???
			return intel_i810_setup(sub_dev);
#endif
        case PCI_DEVICE_ID_INTEL_830_M_0: 
            sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
                           PCI_DEVICE_ID_INTEL_830_M_1, 
                           NULL); 
            if(PCI_FUNC(sub_dev->devfn) != 0) { 
                sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
                               PCI_DEVICE_ID_INTEL_830_M_1, 
                               sub_dev); 
            } 
    
            if (sub_dev == NULL) { 
                printk(KERN_ERR PFX "Detected an " 
                    "Intel 830M, but could not find the" 
                    " secondary device.\n"); 
                agp_bridge.type = NOT_SUPPORTED; 
                return -ENODEV; 
            } 
            printk(KERN_INFO PFX "Detected an Intel " 
                "830M Chipset.\n"); 
            agp_bridge.type = INTEL_I810; 
            return intel_i830_setup(sub_dev);

        case PCI_DEVICE_ID_INTEL_82845G_HB:
            sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                PCI_DEVICE_ID_INTEL_82845G_IG, NULL);
            if(sub_dev && PCI_FUNC(sub_dev->devfn) != 0) {
                sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                    PCI_DEVICE_ID_INTEL_82845G_IG, sub_dev);
            }

            if (sub_dev == NULL) {
                /*
                 * We probably have a I845MP chipset without integrated graphics.
                 * There might be an external graphics card. Init will be done later.
                 */
                printk(KERN_INFO PFX "Detected an Intel 845G Chipset, no integrated grapics found.\n");
                agp_bridge.type = INTEL_I845_G;

            	/* fill in the mode register */
            	pci_read_config_dword(agp_bridge.dev,
			        agp_bridge.capndx + 4, // AGP status register
			        &agp_bridge.mode);

                break;
            }

            printk(KERN_INFO PFX "Detected an Intel 845G Chipset with integrated grapics.\n");
            agp_bridge.type = INTEL_I810;
            return intel_i830_setup(sub_dev);

        case PCI_DEVICE_ID_INTEL_855_GM_0:
            sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                PCI_DEVICE_ID_INTEL_855_GM_1, NULL);
            if(sub_dev && PCI_FUNC(sub_dev->devfn) != 0)
            {
                sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                            PCI_DEVICE_ID_INTEL_855_GM_1,
                            sub_dev);
            }

            if (sub_dev == NULL) {
                /*
                 * We probably have an 855PM chipset with an external graphics
                 * card. It will be initialized later.
                 */
                printk(KERN_INFO PFX "Detected an Intel 855PM Chipset, no integrated grapics found.\n");
                agp_bridge.type = INTEL_I855_PM;

            	/* fill in the mode register */
            	pci_read_config_dword(agp_bridge.dev,
			        agp_bridge.capndx + 4, // AGP status register
			        &agp_bridge.mode);

                break;
            }

            {
                u32 capval = 0;
                const char *name = "855GM/852GM";

                pci_read_config_dword(dev, I85X_CAPID, &capval);
                switch ((capval >> I85X_VARIANT_SHIFT) & I85X_VARIANT_MASK)
                {
                case I855_GME:
                    name = "855GME";
                    break;
                case I855_GM:
                    name = "855GM";
                    break;
                case I852_GME:
                    name = "852GME";
                    break;
                case I852_GM:
                    name = "852GM";
                    break;
                }

                printk(KERN_INFO PFX "Detected an Intel %s Chipset.\n", name);
            }

            agp_bridge.type = INTEL_I810;
            return intel_i830_setup(sub_dev);

        case PCI_DEVICE_ID_INTEL_855_PM_0:
            sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                            PCI_DEVICE_ID_INTEL_855_PM_1, NULL);
            if(sub_dev && PCI_FUNC(sub_dev->devfn) != 0)
            {
                sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                            PCI_DEVICE_ID_INTEL_855_PM_1,
                            sub_dev);
            }

            if (sub_dev == NULL)
            {
                /*
                 * We probably have an 855PM chipset with an external graphics
                 * card. It will be initialized later.
                 */
                printk(KERN_INFO PFX "Detected an Intel 855PM Chipset, no integrated grapics found.\n");
                agp_bridge.type = INTEL_I855_PM;

            	/* fill in the mode register */
            	pci_read_config_dword(agp_bridge.dev,
			        agp_bridge.capndx + 4, // AGP status register
			        &agp_bridge.mode);

                break;
            }

            {
                u32 capval = 0;
                const char *name = "855PM/852PM";

                pci_read_config_dword(dev, I85X_CAPID, &capval);
                switch ((capval >> I85X_VARIANT_SHIFT) & I85X_VARIANT_MASK)
                {
                case I855_PME:
                    name = "855PME";
                    break;
                case I855_PM:
                    name = "855PM";
                    break;
                case I852_PME:
                    name = "852PME";
                    break;
                case I852_PM:
                    name = "852PM";
                    break;
                }

                printk(KERN_INFO PFX "Detected an Intel %s Chipset.\n", name);
            }

            agp_bridge.type = INTEL_I810;
            return intel_i830_setup(sub_dev);

        case PCI_DEVICE_ID_INTEL_82865_HB:
            sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                        PCI_DEVICE_ID_INTEL_82865_IG,
                        NULL);
            if (sub_dev && PCI_FUNC(sub_dev->devfn) != 0) {
                sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                            PCI_DEVICE_ID_INTEL_82865_IG,
                            sub_dev);
            }
                
            if (sub_dev == NULL) {
                /*
                 * We probably have a 865G chipset with an external graphics
                 * card. It will be initialized later.
                 */
                printk(KERN_INFO PFX "Detected an Intel 865G Chipset, no integrated grapics found.\n");
                agp_bridge.type = INTEL_I865_G;

            	/* fill in the mode register */
            	pci_read_config_dword(agp_bridge.dev,
			        agp_bridge.capndx + 4, // AGP status register
			        &agp_bridge.mode);

                break;
            }

            printk(KERN_INFO PFX "Detected an Intel(R) 865G Chipset.\n");
            agp_bridge.type = INTEL_I810;
            return intel_i830_setup(sub_dev);

        case PCI_DEVICE_ID_INTEL_875_G_0:
            sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                PCI_DEVICE_ID_INTEL_875_G_1, NULL);
            if(sub_dev && PCI_FUNC(sub_dev->devfn) != 0) {
                sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL,
                    PCI_DEVICE_ID_INTEL_875_G_1, sub_dev);
            }

	    {
                printk(KERN_INFO PFX "Detected an Intel PCI to CSA bridge\n");
                agp_bridge.type = INTEL_I875_G;

            	/* fill in the mode register */
            	pci_read_config_dword(agp_bridge.dev,
			        agp_bridge.capndx + 4, // AGP status register
			        &agp_bridge.mode);

                break;
            }

            printk(KERN_INFO PFX "Detected an Intel 875G Chipset with integrated grapics.\n");
            agp_bridge.type = INTEL_I810;
            return intel_i830_setup(sub_dev);

            /*  
             * For the 7205 & 7505 chipset look for the AGP 3.0 
             * bridge on device 1.  If for some reason we don't  
             * find anything then fallback to device 0 used for 
             * backward compatibility. 
             */ 
        case PCI_DEVICE_ID_INTEL_7205_0:
        case PCI_DEVICE_ID_INTEL_7505_0: 
            sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
                           PCI_DEVICE_ID_INTEL_7505_1, 
                           NULL); 
            if(sub_dev && PCI_FUNC(sub_dev->devfn) != 0) { 
                sub_dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
                              PCI_DEVICE_ID_INTEL_7505_1, 
                              sub_dev); 
            } 

            if(dev->device == PCI_DEVICE_ID_INTEL_7205_0)
                printk(KERN_INFO PFX "Detected an Intel E7205 Chipset.\n");
            else
                printk(KERN_INFO PFX "Detected an Intel E7505 Chipset.\n");

            if(sub_dev == NULL) { 

                printk(KERN_INFO PFX "Could not find the AGP 3.0 bridge on sub device #1!\n"); 
                break; /* compatibility mode? fall trough and resolve via the list */
            } 
 
            agp_bridge.type = INTEL_I7505; 

            /* it might happen that AGP_CAPS and bus bridge device data are split
                across two separate devices on some early chipset versions */
            agp_bridge_pci2pci.dev = sub_dev;

#if 1
            /* better fall trough to classical chipset programming for now */
            break;
#else
            if(1)
            {
                u8 cap_ptr;

                /* check for AGP 3.5 device(?) - this variant needs some more research */
                pci_read_config_byte(sub_dev, PCI_CAPABILITY_LIST, &cap_ptr); 
                if(cap_ptr != 0x00) 
                {
                    printk(KERN_INFO PFX "Bridge is in native mode.\n"); 
                    /* Update the pci device with the AGP bridge info */ 
                    agp_bridge.dev = sub_dev; 
                    agp_bridge.capndx = cap_ptr; 
 
                	/* fill in the mode register */
            	    pci_read_config_dword(agp_bridge.dev,
			            agp_bridge.capndx + 4, // AGP status register
    			        &agp_bridge.mode);

                    return intel_7505_setup(agp_bridge.dev); 
                }
                else
                {
                   	printk(KERN_INFO PFX "Bridge is in compatibility mode.\n"); 
                	/* in compatibility mode the bridge is similar to i860 */
                    /* therefore we dont do anything here but resolve it later */
                    /* when browsing chipsets via the list */
    //                return -ENODEV; 
                    break;
                }
            }
#endif

		default:
			break;
		}
	}

#ifdef FIREGL_AGP_SVRWKS
	/* need to test for ServerWorks CNB20HE here - this is a special exit path */
	if (dev->vendor == PCI_VENDOR_ID_SERVERWORKS )
    {
		struct pci_dev *svrwks_agp_dev;

		switch (dev->device)
        {
		case PCI_DEVICE_ID_SERVERWORKS_CNB20LE_MCH_0:
			svrwks_agp_dev = pci_find_device(PCI_VENDOR_ID_SERVERWORKS,
					       PCI_DEVICE_ID_SERVERWORKS_CNB20LE_AGP_1,
						   NULL);
			if (svrwks_agp_dev == NULL) {
				printk(KERN_ERR PFX "Detected a ServerWorks CNB20LE,"
				       " but could not find related AGP"
				       " device.\n");
				return -ENODEV;
			}
			printk(KERN_INFO PFX "Detected a ServerWorks "
			       "CNB20LE chipset in AGP mode.\n");
			agp_bridge.type = SERVERWORKS_CNB20LE;

			/* determine AGP register set offset */
			agp_bridge.capndx = 0x80;
            /* use a fixed value for this chipset config - skip browsing linked list */

			/* fill in the mode register */
			pci_read_config_dword(svrwks_agp_dev,
			      agp_bridge.capndx + 4, // AGP status register
			      &agp_bridge.mode);

			return fgl_svrwks_cnb20le_setup (svrwks_agp_dev);

		case PCI_DEVICE_ID_SERVERWORKS_CNB20HE_MCH_0:
			svrwks_agp_dev = pci_find_device(PCI_VENDOR_ID_SERVERWORKS,
					       PCI_DEVICE_ID_SERVERWORKS_CNB20HE_AGP_1,
						   NULL);
			if (svrwks_agp_dev == NULL) {
				printk(KERN_ERR PFX "Detected a ServerWorks CNB20HE,"
				       " but could not find related AGP"
				       " device.\n");
				return -ENODEV;
			}
			printk(KERN_INFO PFX "Detected a ServerWorks "
			       "CNB20HE chipset in AGP mode.\n");
			agp_bridge.type = SERVERWORKS_CNB20HE;

			/* determine AGP register set offset */
			agp_bridge.capndx = 0x80;
            /* use a fixed value for this chipset config - skip browsing linked list */

			/* fill in the mode register */
			pci_read_config_dword(svrwks_agp_dev,
			      agp_bridge.capndx + 4, // AGP status register
			      &agp_bridge.mode);

			return fgl_svrwks_cnb20he_setup (svrwks_agp_dev);

		default:
			break;
		}
    }
#endif // FIREGL_AGP_SVRWKS

#ifndef FIREGL_AGP_SVRWKS
	/* Everything is on func 1 here so we are hardcoding function one */
	if (dev->vendor == PCI_VENDOR_ID_SERVERWORKS) {
		struct pci_dev *bridge_dev;

		bridge_dev = pci_find_slot ((unsigned int)dev->bus->number, 
					    PCI_DEVFN(0, 1));
		if(bridge_dev == NULL) {
			printk(KERN_INFO PFX "agpgart: Detected a Serverworks "
			       "Chipset, but could not find the secondary "
			       "device.\n");
			return -ENODEV;
		}

		switch (dev->device) {
		case PCI_DEVICE_ID_SERVERWORKS_HE:
			agp_bridge.type = SVWRKS_HE;
			return serverworks_setup(bridge_dev);

		case PCI_DEVICE_ID_SERVERWORKS_LE:
			agp_bridge.type = SVWRKS_LE;
			return serverworks_setup(bridge_dev);

		default:
			if(agp_try_unsupported) {
				agp_bridge.type = _X(SVWRKS_GENERIC);
				return serverworks_setup(bridge_dev);
			}
			break;
		}
	}
#endif /* !FIREGL_AGP_SVRWKS */
#ifdef __ia64__
	if (dev->vendor == PCI_VENDOR_ID_HP) {
		do {
			/* ZX1 LBAs can be either PCI or AGP bridges */
            agp_bridge.capndx = pci_find_capability(dev, PCI_CAP_ID_AGP);
			if ( agp_bridge.capndx )
            {
				printk(KERN_INFO PFX "Detected HP ZX1 AGP "
				       "chipset at %s\n", pci_name(dev));
				agp_bridge.type = HP_ZX1;
				agp_bridge.dev = dev;
              	/* fill in the mode register */
                pci_read_config_dword(agp_bridge.dev,
                    agp_bridge.capndx + 4, // AGP status register
                    &agp_bridge.mode);
				return hp_zx1_setup(dev);
			}
			dev = firegl_pci_find_class(PCI_CLASS_BRIDGE_HOST << 8, dev);
		} while (dev);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10) 
                if(dev) pci_dev_put(dev);
#endif
		return -ENODEV;
	}
#endif	/* __ia64__ */

#if 1
    // special case handling for Via chipsets in AGPv3 mode
    if (dev->vendor == PCI_VENDOR_ID_VIA)
    {
		switch (dev->device) {
        case PCI_DEVICE_ID_VIA_8367_0:
            { /* there seem to be some VIA KT400 designs around that expose a VIA KT266 ID. */
                u8 reg;

                /* if bit 1 of AGPSEL (agp v2 subsystem bit) is set then anything is simple */
                pci_read_config_byte(dev, VIA_AGPSEL, &reg);
                if( reg & (1<<1) )
                {
                    /* chip looks like the real VIA KT266 or it is a compatible one, */
                    /* no reason to report any extra messages, this is the normal case. */
                    break; /* pass on to VIA generic setup */
                }

                /* the agp v2 subsystem bit was cleared, we have to use the VIA KT400 specific setup */
                printk(KERN_INFO PFX "Detected a VIA KT266 missing the AGPv2 registers, chip might be a VIA KT400 or compatible. (VIA flag)\n");

                /* note: the agp v3 subsystem bit is most likely set now,
                   that is supposed to be VIA_AGPSEL bit 2, as seen in running systems */
            }
            /* this is an intentional falltrough! */
        case PCI_DEVICE_ID_VIA_8377_0: /* VIA KT400 */
            {
                u8 cap_ptr;
                u8 reg;

                /* VIA KT400 might run in VIA generic mode */
                /* if bit 1 of AGPSEL is set then we have a generic VIA setup */
                pci_read_config_byte(dev, VIA_AGPSEL, &reg);
                if( reg & (1<<1) )
                {
                    printk(KERN_INFO PFX "Detected a Via Apollo KT400 chipset in AGP v2 mode. (VIA flag)\n");
            	    return via_generic_setup(dev); /* the bit was set - pass on to VIA generic setup */
                }
                /* note: the agp v3 subsystem bit is most likely set now,
                   that is supposed to be VIA_AGPSEL bit 2, as seen in running systems */

                // agp v3
                cap_ptr = get_pcidev_capptr(dev);
                cap_ptr = find_agpv3_capptr(dev, cap_ptr);
                if( cap_ptr )
                {
    				printk(KERN_INFO PFX "Detected a Via Apollo KT400 chipset in AGP v3 mode "
	    			       "at %s\n", pci_name(dev));

                    agp_bridge.type = VIA_APOLLO_KT400;
    				agp_bridge.dev = dev;

                    agp_bridge.capndx = cap_ptr;
                	/* fill in the mode register */
                	pci_read_config_dword(agp_bridge.dev,
			            agp_bridge.capndx + 4, // AGP status register
			            &agp_bridge.mode);

            	    return via_kt400_setup(dev);
                }

                // agp v2
                cap_ptr = get_pcidev_capptr(dev);
                cap_ptr = find_agpv2_capptr(dev, cap_ptr);
                if( cap_ptr )
                {
    				printk(KERN_DEBUG PFX "Detected a Via Apollo KT400 chipset in AGP v2 compat mode "
	    			       "at %s\n", pci_name(dev));
                }
                else
                {
    				printk(KERN_ERR PFX "Detected a Via Apollo KT400 chipset but no AGPv2/v3 caps found "
	    			       "at %s\n", pci_name(dev));
                }
            }
            break;
        case PCI_DEVICE_ID_VIA_8754: /* VIA P4P400/P4X333 */
            {
                u8 cap_ptr;
                u8 reg;

                /* this VIA P4P400/P4X333 might run in VIA generic mode */
                /* if bit 1 of AGPSEL is set then we have a generic VIA setup */
                pci_read_config_byte(dev, VIA_AGPSEL, &reg);
                if( reg & (1<<1) )
                {
                    printk(KERN_INFO PFX "Detected a Via P4X333 chipset in AGP v2 mode. (VIA flag)\n");
            	    return via_generic_setup(dev); /* the bit was set - pass on to VIA generic setup */
                }
                /* note: the agp v3 subsystem bit is most likely set now,
                   that is supposed to be VIA_AGPSEL bit 2, as seen in running systems */

                // agp v3 caps, chipset in AGP v3.x mode
                cap_ptr = get_pcidev_capptr(dev);
                cap_ptr = find_agpv3_capptr(dev, cap_ptr);
                if( cap_ptr )
                {
                    printk(KERN_INFO PFX "Detected a Via P4X400 chipset in AGP v3 mode "
                           "at %s\n", pci_name(dev));

                    agp_bridge.type = VIA_APOLLO_P4X400;
                    agp_bridge.dev = dev;

                    agp_bridge.capndx = cap_ptr;
                    /* fill in the mode register */
                    pci_read_config_dword(agp_bridge.dev,
                        agp_bridge.capndx + 4, // AGP status register 
                        &agp_bridge.mode);

                    return via_kt400_setup(dev); /* p4x400 has identical setup like kt400 */
                }
                
                // agp v3 caps, chipset in AGP v2 compatibility mode
                cap_ptr = get_pcidev_capptr(dev);
                cap_ptr = find_agpv3_agpv2compat_capptr(dev, cap_ptr);
                if( cap_ptr )
                {
                    printk(KERN_INFO PFX "Detected a Via P4X400 chipset with AGP v3.x caps in AGP v2 compatibility mode "
                           "at %s\n", pci_name(dev));

                    agp_bridge.type = VIA_APOLLO_P4X400;
                    agp_bridge.dev = dev;

                    agp_bridge.capndx = cap_ptr;
                    /* fill in the mode register */
                    pci_read_config_dword(agp_bridge.dev,
                        agp_bridge.capndx + 4, // AGP status register 
                        &agp_bridge.mode);

                    return via_kt400_setup(dev); /* p4x400 has identical setup like kt400 */
                }
                
                // agp v2 
                cap_ptr = get_pcidev_capptr(dev);
                cap_ptr = find_agpv2_capptr(dev, cap_ptr);
                if( cap_ptr )
                {
                    printk(KERN_DEBUG PFX "Detected a Via P4X400 chipset with AGP v2 caps in true AGP v2 mode "
                           "at %s\n", pci_name(dev));
                }
                else
                {
                    printk(KERN_ERR PFX "Detected a Via P4X400 chipset but no AGPv2/v3 caps found "
                           "at %s\n", pci_name(dev));
                }
            }
            break;
        default:
            break;
        }
    }
#endif

#if 1
    // special case handling for nVidia chipsets
    if (dev->vendor == PCI_VENDOR_ID_NVIDIA)
    {
        int use_nforce12_init = 0;

		switch (dev->device) {
        case PCI_DEVICE_ID_NVIDIA_NFORCE:
            printk(KERN_ERR PFX "Detected nVidia nForce1 chipset\n");
            use_nforce12_init = 1;
            break;
        case PCI_DEVICE_ID_NVIDIA_NFORCE2:
            printk(KERN_ERR PFX "Detected nVidia nForce2 chipset\n");
            use_nforce12_init = 1;
            break;
        default:
            if( agp_try_unsupported )
            {
                printk(KERN_ERR PFX "Found unknown nVidia chipset, doing generic nForce1/2 init\n");
                use_nforce12_init = 1;
            }
            break;
        }

        if( use_nforce12_init )
        {
            struct pci_dev *dev1;
            struct pci_dev *dev2;
            struct pci_dev *dev3;
            u8 cap_ptr;

            dev1 = pci_find_slot((unsigned int)dev->bus->number, PCI_DEVFN(0,1));
            if( !dev1 )
            {
                printk(KERN_ERR PFX "PCI device x:0:1 not found\n");
         	    return -ENODEV;
            }

            dev2 = pci_find_slot((unsigned int)dev->bus->number, PCI_DEVFN(0,2));
            if( !dev2 )
            {
                printk(KERN_ERR PFX "PCI device x:0:2 not found\n");
                return -ENODEV;
            }

            dev3 = pci_find_slot((unsigned int)dev->bus->number, PCI_DEVFN(30,0));
            if( !dev3 )
            {
                printk(KERN_ERR PFX "PCI device x:30:0 not found\n");
                return -ENODEV;
            }

            // agp v3
            cap_ptr = get_pcidev_capptr(dev);
            cap_ptr = find_agpv3_capptr(dev, cap_ptr);
            if( cap_ptr )
            {
                printk(KERN_ERR PFX "Bridge device is in AGP v3 mode\n");
            }
            else
            {
                // agp v2
                cap_ptr = get_pcidev_capptr(dev);
                cap_ptr = find_agpv2_capptr(dev, cap_ptr);
                if( cap_ptr )
                {
                    printk(KERN_ERR PFX "Bridge device is in AGP v1/v2 mode\n");
                }
                else
                {
                    printk(KERN_ERR PFX "AGP capabilites on bridge device not found\n");
                    return -ENODEV;
                }
            }

            /* Update the pci device with the AGP bridge info */ 
            agp_bridge.dev = dev; 
            agp_bridge.capndx = cap_ptr;

            return nvidia_nforce_setup(agp_bridge.dev);
        }

        // unknown or non special nvidia chipset - go on with list based resolve
    }
#endif

//#ifdef CONFIG_AGP_ATI
    if (dev->vendor == PCI_VENDOR_ID_ATI)
    {
        /* find capndx */
        cap_ptr = pci_find_capability(dev, PCI_CAP_ID_AGP);
    
        printk(KERN_INFO PFX "cap_ptr=0x%08x\n", cap_ptr);
    
        if (cap_ptr == 0x00)
                return -ENODEV;
        agp_bridge.capndx = cap_ptr;
    
        /* Fill in the mode register */
        pci_read_config_dword(agp_bridge.dev,
                              agp_bridge.capndx + 4,
                              &agp_bridge.mode);
    
        /* probe for known chipsets */
        return agp_lookup_host_bridge (dev);
    }
//#endif /* CONFIG_AGP_ATI */
    
    if(1) // some generic pre processing
    {
        u8 cap_ptr;

#if 1
        cap_ptr = pci_find_capability(dev, PCI_CAP_ID_AGP);
#else
    	/* find capndx by browsing a linked list */
	    pci_read_config_dword(dev, 0x04, &scratch); // command and status register
    	if (!(scratch & 0x00100000))    // is the capabilitys-pointer-valid flag set?
    		return -ENODEV;

    	pci_read_config_byte(dev, 0x34, &cap_ptr); // capability pointer itself
    	if (cap_ptr != 0x00) {
	    	do {
		    	pci_read_config_dword(dev, cap_ptr, &cap_id); // agp caps

			    if ((cap_id & 0xff) != 0x02) // not an agp list entry?
				    cap_ptr = (cap_id >> 8) & 0xff; // pointer to next entry in linked list
    		}
	    	while (((cap_id & 0xff) != 0x02) && (cap_ptr != 0x00));
    	}
#endif
	    if (cap_ptr == 0x00) // agp entry not found in list?
		    return -ENODEV;

        // assign the agp capabilities offset to the agp bridge device structure
	    agp_bridge.capndx = cap_ptr;

    	/* fill in the agp caps mode register value for later use (in the calling code) */
	    pci_read_config_dword(agp_bridge.dev,
		    agp_bridge.capndx + 4, // AGP status register
			&agp_bridge.mode);
    }

	/* resolve handler for chipset - this is the default path */
	return agp_lookup_host_bridge (dev);
}

struct agp_max_table {
	int mem;
	int agp;
};

static struct agp_max_table maxes_table[9] __initdata =
{
	{0, 0},
	{32, 4},
	{64, 28},
	{128, 96},
	{256, 204},
	{512, 440},
	{1024, 942},
	{2048, 1920},
	{4096, 3932}
};

static int __init agp_find_max (void)
{
	long memory, index, result;

	memory = virt_to_phys(high_memory) >> 20;
	index = 1;

	while ((memory > maxes_table[index].mem) &&
	       (index < 8)) {
		index++;
	}

	result = maxes_table[index - 1].agp +
	   ( (memory - maxes_table[index - 1].mem)  *
	     (maxes_table[index].agp - maxes_table[index - 1].agp)) /
	   (maxes_table[index].mem - maxes_table[index - 1].mem);

	printk(KERN_INFO PFX "Maximum main memory to use "
	       "for agp memory: %ldM\n", result);
	result = result << (20 - PAGE_SHIFT);
        return result;
}

#define AGPGART_VERSION_MAJOR 0
#define AGPGART_VERSION_MINOR 99

static agp_version agp_current_version =
{
	AGPGART_VERSION_MAJOR,
	AGPGART_VERSION_MINOR
};

static int __init agp_backend_initialize(void)
{
	int size_value, rc, got_gatt=0, got_keylist=0;

	memset(&agp_bridge, 0, sizeof(struct agp_bridge_data));
	agp_bridge.type = NOT_SUPPORTED;
	agp_bridge.max_memory_agp = agp_find_max();
	agp_bridge.version = &agp_current_version;

	rc = agp_find_supported_device();
	if (rc) {
		/* not KERN_ERR because error msg should have already printed */
		printk(KERN_DEBUG PFX "no supported devices found.\n");
		return rc;
	}

	if (agp_bridge.needs_scratch_page == TRUE) {
		agp_bridge.scratch_page = agp_bridge.agp_alloc_page();

		if (agp_bridge.scratch_page == 0) {
			printk(KERN_ERR PFX "unable to get memory for "
			       "scratch page.\n");
			return -ENOMEM;
		}
		agp_bridge.scratch_page =
		    virt_to_phys((void *) agp_bridge.scratch_page);
		agp_bridge.scratch_page =
		    agp_bridge.mask_memory(agp_bridge.scratch_page, 0);
	}

	size_value = agp_bridge.fetch_size();

	if (size_value == 0) {
		printk(KERN_ERR PFX "unable to determine aperture size.\n");
		rc = -EINVAL;
		goto err_out;
	}
	if (agp_bridge.create_gatt_table()) {
		printk(KERN_ERR PFX "unable to get memory for graphics "
		       "translation table.\n");
		rc = -ENOMEM;
		goto err_out;
	}
	got_gatt = 1;
	
	agp_bridge.key_list = vmalloc(PAGE_SIZE * 4);
	if (agp_bridge.key_list == NULL) {
		printk(KERN_ERR PFX "error allocating memory for key lists.\n");
		rc = -ENOMEM;
		goto err_out;
	}
	got_keylist = 1;
	
	/* FIXME vmalloc'd memory not guaranteed contiguous */
	memset(agp_bridge.key_list, 0, PAGE_SIZE * 4);

	if (agp_bridge.configure()) {
		printk(KERN_ERR PFX "error configuring host chipset.\n");
		rc = -EINVAL;
		goto err_out;
	}

	printk(KERN_INFO PFX "AGP aperture is %dM @ 0x%lx\n",
	       size_value, agp_bridge.gart_bus_addr);

	return 0;

err_out:
	if (agp_bridge.needs_scratch_page == TRUE) {
		agp_bridge.scratch_page &= ~(0x00000fff);
		agp_bridge.agp_destroy_page((unsigned long)
				 phys_to_virt(agp_bridge.scratch_page));
	}
	if (got_gatt)
		agp_bridge.free_gatt_table();
	if (got_keylist)
		vfree(agp_bridge.key_list);
	return rc;
}


/* cannot be __exit b/c as it could be called from __init code */
static void agp_backend_cleanup(void)
{
	agp_bridge.cleanup();
	agp_bridge.free_gatt_table();
	vfree(agp_bridge.key_list);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10)
        // decrease the reference count.
        pci_dev_put(agp_bridge.dev);
#endif

	if (agp_bridge.needs_scratch_page == TRUE) {
		agp_bridge.scratch_page &= ~(0x00000fff);
		agp_bridge.agp_destroy_page((unsigned long)
				 phys_to_virt(agp_bridge.scratch_page));
	}
}

#ifdef FGL_PM_PRESENT
static int agp_power(struct pm_dev *dev, pm_request_t rq, void *data)
{
    switch(rq)                                                                
    {
        case PM_SUSPEND:
            return agp_bridge.suspend();
        case PM_RESUME:
            agp_bridge.resume();
            return 0;
    }
    return 0;
}
#endif /* FGL_PM_PRESENT */

drm_agp_t drm_agp = {
	&agp_free_memory,
	&agp_allocate_memory,
	&agp_bind_memory,
	&agp_unbind_memory,
	&agp_enable,
	&agp_backend_acquire,
	&agp_backend_release,
	&agp_copy_info,
};

#ifdef STANDALONE_AGPGART

extern int agp_frontend_initialize(void);
extern void agp_frontend_cleanup(void);

static int __init agp_init(void)
{
	int ret_val;

	printk(KERN_INFO "Linux agpgart interface v%d.%d (c) Jeff Hartmann\n",
	       AGPGART_VERSION_MAJOR, AGPGART_VERSION_MINOR);
	ret_val = agp_backend_initialize();
	if (ret_val) {
		agp_bridge.type = NOT_SUPPORTED;
		return ret_val;
	}
	ret_val = agp_frontend_initialize();
	if (ret_val) {
		agp_bridge.type = NOT_SUPPORTED;
		agp_backend_cleanup();
		return ret_val;
	}

#ifndef AGPGART_2_2
	inter_module_register("drm_agp", THIS_MODULE, &drm_agp);
#endif /* !AGPGART_2_2 */

#ifdef FGL_PM_PRESENT
    pm_register(PM_PCI_DEV, PM_PCI_ID(agp_bridge.dev), agp_power);
#endif
	return 0;
}

static void __exit agp_cleanup(void)
{
#ifdef FGL_PM_PRESENT
    pm_unregister_all(agp_power);
#endif
	agp_frontend_cleanup();
	agp_backend_cleanup();
#ifndef AGPGART_2_2
	inter_module_unregister("drm_agp");
#endif /* !AGPGART_2_2 */
}

module_init(agp_init);
module_exit(agp_cleanup);

#else /* !STANDALONE_AGPGART */

int _X(agp_init)(void)
{
	int ret_val;

	printk(KERN_INFO "Fire GL built-in AGP-support\n");
	printk(KERN_INFO "Based on agpgart interface v%d.%d (c) Jeff Hartmann\n",
	       AGPGART_VERSION_MAJOR, AGPGART_VERSION_MINOR);

	ret_val = agp_backend_initialize();
	if (ret_val) {
		agp_bridge.type = NOT_SUPPORTED;
		return ret_val;
	}

#ifdef FGL_PM_PRESENT
    pm_register(PM_PCI_DEV, PM_PCI_ID(agp_bridge.dev), agp_power);
	printk(KERN_INFO "Power management callback for AGP chipset installed\n");
#endif

	return 0;
}

void _X(agp_cleanup)(void)
{
#ifdef FGL_PM_PRESENT
    pm_unregister_all(agp_power);
#endif

	agp_backend_cleanup();
}

#endif /* !STANDALONE_AGPGART */
