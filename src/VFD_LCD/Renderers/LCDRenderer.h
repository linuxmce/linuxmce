#ifndef __LCD_RENDERER_H__
#define __LCD_RENDERER_H__
//--------------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
#include "Serial/SerialPort.h"
//--------------------------------------------------------------------------------------------------------
#include "../LCDLogic/IRenderer.h"
#include "../LCDLogic/IInputProvider.h"
#include <memory>
using namespace std;
//--------------------------------------------------------------------------------------------------------
class LCDRenderer : public IRenderer, public IInputProvider
{
	int m_fd;
	pluto_pthread_mutex_t m_ScreenMutex;
	pthread_t m_InputProviderThreadID;
	auto_ptr<CSerialPort> m_spSerial;
	bool m_bQuit;

	string AsLine(string sText);
	string AsEndLine(string sText);

public:

	LCDRenderer(IInputProcessor *pInputProcessor, string sSerialPort, unsigned int bps = 19200, eParityBitStop parity = epbsN81);
	~LCDRenderer();

	void Render(const DisplayState &display_state);

	size_t ReadData(string &sData);
	bool Quit();
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_LOADER_H__
