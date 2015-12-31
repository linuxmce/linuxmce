#include "MenuItem.h"
#include "MenuItemAction.h"

#include <algorithm>

#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
MenuItem::MenuItem(string sDescription, MenuItem *pParent, ItemType type, MenuItemAction *pMenuItemAction) : 
	m_type(type), m_sDescription(sDescription), m_pParent(pParent), m_pMenuItemAction(pMenuItemAction)
{
	if(NULL == m_pMenuItemAction)
		m_pMenuItemAction = new MenuItemAction("", atNone);
}
//--------------------------------------------------------------------------------------------------------
MenuItem::~MenuItem()
{
	for(vector<MenuItem *>::iterator it = m_vectChildren.begin(), end = m_vectChildren.end(); it != end; ++it)
        delete *it;

	delete m_pMenuItemAction;
}
//--------------------------------------------------------------------------------------------------------
string MenuItem::Value()
{
	return m_sValue;
}
//--------------------------------------------------------------------------------------------------------
void MenuItem::Value(string sValue)
{
	m_sValue = sValue;
}
//--------------------------------------------------------------------------------------------------------
string MenuItem::Description()
{
	return m_sDescription;
}
//--------------------------------------------------------------------------------------------------------
MenuItemAction *MenuItem::Action()
{
	return m_pMenuItemAction;
}
//--------------------------------------------------------------------------------------------------------
ItemType MenuItem::Type()
{
	return m_type;
}
//--------------------------------------------------------------------------------------------------------
void MenuItem::Status(string sStatus)
{
	m_sStatus = sStatus;
}
//--------------------------------------------------------------------------------------------------------
string MenuItem::Status()
{
	return m_sStatus;
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoRight()
{
	return !m_vectChildren.empty();
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoLeft()
{
	return NULL != m_pParent;
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoUp()
{
	return NULL != m_pParent && m_pParent->PrevChild(this);
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::CanGoDown()
{
	return NULL != m_pParent && m_pParent->NextChild(this);
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuItem::Parent()
{
	return m_pParent;
}
//--------------------------------------------------------------------------------------------------------
void MenuItem::Parent(MenuItem *pParent)
{
	m_pParent = pParent;
}
//--------------------------------------------------------------------------------------------------------
const vector<MenuItem *>& MenuItem::Children() const
{
	return m_vectChildren;
}
//--------------------------------------------------------------------------------------------------------
void MenuItem::AddChild(MenuItem *pMenuItem)
{
	if(NULL != pMenuItem)
	{
		m_vectChildren.push_back(pMenuItem);
		pMenuItem->Parent(this);
	}
}
//--------------------------------------------------------------------------------------------------------
MenuItem* MenuItem::FirstChild()
{
	if(!m_vectChildren.empty())
		return m_vectChildren[0];

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
MenuItem* MenuItem::NextChild(MenuItem *pChildMenuItem)
{
	vector<MenuItem *>::iterator it = std::find(m_vectChildren.begin(), m_vectChildren.end(), pChildMenuItem);

	if(it == m_vectChildren.end())
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "%p/%s is not child for %p/%s",
			pChildMenuItem, pChildMenuItem->Description().c_str(),
			this, Description().c_str()
		);

		return NULL;
	}

	++it;

	if(it == m_vectChildren.end())
		return NULL;

	return *it;
}
//--------------------------------------------------------------------------------------------------------
MenuItem* MenuItem::PrevChild(MenuItem *pChildMenuItem)
{
	vector<MenuItem *>::iterator it = std::find(m_vectChildren.begin(), m_vectChildren.end(), pChildMenuItem);

	if(it == m_vectChildren.end())
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "%p/%s is not child for %p/%s",
			pChildMenuItem, pChildMenuItem->Description().c_str(),
			this, Description().c_str()
		);

		return NULL;
	}

	if(!m_vectChildren.empty() && pChildMenuItem == *m_vectChildren.begin())
		return NULL;

	--it;

	return *it;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuItem::MoveUp()
{
	if(NULL != m_pParent)
	{
		MenuItem *pNextMenuItem = m_pParent->PrevChild(this);

		if(NULL != pNextMenuItem)
			return pNextMenuItem;
	}

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuItem::MoveDown()
{
	if(NULL != m_pParent)
	{
		MenuItem *pNextMenuItem = m_pParent->NextChild(this);

		if(NULL != pNextMenuItem)
			return pNextMenuItem;
	}

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuItem::MoveRight()
{
	if(NULL != FirstChild())
		return FirstChild();

	return this;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuItem::MoveLeft()
{
	if(NULL != m_pParent)
		return m_pParent;

	return this;
}
//--------------------------------------------------------------------------------------------------------
bool MenuItem::IsLeaf()
{
	return m_vectChildren.empty();
}
//--------------------------------------------------------------------------------------------------------
void MenuItem::Expand(MenuItem *pChildMenuItem, const list<MenuItem *>& listExpandedItems)
{
	vector<MenuItem *>::iterator it = std::find(m_vectChildren.begin(), m_vectChildren.end(), pChildMenuItem);

	if(it == m_vectChildren.end())
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "%p/%s is not child for %p/%s",
			pChildMenuItem, pChildMenuItem->Description().c_str(),
			this, Description().c_str()
		);

		return;
	}

	it = m_vectChildren.erase(it);
	m_vectChildren.insert(it, listExpandedItems.begin(), listExpandedItems.end()); 

	delete pChildMenuItem;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuItem::Clone()
{
	MenuItem *pClone = new MenuItem(m_sDescription, m_pParent, m_type, m_pMenuItemAction->Clone());
	pClone->Value(m_sValue);

	for(vector<MenuItem *>::const_iterator it = m_vectChildren.begin(); it != m_vectChildren.end(); ++it)
	{
		MenuItem *pChild = *it;
		pClone->AddChild(pChild->Clone());
	}

	return pClone;
}
//--------------------------------------------------------------------------------------------------------
void MenuItem::ReplaceVariable(string sVariableName, string sValue)
{
	if(m_sValue == sVariableName)
	{
		m_sValue = sValue;
		m_pMenuItemAction->UpdateValueParam(sValue);
	}

	for(vector<MenuItem *>::const_iterator it = m_vectChildren.begin(); it != m_vectChildren.end(); ++it)
	{
		MenuItem *pChild = *it;
		pChild->ReplaceVariable(sVariableName, sValue);
	}
}
//--------------------------------------------------------------------------------------------------------
