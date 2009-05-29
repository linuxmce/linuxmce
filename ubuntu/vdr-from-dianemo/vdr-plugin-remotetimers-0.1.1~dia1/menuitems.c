/*
 * menuitems.c: General purpose menu items
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menuitems.c 1.58 2008/02/10 16:03:30 kls Exp $
 */

#include "menuitems.h"
#include <ctype.h>
#if VDRVERSNUM >10503
#include <wctype.h>
#endif
#include <stdlib.h>
#include <vdr/remote.h>

namespace PluginRemoteTimers {

// --- cMenuEditUserItem ------------------------------------------------------

bool OneBitSet(int Bits)
{
  for (int mask = USER_MASK(1); mask <= USER_MASK(MAX_USER); mask <<= 1) {
     if ((Bits & mask) == mask)
        return (Bits & mask) == Bits;
  }
  return false;
}

cMenuEditUserItem::cMenuEditUserItem(const char *Name, int *Value, const char *NegativeString)
:cMenuEditItem(Name)
{
  value = Value;
  negativeString = NegativeString;
  Set();
}

int cMenuEditUserItem::Parse(const char *s)
{
  int result = 0;
  const char *p = s;
  while (p && *p) {
     int i = strtol(p, (char**) &p, 10);
     if (i <= 0)	// also returns 0 on error
        return i;
     else if (i <= MAX_USER)
        result |= USER_MASK(i);
     while (*p == ',')
        p++;
  }
  return result;
}

cString cMenuEditUserItem::ToString(int Value)
{
  if (Value <= 0)
     return cString::sprintf("%d", Value);

  char buffer[MAX_USER * 2];
  char *p = buffer;
  for (int i = 1; i <= MAX_USER; i++) {
     if (Value & USER_MASK(i)) {
	if (p != buffer)
	   *p++ = ',';
        *p++ = '0' + i;
     }
  }
  *p = 0;
  return cString(buffer);
}

void cMenuEditUserItem::Set()
{
  if (*value < 0 && negativeString)
  {
     SetValue(negativeString);
  }
  else if (*value == 0)
  {
     SetValue("0");
  }
  else
  {
     char buffer[] = USER_BUFFER;
     int mask = USER_MASK(1);
     for (char *p = buffer; *p; p++) {
        if ((*value & mask) == 0)
	   *p = '-';
	mask <<= 1;
     }
     buffer[MAX_USER] = 0;
     SetValue(buffer);
  }
}

eOSState cMenuEditUserItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) {
     int newValue = *value;
     Key = NORMALKEY(Key);
     switch (Key) {
        case k0:
	   newValue = 0;
	   break;
	case k1 ... k9:
	   if (newValue < 0)
	      newValue = 0;
	   if (Key - k0 <= MAX_USER)
	      newValue ^= USER_MASK(Key - k0);
	   break;
	case kLeft:
	   if (newValue <= 0 && negativeString)
	      newValue = -1;
	   else if (newValue == USER_MASK_ALL)
	      newValue = USER_MASK(MAX_USER);
	   else if (OneBitSet(newValue))
	      newValue >>= 1;
	   else
	      newValue = 0;
	   break;
	case kRight:
	   if (newValue < 0)
	      newValue = 0;
	   else if (newValue == 0) 
	      newValue = USER_MASK(1);
	   else if (OneBitSet(newValue) && newValue != USER_MASK(MAX_USER))
	      newValue <<= 1;
	   else
	      newValue = USER_MASK_ALL;
	   break;
	default:
	   return state;
     }
     if (newValue != -1)
        newValue &= USER_MASK_ALL;
     if (newValue != *value) {
        *value = newValue;
	Set();
     }
     state = osContinue;
  }
  return state;
}

// --- cMenuEditPathItem ------------------------------------------------------

#define AUTO_ADVANCE_TIMEOUT  1500 // ms before auto advance when entering characters via numeric keys

