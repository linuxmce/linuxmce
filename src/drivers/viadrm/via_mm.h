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
#ifndef _via_drm_mm_h_
#define _via_drm_mm_h_

typedef struct {
/*=* John Sheng [2004.6.3] For AMD64 *=*/
#ifndef __x86_64__
    unsigned int context;
#else
    unsigned long context;
#endif
    unsigned int size;
    unsigned long offset;
/*=* John Sheng [2004.6.3] For AMD64 *=*/
#ifndef __x86_64__
    unsigned int free;
#else
    unsigned long free;
#endif
} drm_via_mm_t;

typedef struct {
    unsigned int size;
    unsigned long handle;
    void *virtual;
} drm_via_dma_t;

int via_fb_alloc(drm_via_mem_t *mem);
int via_fb_free(drm_via_mem_t *mem);
int via_agp_alloc(drm_via_mem_t *mem);
int via_agp_free(drm_via_mem_t *mem);

#endif
