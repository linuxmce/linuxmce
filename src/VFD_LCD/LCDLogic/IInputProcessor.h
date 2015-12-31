#ifndef __IINPUT_PROCESSOR__
#define __IINPUT_PROCESSOR__
//--------------------------------------------------------------------------------------------------------
#include "Input.h"
//--------------------------------------------------------------------------------------------------------
class IInputProcessor
{
public:

	IInputProcessor() 
	{
	}

	virtual ~IInputProcessor() 
	{
	}

	virtual bool ProcessInput(const Input &input) = 0;
};
//--------------------------------------------------------------------------------------------------------
#endif //__IINPUT_PROCESSOR__