#if VDRVERSNUM >10503

cMenuEditPathItem::cMenuEditPathItem(const char *Name, char *Value, int Length, const char *Allowed)
:cMenuEditItem(Name)
{
  value = Value;
  length = Length;
  allowed = Allowed ? Allowed : tr(FileNameChars);
  pos = -1;
  offset = 0;
  insert = uppercase = false;
  newchar = true;
  lengthUtf8 = 0;
  valueUtf8 = NULL;
  allowedUtf8 = NULL;
  charMapUtf8 = NULL;
  currentCharUtf8 = NULL;
  lastKey = kNone;
  Set();
}

cMenuEditPathItem::~cMenuEditPathItem()
{
  delete valueUtf8;
  delete allowedUtf8;
  delete charMapUtf8;
}

void cMenuEditPathItem::EnterEditMode(void)
{
  if (!valueUtf8) {
     valueUtf8 = new uint[length];
     lengthUtf8 = Utf8ToArray(value, valueUtf8, length);
     int l = strlen(allowed) + 1;
     allowedUtf8 = new uint[l];
     Utf8ToArray(allowed, allowedUtf8, l);
     const char *charMap = tr("CharMap$ 0\t-.,1#~\\^$[]|()*+?{}/:%@&\tabc2\tdef3\tghi4\tjkl5\tmno6\tpqrs7\ttuv8\twxyz9");
     l = strlen(charMap) + 1;
     charMapUtf8 = new uint[l];
     Utf8ToArray(charMap, charMapUtf8, l);
     currentCharUtf8 = charMapUtf8;
     AdvancePos();
     }
}

void cMenuEditPathItem::LeaveEditMode(bool SaveValue)
{
  if (valueUtf8) {
     if (SaveValue) {
        Utf8FromArray(valueUtf8, value, length);
        //stripspace(value);
	char *p = value + strlen(value) - 1;
	// strip trailing ~
	while (p >= value && *p == '~')
	   *(p--) = 0;
	// strip trailing whitespace unless it's the last char of a path segment
	while (p > value && isspace(*p) && *(p-1) != '~')
	   *(p--) = 0;
        }
     lengthUtf8 = 0;
     delete[] valueUtf8;
     valueUtf8 = NULL;
     delete[] allowedUtf8;
     allowedUtf8 = NULL;
     delete[] charMapUtf8;
     charMapUtf8 = NULL;
     pos = -1;
     offset = 0;
     newchar = true;
     }
}

void cMenuEditPathItem::SetHelpKeys(void)
{
  if (InEditMode())
     cSkinDisplay::Current()->SetButtons(tr("Button$ABC/abc"), insert ? tr("Button$Overwrite") : tr("Button$Insert"), tr("Button$Delete"));
  else
     cSkinDisplay::Current()->SetButtons(NULL);
}

uint *cMenuEditPathItem::IsAllowed(uint c)
{
  if (allowedUtf8) {
     for (uint *a = allowedUtf8; *a; a++) {
         if (c == *a)
            return a;
         }
     }
  return NULL;
}

void cMenuEditPathItem::AdvancePos(void)
{
  if (pos < length - 2 && pos < lengthUtf8) {
     if (++pos >= lengthUtf8) {
        if (pos >= 2 && valueUtf8[pos - 1] == ' ' && valueUtf8[pos - 2] == ' ')
           pos--; // allow only two blanks at the end
        else {
           valueUtf8[pos] = ' ';
           valueUtf8[pos + 1] = 0;
           lengthUtf8++;
           }
        }
     }
  newchar = true;
  if (!insert && Utf8is(alpha, valueUtf8[pos]))
     uppercase = Utf8is(upper, valueUtf8[pos]);
}

