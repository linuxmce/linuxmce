/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/*
 * $Id: wce_io.h,v 1.2 2006/04/09 16:48:18 mloskot Exp $
 *
 * Equivalent of <io.h> - file handling functions.
 *
 * Created by Mateusz Loskot, mloskot@taxussi.com.pl
 *
 * Copyright (c) 2006 Taxus SI Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom 
 * the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT License:
 * http://opensource.org/licenses/mit-license.php
 *
 * Contact:
 * Taxus SI Ltd.
 * http://www.taxussi.com.pl
 *
 */
#ifndef WCEEX_IO_H
#define WCEEX_IO_H 1

#if !defined(_WIN32_WCE)
# error "Only Winddows CE target is supported!"
#endif

#include <wce_time.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/*******************************************************************************
    I/O Types and Structures
*******************************************************************************/

#ifndef _INTPTR_T_DEFINED
typedef long intptr_t;
# define _INTPTR_T_DEFINED
#endif

#ifndef _FSIZE_T_DEFINED
typedef unsigned long fsize_t;
# define _FSIZE_T_DEFINED
#endif

#define MAX_PATH    260

#ifndef _FINDDATA_T_DEFINED
struct _finddata_t
{
        unsigned    attrib;         /* File attributes */
        time_t      time_create;    /* -1L for FAT file systems */
        time_t      time_access;    /* -1L for FAT file systems */
        time_t      time_write;     /* Time of last modification */
        fsize_t     size;           /* Size of file in bytes */
        char        name[MAX_PATH]; /* Name of file witout complete path */
};
# define _FINDDATA_T_DEFINED
#endif

/* File attribute constants for _findfirst() */

/* XXX - mloskot - macra IS_xxx */
#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */


/*******************************************************************************
    I/O Functions
*******************************************************************************/

intptr_t wceex_findfirst(const char *filespec, struct _finddata_t *fileinfo);
int      wceex_findnext(intptr_t handle, struct _finddata_t *fileinfo);
int      wceex_findclose(intptr_t hFile);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* #ifndef WCEEX_IO_H */
