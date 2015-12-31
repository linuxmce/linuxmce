#include "LCDRenderer.h"

#define LINE_WIDTH 24
#define NUM_LINES   4
#define END_OF_LINE "\r\n"
#define IDLE_INTERVAL 50
//--------------------------------------------------------------------------------------------------------
void *InputProviderThread(void *p);
//--------------------------------------------------------------------------------------------------------
LCDRenderer::LCDRenderer(IInputProcessor *pInputProcessor, string sSerialPort, unsigned int BPS, eParityBitStop ParityBitStop) : 
	IInputProvider(pInputProcessor), m_ScreenMutex("screen rendering"), m_InputProviderThreadID(0), m_bQuit(false)
{
	m_ScreenMutex.Init(NULL);

    try 
	{
		m_spSerial.reset(new CSerialPort(sSerialPort, BPS, ParityBitStop));
		Sleep(100);
	} 
	catch(...) 
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed Opening serial port: %s.", sSerialPort.c_str());
		m_spSerial.reset(NULL);
    }

	if(!m_InputProviderThreadID)
		pthread_create(&m_InputProviderThreadID, NULL, InputProviderThread, (void *)this);
}
//--------------------------------------------------------------------------------------------------------
LCDRenderer::~LCDRenderer()
{
	m_bQuit = true;

	if(m_InputProviderThreadID)
		pthread_join(m_InputProviderThreadID, 0);

	pthread_mutex_destroy(&m_ScreenMutex.mutex);
}
//--------------------------------------------------------------------------------------------------------
void LCDRenderer::Render(const DisplayState &display_state)
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	string sText = 
		AsLine(display_state.m_sHeader) + 
		AsLine(display_state.m_sDescription) + 
		AsLine(display_state.m_sStatusMessage) + 
		AsEndLine(display_state.m_sFooter);

	if(NULL != m_spSerial.get())
	{
		char pClear[] = {0x1B, '[', '2', 'J'}; //ASCII CODES

		//clear screen
		m_spSerial->Write(pClear, 4);

		//send text
		DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "LCDRenderer: Sending '%s' to LCD", sText.c_str());
		m_spSerial->Write(const_cast<char *>(sText.c_str()), sText.length());
	}
}
//--------------------------------------------------------------------------------------------------------
size_t LCDRenderer::ReadData(string &sData)
{
	if(NULL != m_spSerial.get())
	{
		const size_t nBufSize = 8;
		char pBuf[nBufSize];
		memset(pBuf, 0, nBufSize);
		size_t nRead = m_spSerial->Read(pBuf, 1);
		string sData(pBuf);

		if(nRead > 0)
			DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "LCDRenderer::ReadData: serial received %s, bytes %d", sData.c_str(), nRead);

		return nRead;
	}

	sData.clear();
	return 0;
}
//--------------------------------------------------------------------------------------------------------
string LCDRenderer::AsLine(string sText)
{
	string sResult;

	if(sText.size() <= LINE_WIDTH - 1)
		sResult = sText + END_OF_LINE;
	else if(sText.size() == LINE_WIDTH)
		sResult = sText;
	else
		sResult = sText.substr(0, LINE_WIDTH);

	return sResult;
}
//--------------------------------------------------------------------------------------------------------
string LCDRenderer::AsEndLine(string sText)
{
	string sResult;

	if(sText.size() <= LINE_WIDTH)
		sResult = sText;
	else
		sResult = sText.substr(0, LINE_WIDTH);

	return sResult;
}
//--------------------------------------------------------------------------------------------------------
bool LCDRenderer::Quit()
{
	return m_bQuit;
}
//--------------------------------------------------------------------------------------------------------
void *InputProviderThread(void *p)
{
	LCDRenderer *pRenderer = reinterpret_cast<LCDRenderer *>(p);

	while(NULL != pRenderer && !pRenderer->Quit())
	{
		string sData;
		if(pRenderer->ReadData(sData) == 1)
		{
			Input input;
			input.type = itKeyboardInput;
			switch(sData[0])
			{
				case '4':
					input.keyboardinput = kiUp;
					break;
				case '6':
					input.keyboardinput = kiDown;
					break;
				case '3':
					input.keyboardinput = kiLeft;
					break;
				case '7':
					input.keyboardinput = kiRight;
					break;
				default:
					input.type = itNone;
					break;
			}

			if(input.type != itNone)
				pRenderer->HandleInput(input);
		}
		else
		{
			Sleep(IDLE_INTERVAL);
		}
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