void cMenuEditPathItem::Set(void)
{
  if (InEditMode()) {
     // This is an ugly hack to make editing strings work with the 'skincurses' plugin.
     const cFont *font = dynamic_cast<cSkinDisplayMenu *>(cSkinDisplay::Current())->GetTextAreaFont(false);
     if (!font || font->Width("W") != 1) // all characters have with == 1 in the font used by 'skincurses'
        font = cFont::GetFont(fontOsd);

     int width = cSkinDisplay::Current()->EditableWidth();
     width -= font->Width("[]");
     width -= font->Width("<>"); // reserving this anyway make the whole thing simpler

     if (pos < offset)
        offset = pos;
     int WidthFromOffset = 0;
     int EndPos = lengthUtf8;
     for (int i = offset; i < lengthUtf8; i++) {
         WidthFromOffset += font->Width(valueUtf8[i]);
         if (WidthFromOffset > width) {
            if (pos >= i) {
               do {
                  WidthFromOffset -= font->Width(valueUtf8[offset]);
                  offset++;
                  } while (WidthFromOffset > width && offset < pos);
               EndPos = pos + 1;
               }
            else {
               EndPos = i;
               break;
               }
            }
         }

     char buf[1000];
     char *p = buf;
     if (offset)
        *p++ = '<';
     p += Utf8FromArray(valueUtf8 + offset, p, sizeof(buf) - (p - buf), pos - offset);
     *p++ = '[';
     if (insert && newchar)
        *p++ = ']';
     p += Utf8FromArray(&valueUtf8[pos], p, sizeof(buf) - (p - buf), 1);
     if (!(insert && newchar))
        *p++ = ']';
     p += Utf8FromArray(&valueUtf8[pos + 1], p, sizeof(buf) - (p - buf), EndPos - pos - 1);
     if (EndPos != lengthUtf8)
        *p++ = '>';
     *p = 0;

     SetValue(buf);
     }
  else
     SetValue(value);
}

uint cMenuEditPathItem::Inc(uint c, bool Up)
{
  uint *p = IsAllowed(c);
  if (!p)
     p = allowedUtf8;
  if (Up) {
     if (!*++p)
        p = allowedUtf8;
     }
  else if (--p < allowedUtf8) {
     p = allowedUtf8;
     while (*p && *(p + 1))
           p++;
     }
  return *p;
}

void cMenuEditPathItem::Insert(void)
{
  memmove(valueUtf8 + pos + 1, valueUtf8 + pos, (lengthUtf8 - pos + 1) * sizeof(*valueUtf8));
  lengthUtf8++;
  valueUtf8[pos] = ' ';
}

void cMenuEditPathItem::Delete(void)
{
  memmove(valueUtf8 + pos, valueUtf8 + pos + 1, (lengthUtf8 - pos) * sizeof(*valueUtf8));
  lengthUtf8--;
}

