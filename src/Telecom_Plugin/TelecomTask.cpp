//
// C++ Implementation: TelecomTask
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "TelecomTask.h"
#include "Telecom_Plugin.h"

using namespace DCE;

Telecom_Plugin * TelecomTask::telecom = NULL;
unsigned TelecomTask::uID = 0;

void TelecomTask::SetTelecom(const Telecom_Plugin * tp)
{
	telecom = (Telecom_Plugin *)tp;
}

TelecomTask::TelecomTask()
	: state( TelecomTask::Ready ),
	  sJobID("start"),
	  jobState(TelecomTask::Stop)
{
	sID = StringUtils::itos(uID++);
}

TelecomTask::~TelecomTask()
{
}


