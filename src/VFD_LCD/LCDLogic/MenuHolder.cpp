#include "MenuHolder.h"
#include "MenuItem.h"
#include "MenuItemAction.h"
#include "ActionProcessor.h"

#include "DCE/Logger.h"
//--------------------------------------------------------------------------------------------------------
MenuHolder::MenuHolder() : m_pRootMenuItem(NULL), m_pCurrentMenuItem(NULL), 
	m_pLastCurrentMenuItem(NULL), m_pActionProcessor(NULL)
{

}
//--------------------------------------------------------------------------------------------------------
MenuHolder::~MenuHolder()
{
	delete m_pRootMenuItem;
	delete m_pActionProcessor;
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::Setup(ActionProcessor *pActionProcessor)
{
	m_pActionProcessor = pActionProcessor;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuHolder::RootMenu()
{
	if(NULL == m_pRootMenuItem)
	{
		m_pRootMenuItem = new MenuItem("Status", NULL, itStatusItem);
		m_pRootMenuItem->Value("{CURRENT_STATUS}");
	}

	return m_pRootMenuItem;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuHolder::CurrentMenuItem()
{
	if(NULL == m_pCurrentMenuItem) 
		m_pCurrentMenuItem = m_pRootMenuItem->FirstChild();

	return m_pCurrentMenuItem;
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveUp()
{
	m_pCurrentMenuItem = m_pCurrentMenuItem->MoveUp();
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveDown()
{
	m_pCurrentMenuItem = m_pCurrentMenuItem->MoveDown();
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveLeft()
{
	MenuItem *pOldItem = m_pCurrentMenuItem;
	m_pCurrentMenuItem = m_pCurrentMenuItem->MoveLeft();

	if(itInputBox == pOldItem->Type() && m_pCurrentMenuItem != pOldItem)
	{
		//execute action
		MenuItemAction *pAction = pOldItem->Action();

		if(NULL != m_pActionProcessor && pAction->Type() != atNone)
		{
			pAction->UpdateValueParam(pOldItem->Value());
			m_pActionProcessor->ProcessAction(pAction);
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::MoveRight()
{
	if(itInputBox != m_pCurrentMenuItem->Type())
	{
		MenuItemAction *pAction = m_pCurrentMenuItem->Action();

		if(NULL != m_pActionProcessor && pAction->Type() != atNone)
			m_pActionProcessor->ProcessAction(pAction);
	}

	m_pCurrentMenuItem = m_pCurrentMenuItem->MoveRight();
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::Home()
{
	m_pLastCurrentMenuItem = m_pCurrentMenuItem;
	m_pCurrentMenuItem = m_pRootMenuItem;
}
//--------------------------------------------------------------------------------------------------------
void MenuHolder::Restore()
{
	m_pCurrentMenuItem = m_pLastCurrentMenuItem;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuHolder::GetTopChildNode(string sDescription)
{
	for(vector<MenuItem *>::const_iterator it = m_pRootMenuItem->Children().begin(); 
		it != m_pRootMenuItem->Children().end(); ++it)
	{
		MenuItem *pChild = *it;
		if(pChild->Description() == sDescription)
			return pChild;
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
MenuItem *MenuHolder::GetChildNode(std::string sTopChildDescription, std::string sSecondLevelChildDescription)
{
	MenuItem *pTopChildNone = GetTopChildNode(sTopChildDescription);

	if(NULL != pTopChildNone)
	{
		for(vector<MenuItem *>::const_iterator it = pTopChildNone->Children().begin(); 
			it != pTopChildNone->Children().end(); ++it)
		{
			MenuItem *pChild = *it;
			if(pChild->Description() == sSecondLevelChildDescription)
				return pChild;
		}
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
