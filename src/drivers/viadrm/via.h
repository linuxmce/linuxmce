/*
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 */
/*
 * Copyright 1998-2005 VIA Technologies, Inc. All Rights Reserved.
 * Copyright 2001-2005 S3 Graphics, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) OR COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef __VIA_H__
#define __VIA_H__


#define DRM(x) viadrv_##x


#define __HAVE_AGP		1
#define __MUST_HAVE_AGP		0
#define __HAVE_MTRR		1
#define __HAVE_CTX_BITMAP	1


#define DRIVER_AGP_BUFFERS_MAP( dev )					\
	((drm_via_private_t *)((dev)->dev_private))->buffers
/*=* John Sheng [2004.5.13] For linux kernel 2.6.x *=*/
/* VIA specific ioctls */
#define DRM_IOCTL_VIA_ALLOCMEM	DRM_IOWR(0x40, drm_via_mem_t)
#define DRM_IOCTL_VIA_FREEMEM	DRM_IOW(0x41, drm_via_mem_t)
#define DRM_IOCTL_VIA_AGP_INIT	DRM_IOWR(0x42, drm_via_agp_t)
#define DRM_IOCTL_VIA_FB_INIT	DRM_IOWR(0x43, drm_via_fb_t)
#define DRM_IOCTL_VIA_MAP_INIT	DRM_IOWR(0x44, drm_via_init_t)

#define DRIVER_PCI_IDS						\
	{0x1106, 0x3122, 0, "UNICHROME_CLE266"},		\
	{0x1106, 0x7205, 0, "UNICHROME_K400"},			\
	{0x1106, 0x3108, 0, "UNICHROME_K800"},			\
	{0x1106, 0x3118, 0, "UNICHROME_PM800"},			\
	{0, 0, 0, NULL}

extern int via_init_context(int context);
extern int via_final_context(int context);

#define DRIVER_CTX_CTOR via_init_context
#define DRIVER_CTX_DTOR via_final_context

#endif
