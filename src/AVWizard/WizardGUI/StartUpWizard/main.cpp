/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//---------------------------------------------------------------------------

#include "Wizard.h"
//---------------------------------------------------------------------------
#include "PageFileParser.h"
//---------------------------------------------------------------------------
#include "WizardWidgetPage.h"
#include "WizardWidgetsFactory.h"
//---------------------------------------------------------------------------
#include "GenerateWizardConfigDefaults.h"
#include "ConfigureCommons.h"
//---------------------------------------------------------------------------
#include "SocketRemoteServer.h"
//---------------------------------------------------------------------------
bool IsConsole = false;
//---------------------------------------------------------------------------
#include "SkinGenerator.h"
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ExitCode = 0;

	GenerateWizardConfigDefaults Generator;
	Generator.GenerateDefaults();
	Generator.GenerateDefaultPages("", SkinGenerator::Instance()->PathPixmaps);

	Wizard* m_Wizard = Wizard::GetInstance();
	m_Wizard->ParseCommandLineParameters(argc, argv);
	FILE * f = fopen("/tmp/AVWizard_Started", "w");
	if (f != NULL)
		fclose(f);
	m_Wizard->StartSDLVideoMode();
	m_Wizard->CreateDialogs();
	m_Wizard->MainLoop();
	ExitCode = m_Wizard->GetExitCode();
	m_Wizard->CleanUp();

	return ExitCode;
}