eOSState cMenuEditPathItem::ProcessKey(eKeys Key)
{
  bool SameKey = NORMALKEY(Key) == lastKey;
  if (Key != kNone)
     lastKey = NORMALKEY(Key);
  else if (!newchar && k0 <= lastKey && lastKey <= k9 && autoAdvanceTimeout.TimedOut()) {
     AdvancePos();
     newchar = true;
     currentCharUtf8 = NULL;
     Set();
     return osContinue;
     }
  switch (Key) {
    case kRed:   // Switch between upper- and lowercase characters
                 if (InEditMode()) {
                    if (!insert || !newchar) {
                       uppercase = !uppercase;
                       valueUtf8[pos] = uppercase ? Utf8to(upper, valueUtf8[pos]) : Utf8to(lower, valueUtf8[pos]);
                       }
                    }
                 else
                    return osUnknown;
                 break;
    case kGreen: // Toggle insert/overwrite modes
                 if (InEditMode()) {
                    insert = !insert;
                    newchar = true;
                    SetHelpKeys();
                    }
                 else
                    return osUnknown;
                 break;
    case kYellow|k_Repeat:
    case kYellow: // Remove the character at the current position; in insert mode it is the character to the right of the cursor
                 if (InEditMode()) {
                    if (lengthUtf8 > 1) {
                       if (!insert || pos < lengthUtf8 - 1)
                          Delete();
                       else if (insert && pos == lengthUtf8 - 1)
                          valueUtf8[pos] = ' '; // in insert mode, deleting the last character replaces it with a blank to keep the cursor position
                       // reduce position, if we removed the last character
                       if (pos == lengthUtf8)
                          pos--;
                       }
                    else if (lengthUtf8 == 1)
                       valueUtf8[0] = ' '; // This is the last character in the string, replace it with a blank
                    if (Utf8is(alpha, valueUtf8[pos]))
                       uppercase = Utf8is(upper, valueUtf8[pos]);
                    newchar = true;
                    }
                 else
                    return osUnknown;
                 break;
    case kBlue|k_Repeat:
    case kBlue:  // consume the key only if in edit-mode
                 if (!InEditMode())
                    return osUnknown;
                 break;
    case kLeft|k_Repeat:
    case kLeft:  if (pos > 0) {
                    if (!insert || newchar)
                       pos--;
                    newchar = true;
                    if (!insert && Utf8is(alpha, valueUtf8[pos]))
                       uppercase = Utf8is(upper, valueUtf8[pos]);
                    }
                 break;
    case kRight|k_Repeat:
    case kRight: if (InEditMode())
                    AdvancePos();
                 else {
                    EnterEditMode();
                    SetHelpKeys();
                    }
                 break;
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:  if (InEditMode()) {
                    if (insert && newchar) {
                       // create a new character in insert mode
                       if (lengthUtf8 < length - 1)
                          Insert();
                       }
                    if (uppercase)
                       valueUtf8[pos] = Utf8to(upper, Inc(Utf8to(lower, valueUtf8[pos]), NORMALKEY(Key) == kUp));
                    else
                       valueUtf8[pos] =               Inc(              valueUtf8[pos],  NORMALKEY(Key) == kUp);
                    newchar = false;
                    }
                 else
                    return cMenuEditItem::ProcessKey(Key);
                 break;
    case k0|k_Repeat ... k9|k_Repeat:
    case k0 ... k9: {
                 if (InEditMode()) {
                    if (!SameKey) {
                       if (!newchar)
                          AdvancePos();
                       currentCharUtf8 = NULL;
                       }
                    if (!currentCharUtf8 || !*currentCharUtf8 || *currentCharUtf8 == '\t') {
                       // find the beginning of the character map entry for Key
                       int n = NORMALKEY(Key) - k0;
                       currentCharUtf8 = charMapUtf8;
                       while (n > 0 && *currentCharUtf8) {
                             if (*currentCharUtf8++ == '\t')
                                n--;
                             }
                       // find first allowed character
                       while (*currentCharUtf8 && *currentCharUtf8 != '\t' && !IsAllowed(*currentCharUtf8))
                             currentCharUtf8++;
                       }
                    if (*currentCharUtf8 && *currentCharUtf8 != '\t') {
                       if (insert && newchar) {
                          // create a new character in insert mode
                          if (lengthUtf8 < length - 1)
                             Insert();
                          }
                       valueUtf8[pos] = *currentCharUtf8;
                       if (uppercase)
                          valueUtf8[pos] = Utf8to(upper, valueUtf8[pos]);
                       // find next allowed character
                       do {
                          currentCharUtf8++;
                          } while (*currentCharUtf8 && *currentCharUtf8 != '\t' && !IsAllowed(*currentCharUtf8));
                       newchar = false;
                       autoAdvanceTimeout.Set(AUTO_ADVANCE_TIMEOUT);
                       }
                    }
                 else
                    return cMenuEditItem::ProcessKey(Key);
                 }
                 break;
    case kBack:
    case kOk:    if (InEditMode()) {
                    LeaveEditMode(Key == kOk);
                    SetHelpKeys();
                    break;
                    }
                 // run into default
    default:     if (InEditMode() && BASICKEY(Key) == kKbd) {
                    int c = KEYKBD(Key);
                    if (c <= 0xFF) { // FIXME what about other UTF-8 characters?
                       uint *p = IsAllowed(Utf8to(lower, c));
                       if (p) {
                          if (insert && lengthUtf8 < length - 1)
                             Insert();
                          valueUtf8[pos] = c;
                          if (pos < length - 2)
                             pos++;
                          if (pos >= lengthUtf8) {
                             valueUtf8[pos] = ' ';
                             valueUtf8[pos + 1] = 0;
                             lengthUtf8 = pos + 1;
                             }
                          }
                       else {
                          switch (c) {
                            case 0x7F: // backspace
                                       if (pos > 0) {
                                          pos--;
                                          return ProcessKey(kYellow);
                                          }
                                       break;
                            }
                          }
                       }
                    else {
                       switch (c) {
                         case kfHome: pos = 0; break;
                         case kfEnd:  pos = lengthUtf8 - 1; break;
                         case kfIns:  return ProcessKey(kGreen);
                         case kfDel:  return ProcessKey(kYellow);
                         }
                       }
                    }
                 else
                    return cMenuEditItem::ProcessKey(Key);
    }
  Set();
  return osContinue;
}

