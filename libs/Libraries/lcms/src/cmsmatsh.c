//
//  Little cms
//  Copyright (C) 1998-2000 Marti Maria
//
// THIS SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//
// IN NO EVENT SHALL MARTI MARIA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
// INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
// OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
// LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
// OF THIS SOFTWARE.
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#include "lcms.h"


// Shaper/Matrix handling
// This routines handles the matrix-shaper method. A note about domain
// is here required. If the shaper-matrix is invoked on INPUT profiles,
// after the shaper process, we have a value between 0 and 0xFFFF. Thus,
// for proper matrix handling, we must convert it to 15fix16, so
// ToFixedDomain might be called. But cmsLinearInterpFixed() returns
// data yet in fixed point, so no additional process is required.
// Then, we obtain data on 15.16, so we need to shift >> by 1 to
// obtain 1.15 PCS format.
// On OUTPUT profiles, things are inverse, we must first expand 1 bit
// by shifting left, and then convert result between 0 and 1.000 to
// RGB, so FromFixedDomain() must be called before pass values to
// shaper. Trickly, there is a situation where this shifts works
// little different. Sometimes, lcms smelts input/output
// matrices into a single, one shaper, process. In such cases, since
// input is encoded from 0 to 0xffff, we must first use the shaper and
// then the matrix, an additional FromFixedDomain() must be used to
// accomodate output values.
// For a sake of simplicity, I will handle this three behaviours
// with different routines, so the flags MATSHAPER_INPUT and MATSHAPER_OUTPUT
// can be conbined to signal smelted matrix-shapers



// Creation & Destruction

LPMATSHAPER cmsAllocMatShaper(LPMAT3 Matrix, LPGAMMATABLE Tables[], DWORD Behaviour)
{
       LPMATSHAPER NewMatShaper;
       int i, AllLinear;

       NewMatShaper = (LPMATSHAPER) malloc(sizeof(MATSHAPER));
       if (NewMatShaper)
              ZeroMemory(NewMatShaper, sizeof(MATSHAPER));

       NewMatShaper->dwFlags = Behaviour & (MATSHAPER_ALLSMELTED);

       // Fill matrix part

       MAT3toFix(&NewMatShaper -> Matrix, Matrix);

       // Reality check

       if (!MAT3isIdentity(&NewMatShaper -> Matrix, 0.00001))
                     NewMatShaper -> dwFlags |= MATSHAPER_HASMATRIX;

       // Now, on the table characteristics

       cmsCalcL16Params(Tables[0] -> nEntries, &NewMatShaper -> p16);

       // Copy tables

       AllLinear = 0;
       for (i=0; i < 3; i++)
       {
        LPWORD PtrW;

        PtrW = (LPWORD) malloc(sizeof(WORD) * NewMatShaper -> p16.nSamples);

        if (PtrW == NULL) {
              cmsFreeMatShaper(NewMatShaper);
              return NULL;
        }

        CopyMemory(PtrW, Tables[i] -> GammaTable,
                            sizeof(WORD) * Tables[i] -> nEntries);

        NewMatShaper -> L[i] = PtrW;      // Set table pointer

        // Linear after all?

        AllLinear   += cmsIsLinear(PtrW, NewMatShaper -> p16.nSamples);
       }

       // If is all linear, then supress table interpolation (this
       // will speed greately some trivial operations

       if (AllLinear != 3)
              NewMatShaper -> dwFlags |= MATSHAPER_HASSHAPER;

       return NewMatShaper;
}


LPMATSHAPER cmsAllocMonoMatShaper(LPGAMMATABLE Tables[], DWORD Behaviour)
{
       LPMATSHAPER NewMatShaper;
       int i, AllLinear;

       NewMatShaper = (LPMATSHAPER) malloc(sizeof(MATSHAPER));
       if (NewMatShaper)
              ZeroMemory(NewMatShaper, sizeof(MATSHAPER));

       NewMatShaper->dwFlags = Behaviour & (MATSHAPER_ALLSMELTED);


       // Now, on the table characteristics

       cmsCalcL16Params(Tables[0] -> nEntries, &NewMatShaper -> p16);

       // Copy tables

       AllLinear = 0;
       for (i=0; i < 3; i++)
       {
        LPWORD PtrW;

        PtrW = (LPWORD) malloc(sizeof(WORD) * NewMatShaper -> p16.nSamples);

        if (PtrW == NULL) {
              cmsFreeMatShaper(NewMatShaper);
              return NULL;
        }

        CopyMemory(PtrW, Tables[i] -> GammaTable,
                            sizeof(WORD) * Tables[i] -> nEntries);

        NewMatShaper -> L[i] = PtrW;      // Set table pointer

        // Linear after all?

        AllLinear   += cmsIsLinear(PtrW, NewMatShaper -> p16.nSamples);
       }

       // If is all linear, then supress table interpolation (this
       // will speed greately some trivial operations

       if (AllLinear != 3)
              NewMatShaper -> dwFlags |= MATSHAPER_HASSHAPER;

       return NewMatShaper;
}



// Free associated memory

void cmsFreeMatShaper(LPMATSHAPER MatShaper)
{
       int i;

       if (!MatShaper) return;

       for (i=0; i < 3; i++)
       {
              if (MatShaper -> L[i]) free(MatShaper ->L[i]);
       }

       free(MatShaper);
}


