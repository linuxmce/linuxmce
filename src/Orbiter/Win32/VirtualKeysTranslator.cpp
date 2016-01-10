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
#include "VirtualKeysTranslator.h"
#include "../pluto_main/Define_Button.h"
//-----------------------------------------------------------------------------------------------------
#ifdef WINCE
#define VK_OEM_1		0xBA
#define VK_OEM_PLUS		0xBB
#define VK_OEM_COMMA    0xBC
#define VK_OEM_MINUS	0xBD
#define VK_OEM_PERIOD	0xBE
#define VK_OEM_7		0xDE
#endif
//-----------------------------------------------------------------------------------------------------
bool TranslateVirtualKeys2PlutoKeys(UINT uMsg, WPARAM wParam, LPARAM lParam,
                               int& iPK_Button, bool& bShiftDown, bool& bAltDown, 
                               bool& bControlDown, bool& bCapsLock)
{
    static WPARAM wOldParam = 0;
    static bool bLastEvent_KeyDown = false;
	static bool bWindows_Down = false;

    // Prevent auto-repeat
    if(uMsg == WM_KEYDOWN && bLastEvent_KeyDown && wOldParam == wParam) 
        return false;

    if (uMsg == WM_KEYDOWN)
    {
        switch (wParam)
        {
            case VK_SHIFT:                      bShiftDown=true;              break;
            case VK_CONTROL:                    bControlDown=true;            break;
            case VK_MENU:                       bAltDown=true;                break;
            case VK_CAPITAL:                    bCapsLock = !bCapsLock;       break;
			case VK_LWIN:						bWindows_Down=true;			  break;
        }
    }

    if (uMsg == WM_KEYUP)
    {
        switch (wParam)
        {
            case VK_SHIFT:                      bShiftDown=false;              break;
            case VK_CONTROL:                    bControlDown=false;            break;
            case VK_MENU:                       bAltDown=false;                break;
			case VK_LWIN:						bWindows_Down=false;			   if( wOldParam==VK_LWIN ) iPK_Button = BUTTON_F1_CONST; break;  // On PDA's, if the user hits left windows and let's go, we treat it as F1
        }
    }

    wOldParam = wParam;
    bLastEvent_KeyDown = uMsg == WM_KEYDOWN; 

	switch (wParam)
    {
        case '0':       iPK_Button = BUTTON_0_CONST;     break;
        case '1':       iPK_Button = BUTTON_1_CONST;     break;
        case '2':       iPK_Button = BUTTON_2_CONST;     break;
        case '3':       iPK_Button = BUTTON_3_CONST;     break;
        case '4':       iPK_Button = BUTTON_4_CONST;     break;
        case '5':       iPK_Button = BUTTON_5_CONST;     break;
        case '6':       iPK_Button = BUTTON_6_CONST;     break;
        case '7':       iPK_Button = BUTTON_7_CONST;     break;
        case '8':       iPK_Button = BUTTON_8_CONST;     break;
        case '9':       iPK_Button = BUTTON_9_CONST;     break;
#ifndef WINCE
		case VK_OEM_2:		iPK_Button = BUTTON_slash_CONST; break;
#endif
        case VK_SPACE:  iPK_Button = BUTTON_space_CONST; break;
        case VK_ESCAPE:	iPK_Button = BUTTON_escape_CONST; break;
		case VK_PRIOR:	iPK_Button = BUTTON_Scroll_Up_CONST; break;
		case VK_NEXT:	iPK_Button = BUTTON_Scroll_Down_CONST; break;

#ifdef PHONEKEYS
        case VK_C:      iPK_Button = BUTTON_Phone_C_CONST;           break;
        case VK_P:      iPK_Button = BUTTON_Phone_Pencil_CONST;      break;
        case VK_T:      iPK_Button = BUTTON_Phone_Talk_CONST;        break;
        case VK_E:      iPK_Button = BUTTON_Phone_End_CONST;         break;
        case VK_L:      iPK_Button = BUTTON_Phone_Soft_left_CONST;   break;
        case VK_R:      iPK_Button = BUTTON_Phone_Soft_right_CONST;  break;
        case '*':       iPK_Button = BUTTON_Asterisk_CONST;          break;
        case '#':       iPK_Button = BUTTON_Pound_CONST;             break;
        case VK_W:      iPK_Button = BUTTON_Terminate_Text_CONST;    break;
#endif 

        case VK_F1:     iPK_Button = BUTTON_F1_CONST;            break;
        case VK_F2:     iPK_Button = BUTTON_F2_CONST;            break;
        case VK_F3:     iPK_Button = BUTTON_F3_CONST;            break;
        case VK_F4:     iPK_Button = BUTTON_F4_CONST;            break;
        case VK_F5:     iPK_Button = BUTTON_F5_CONST;            break;
        case VK_F6:     iPK_Button = BUTTON_F6_CONST;            break;
        case VK_F7:     iPK_Button = BUTTON_F7_CONST;            break;
        case VK_F8:     iPK_Button = BUTTON_F8_CONST;            break;
        case VK_F9:     iPK_Button = BUTTON_F9_CONST;            break;
        case VK_F10:    iPK_Button = BUTTON_F10_CONST;            break;
        case VK_UP:     iPK_Button = BUTTON_Up_Arrow_CONST;      break;
        case VK_DOWN:   iPK_Button = BUTTON_Down_Arrow_CONST;    break;
        case VK_LEFT:   iPK_Button = BUTTON_Left_Arrow_CONST;    break;
        case VK_RIGHT:  iPK_Button = BUTTON_Right_Arrow_CONST;   break;
        case VK_RETURN: iPK_Button = BUTTON_Enter_CONST;         break;
		case VK_BACK:   iPK_Button = BUTTON_Back_CONST;          break;
        case VK_SHIFT:  iPK_Button = BUTTON_left_shift_CONST;    break;

		case VK_OEM_MINUS:  iPK_Button = BUTTON_dash_CONST;    break;
		case VK_OEM_PLUS:  iPK_Button = BUTTON_equals_sign_CONST;    break;

		case VK_OEM_PERIOD:  iPK_Button = BUTTON_dot_CONST;    break;
		case VK_OEM_COMMA:  iPK_Button = BUTTON_comma_CONST;    break;
		case VK_OEM_1:  iPK_Button = BUTTON_semicolumn_CONST;    break;
		case VK_OEM_7:  iPK_Button = BUTTON_single_quote_CONST;    break;

		case VK_ADD: iPK_Button = BUTTON_dash_CONST;		 break;
		case VK_SUBTRACT: iPK_Button = BUTTON_plus_CONST;    break;
    }

    if( bShiftDown && !bControlDown && !bAltDown )
    {
        switch (wParam)
        {
            case VK_UP:     iPK_Button = BUTTON_Shift_Up_Arrow_CONST;				break;
            case VK_DOWN:   iPK_Button = BUTTON_Shift_Down_Arrow_CONST;				break;
            case VK_LEFT:   iPK_Button = BUTTON_Shift_Left_Arrow_CONST;				break;
            case VK_RIGHT:  iPK_Button = BUTTON_Shift_Right_Arrow_CONST;			break;

			case VK_OEM_MINUS:		iPK_Button = BUTTON_underscore_CONST;					break;
			//case VK_OEM_PLUS:		iPK_Button = BUTTON_equals_sign_CONST;					break;
			case VK_OEM_1:       iPK_Button = BUTTON_colon_CONST;						break;
			case VK_OEM_7:       iPK_Button = BUTTON_double_quote_CONST;					break;

			case '0':       iPK_Button = BUTTON_right_parenthesis_CONST;            break;
			case '1':       iPK_Button = BUTTON_exclamation_point_CONST;            break;
			case '2':       iPK_Button = BUTTON_at_sign_CONST;						break;
            case '3':       iPK_Button = BUTTON_Pound_CONST;						break;
            case '4':       iPK_Button = BUTTON_dollar_CONST;						break;
            case '5':       iPK_Button = BUTTON_percent_CONST;						break;
            case '6':       iPK_Button = BUTTON_caret_CONST;						break;
            case '7':       iPK_Button = BUTTON_ampersand_CONST;					break;
            case '8':       iPK_Button = BUTTON_Asterisk_CONST;						break;
            case '9':       iPK_Button = BUTTON_left_parenthesis_CONST;             break;
        }
    }

#ifndef PHONEKEYS
    if(wParam >= VK_A && wParam <= VK_Z && !bControlDown && !bAltDown) // A-Z keys
    {
		iPK_Button = BUTTON_a_CONST + int(wParam) - VK_A;
    }
#endif 

    switch (wParam)  // The 3 hard buttons on a PDA
    {
        case 0xC2:		iPK_Button = BUTTON_F2_CONST;    break;
        case 0xC3:		iPK_Button = BUTTON_F3_CONST;    break;
        case 0xC4:		iPK_Button = BUTTON_F4_CONST;    break;
    }

    return true;
}
//-----------------------------------------------------------------------------------------------------