#else

cMenuEditPathItem::cMenuEditPathItem(const char *Name, char *Value, int Length, const char *Allowed)
:cMenuEditItem(Name)
{
  orgValue = NULL;
  value = Value;
  length = Length;
  allowed = strdup(Allowed ? Allowed : "");
  pos = -1;
  insert = uppercase = false;
  newchar = true;
  charMap = tr(" 0\t-.#~,/_@1\tabc2\tdef3\tghi4\tjkl5\tmno6\tpqrs7\ttuv8\twxyz9");
  currentChar = NULL;
  lastKey = kNone;
  Set();
}

cMenuEditPathItem::~cMenuEditPathItem()
{
  free(orgValue);
  free(allowed);
}

void cMenuEditPathItem::SetHelpKeys(void)
{
  if (InEditMode())
     cSkinDisplay::Current()->SetButtons(tr("Button$ABC/abc"), tr(insert ? "Button$Overwrite" : "Button$Insert"), tr("Button$Delete"));
  else
     cSkinDisplay::Current()->SetButtons(NULL);
}

void cMenuEditPathItem::AdvancePos(void)
{
  if (pos < length - 2 && pos < int(strlen(value)) ) {
     if (++pos >= int(strlen(value))) {
        if (pos >= 2 && value[pos - 1] == ' ' && value[pos - 2] == ' ')
           pos--; // allow only two blanks at the end
        else {
           value[pos] = ' ';
           value[pos + 1] = 0;
           }
        }
     }
  newchar = true;
  if (!insert && isalpha(value[pos]))
     uppercase = isupper(value[pos]);
}

void cMenuEditPathItem::Set(void)
{
  char buf[1000];

  if (InEditMode()) {
     // This is an ugly hack to make editing strings work with the 'skincurses' plugin.
     const cFont *font = dynamic_cast<cSkinDisplayMenu *>(cSkinDisplay::Current())->GetTextAreaFont(false);
     if (!font || font->Width("W") != 1) // all characters have with == 1 in the font used by 'skincurses'
        font = cFont::GetFont(fontOsd);
     strncpy(buf, value, pos);
     snprintf(buf + pos, sizeof(buf) - pos - 2, insert && newchar ? "[]%c%s" : "[%c]%s", *(value + pos), value + pos + 1);
     int width = cSkinDisplay::Current()->EditableWidth();
     if (font->Width(buf) <= width) {
        // the whole buffer fits on the screen
        SetValue(buf);
        return;
        }
     width -= font->Width('>'); // assuming '<' and '>' have the same width
     int w = 0;
     int i = 0;
     int l = strlen(buf);
     while (i < l && w <= width)
           w += font->Width(buf[i++]);
     if (i >= pos + 4) {
        // the cursor fits on the screen
        buf[i - 1] = '>';
        buf[i] = 0;
        SetValue(buf);
        return;
        }
     // the cursor doesn't fit on the screen
     w = 0;
     if (buf[i = pos + 3]) {
        buf[i] = '>';
        buf[i + 1] = 0;
        }
     else
        i--;
     while (i >= 0 && w <= width)
           w += font->Width(buf[i--]);
     buf[++i] = '<';
     SetValue(buf + i);
     }
  else
     SetValue(value);
}

