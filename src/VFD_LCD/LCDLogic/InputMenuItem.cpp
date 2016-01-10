#include "InputMenuItem.h"
#include "PlutoUtils/StringUtils.h"
//--------------------------------------------------------------------------------------------------------
#define EMPTY_CHAR '_'
#define MASK_CHAR  'x'
#define SPACE_CHAR ' '
//--------------------------------------------------------------------------------------------------------
InputMenuItem::InputMenuItem(string sDescription, MenuItem *pParent, ItemType type, MenuItemAction *pMenuItemAction, string sMask) :
	MenuItem(sDescription, pParent, type, pMenuItemAction),	
	m_sMask(sMask), m_nCaretIndex(0), m_nOptionIndex(0), m_csCaretOptions("_0123456789"), m_bBlinked(false)
{
	m_sDescription = m_sMask;
	m_sDescription = StringUtils::Replace(m_sDescription, string() + MASK_CHAR, string() + EMPTY_CHAR);
}
//--------------------------------------------------------------------------------------------------------
InputMenuItem::~InputMenuItem()
{
}
//--------------------------------------------------------------------------------------------------------
string InputMenuItem::Mask()
{
	return m_sMask;
}
//--------------------------------------------------------------------------------------------------------
string InputMenuItem::Value()
{
	m_sValue = m_sDescription;
	m_sValue = StringUtils::Replace(m_sValue, string() + EMPTY_CHAR, string() + SPACE_CHAR);
	return m_sValue;
}
//--------------------------------------------------------------------------------------------------------
string InputMenuItem::Description()
{
	string sDescription(m_sDescription);

	if(!m_sMask.empty() && m_sMask[m_nCaretIndex] == MASK_CHAR)
	{
		if(m_bBlinked)
			sDescription[m_nCaretIndex] = m_csCaretOptions[m_nOptionIndex];
		else
			if(sDescription[m_nCaretIndex] != EMPTY_CHAR)
				sDescription[m_nCaretIndex] = EMPTY_CHAR;
			else
				sDescription[m_nCaretIndex] = SPACE_CHAR;

		m_bBlinked = !m_bBlinked;
	}

	return sDescription;
}
//--------------------------------------------------------------------------------------------------------
void InputMenuItem::Value(string sValue)
{
	m_sValue = sValue;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoRight()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoLeft()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoUp()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
bool InputMenuItem::CanGoDown()
{
	return true;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveUp()
{
	if(!m_sMask.empty() && m_sMask[m_nCaretIndex] == MASK_CHAR)
	{
		m_nOptionIndex = (m_csCaretOptions.length() + m_nOptionIndex - 1) % m_csCaretOptions.length();

		if(!m_sDescription.empty())
			m_sDescription[m_nCaretIndex] = m_csCaretOptions[m_nOptionIndex];
	}

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveDown()
{
	if(!m_sMask.empty() && m_sMask[m_nCaretIndex] == MASK_CHAR)
	{
		m_nOptionIndex = (m_nOptionIndex + 1) % m_csCaretOptions.length();

		if(!m_sDescription.empty())
			m_sDescription[m_nCaretIndex] = m_csCaretOptions[m_nOptionIndex];
	}

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveRight()
{
	do
	{
		m_nCaretIndex = (m_nCaretIndex + 1) % m_sDescription.length();
	}
	while(!m_sMask.empty() && m_sMask[m_nCaretIndex] != MASK_CHAR);

	//set the right position
	size_t pos = m_csCaretOptions.find(m_sDescription[m_nCaretIndex]);
	if(pos != string::npos)
		m_nOptionIndex = pos;
	else
		m_nOptionIndex = 0;

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *InputMenuItem::MoveLeft()
{
	if(m_nCaretIndex > 0)
	{
		do
		{
			m_nCaretIndex = (m_sDescription.length() + m_nCaretIndex - 1) % m_sDescription.length();
		}
		while(!m_sMask.empty() && m_sMask[m_nCaretIndex] != MASK_CHAR);

		//set the right position
		size_t pos = m_csCaretOptions.find(m_sDescription[m_nCaretIndex]);
		if(pos != string::npos)
			m_nOptionIndex = pos;
		else
			m_nOptionIndex = 0;

		return this;
	}

	return MenuItem::MoveLeft();
}
//--------------------------------------------------------------------------------------------------------
