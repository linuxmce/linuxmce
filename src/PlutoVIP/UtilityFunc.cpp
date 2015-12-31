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
#include "stdafx.h"
#include "UtilityFunc.h"


typedef DWORD (WINAPI *PSLWA)(HWND, DWORD, BYTE, DWORD);

static PSLWA pSetLayeredWindowAttributes = NULL;
static BOOL initialized = FALSE;


BOOL MakeWindowTransparent(HWND hWnd, unsigned char factor)
{
   /* First, see if we can get the API call we need. If we've tried
    * once, we don't need to try again. */
   if (!initialized)
   {
      HMODULE hDLL = LoadLibrary ("user32");

      pSetLayeredWindowAttributes = 
         (PSLWA) GetProcAddress(hDLL, "SetLayeredWindowAttributes");

      initialized = TRUE;
   }

   if (pSetLayeredWindowAttributes == NULL) 
      return FALSE;

   /* Windows need to be layered to be made transparent. This is done
    * by modifying the extended style bits to contain WS_EX_LAYARED. */
   SetLastError(0);

   SetWindowLong(hWnd, 
                 GWL_EXSTYLE , 
                 GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

   if (GetLastError())
      return FALSE;

   /* Now, we need to set the 'layered window attributes'. This
    * is where the alpha values get set. */
   return pSetLayeredWindowAttributes (hWnd, 
                                       RGB(255,255,255), 
                                       factor,
                                       LWA_COLORKEY|LWA_ALPHA);
}

