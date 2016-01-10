#ifndef __DISPLAY_STATE_H__
#define __DISPLAY_STATE_H__
//--------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
using namespace std;
//--------------------------------------------------------------------------------------------------------
class DisplayState
{
public: 
	
	string m_sHeader;
	string m_sFooter;
	string m_sMainText;
	string m_sDescription;
	string m_sFinePrint;
	string m_sProgress;
	string m_sStatusMessage;
	vector<string> m_vectPriorOptions;
	vector<string> m_vectNextOptions;

	bool m_bCanGoDown;
	bool m_bCanGoUp;
	bool m_bCanGoLeft;
	bool m_bCanGoRight;
};
//--------------------------------------------------------------------------------------------------------
#endif //__DISPLAY_STATE_H__

