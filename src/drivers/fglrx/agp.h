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
 * AGP/AGPGART specific includes.
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
 * - http://innominate.org/cgi-bin/lksr/linux/drivers/char/agp/agp.h
 *   version 1.1.1.6, 2001-10-09, CVS-Tag: LINUX_2_4_13
 */

#ifndef _FGL_AGP_H
#define _FGL_AGP_H

// build config defines
#define FGL_USE_AGPV3_OFFICIAL_ENABLE       /* use offical AGP V3 code */

// helper defines - for any public source of firegl code tree.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,71)
#define pci_for_each_dev(dev) \
    (dev) = NULL; \
    while ((dev = pci_find_device(PCI_ANY_ID, PCI_ANY_ID, dev)) != NULL)
#endif

// min/max defines taken from include/linux/kernel.h of linux-2.4.10-pre9

// in Linux kernel 2.4.9 there was a pretty incompatible version
// of the min/max macros, but this situation didnt last for long.
#undef min
#undef max

#if 0
/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x,y) ({ \
    const typeof(x) _x = (x);       \
    const typeof(y) _y = (y);       \
    (void) (&_x == &_y);            \
    _x < _y ? _x : _y; })

#define max(x,y) ({ \
    const typeof(x) _x = (x);       \
    const typeof(y) _y = (y);       \
    (void) (&_x == &_y);            \
    _x > _y ? _x : _y; })
#else
// FireGL variant - strip off the const parameter - some compilers are verbose!
// (Should the min/max macros qualify as an lvalue operation?)
#define min(x,y) ({ \
    typeof(x) _x = (x);             \
    typeof(y) _y = (y);             \
    (void) (&_x == &_y);            \
    _x < _y ? _x : _y; })

#define max(x,y) ({ \
    typeof(x) _x = (x);             \
    typeof(y) _y = (y);             \
    (void) (&_x == &_y);            \
    _x > _y ? _x : _y; })
#endif

// in Linux kernel 2.4.10 there is an alternate min/max definition
#undef min_t
#undef max_t

#if 0 /* these macros are of lesser quality - we dont use them */
/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max at all, of course.
 */
