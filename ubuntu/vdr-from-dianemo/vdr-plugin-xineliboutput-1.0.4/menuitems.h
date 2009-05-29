/*
 * menuitems.h: New menu item types
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menuitems.h,v 1.7 2007/06/21 10:00:29 phintuka Exp $
 *
 */

#ifndef __XINELIB_MENUITEMS_H_
#define __XINELIB_MENUITEMS_H_

#include <vdr/menuitems.h>

// --- cMenuEditTypedIntItem -------------------------------------------------

class cMenuEditTypedIntItem : public cMenuEditIntItem 
{
  protected:
    cString type;
    cString zeroString;

    virtual void Set(void);

  public:
    cMenuEditTypedIntItem(const char *Name, const char *Type, int *Value, 
			  int Min = 0, int Max = INT_MAX, const char *ZeroString = NULL,
			  const char *MinString = NULL, const char *MaxString = NULL);
};

// --- cMenuEditOddIntItem -------------------------------------------------

class cMenuEditOddIntItem : public cMenuEditIntItem
{
  public:
    cMenuEditOddIntItem(const char *Name, int *Value, int Min = 1, int Max = INT_MAX, const char *MinString = NULL, const char *MaxString = NULL);
    eOSState ProcessKey(eKeys Key);
};


// --- cMenuEditFpIntItem -------------------------------------------------

// Fixed-point decimal number

class cMenuEditFpIntItem : public cMenuEditIntItem
{
  protected:
    int decimals;
    cString zeroString;

    virtual void Set(void);

  public:
    cMenuEditFpIntItem(const char *Name, int *Value, int Min = 1, int Max = INT_MAX,
                       int Decimals = 1, const char *ZeroString = NULL,
                       const char *MinString = NULL, const char *MaxString = NULL);
};


// --- cMenuEditStraI18nItem -------------------------------------------------

class cMenuEditStraI18nItem : public cMenuEditIntItem 
{
  private:
    const char * const *strings;

  protected:
    virtual void Set(void);

  public:
    cMenuEditStraI18nItem(const char *Name, int *Value, 
			  int NumStrings, const char * const *Strings);
};

// --- cFileListItem ---------------------------------------------------------

class cFileListItem : public cOsdItem 
{
  private:
    cString m_Name;
    cString m_SubFile;
    bool  m_IsDir, m_HasResume, m_ShowFlags, m_Up;
    bool  m_IsDvd;

  protected:
    virtual void Set(void);

  public:
    cFileListItem(const char *name, bool isDir, 
		  bool HasResume, const char *subfile,
		  bool IsDvd = false);
    cFileListItem(const char *name, bool isDir);

    const char *Name(void)    { return m_Name; }
    const char *SubFile(void) { return m_SubFile; }
    bool IsDir(void)       { return m_IsDir; }
    bool IsDvd(void)       { return m_IsDvd; }

    virtual bool operator< (const cListObject &ListObject);
    virtual int Compare(const cListObject &ListObject) const;
};

#endif //__XINELIB_MENUITEMS_H_
