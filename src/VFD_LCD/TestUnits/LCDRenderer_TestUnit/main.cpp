#include "../../Renderers/LCDRenderer.h"

int main( int argc, char* argv[] )
{
	string sSerialPort("/dev/ttyUSB0");
	DisplayState display_state;

	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		c = argv[optnum][1];
		switch (c)
		{
		case 'h':
			display_state.m_sHeader = argv[++optnum];
			break;
		case 'd':
			display_state.m_sDescription = argv[++optnum];
			break;
		case 's':
			display_state.m_sStatusMessage = argv[++optnum];
			break;
		case 'f':
			display_state.m_sFooter = argv[++optnum];
			break;
		case 'p':
			sSerialPort = argv[++optnum];
			break;
		default:
			cout << endl << 
				"\tUsage: LCDRenderer" << endl <<
				"\t\t-p <port>" << endl <<
				"\t\t-h <header>" << endl <<
				"\t\t-d <description>" << endl <<
				"\t\t-s <status>" << endl <<
				"\t\t-f <footer>" << endl << endl;

			return 1;
		};
	}

	LCDRenderer lcd_renderer(NULL, sSerialPort);
	lcd_renderer.Render(display_state);

	return 0;
}