#define min_t(type,x,y) \
    ({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y) \
    ({ type __x = (x); type __y = (y); __x > __y ? __x: __y; }}
#endif /* 0/1 */


#ifndef STANDALONE_AGPGART

// since the AGP sub-module can be called at any time 
// we must not apply those special kernel qualifiers
#undef __initdata
#define __initdata  /* empty */
#undef __init
#define __init      /* empty */
#undef __exit
#define __exit      /* empty */

/* alias defines */
#define chipset_type            _X(chipset_type)
    #define NOT_SUPPORTED           _X(NOT_SUPPORTED)
    #define INTEL_GENERIC           _X(INTEL_GENERIC)
    #define INTEL_LX                _X(INTEL_LX)
    #define INTEL_BX                _X(INTEL_BX)
    #define INTEL_GX                _X(INTEL_GX)
    #define INTEL_I810              _X(INTEL_I810)
    #define INTEL_I815              _X(INTEL_I815)
//#ifdef FIREGL_AGP_HOOK
    #define INTEL_I820              _X(INTEL_I820)
//#endif
    #define INTEL_I830_M            _X(INTEL_I830_M)
    #define INTEL_I865_G            _X(INTEL_I865_G)
    #define INTEL_I875_G            _X(INTEL_I875_G)
    #define INTEL_I840              _X(INTEL_I840)
    #define INTEL_I845              _X(INTEL_I845)
    #define INTEL_I845_G            _X(INTEL_I845_G)
    #define INTEL_I850              _X(INTEL_I850)
    #define INTEL_I855_PM           _X(INTEL_I855_PM)
    #define INTEL_I860              _X(INTEL_I860)
    #define INTEL_I7205             _X(INTEL_I7205)
    #define INTEL_I7505             _X(INTEL_I7505)
    #define VIA_GENERIC             _X(VIA_GENERIC)
    #define VIA_VP3                 _X(VIA_VP3)
    #define VIA_MVP3                _X(VIA_MVP3)
    #define VIA_MVP4                _X(VIA_MVP4)
    #define VIA_APOLLO_PLE133       _X(VIA_APOLLO_PLE133)
    #define VIA_APOLLO_PRO          _X(VIA_APOLLO_PRO)
    #define VIA_APOLLO_KX133        _X(VIA_APOLLO_KX133)
    #define VIA_APOLLO_KT133        _X(VIA_APOLLO_KT133)
    #define VIA_APOLLO_KM266        _X(VIA_APOLLO_KM266)
    #define VIA_APOLLO_KT266        _X(VIA_APOLLO_KT266)
    #define VIA_APOLLO_KT400        _X(VIA_APOLLO_KT400)
    #define VIA_APOLLO_P4M266       _X(VIA_APOLLO_P4M266)
    #define VIA_APOLLO_P4X400       _X(VIA_APOLLO_P4X400)
    #define SIS_GENERIC             _X(SIS_GENERIC)
    #define AMD_GENERIC             _X(AMD_GENERIC)
    #define AMD_IRONGATE            _X(AMD_IRONGATE)
//    #define AMD_IGD4                _X(AMD_IGD4)
//    #define AMD_IGD4_2P             _X(AMD_IGD4_2P)
    #define AMD_761                 _X(AMD_761)
    #define AMD_762                 _X(AMD_762)
	#define AMD_8151				_X(AMD_8151)
    #define ALI_M1541               _X(ALI_M1541)
    #define ALI_M1621               _X(ALI_M1621)
    #define ALI_M1631               _X(ALI_M1631)
    #define ALI_M1632               _X(ALI_M1632)
    #define ALI_M1641               _X(ALI_M1641)
    #define ALI_M1647               _X(ALI_M1647)
    #define ALI_M1651               _X(ALI_M1651)
    #define ALI_GENERIC             _X(ALI_GENERIC)
    #define SVWRKS_LE               _X(SVWRKS_LE)
    #define SVWRKS_HE               _X(SVWRKS_HE)
    #define SVWRKS_GENERIC          _X(SVWRKS_GENERIC)
	#define HP_ZX1					_X(HP_ZX1)
#ifdef FIREGL_AGP_HOOK
#ifdef FIREGL_AGP_SVRWKS
    #define SERVERWORKS_CNB20LE     _X(SERVERWORKS_CNB20LE)
    #define SERVERWORKS_CNB20HE     _X(SERVERWORKS_CNB20HE)
#endif // FIREGL_AGP_SVRWKS
#endif // FIREGL_AGP_HOOK
#if 1
    #define NVIDIA_NFORCE           _X(NVIDIA_NFORCE)
    #define NVIDIA_NFORCE2          _X(NVIDIA_NFORCE2)
    #define NVIDIA_NFORCE3          _X(NVIDIA_NFORCE3)
    #define NVIDIA_GENERIC          _X(NVIDIA_GENERIC)
#endif

    #define ATI_RS100               _X(ATI_RS100)
    #define ATI_RS200               _X(ATI_RS200)
    #define ATI_RS250               _X(ATI_RS250)
    #define ATI_RS300_100           _X(ATI_RS300_100)
    #define ATI_RS300_133           _X(ATI_RS300_133)
    #define ATI_RS300_166           _X(ATI_RS300_166)
    #define ATI_RS300_200           _X(ATI_RS300_200)
    #define ATI_RS350_100           _X(ATI_RS350_100)
    #define ATI_RS350_133           _X(ATI_RS350_133)
    #define ATI_RS350_166           _X(ATI_RS350_166)
    #define ATI_RS350_200           _X(ATI_RS350_200)

#define _agp_version            _X(_agp_version)
#define agp_version             _X(agp_version)
#define _agp_kern_info          _X(_agp_kern_info)
#define agp_kern_info           _X(agp_kern_info)
#define _agp_memory             _X(_agp_memory)
#define agp_memory              _X(agp_memory)
#ifdef FGL
#undef agp_allocate_memory_phys_list
#define agp_allocate_memory_phys_list   _X(agp_allocate_memory_phys_list)
#endif
#undef agp_free_memory
#define agp_free_memory         _X(agp_free_memory)
#undef agp_allocate_memory
#define agp_allocate_memory     _X(agp_allocate_memory)
#undef agp_copy_info
#define agp_copy_info           _X(agp_copy_info)
#undef agp_bind_memory
#define agp_bind_memory         _X(agp_bind_memory)
#undef agp_unbind_memory
#define agp_unbind_memory       _X(agp_unbind_memory)
#undef agp_enable
#define agp_enable              _X(agp_enable)
#undef agp_backend_acquire
#define agp_backend_acquire     _X(agp_backend_acquire)
#undef agp_backend_release
#define agp_backend_release     _X(agp_backend_release)
#undef agp_try_unsupported
#define agp_try_unsupported     _X(agp_try_unsupported)
#endif /* !STANDALONE_AGPGART */

#ifndef FIREGL_AGP_HOOK_DEFINES_ONLY

#if (LINUX_VERSION_CODE >= 0x020200) && (LINUX_VERSION_CODE < 0x0202ff)
#define AGPGART_2_2
#endif

#endif // !FIREGL_AGP_HOOK_DEFINES_ONLY


#ifdef AGPGART_2_2

#ifndef FIREGL_AGP_HOOK_DEFINES_ONLY

#define __raw_readb readb
#define __raw_readw readw
#define __raw_readl readl
#define __raw_writeb writeb
#define __raw_writew writew
#define __raw_writel writel

#undef __exit
#define __exit

#endif // !FIREGL_AGP_HOOK_DEFINES_ONLY

#ifndef module_init
#ifdef MODULE
#define module_init(x)  int init_module(void) { return x(); }
#define module_exit(x)  void cleanup_module(void) { x(); }
#else
#define module_init(x)
#define module_exit(x)
#endif /* MODULE */
#endif /* module_init */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(*(x)))
#endif
#endif /* AGPGART_2_2 */

/* generic routines for AGP 3.0 */ 
int agp_generic_agp_3_0_enable(u32);

/* chipset specific routines (init) */
int intel_7505_setup (struct pci_dev *pdev);

/* general routines */
#if LINUX_VERSION_CODE > 0x020500
#ifdef CONFIG_SMP
void global_cache_flush(void);
#else
void global_cache_flush(void);
#endif
#else /* LINUX_VERSION_CODE > 0x020500 */
#ifdef CONFIG_SMP
void smp_flush_cache(void);
#define global_cache_flush      smp_flush_cache
#else
void simple_flush_cache(void);
#define global_cache_flush      simple_flush_cache
#endif
#endif /* LINUX_VERSION_CODE > 0x020500 */

/* generic functions */
int agp_generic_agp_enable(u32);
#if 1 /* FGL */
int agp_generic_agp_v2_enable(u32 mode);
#endif
int agp_generic_create_gatt_table(void);
int agp_generic_free_gatt_table(void);
int agp_generic_insert_memory(agp_memory * mem, off_t pg_start, int type);
int agp_generic_remove_memory(agp_memory * mem, off_t pg_start, int type);
agp_memory *agp_generic_alloc_by_type(size_t page_count, int type);
void agp_generic_free_by_type(agp_memory * curr);
unsigned long agp_generic_alloc_page(void);
void agp_generic_destroy_page(unsigned long page);
int agp_generic_suspend(void);
void agp_generic_resume(void);


/* type definitions and struct specifications */
struct gatt_mask {
        unsigned long mask;
        u32 type;
        /* totally device specific, for integrated chipsets that
         * might have different types of memory masks.  For other
         * devices this will probably be ignored */
};

struct aper_size_info_8 {
        int size;
        int num_entries;
        int page_order;
        u8 size_value;
};

struct aper_size_info_16 {
        int size;
        int num_entries;
        int page_order;
        u16 size_value;
};

struct aper_size_info_32 {
        int size;
        int num_entries;
        int page_order;
        u32 size_value;
};

struct aper_size_info_lvl2 {
        int size;
        int num_entries;
        u32 size_value;
};

struct aper_size_info_fixed {
        int size;
        int num_entries;
        int page_order;
};

enum aper_size_type {
	U8_APER_SIZE,
	U16_APER_SIZE,
	U32_APER_SIZE,
	LVL2_APER_SIZE,
	FIXED_APER_SIZE
};

#if 0
typedef struct _gatt_mask {
	unsigned long mask;
	u32 type;
	/* totally device specific, for integrated chipsets that 
	 * might have different types of memory masks.  For other
	 * devices this will probably be ignored */
} gatt_mask;

typedef struct _aper_size_info_8 {
	int size;
	int num_entries;
	int page_order;
	u8 size_value;
} aper_size_info_8;

typedef struct _aper_size_info_16 {
	int size;
	int num_entries;
	int page_order;
	u16 size_value;
} aper_size_info_16;

typedef struct _aper_size_info_32 {
	int size;
	int num_entries;
	int page_order;
	u32 size_value;
} aper_size_info_32;

typedef struct _aper_size_info_lvl2 {
	int size;
	int num_entries;
	u32 size_value;
} aper_size_info_lvl2;

typedef struct _aper_size_info_fixed {
	int size;
	int num_entries;
	int page_order;
} aper_size_info_fixed;
#endif // 0

struct agp_bridge_data {
	agp_version *version;
	void *aperture_sizes;
	void *previous_size;
	void *current_size;
	void *dev_private_data;
	struct pci_dev *dev;
	struct gatt_mask *masks;
	u32 *gatt_table;
	u32 *gatt_table_real;
	unsigned long scratch_page;
	unsigned long gart_bus_addr;
	unsigned long gatt_bus_addr;
	u32 mode;
	enum chipset_type type;
	enum aper_size_type size_type;
	unsigned long *key_list;
	atomic_t current_memory_agp;
	atomic_t agp_in_use;
	int max_memory_agp;	/* in number of pages */
	int needs_scratch_page;
	int aperture_size_idx;
	int num_aperture_sizes;
	int num_of_masks;
	int capndx;
    int cant_use_aperture;

	/* Links to driver specific functions */

	int (*fetch_size) (void);
	int (*configure) (void);
	int (*agp_enable) (u32);
	void (*cleanup) (void);
	void (*tlb_flush) (agp_memory *);
	unsigned long (*mask_memory) (unsigned long, int);
	void (*cache_flush) (void);
	int (*create_gatt_table) (void);
	int (*free_gatt_table) (void);
	int (*insert_memory) (agp_memory *, off_t, int);
	int (*remove_memory) (agp_memory *, off_t, int);
	agp_memory *(*alloc_by_type) (size_t, int);
	void (*free_by_type) (agp_memory *);
	unsigned long (*agp_alloc_page) (void);
	void (*agp_destroy_page) (unsigned long);
    int (*suspend)(void);
    void (*resume)(void);
};

#define OUTREG64(mmap, addr, val)   __raw_writeq((val), (mmap)+(addr))
#define OUTREG32(mmap, addr, val)   __raw_writel((val), (mmap)+(addr))
#define OUTREG16(mmap, addr, val)   __raw_writew((val), (mmap)+(addr))
#define OUTREG8(mmap, addr, val)    __raw_writeb((val), (mmap)+(addr))

#define INREG64(mmap, addr)         __raw_readq((mmap)+(addr))
#define INREG32(mmap, addr)         __raw_readl((mmap)+(addr))
#define INREG16(mmap, addr)         __raw_readw((mmap)+(addr))
#define INREG8(mmap, addr)          __raw_readb((mmap)+(addr))

#define KB(x) ((x) * 1024)
#define MB(x) (KB (KB (x)))
#define GB(x) (MB (KB (x)))

#define CACHE_FLUSH	agp_bridge.cache_flush
#define A_SIZE_8(x)	((struct aper_size_info_8 *) x)
#define A_SIZE_16(x)	((struct aper_size_info_16 *) x)
#define A_SIZE_32(x)	((struct aper_size_info_32 *) x)
#define A_SIZE_LVL2(x)  ((struct aper_size_info_lvl2 *) x)
#define A_SIZE_FIX(x)	((struct aper_size_info_fixed *) x)
#define A_IDX8()	(A_SIZE_8(agp_bridge.aperture_sizes) + i)
#define A_IDX16()	(A_SIZE_16(agp_bridge.aperture_sizes) + i)
#define A_IDX32()	(A_SIZE_32(agp_bridge.aperture_sizes) + i)
#define A_IDXLVL2()	(A_SIZE_LVL2(agp_bridge.aperture_sizes) + i)
#define A_IDXFIX()	(A_SIZE_FIX(agp_bridge.aperture_sizes) + i)
#define MAXKEY		(4096 * 32)

#define AGPGART_MODULE_NAME	"agpgart"
#define PFX			AGPGART_MODULE_NAME ": "

#define PGE_EMPTY(p) (!(p) || (p) == (unsigned long) agp_bridge.scratch_page)

/* 64bits supports */
#define PGE_EMPTY_64(p) (!(p) || (p) == addr2entry((unsigned long) agp_bridge.scratch_page))

#ifndef PCI_VENDOR_ID_AL
#define PCI_VENDOR_ID_AL					0x10b9
#endif
	#ifndef PCI_DEVICE_ID_AL_M1541_0
	#define PCI_DEVICE_ID_AL_M1541_0			0x1541
	#endif
    #ifndef PCI_DEVICE_ID_AL_M1621_0
    #define PCI_DEVICE_ID_AL_M1621_0	        0x1621
    #endif
    #ifndef PCI_DEVICE_ID_AL_M1631_0
    #define PCI_DEVICE_ID_AL_M1631_0	        0x1631
    #endif
    #ifndef PCI_DEVICE_ID_AL_M1632_0
    #define PCI_DEVICE_ID_AL_M1632_0	        0x1632
    #endif
    #ifndef PCI_DEVICE_ID_AL_M1641_0
    #define PCI_DEVICE_ID_AL_M1641_0	        0x1641
    #endif
    #ifndef PCI_DEVICE_ID_AL_M1647_0
    #define PCI_DEVICE_ID_AL_M1647_0	        0x1647
    #endif
    #ifndef PCI_DEVICE_ID_AL_M1651_0
    #define PCI_DEVICE_ID_AL_M1651_0	        0x1651
    #endif

#ifndef PCI_VENDOR_ID_AMD
#define PCI_VENDOR_ID_AMD					0x1022
#endif
    #ifndef PCI_DEVICE_ID_AMD_IRONGATE_0
    #define PCI_DEVICE_ID_AMD_IRONGATE_0		0x7006
    #endif
    #ifndef PCI_DEVICE_ID_AMD_IGD4_0
    #define PCI_DEVICE_ID_AMD_IGD4_0            0x700E
    #endif
    #ifndef PCI_DEVICE_ID_AMD_761_0
    #define PCI_DEVICE_ID_AMD_761_0             0x700e
    #endif
    #ifndef PCI_DEVICE_ID_AMD_IGD4_2P_0
    #define PCI_DEVICE_ID_AMD_IGD4_2P_0         0x700C
    #endif
    #ifndef PCI_DEVICE_ID_AMD_762_0
    #define PCI_DEVICE_ID_AMD_762_0             0x700C
    #endif
    #ifndef PCI_DEVICE_ID_AMD_8151_0
    #define PCI_DEVICE_ID_AMD_8151_0            0x7454
    #endif

#ifndef PCI_VENDOR_ID_INTEL
#define PCI_VENDOR_ID_INTEL					0x8086
#endif
    #ifndef PCI_DEVICE_ID_INTEL_82443LX_0
    #define PCI_DEVICE_ID_INTEL_82443LX_0		0x7180
    #endif
#ifdef FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_82443LX_2
    #define PCI_DEVICE_ID_INTEL_82443LX_2		0x7182  // ???
    #endif
#endif // FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_82443BX_0
    #define PCI_DEVICE_ID_INTEL_82443BX_0		0x7190
    #endif
#ifdef FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_82443BX_2
    #define PCI_DEVICE_ID_INTEL_82443BX_2		0x7192  // 440BX/ZX - 82443BX/ZX Host bridge (AGP disabled)
    #endif
#endif // FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_82443GX_0
    #define PCI_DEVICE_ID_INTEL_82443GX_0		0x71a0  // 440GX - 82443GX Host-to-PCI Bridge with AGP_CAPS
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_82443GX_AGP_0
    #define PCI_DEVICE_ID_INTEL_82443GX_AGP_0   0x71a1  // 440GX - 82443GX PCI-to-PCI Bridge (AGP) with bridge data
    #endif
#ifdef FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_82443GX_2
    #define PCI_DEVICE_ID_INTEL_82443GX_2		0x71a2  // 440GX - 82443GX Host bridge (AGP disabled)
    #endif
#endif // FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_810_0
    #define PCI_DEVICE_ID_INTEL_810_0			0x7120
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_810_1
    #define PCI_DEVICE_ID_INTEL_810_1			0x7121
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_810_DC100_0
    #define PCI_DEVICE_ID_INTEL_810_DC100_0		0x7122
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_810_DC100_1
    #define PCI_DEVICE_ID_INTEL_810_DC100_1		0x7123
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_810_E_0
    #define PCI_DEVICE_ID_INTEL_810_E_0			0x7124
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_810_E_1
    #define PCI_DEVICE_ID_INTEL_810_E_1			0x7125
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_815_0
    #define PCI_DEVICE_ID_INTEL_815_0			0x1130
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_815_1
    #define PCI_DEVICE_ID_INTEL_815_1			0x1132
    #endif
#ifdef FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_815_MCH
    #define PCI_DEVICE_ID_INTEL_815_MCH			0x1130  // 82815 i815 (Solano) Funtion 0 - Memory Controller Hub
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_815_AGP
    #define PCI_DEVICE_ID_INTEL_815_AGP			0x1131  // 82815 i815 (Solano) Funtion 1 - AGP/PCUI bridge
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_815_GFX
    #define PCI_DEVICE_ID_INTEL_815_GFX			0x1132  // 82815 i815 (Solano) Funtion 2 - Grafics Controller
    #endif
#endif
    #ifndef PCI_DEVICE_ID_INTEL_855_0
    #define PCI_DEVICE_ID_INTEL_855_0           0x3340  // 855_PM func_0, alternate name
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_855_1
    #define PCI_DEVICE_ID_INTEL_855_1           0x3341  // 855_PM func_1, not an alternate name
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_855_GM_0
    #define PCI_DEVICE_ID_INTEL_855_GM_0        0x3580
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_855_GM_1
    #define PCI_DEVICE_ID_INTEL_855_GM_1        0x3582
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_855_PM_0
    #define PCI_DEVICE_ID_INTEL_855_PM_0        0x3340
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_855_PM_1
    #define PCI_DEVICE_ID_INTEL_855_PM_1        0x3342
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_82865_HB
    #define PCI_DEVICE_ID_INTEL_82865_HB        0x2570  // 865G - hostbridge
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_82865_IG
    #define PCI_DEVICE_ID_INTEL_82865_IG        0x2572  // 865G - graphics
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_865_G_0
    #define PCI_DEVICE_ID_INTEL_865_G_0         0x2570  // alternate macro
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_865_G_1
    #define PCI_DEVICE_ID_INTEL_865_G_1         0x2572  // alternate macro
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_875_G_0
    #define PCI_DEVICE_ID_INTEL_875_G_0         0x2578
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_875_G_1
    #define PCI_DEVICE_ID_INTEL_875_G_1         0x257b
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_820_0
    #define PCI_DEVICE_ID_INTEL_820_0           0x2500  // 82820 820 (Camino) Chipset Host Bridge (MCH)
    #endif
#ifdef FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_820_1
    #define PCI_DEVICE_ID_INTEL_820_1           0x2501  // 82820 820 (Camino) Chipset Host Bridge (MCH)
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_820_2
    #define PCI_DEVICE_ID_INTEL_820_2           0x2502  // ???
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_820_3
    #define PCI_DEVICE_ID_INTEL_820_3           0x2503  // ???
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_820_4
    #define PCI_DEVICE_ID_INTEL_820_4           0x2504  // ???
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_820_F
    #define PCI_DEVICE_ID_INTEL_820_F           0x250F  // 82820 820 (Camino) Chipset PCI to AGP Bridge
    #endif
#endif // FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_830_M_0
    #define PCI_DEVICE_ID_INTEL_830_M_0         0x3575
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_830_M_1
    #define PCI_DEVICE_ID_INTEL_830_M_1         0x3577
    #endif
#ifdef FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_840_0
    #define PCI_DEVICE_ID_INTEL_840_0			0x1a20  // ???
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_840_1
    #define PCI_DEVICE_ID_INTEL_840_1			0x1a21  // 82840 840 (Carmel) Chipset Host Bridge (Hub A) with AGP_CAPS
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_840_2
    #define PCI_DEVICE_ID_INTEL_840_2			0x1a22  // ???
    #endif
#else 
    // !FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_840_0
    #define PCI_DEVICE_ID_INTEL_840_0			0x1a21  // 82840 840 (Carmel) Chipset Host Bridge (Hub A) with AGP_CAPS
    #endif
#endif // !FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_INTEL_840_AGP_0
    #define PCI_DEVICE_ID_INTEL_840_AGP_0       0x1a23  // 82840 840 (Carmel) Chipset AGP Bridge with bus id data
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_845_0
    #define PCI_DEVICE_ID_INTEL_845_0           0x1a30
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_82845G_HB
    #define PCI_DEVICE_ID_INTEL_82845G_HB       0x2560
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_82845G_IG
    #define PCI_DEVICE_ID_INTEL_82845G_IG       0x2562
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_845_G_0
    #define PCI_DEVICE_ID_INTEL_845_G_0         0x2560  /* func_0 = HB, alternate macro name */
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_845_G_1
    #define PCI_DEVICE_ID_INTEL_845_G_1         0x2562  /* func_1 = IG, alternate macro name */
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_850_0
    #define PCI_DEVICE_ID_INTEL_850_0           0x2530
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_860_0
    #define PCI_DEVICE_ID_INTEL_860_0           0x2532
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_860_DP_0
    #define PCI_DEVICE_ID_INTEL_860_DP_0        0x2531
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_860_QP_0
    #define PCI_DEVICE_ID_INTEL_860_QP_0        0x2539
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_7205_0
    #define PCI_DEVICE_ID_INTEL_7205_0          0x255D
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_7505_0
    #define PCI_DEVICE_ID_INTEL_7505_0          0x2550
    #endif
    #ifndef PCI_DEVICE_ID_INTEL_7505_1
    #define PCI_DEVICE_ID_INTEL_7505_1          0x2552
    #endif

#ifndef PCI_VENDOR_ID_SI
#define PCI_VENDOR_ID_SI					0x1039
#endif
    #ifndef PCI_DEVICE_ID_SI_530
    #define PCI_DEVICE_ID_SI_530				0x0530
    #endif
    #ifndef PCI_DEVICE_ID_SI_540
    #define PCI_DEVICE_ID_SI_540				0x0540
    #endif
    #ifndef PCI_DEVICE_ID_SI_550
    #define PCI_DEVICE_ID_SI_550                0x0550
    #endif
    #ifndef PCI_DEVICE_ID_SI_620
    #define PCI_DEVICE_ID_SI_620				0x0620
    #endif
    #ifndef PCI_DEVICE_ID_SI_630
    #define PCI_DEVICE_ID_SI_630				0x0630
    #endif
    #ifndef PCI_DEVICE_ID_SI_635
    #define PCI_DEVICE_ID_SI_635                0x0635
    #endif
    #ifndef PCI_DEVICE_ID_SI_640
    #define PCI_DEVICE_ID_SI_640                0x0640
    #endif
    #ifndef PCI_DEVICE_ID_SI_645
    #define PCI_DEVICE_ID_SI_645                0x0645
    #endif
    #ifndef PCI_DEVICE_ID_SI_646
    #define PCI_DEVICE_ID_SI_646                0x0646
    #endif
    #ifndef PCI_DEVICE_ID_SI_645DX
    #define PCI_DEVICE_ID_SI_645DX              0x0648 /* == SiS 648 */
    #endif
    #ifndef PCI_DEVICE_ID_SI_648
    #define PCI_DEVICE_ID_SI_648                0x0648
    #endif
    #ifndef PCI_DEVICE_ID_SI_650
    #define PCI_DEVICE_ID_SI_650                0x0650
    #endif
    #ifndef PCI_DEVICE_ID_SI_651
    #define PCI_DEVICE_ID_SI_651                0x0651
    #endif
    #ifndef PCI_DEVICE_ID_SI_735
    #define PCI_DEVICE_ID_SI_735                0x0735
    #endif
    #ifndef PCI_DEVICE_ID_SI_740
    #define PCI_DEVICE_ID_SI_740                0x0740
    #endif
    #ifndef PCI_DEVICE_ID_SI_746
    #define PCI_DEVICE_ID_SI_746                0x0746
    #endif

#ifndef PCI_VENDOR_ID_VIA
#define PCI_VENDOR_ID_VIA					0x1106
#endif
    #ifndef PCI_DEVICE_ID_VIA_8501_0
    #define PCI_DEVICE_ID_VIA_8501_0			0x0501
    #endif
    #ifndef PCI_DEVICE_ID_VIA_82C597_0
    #define PCI_DEVICE_ID_VIA_82C597_0			0x0597
    #endif
    #ifndef PCI_DEVICE_ID_VIA_82C598_0
    #define PCI_DEVICE_ID_VIA_82C598_0			0x0598
    #endif
    #ifndef PCI_DEVICE_ID_VIA_82C691_0
    #define PCI_DEVICE_ID_VIA_82C691_0			0x0691
    #endif
    #ifndef PCI_DEVICE_ID_VIA_8371_0
    #define PCI_DEVICE_ID_VIA_8371_0			0x0391
    #endif
    #ifndef PCI_DEVICE_ID_VIA_8375
    #define PCI_DEVICE_ID_VIA_8375              0x3116
    #endif
    #ifndef PCI_DEVICE_ID_VIA_8363_0
    #define PCI_DEVICE_ID_VIA_8363_0			0x0305
    #endif
    #ifndef PCI_DEVICE_ID_VIA_8601_0
    #define PCI_DEVICE_ID_VIA_8601_0            0x0601
    #endif
    #ifndef PCI_DEVICE_ID_VIA_82C694X_0
    #define PCI_DEVICE_ID_VIA_82C694X_0         0x0605
    #endif
    #ifndef PCI_DEVICE_ID_VIA_8377_0 /* VIA Apollo KT400, AMD Athlon HB */
    #define PCI_DEVICE_ID_VIA_8377_0			0x3189
    #endif
    #ifndef PCI_DEVICE_ID_VIA_8754 /* VIA P4P400, Pentium 4 HB */
    #define PCI_DEVICE_ID_VIA_8754              0x3168
    #endif
    #ifndef PCI_DEVICE_ID_VIA_P4X333
    #define PCI_DEVICE_ID_VIA_P4X333            0x3168  // identical to above
    #endif
    #ifndef PCI_DEVICE_ID_VIA_P4M266
    #define PCI_DEVICE_ID_VIA_P4M266            0x3148
    #endif

#ifdef FIREGL_AGP_HOOK
// ServerWorks aka Fujitsu - Reliance Computer Corp. (RCC)
#ifndef PCI_VENDOR_ID_SERVERWORKS
#define PCI_VENDOR_ID_SERVERWORKS       0x1166
#endif // !
    // CNB20LE chipset
    #ifndef PCI_DEVIDE_ID_SERVERWORKS_CNB20LE_MCH_0
    #define PCI_DEVICE_ID_SERVERWORKS_CNB20LE_MCH_0 0x0007  // CNB20LE - host bridge & MCH - func 0
    #endif
    #ifndef PCI_DEVICE_ID_SERVERWORKS_CNB20LE_AGP_1
    #define PCI_DEVICE_ID_SERVERWORKS_CNB20LE_AGP_1 0x0005  // CNB20LE - AGP bridge - func 1
    #endif

    // CNB20HE chipset
    #ifndef PCI_DEVIDE_ID_SERVERWORKS_CNB20HE_MCH_0
    #define PCI_DEVICE_ID_SERVERWORKS_CNB20HE_MCH_0 0x0008  // CNB20HE - host bridge & MCH - func 0
    #endif
    #ifndef PCI_DEVICE_ID_SERVERWORKS_CNB20HE_AGP_1
    #define PCI_DEVICE_ID_SERVERWORKS_CNB20HE_AGP_1 0x0009  // CNB20HE - AGP bridge - func 1
    #endif
    #ifndef PCI_DEVICE_ID_SERVERWORKS_CNB20HE_PCI_1
    #define PCI_DEVICE_ID_SERVERWORKS_CNB20HE_PCI_1 0x0005  // CNB20HE - second PCI bridge - func 1
    #endif
#endif // FIREGL_AGP_HOOK
    #ifndef PCI_DEVICE_ID_SERVERWORKS_LE
    #define PCI_DEVICE_ID_SERVERWORKS_LE			0x0007
    #endif
    #ifndef PCI_DEVICE_ID_SERVERWORKS_HE
    #define PCI_DEVICE_ID_SERVERWORKS_HE			0x0008
    #endif

#ifndef PCI_VENDOR_ID_ATI
#define PCI_VENDOR_ID_ATI               0x1002
#endif
#ifndef PCI_DEVICE_ID_ATI_RS100
#define PCI_DEVICE_ID_ATI_RS100         0xcab0
#endif
#ifndef PCI_DEVICE_ID_ATI_RS200
#define PCI_DEVICE_ID_ATI_RS200         0xcab2
#endif
#ifndef PCI_DEVICE_ID_ATI_RS250
#define PCI_DEVICE_ID_ATI_RS250         0xcab3
#endif
#ifndef PCI_DEVICE_ID_ATI_RS300_100
#define PCI_DEVICE_ID_ATI_RS300_100     0x5830
#endif
#ifndef PCI_DEVICE_ID_ATI_RS300_133
#define PCI_DEVICE_ID_ATI_RS300_133     0x5831
#endif
#ifndef PCI_DEVICE_ID_ATI_RS300_166
#define PCI_DEVICE_ID_ATI_RS300_166     0x5832
#endif
#ifndef PCI_DEVICE_ID_ATI_RS300_200
#define PCI_DEVICE_ID_ATI_RS300_200     0x5833
#endif
#ifndef PCI_DEVICE_ID_ATI_RS350_100
#define PCI_DEVICE_ID_ATI_RS350_100     0x7830
#endif
#ifndef PCI_DEVICE_ID_ATI_RS350_133
#define PCI_DEVICE_ID_ATI_RS350_133     0x7831
#endif
#ifndef PCI_DEVICE_ID_ATI_RS350_166
#define PCI_DEVICE_ID_ATI_RS350_166     0x7832
#endif
#ifndef PCI_DEVICE_ID_ATI_RS350_200
#define PCI_DEVICE_ID_ATI_RS350_200     0x7833
#endif
                        
#ifdef PCI_VENDOR_ID_NVIDIA
#define PCI_VENDOR_ID_NVIDIA                    0x10de
#endif
    #ifndef PCI_DEVICE_ID_NVIDIA_NFORCE
    #define PCI_DEVICE_ID_NVIDIA_NFORCE             0x01a4
    #endif
    #ifndef PCI_DEVICE_ID_NVIDIA_NFORCE2
    #define PCI_DEVICE_ID_NVIDIA_NFORCE2            0x01e0
    #endif

    #ifndef PCI_DEVICE_ID_NVIDIA_NFORCE3
    #define PCI_DEVICE_ID_NVIDIA_NFORCE3		0x00d1
    #endif
    #ifndef PCI_DEVICE_ID_NVIDIA_NFORCE3S
    #define PCI_DEVICE_ID_NVIDIA_NFORCE3S		0x00e1
    #endif

#ifdef FGL
/* pci spec conforming register offsets */
#define PCI_VENDOR_ID       0x00
#define PCI_DEVICE_ID       0x02
#define PCI_SUB_VENDOR_ID   0x2C
#define PCI_SUB_DEVICE_ID   0x2E

#define PCI_RAM_BAR0        0x10 // typically AGP aperture base in this context
#define PCI_RAM_BAR1        0x14 // sometimes memory mapped registers
#define PCI_RAM_BAR2        0x18
#define PCI_RAM_BAR3        0x1C
#define PCI_RAM_BAR4        0x20
#define PCI_RAM_BAR5        0x24

#define PCI_ROM_BAR0        0x30

#ifndef PCI_BASE_ADDRESS_MEM_MASK
#define  PCI_BASE_ADDRESS_MEM_MASK      (~0x0fUL)
#endif

#ifndef PCI_CLASS_DISPLAY_VGA
#define PCI_CLASS_DISPLAY_VGA           0x0300
#endif

#endif

/* intel register */
#define INTEL_APBASE    0x10
#define INTEL_APSIZE    0xb4
#define INTEL_ATTBASE   0xb8
#define INTEL_AGPCTRL   0xb0
#define INTEL_NBXCFG    0x50
#define INTEL_ERRSTS    0x91

/* intel i810 registers */
#define I810_GMADDR 0x10
#define I810_MMADDR 0x14
#define I810_PTE_BASE          0x10000
#define I810_PTE_MAIN_UNCACHED 0x00000000
#define I810_PTE_LOCAL         0x00000002
#define I810_PTE_VALID         0x00000001
#define I810_SMRAM_MISCC       0x70
#define I810_GFX_MEM_WIN_SIZE  0x00010000
#define I810_GFX_MEM_WIN_32M   0x00010000
#define I810_GMS               0x000000c0
#define I810_GMS_DISABLE       0x00000000
#define I810_PGETBL_CTL        0x2020
#define I810_PGETBL_ENABLED    0x00000001
#define I810_DRAM_CTL          0x3000
#define I810_DRAM_ROW_0        0x00000001
#define I810_DRAM_ROW_0_SDRAM  0x00000001

/* intel i820 registers */
#ifdef FGL
#define INTEL_I820_MCHCFG   0x50
#endif
#define INTEL_I820_RDCR     0x51
#define INTEL_I820_ERRSTS   0xc8

/* Intel 855GM/852GM registers */
#define I855_GMCH_CTRL             0x52
#define I855_GMCH_ENABLED          0x4
#define I855_GMCH_GMS_MASK         (0x7 << 4)
#define I855_GMCH_GMS_STOLEN_0M    0x0
#define I855_GMCH_GMS_STOLEN_1M    (0x1 << 4)
#define I855_GMCH_GMS_STOLEN_4M    (0x2 << 4)
#define I855_GMCH_GMS_STOLEN_8M    (0x3 << 4)
#define I855_GMCH_GMS_STOLEN_16M   (0x4 << 4)
#define I855_GMCH_GMS_STOLEN_32M   (0x5 << 4)
#define I85X_CAPID                 0x44
#define I85X_VARIANT_MASK          0x7
#define I85X_VARIANT_SHIFT         5
#define I855_GME                   0x0
#define I855_GM                    0x4
#define I852_GME                   0x2
#define I852_GM                    0x5
#define I855_PME                   0x0
#define I855_PM                    0x4
#define I852_PME                   0x2
#define I852_PM                    0x5

/* intel i830 registers */
#define I830_GMCH_CTRL             0x52
#define I830_GMCH_ENABLED          0x4
#define I830_GMCH_MEM_MASK         0x1
#define I830_GMCH_MEM_64M          0x1
#define I830_GMCH_MEM_128M         0
#define I830_GMCH_GMS_MASK         0x70
#define I830_GMCH_GMS_DISABLED     0x00
#define I830_GMCH_GMS_LOCAL        0x10
#define I830_GMCH_GMS_STOLEN_512   0x20
#define I830_GMCH_GMS_STOLEN_1024  0x30
#define I830_GMCH_GMS_STOLEN_8192  0x40
#define I830_RDRAM_CHANNEL_TYPE    0x03010
#define I830_RDRAM_ND(x)           (((x) & 0x20) >> 5)
#define I830_RDRAM_DDT(x)          (((x) & 0x18) >> 3)
/* This one is for I830MP w. an external graphic card */
#define INTEL_I830_ERRSTS       0x92

/* intel i840 registers */
#define INTEL_I840_MCHCFG   0x50
#define INTEL_I840_ERRSTS	0xc8

/* intel i845 registers */
#define INTEL_I845_AGPM     0x51
#define INTEL_I845_ERRSTS   0xc8

/* intel i850 registers */
#define INTEL_I850_MCHCFG   0x50
#define INTEL_I850_ERRSTS   0xc8

/* intel i860 registers */
#define INTEL_I860_MCHCFG      0x50
#define INTEL_I860_ERRSTS      0xc8

/* Intel 7505 registers */ 
#define INTEL_I7505_NAPBASELO   0x10 
#define INTEL_I7505_APSIZE      0x74 
#define INTEL_I7505_NCAPID      0x60 
#define INTEL_I7505_NISTAT      0x6c 
#define INTEL_I7505_ATTBASE     0x78 
#define INTEL_I7505_ERRSTS      0x42 
#define INTEL_I7505_AGPCTRL     0x70

/* VIA register */
#define VIA_APBASE      0x10
#define VIA_GARTCTRL    0x80
#define VIA_APSIZE      0x84
#define VIA_ATTBASE     0x88

/* VIA KT400 */
#define VIA_AGP3_GARTCTRL 	0x90
#define VIA_AGP3_APSIZE   	0x94
#define VIA_AGP3_ATTBASE  	0x98
#define VIA_AGPSEL        	0xfd

/* SiS registers */
#define SIS_APBASE      0x10
#define SIS_ATTBASE     0x90
#define SIS_APSIZE      0x94
#define SIS_TLBCNTRL    0x97
#define SIS_TLBFLUSH    0x98

/* AMD registers */
#define AMD_APBASE      0x10
#define AMD_MMBASE      0x14
#define AMD_APSIZE      0xac
#define AMD_MODECNTL    0xb0
#define AMD_MODECNTL2   0xb2
#define AMD_GARTENABLE  0x02	/* In mmio region (16-bit register) */
#define AMD_ATTBASE     0x04	/* In mmio region (32-bit register) */
#define AMD_TLBFLUSH    0x0c	/* In mmio region (32-bit register) */
#define AMD_CACHEENTRY  0x10	/* In mmio region (32-bit register) */

#define AMD_8151_APSIZE	0xb4
#define AMD_8151_GARTBLOCK	0xb8

#define AMD_X86_64_GARTAPERTURECTL	0x90
#define AMD_X86_64_GARTAPERTUREBASE	0x94
#define AMD_X86_64_GARTTABLEBASE	0x98
#define AMD_X86_64_GARTCACHECTL		0x9c
#define AMD_X86_64_GARTEN	        (1<<0)
/* Aperture control register bits. */
#define GARTEN				(1<<0)
#define DISGARTCPU			(1<<4)
#define DISGARTIO			(1<<5)


#define AMD_8151_VMAPERTURE		0x10
#define AMD_8151_AGP_CTL		0xb0
#define AMD_8151_APERTURESIZE	0xb4
#define AMD_8151_GARTPTR		0xb8
#define AMD_8151_GTLBEN	1<<7
#define AMD_8151_APEREN	1<<8



/* ALi registers */
#define ALI_APBASE	0x10
#define ALI_AGPCTRL	0xb8
#define ALI_ATTBASE	0xbc
#define ALI_TLBCTRL	0xc0
#define ALI_TAGCTRL	0xc4
#define ALI_CACHE_FLUSH_CTRL	0xD0
#define ALI_CACHE_FLUSH_ADDR_MASK	0xFFFFF000
#define ALI_CACHE_FLUSH_EN	0x100
                                                                                 
/* Serverworks Registers */                                                        
#define SVWRKS_APSIZE 0x10                                                         
#define SVWRKS_SIZE_MASK 0xfe000000                                                
                                                                                   
#define SVWRKS_MMBASE 0x14                                                         
#define SVWRKS_CACHING 0x4b                                                        
#define SVWRKS_FEATURE 0x68                                                        
                                                                                   
/* func 1 registers */                                                             
#define SVWRKS_AGP_ENABLE 0x60                                                     
#define SVWRKS_COMMAND 0x04                                                        
                                                                                   
/* Memory mapped registers */                                                      
#define SVWRKS_GART_CACHE 0x02                                                     
#define SVWRKS_GATTBASE   0x04                                                     
#define SVWRKS_TLBFLUSH   0x10                                                     
#define SVWRKS_POSTFLUSH  0x14                                                     
#define SVWRKS_DIRFLUSH   0x0c

/* NVIDIA registers */
#define NVIDIA_0_APBASE     0x10
#define NVIDIA_0_APSIZE     0x80
#define NVIDIA_1_WBC        0xf0
#define NVIDIA_2_GARTCTRL   0xd0
#define NVIDIA_2_APBASE     0xd8
#define NVIDIA_2_APLIMIT    0xdc
#define NVIDIA_2_ATTBASE(i) (0xe0 + (i) * 4)
#define NVIDIA_3_APBASE     0x50
#define NVIDIA_3_APLIMIT    0x54

/* NVIDIA x86-64 registers */
#define NVIDIA_X86_64_0_APBASE		0x10
#define NVIDIA_X86_64_1_APBASE1		0x50
#define NVIDIA_X86_64_1_APLIMIT1	0x54
#define NVIDIA_X86_64_1_APSIZE		0xa8
#define NVIDIA_X86_64_1_APBASE2		0xd8
#define NVIDIA_X86_64_1_APLIMIT2	0xdc

/* HP ZX1 SBA registers */
#define HP_ZX1_CTRL		0x200
#define HP_ZX1_IBASE		0x300
#define HP_ZX1_IMASK		0x308
#define HP_ZX1_PCOM		0x310
#define HP_ZX1_TCNFG		0x318
#define HP_ZX1_PDIR_BASE	0x320
#define HP_ZX1_CACHE_FLUSH	0x428

/* HP ZX1 SBA registers */
#define HP_ZX1_CTRL		0x200
#define HP_ZX1_IBASE		0x300
#define HP_ZX1_IMASK		0x308
#define HP_ZX1_PCOM		0x310
#define HP_ZX1_TCNFG		0x318
#define HP_ZX1_PDIR_BASE	0x320
#define HP_ZX1_CACHE_FLUSH	0x428

/* ATI register */
#define ATI_APBASE                  0x10
#define ATI_GART_MMBASE_ADDR        0x14
#define ATI_RS100_APSIZE            0xac
#define ATI_RS300_APSIZE            0xf8
#define ATI_RS100_IG_AGPMODE        0xb0
#define ATI_RS300_IG_AGPMODE        0xfc

#define ATI_GART_FEATURE_ID         0x00
#define ATI_GART_MCIND_FEATURE_ID   0x2b

/* Add masks for field of ATI_GART_FEATURE_ID */
#define ATI_GART_IND_FEATURE_ID          0x2b
#define ATI_GART_IND_GART_BASE           0x2c
#define ATI_GART_IND_CACHE_SZBASE        0x2d
#define ATI_GART_IND_CACHE_CNTRL         0x2e
#define ATI_GART_IND_CACHE_ENTRY_CNTRL   0x2f

#define ATI_GART_FEATURE_ID__HANG_EN     (0x1L << 11)
#define ATI_GART_FEATURE_ID__TLB_ENABLE  (0x1L << 18)
#define ATI_GART_FEATURE_ID__GTW_LAC_EN  (0x1L << 25)
#define ATI_GART_FEATURE_ID__LV1_INDEX   (0x1L << 30)
#define ATI_GART_FEATURE_ID__PDC_EN      (0x1L << 31)

#define ATI_GART_BASE               0x04
#define ATI_GART_CACHE_SZBASE       0x08
#define ATI_GART_CACHE_CNTRL        0x0c
#define ATI_GART_CACHE_ENTRY_CNTRL  0x10


/* some external prototypes */
extern struct agp_bridge_data agp_bridge; /* device with AGP_CAPS */
#if 1 /* FGL */
extern struct agp_bridge_data agp_bridge_pci2pci; /* device with PCI-to-PCI data */
extern int agp_graphics_device_bus; /* bus where the agp graphics device is */
#endif
#ifdef FIREGL_AGP_HOOK
extern void firegl_agp_flush_cache(void);

extern int __init intel_generic_setup (struct pci_dev *pdev);
extern int __init fgl_intel_i815_AGP_setup (struct pci_dev *pAgpDev);
#ifdef FIREGL_AGP_SVRWKS
extern int __init fgl_svrwks_cnb20le_setup (struct pci_dev *pAgpDev);
extern int __init fgl_svrwks_cnb20he_setup (struct pci_dev *pAgpDev);
#if 0
extern int __init fgl_svrwks_generic_setup (struct pci_dev *pdev);
#endif
#endif // FIREGL_AGP_SVRWKS
#endif // FIREGL_AGP_HOOK

#if 1
extern int __init nvidia_nforce_setup(struct pci_dev *pdev);
#endif

#if LINUX_VERSION_CODE > 0x020500
#if defined(__i386__) || defined(__x86_64__)
#define flush_agp_cache() asm volatile("wbinvd":::"memory")
#endif /* defined(__i386__) */
#endif /* LINUX_VERSION_CODE > 0x020500 */

#endif /* _FGL_AGP_H */
