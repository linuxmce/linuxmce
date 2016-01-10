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
 * AGPGART backend specific includes. Not for userspace consumption.
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
 * - http://innominate.org/cgi-bin/lksr/linux/include/linux/agp_backend.h
 *   version 1.1.5, 2001-10-09, CVS-Tag: LINUX_2_4_13
 */

#ifndef _FGL_AGP_BACKEND_H
#define _FGL_AGP_BACKEND_H

#ifdef FGL
/* AGP module will be staticly linked to FireGL kernel module */
#define _X(_x) __fgl_##_x
extern int _X(agp_init)(void);
extern void _X(agp_cleanup)(void);
extern int _X(agp_try_unsupported);
#else /* Original AGPGART module */
#define STANDALONE_AGPGART
#define _X(_x) _x
#endif /* FGL */

#ifndef __GFP_COMP
#define __GFP_COMP 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef STANDALONE_AGPGART
#define AGPGART_VERSION_MAJOR 0
#define AGPGART_VERSION_MINOR 99
#endif /* STANDALONE_AGPGART */

enum _X(chipset_type) {
	_X(NOT_SUPPORTED),
	_X(INTEL_GENERIC),
	_X(INTEL_LX),
	_X(INTEL_BX),
	_X(INTEL_GX),
	_X(INTEL_I810),
    _X(INTEL_I815),
    _X(INTEL_I820),
    _X(INTEL_I830_M),
	_X(INTEL_I865_G),
	_X(INTEL_I875_G),
	_X(INTEL_I840),
	_X(INTEL_I845),
	_X(INTEL_I845_G),
	_X(INTEL_I850),
    _X(INTEL_I855_PM),
	_X(INTEL_I860),
	_X(INTEL_I7205),
	_X(INTEL_I7505),
	_X(VIA_GENERIC),
	_X(VIA_VP3),
	_X(VIA_MVP3),
	_X(VIA_MVP4),
	_X(VIA_APOLLO_PLE133),
	_X(VIA_APOLLO_PRO),
	_X(VIA_APOLLO_KX133),
	_X(VIA_APOLLO_KT133),
	_X(VIA_APOLLO_KM266),
	_X(VIA_APOLLO_KT266),
	_X(VIA_APOLLO_KT400),
    _X(VIA_APOLLO_P4M266),
    _X(VIA_APOLLO_P4X400),
	_X(SIS_GENERIC),
	_X(AMD_GENERIC),
	_X(AMD_IRONGATE),
    _X(AMD_761),
    _X(AMD_762),
/*	_X(AMD_IGD4), */
/*	_X(AMD_IGD4_2P), */
	_X(AMD_8151),
	_X(ALI_M1541),
	_X(ALI_M1621),
	_X(ALI_M1631),
	_X(ALI_M1632),
	_X(ALI_M1641),
	_X(ALI_M1647),
	_X(ALI_M1651),
	_X(ALI_GENERIC),   
	_X(SVWRKS_HE),
	_X(SVWRKS_LE),
	_X(SVWRKS_GENERIC),
	_X(HP_ZX1),
#ifdef FIREGL_AGP_HOOK
#ifdef FIREGL_AGP_SVRWKS
	_X(SERVERWORKS_CNB20LE),
	_X(SERVERWORKS_CNB20HE),
//	_X(SERVERWORKS_GENERIC),
#endif
#endif
    _X(NVIDIA_NFORCE),
    _X(NVIDIA_NFORCE2),
    _X(NVIDIA_NFORCE3),
    _X(NVIDIA_GENERIC),
    _X(ATI_RS100),
    _X(ATI_RS200),
    _X(ATI_RS250),
    _X(ATI_RS300_100),
    _X(ATI_RS300_133),
    _X(ATI_RS300_166),
    _X(ATI_RS300_200),
    _X(ATI_RS350_100),
    _X(ATI_RS350_133),
    _X(ATI_RS350_166),
    _X(ATI_RS350_200),
};

typedef struct _X(_agp_version) {
	u16 major;
	u16 minor;
} _X(agp_version);

typedef struct _X(_agp_kern_info) {
	_X(agp_version) version;
	struct pci_dev *device;
	enum _X(chipset_type) chipset;
	unsigned long mode;
	off_t aper_base;
	size_t aper_size;
	int max_memory;		/* In pages */
	int current_memory;
    int cant_use_aperture;
    unsigned long page_mask;
} _X(agp_kern_info);

/* 
 * The agp_memory structure has information
 * about the block of agp memory allocated.
 * A caller may manipulate the next and prev
 * pointers to link each allocated item into
 * a list.  These pointers are ignored by the 
 * backend.  Everything else should never be
 * written to, but the caller may read any of
 * the items to detrimine the status of this
 * block of agp memory.
 * 
 */

