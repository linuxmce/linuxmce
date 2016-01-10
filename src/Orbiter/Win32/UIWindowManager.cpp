#include "UIWindowManager.h"
//-----------------------------------------------------------------------------------------------------
UIWindowBase *UIWindowManager::m_pUIWindowBase = NULL;
UIWindowType UIWindowManager::m_UIWindowType = uwtNormal;
//-----------------------------------------------------------------------------------------------------
/*static*/ UIWindowBase *UIWindowManager::CreateUIWindow(UIWindowType type) 
{
	switch(type)
	{
		case uwtMasked:
			m_pUIWindowBase = new UIWindowMasked();
			m_UIWindowType = uwtMasked;
			break;

		case uwtAlphaBlended:
			m_pUIWindowBase = new UIWindowAlphaBlended();
			m_UIWindowType = uwtAlphaBlended;
			break;

		case uwtNormal:
		default:
			m_pUIWindowBase = new UIWindowNormal();
			m_UIWindowType = uwtNormal;
			break;
	}

	return m_pUIWindowBase;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ UIWindowBase *UIWindowManager::UIWindow()
{
	return m_pUIWindowBase;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ UIWindowType UIWindowManager::Type()
{
	return m_UIWindowType;
}
//-----------------------------------------------------------------------------------------------------