// All smelted must postpose gamma to last stage.

static
void AllSmeltedBehaviour(LPMATSHAPER MatShaper, WORD In[], WORD Out[])
{

	   WORD tmp[3];


       if (MatShaper -> dwFlags & MATSHAPER_HASMATRIX)
       {       
			 WVEC3 InVect, OutVect;

		     InVect.n[VX] = ToFixedDomain(In[0]);
			 InVect.n[VY] = ToFixedDomain(In[1]);
			 InVect.n[VZ] = ToFixedDomain(In[2]);

             MAT3evalW(&OutVect, &MatShaper -> Matrix, &InVect);

			 tmp[0] = Clamp_XYZ(FromFixedDomain(OutVect.n[VX]));
			 tmp[1] = Clamp_XYZ(FromFixedDomain(OutVect.n[VY]));
			 tmp[2] = Clamp_XYZ(FromFixedDomain(OutVect.n[VZ]));

       }
       else
       {
			tmp[0] = In[0];
			tmp[1] = In[1];
			tmp[2] = In[2];
       }

	   
	   if (MatShaper -> dwFlags & MATSHAPER_HASSHAPER)
       {
       Out[0] = cmsLinearInterpLUT16(tmp[0], MatShaper -> L[0], &MatShaper -> p16);
       Out[1] = cmsLinearInterpLUT16(tmp[1], MatShaper -> L[1], &MatShaper -> p16);
       Out[2] = cmsLinearInterpLUT16(tmp[2], MatShaper -> L[2], &MatShaper -> p16);
       }
       else
	   {
		   Out[0] = tmp[0];
		   Out[1] = tmp[1];
		   Out[2] = tmp[2];
	   }
        
}


static
void InputBehaviour(LPMATSHAPER MatShaper, WORD In[], WORD Out[])
{
       WVEC3 InVect, OutVect;

       // This is an smelted process

       if (MatShaper -> dwFlags & MATSHAPER_HASSHAPER)
       {
       InVect.n[VX] = cmsLinearInterpFixed(In[0], MatShaper -> L[0], &MatShaper -> p16);
       InVect.n[VY] = cmsLinearInterpFixed(In[1], MatShaper -> L[1], &MatShaper -> p16);
       InVect.n[VZ] = cmsLinearInterpFixed(In[2], MatShaper -> L[2], &MatShaper -> p16);
       }
       else
       {
       InVect.n[VX] = ToFixedDomain(In[0]);
       InVect.n[VY] = ToFixedDomain(In[1]);
       InVect.n[VZ] = ToFixedDomain(In[2]);
       }

       if (MatShaper -> dwFlags & MATSHAPER_HASMATRIX)
       {
              MAT3evalW(&OutVect, &MatShaper -> Matrix, &InVect);
       }
       else
       {
       OutVect =  InVect;
       }

       // PCS in 1Fixed15 format, adjusting

       Out[0] = (WORD) Clamp_XYZ(OutVect.n[VX] >> 1);
       Out[1] = (WORD) Clamp_XYZ(OutVect.n[VY] >> 1);
       Out[2] = (WORD) Clamp_XYZ(OutVect.n[VZ] >> 1);

}


static
void OutputBehaviour(LPMATSHAPER MatShaper, WORD In[], WORD Out[])
{
       WVEC3 InVect, OutVect;
       int i;

       // We need to convert from XYZ to RGB, here we must
       // shift << 1 to pass between 1.15 to 15.16 formats

       InVect.n[VX] = (Fixed32) In[0] << 1;
       InVect.n[VY] = (Fixed32) In[1] << 1;
       InVect.n[VZ] = (Fixed32) In[2] << 1;

       if (MatShaper -> dwFlags & MATSHAPER_HASMATRIX)
       {
              MAT3evalW(&OutVect, &MatShaper -> Matrix, &InVect);
       }
       else
       {
       OutVect = InVect;
       }


       if (MatShaper -> dwFlags & MATSHAPER_HASSHAPER)
       {
              for (i=0; i < 3; i++)
              {

              Out[i] = cmsLinearInterpLUT16(
                     Clamp_RGB(FromFixedDomain(OutVect.n[i])),
                     MatShaper -> L[i],
                     &MatShaper ->p16);
              }
       }
       else
       {
       // Result from fixed domain to RGB

       Out[0] = Clamp_RGB(FromFixedDomain(OutVect.n[VX]));
       Out[1] = Clamp_RGB(FromFixedDomain(OutVect.n[VY]));
       Out[2] = Clamp_RGB(FromFixedDomain(OutVect.n[VZ]));
       }

}


// Master on evaluating shapers, 3 different behaviours

void cmsEvalMatShaper(LPMATSHAPER MatShaper, WORD In[], WORD Out[])
{

       if ((MatShaper -> dwFlags & MATSHAPER_ALLSMELTED) == MATSHAPER_ALLSMELTED)
       {
              AllSmeltedBehaviour(MatShaper, In, Out);
              return;
       }
       if (MatShaper -> dwFlags & MATSHAPER_INPUT)
       {
              InputBehaviour(MatShaper, In, Out);
              return;
       }

       OutputBehaviour(MatShaper, In, Out);
}
