/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "Colorspace_Utils.h"

void Colorspace_Utils::yuy2toyv12 ( uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height )
{
    int i, j, w2;

    w2 = width / 2;

    for ( i = 0; i < height; i += 2 )
    {
        for ( j = 0; j < w2; j++ )
        {
            /*
             * packed YUV 422 is: Y[i] U[i] Y[i+1] V[i]
             */
            *( y++ ) = *( input++ );
            *( u++ ) = *( input++ );
            *( y++ ) = *( input++ );
            *( v++ ) = *( input++ );
        }

        /*
         * down sampling
         */
        for ( j = 0; j < w2; j++ )
        {
            /*
             * skip every second line for U and V
             */
            *( y++ ) = *( input++ );
            input++;
            *( y++ ) = *( input++ );
            input++;
        }
    }
}

#define clip_8_bit(val)           \
{                                 \
   if (val < 0)                   \
      val = 0;                    \
   else                           \
      if (val > 255) val = 255;   \
}

uint8_t * Colorspace_Utils::yv12torgb ( uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height )
{
    int i, j;

    int y, u, v;
    int r, g, b;

    int sub_i_uv;
    int sub_j_uv;

    int uv_width, uv_height;

    uint8_t *rgb;

    uv_width = width / 2;
    uv_height = height / 2;

    rgb = ( uint8_t* ) malloc ( width * height * 3 );
    if ( !rgb )
        return NULL;

    for ( i = 0; i < height; ++i )
    {
        /*
         * calculate u & v rows
         */
        sub_i_uv = ( ( i * uv_height ) / height );

        for ( j = 0; j < width; ++j )
        {
            /*
             * calculate u & v columns
             */
            sub_j_uv = ( ( j * uv_width ) / width );

            /***************************************************
             *
             *  Colour conversion from
             *    http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html#RTFToC30
             *
             *  Thanks to Billy Biggs <vektor@dumbterm.net>
             *  for the pointer and the following conversion.
             *
             *   R' = [ 1.1644         0    1.5960 ]   ([ Y' ]   [  16 ])
             *   G' = [ 1.1644   -0.3918   -0.8130 ] * ([ Cb ] - [ 128 ])
             *   B' = [ 1.1644    2.0172         0 ]   ([ Cr ]   [ 128 ])
             *
             *  Where in xine the above values are represented as
             *
             *   Y' == image->y
             *   Cb == image->u
             *   Cr == image->v
             *
             ***************************************************/

            y = src_y[ ( i * width ) + j ] - 16;
            u = src_u[ ( sub_i_uv * uv_width ) + sub_j_uv ] - 128;
            v = src_v[ ( sub_i_uv * uv_width ) + sub_j_uv ] - 128;

            r = ( int ) ( ( 1.1644 * y ) + ( 1.5960 * v ) );
            g = ( int ) ( ( 1.1644 * y ) - ( 0.3918 * u ) - ( 0.8130 * v ) );
            b = ( int ) ( ( 1.1644 * y ) + ( 2.0172 * u ) );

            clip_8_bit ( r );
            clip_8_bit ( g );
            clip_8_bit ( b );

            rgb[ ( i * width + j ) * 3 + 0 ] = r;
            rgb[ ( i * width + j ) * 3 + 1 ] = g;
            rgb[ ( i * width + j ) * 3 + 2 ] = b;
        }
    }

    return rgb;
}

