#ifndef __IINPUT_PROVIDER__
#define __IINPUT_PROVIDER__
//--------------------------------------------------------------------------------------------------------
#include "IInputProcessor.h"
//--------------------------------------------------------------------------------------------------------
class IInputProvider
{
public:

	IInputProvider(IInputProcessor *pInputProcessor) : m_pInputProcessor(pInputProcessor) 
	{
	}
	
	virtual ~IInputProvider() 
	{
	}	

	bool HandleInput(const Input &input)
	{
		if(NULL != m_pInputProcessor)
			return m_pInputProcessor->ProcessInput(input);

		return false;
	}

private:

	IInputProcessor *m_pInputProcessor;
};
//--------------------------------------------------------------------------------------------------------
#endif //__IINPUT_PROVIDER__