typedef struct _X(_agp_memory) {
	int key;
	struct _X(_agp_memory) *next;
	struct _X(_agp_memory) *prev;
	size_t page_count;
	int num_scratch_pages;
	unsigned long *memory;
#ifdef __ia64__
	// I tink this ifdef can go away, Johannes
	void *vmptr;
#endif
	off_t pg_start;
	u32 type;
#ifdef FGL
    u32 alloc_type;
#endif
	u32 physical;
	u8 is_bound;
	u8 is_flushed;
} _X(agp_memory);

#ifdef STANDALONE_AGPGART
#define AGP_NORMAL_MEMORY 0
#endif /* STANDALONE_AGPGART */

#ifndef FIREGL_AGP_HOOK_DEFINES_ONLY

#ifdef FGL
extern _X(agp_memory) *_X(agp_allocate_memory_phys_list)(size_t, u32, u64 *);

/*
 * agp_allocate_memory_phys_list :
 * 
 * This function assigns a group of physcial adresses of
 * to a agp_memory structure.
 * 
 * It takes a size_t argument of the number of pages, and
 * an u32 argument of the type of memory to be allocated.  
 * Every agp bridge device will allow you to allocate 
 * AGP_NORMAL_MEMORY which maps to physical ram.  Any other
 * type is device dependant.
 * 
 * It returns NULL whenever memory is unavailable.
 * 
 */
#endif

extern void _X(agp_free_memory)(_X(agp_memory) *);

/*
 * agp_free_memory :
 * 
 * This function frees memory associated with
 * an agp_memory pointer.  It is the only function
 * that can be called when the backend is not owned
 * by the caller.  (So it can free memory on client
 * death.)
 * 
 * It takes an agp_memory pointer as an argument.
 * 
 */

extern _X(agp_memory) *_X(agp_allocate_memory)(size_t, u32);

/*
 * agp_allocate_memory :
 * 
 * This function allocates a group of pages of
 * a certain type.
 * 
 * It takes a size_t argument of the number of pages, and
 * an u32 argument of the type of memory to be allocated.  
 * Every agp bridge device will allow you to allocate 
 * AGP_NORMAL_MEMORY which maps to physical ram.  Any other
 * type is device dependant.
 * 
 * It returns NULL whenever memory is unavailable.
 * 
 */

extern void _X(agp_copy_info)(_X(agp_kern_info) *);

/*
 * agp_copy_info :
 * 
 * This function copies information about the
 * agp bridge device and the state of the agp
 * backend into an agp_kern_info pointer.
 * 
 * It takes an agp_kern_info pointer as an
 * argument.  The caller should insure that
 * this pointer is valid.
 * 
 */

extern int _X(agp_bind_memory)(_X(agp_memory) *, off_t);

/*
 * agp_bind_memory :
 * 
 * This function binds an agp_memory structure
 * into the graphics aperture translation table.
 * 
 * It takes an agp_memory pointer and an offset into
 * the graphics aperture translation table as arguments
 * 
 * It returns -EINVAL if the pointer == NULL.
 * It returns -EBUSY if the area of the table
 * requested is already in use.
 * 
 */

extern int _X(agp_unbind_memory)(_X(agp_memory) *);

/* 
 * agp_unbind_memory :
 * 
 * This function removes an agp_memory structure
 * from the graphics aperture translation table.
 * 
 * It takes an agp_memory pointer as an argument.
 * 
 * It returns -EINVAL if this piece of agp_memory
 * is not currently bound to the graphics aperture
 * translation table or if the agp_memory 
 * pointer == NULL
 * 
 */

extern void _X(agp_enable)(u32);

/* 
 * agp_enable :
 * 
 * This function initializes the agp point-to-point
 * connection.
 * 
 * It takes an agp mode register as an argument
 * 
 */

extern int _X(agp_backend_acquire)(void);

/*
 * agp_backend_acquire :
 * 
 * This Function attempts to acquire the agp
 * backend.
 * 
 * returns -EBUSY if agp is in use,
 * returns 0 if the caller owns the agp backend
 */

extern void _X(agp_backend_release)(void);

/*
 * agp_backend_release :
 * 
 * This Function releases the lock on the agp
 * backend.
 * 
 * The caller must insure that the graphics
 * aperture translation table is read for use
 * by another entity.  (Ensure that all memory
 * it bound is unbound.)
 * 
 */

#endif // !FIREGL_AGP_HOOK_DEFINES_ONLY

#ifndef agp_memory
#define agp_memory _X(agp_memory)
#endif
#ifndef agp_kern_info
#define agp_kern_info _X(agp_kern_info)
#endif
typedef struct {
	void       (*free_memory)(agp_memory *);
	agp_memory *(*allocate_memory)(size_t, u32);
	int        (*bind_memory)(agp_memory *, off_t);
	int        (*unbind_memory)(agp_memory *);
	void       (*enable)(u32);
	int        (*acquire)(void);
	void       (*release)(void);
	void       (*copy_info)(agp_kern_info *);
} drm_agp_t;


/*
 * Interface between drm and agp code.  When agp initializes, it makes
 * the above structure available via inter_module_register(), drm might
 * use it.  Keith Owens <kaos@ocs.com.au> 28 Oct 2000.
 */


#endif /* _FGL_AGP_BACKEND_H */
