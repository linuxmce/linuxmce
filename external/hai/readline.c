/*******************************************************************************/
/* Copyright (C) 2005  Bob Sutnavage                                           */
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

#ifdef READLINE_SUPPORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START 64
#define INCREASE ( START + 64 )

char *readline (FILE *f)
{
  size_t buffersize, ninput;
  char *buffer = NULL, *savebuffer = NULL, *input = NULL;

  if ( ( buffer = malloc ( START ) ) == NULL )
    return NULL;

  buffersize = ninput = START;
  input = buffer;

  if ( fgets ( input, ninput, f ) == NULL ) {
    free ( buffer );
    return NULL;
  }

  while ( ( savebuffer = strchr ( input, '\n' ) ) == NULL ) {
    ninput = INCREASE + 1;
    buffersize += INCREASE;

    if ( ( savebuffer = realloc ( buffer, buffersize ) ) == NULL )
      return buffer;

    buffer = savebuffer;
    input = buffer + ( buffersize - INCREASE - 1 );

    if ( fgets ( input, ninput, f ) == NULL ) {
      free ( buffer );
      return NULL;
    }
  }

  *savebuffer = '\0';
  ninput = savebuffer - buffer;

  if ( ( savebuffer = realloc ( buffer, ninput + 1 ) ) == NULL )
    return buffer;

  return savebuffer;
}

#endif /* READLINE_SUPPORT */

