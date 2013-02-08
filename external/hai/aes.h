/*******************************************************************************/
/* Copyright (C) 2004-2005  Chuck Cannon                                       */
/*                                                                             */
/* This program is free software; you can redistribute it and/or               */
/* modify it under the terms of the GNU General Public License                 */
/* as published by the Free Software Foundation; either version 2              */
/* of the License, or (at your option) any later version.                      */
/*                                                                             */
/* This program is distributed in the hope that it will be useful,             */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of              */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               */
/* GNU General Public License for more details.                                */
/*                                                                             */
/* You should have received a copy of the GNU General Public License           */
/* along with this program; if not, write to the Free Software                 */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */
/*******************************************************************************/

#ifndef _AES_H_
#define _AES_H_

/* Includes */

#include "hai_comm.h"

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/* Enumerations */

enum {DEFAULT_BLOCK_SIZE=16};
enum {MAX_BLOCK_SIZE=32, MAX_ROUNDS=14, MAX_KC=8, MAX_BC=8};
enum {ECB=0, CBC=1, CFB=2};

/* Typedefs */

typedef struct
{
    int m_Ke[MAX_ROUNDS+1][MAX_BC];
    int m_Kd[MAX_ROUNDS+1][MAX_BC];
    int m_keylength;
    int m_blockSize;
    int m_iROUNDS;
    char m_chain0[MAX_BLOCK_SIZE];
    char m_chain[MAX_BLOCK_SIZE];
    int tk[MAX_KC];
    int a[MAX_BC];
    int t[MAX_BC];
} aes_t;

/* Function prototypes */

HAIIMPORT void MakeKey(aes_t *aes, char const *key, char const *chain, int keylength,
    int blockSize);
HAIIMPORT void EncryptBlock(aes_t *aes, char const *in, char *result);
HAIIMPORT void DecryptBlock(aes_t *aes, char const *in, char *result);
HAIIMPORT void Encrypt(aes_t *aes, char const *in, char *result, int n, int iMode);
HAIIMPORT void Decrypt(aes_t *aes, char const *in, char *result, int n, int iMode);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif /* _AES_H_ */