char cMenuEditPathItem::Inc(char c, bool Up)
{
  const char *p = strchr(allowed, c);
  if (!p)
     p = allowed;
  if (Up) {
     if (!*++p)
        p = allowed;
     }
  else if (--p < allowed)
     p = allowed + strlen(allowed) - 1;
  return *p;
}

eOSState cMenuEditPathItem::ProcessKey(eKeys Key)
{
  bool SameKey = NORMALKEY(Key) == lastKey;
  if (Key != kNone)
     lastKey = NORMALKEY(Key);
  else if (!newchar && k0 <= lastKey && lastKey <= k9 && autoAdvanceTimeout.TimedOut()) {
     AdvancePos();
     newchar = true;
     currentChar = NULL;
     Set();
     return osContinue;
     }
  switch (Key) {
    case kRed:   // Switch between upper- and lowercase characters
                 if (InEditMode()) {
                    if (!insert || !newchar) {
                       uppercase = !uppercase;
                       value[pos] = uppercase ? toupper(value[pos]) : tolower(value[pos]);
                       }
                    }
                 else
                    return osUnknown;
                 break;
    case kGreen: // Toggle insert/overwrite modes
                 if (InEditMode()) {
                    insert = !insert;
                    newchar = true;
                    SetHelpKeys();
                    }
                 else
                    return osUnknown;
                 break;
    case kYellow|k_Repeat:
    case kYellow: // Remove the character at current position; in insert mode it is the character to the right of cursor
                 if (InEditMode()) {
                    if (strlen(value) > 1) {
                       if (!insert || pos < int(strlen(value)) - 1)
                          memmove(value + pos, value + pos + 1, strlen(value) - pos);
                       else if (insert && pos == int(strlen(value)) - 1)
                          value[pos] = ' '; // in insert mode, deleting the last character replaces it with a blank to keep the cursor position
                       // reduce position, if we removed the last character
                       if (pos == int(strlen(value)))
                          pos--;
                       }
                    else if (strlen(value) == 1)
                       value[0] = ' '; // This is the last character in the string, replace it with a blank
                    if (isalpha(value[pos]))
                       uppercase = isupper(value[pos]);
                    newchar = true;
                    }
                 else
                    return osUnknown;
                 break;
    case kBlue|k_Repeat:
    case kBlue:  // consume the key only if in edit-mode
                 if (InEditMode())
                    ;
                 else
                    return osUnknown;
                 break;
    case kLeft|k_Repeat:
    case kLeft:  if (pos > 0) {
                    if (!insert || newchar)
                       pos--;
                    newchar = true;
                    }
                 if (!insert && isalpha(value[pos]))
                    uppercase = isupper(value[pos]);
                 break;
    case kRight|k_Repeat:
    case kRight: AdvancePos();
                 if (pos == 0) {
                    orgValue = strdup(value);
                    SetHelpKeys();
                    }
                 break;
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:  if (InEditMode()) {
                    if (insert && newchar) {
                       // create a new character in insert mode
                       if (int(strlen(value)) < length - 1) {
                          memmove(value + pos + 1, value + pos, strlen(value) - pos + 1);
                          value[pos] = ' ';
                          }
                       }
                    if (uppercase)
                       value[pos] = toupper(Inc(tolower(value[pos]), NORMALKEY(Key) == kUp));
                    else
                       value[pos] =         Inc(        value[pos],  NORMALKEY(Key) == kUp);
                    newchar = false;
                    }
                 else
                    return cMenuEditItem::ProcessKey(Key);
                 break;
    case k0|k_Repeat ... k9|k_Repeat:
    case k0 ... k9: {
                 if (InEditMode()) {
                    if (!SameKey) {
                       if (!newchar)
                          AdvancePos();
                       currentChar = NULL;
                       }
                    if (!currentChar || !*currentChar || *currentChar == '\t') {
                       // find the beginning of the character map entry for Key
                       int n = NORMALKEY(Key) - k0;
                       currentChar = charMap;
                       while (n > 0 && *currentChar) {
                             if (*currentChar++ == '\t')
                                n--;
                             }
                       // find first allowed character
                       while (*currentChar && *currentChar != '\t' && !strchr(allowed, *currentChar))
                             currentChar++;
                       }
                    if (*currentChar && *currentChar != '\t') {
                       if (insert && newchar) {
                          // create a new character in insert mode
                          if (int(strlen(value)) < length - 1) {
                             memmove(value + pos + 1, value + pos, strlen(value) - pos + 1);
                             value[pos] = ' ';
                             }
                          }
                       value[pos] = *currentChar;
                       if (uppercase)
                          value[pos] = toupper(value[pos]);
                       // find next allowed character
                       do {
                          currentChar++;
                          } while (*currentChar && *currentChar != '\t' && !strchr(allowed, *currentChar));
                       newchar = false;
                       autoAdvanceTimeout.Set(AUTO_ADVANCE_TIMEOUT);
                       }
                    }
                 else
                    return cMenuEditItem::ProcessKey(Key);
                 }
                 break;
    case kBack:
    case kOk:    if (InEditMode()) {
                    if (Key == kBack && orgValue) {
                       strcpy(value, orgValue);
                       free(orgValue);
                       orgValue = NULL;
                       }
                    pos = -1;
                    newchar = true;
                    //stripspace(value);
	            for (char * p = value + strlen(value) - 1; p >= value && *p == '~'; --p)
	               *p = 0;
	            char *p = value + strlen(value) - 1;
	            // strip trailing ~
	            while (p >= value && *p == '~')
	               *(p--) = 0;
	            // strip trailing whitespace unless it's the last char of a path segment
	            while (p > value && isspace(*p) && *(p-1) != '~')
	               *(p--) = 0;
                    SetHelpKeys();
                    break;
                    }
                 // run into default
    default:     if (InEditMode() && BASICKEY(Key) == kKbd) {
                    int c = KEYKBD(Key);
                    if (c <= 0xFF) {
                       const char *p = strchr(allowed, tolower(c));
                       if (p) {
                          int l = strlen(value);
                          if (insert && l < length - 1)
                             memmove(value + pos + 1, value + pos, l - pos + 1);
                          value[pos] = c;
                          if (pos < length - 2)
                             pos++;
                          if (pos >= l) {
                             value[pos] = ' ';
                             value[pos + 1] = 0;
                             }
                          }
                       else {
                          switch (c) {
                            case 0x7F: // backspace
                                       if (pos > 0) {
                                          pos--;
                                          return ProcessKey(kYellow);
                                          }
                                       break;
                            }
                          }
                       }
                    else {
                       switch (c) {
                         case kfHome: pos = 0; break;
                         case kfEnd:  pos = strlen(value) - 1; break;
                         case kfIns:  return ProcessKey(kGreen);
                         case kfDel:  return ProcessKey(kYellow);
                         }
                       }
                    }
                 else
                    return cMenuEditItem::ProcessKey(Key);
    }
  Set();
  return osContinue;
}

#endif

}; // namespace PluginRemoteTimers
