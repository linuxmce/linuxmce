/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "LinphoneManager.h"
#include "DCE/Logger.h"

//DBK not found #include <linphone/linphonecore.h>

using namespace DCE;
using namespace std;

#define LINPHONE_CONFIG_FILE_PATH	"./linphone.cfg"

#define LINPHONE_FPS			30.0
#define LINPHONE_RESOLUTION		1024 * 1024

namespace DCE {

static LinphoneCoreVTable LinphoneVTable = {
        show:(ShowInterfaceCb) LinphoneManager::Linphone_NOOP_CB,
        inv_recv:(InviteReceivedCb) LinphoneManager::Linphone_INVITE_CB,
        bye_recv:(ByeReceivedCb) LinphoneManager::Linphone_BYE_CB,
        display_status:LinphoneManager::Linphone_STATUS_CB,
        display_message:LinphoneManager::Linphone_STATUS_CB,
        display_warning:LinphoneManager::Linphone_WARNING_CB,
        display_url:LinphoneManager::Linphone_URL_CB,
        display_question:(DisplayQuestionCb)LinphoneManager::Linphone_NOOP_CB
};


LinphoneManager::LinphoneManager() {
	linphone_ = new LinphoneCore(); //made linphone_ pointer because of conflicting includes
}


LinphoneManager::~LinphoneManager() {
	Close();
	delete linphone_;
}

bool
LinphoneManager::Open() {
	string conffile = generateConfigFile();
	if(conffile.empty()) {
		return false;
	}

	try {
		linphone_core_init(linphone_, &LinphoneVTable, (gchar*)conffile.c_str(), (void*)this);
	} catch (...) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Exception occured during linphone initialization...");
		return false;
	}
	return true;
}

void
LinphoneManager::Close() {
	linphone_core_uninit(linphone_);
}

bool
LinphoneManager::Invite(std::string extension) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Inviting: %s...", extension.c_str());

	linphone_->sound_conf.enabled = 1;
	linphone_->video_conf.enabled = 1;

	linphone_core_invite(linphone_, (char*)(string("sip:") + extension + "@" + host_ + ":5060").c_str());
	return true;
}

bool
LinphoneManager::Answer() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Answering incoming CALL.");

	int retry = 0;
	for (retry = 5; linphone_core_accept_dialog (linphone_, NULL) < 0 && retry > 0; retry--) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Accept CALL attempt failed.");
		sleep(1);
	}
	if (retry > 0) {
		linphone_core_set_video(linphone_, LINPHONE_FPS, LINPHONE_RESOLUTION);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Accept CALL succeeded.");
		return true;
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Accept CALL failed.");
		return false;
	}
}

void
LinphoneManager::Drop() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Dropping CALL.");
	linphone_core_terminate_dialog (linphone_, NULL);
}

bool
LinphoneManager::SetVideo(double fps, unsigned int resolution) {
	linphone_core_set_video(linphone_, fps, resolution);
	return true;
}

void
LinphoneManager::Linphone_NOOP_CB() {
}

void
LinphoneManager::Linphone_INVITE_CB(LinphoneCore *lc, char *from) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "LinponeInvite: %s", from);

	LinphoneManager* pme = (LinphoneManager*)(lc->impl_this_ptr);
	if(pme->pint_ != NULL) {
		pme->pint_->OnInvite();
	}
}

void
LinphoneManager::Linphone_BYE_CB(LinphoneCore *lc, char *from) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "LinponeBye: %s", from);
}

void
LinphoneManager::Linphone_STATUS_CB(LinphoneCore * lc, char *something) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "LinponeStatus: %s", something);

	LinphoneManager* pme = (LinphoneManager*)(lc->impl_this_ptr);
	if (strncmp(something, "Connected.", 10) == 0) {
		linphone_core_set_video(pme->linphone_, LINPHONE_FPS, LINPHONE_RESOLUTION);
		if(pme->pint_ != NULL) {
			pme->pint_->OnConnected();
		}
    } else
	if (strncmp(something, "Forbidden", 9) == 0
            || strncmp(something, "Timeout.", 8) == 0)
	{
		if(pme->pint_ != NULL) {
			pme->pint_->OnFailConnecting();
		}
	}
}

void
LinphoneManager::Linphone_WARNING_CB(LinphoneCore * lc, char *something) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "LinponeWarning: %s", something);
}

void
LinphoneManager::Linphone_URL_CB(LinphoneCore * lc, char *something, char *url) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "LinponeUrl: %s, %s", something, url);
}

std::string
LinphoneManager::generateConfigFile() {
	ofstream f(LINPHONE_CONFIG_FILE_PATH, fstream::out | fstream::trunc);
	if(!f.is_open()) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not open temporary Linphone config file.");
		return NULL;
	}

	std::string confstr = ""
		"[net]\n"
		"if_name=eth0\n"
		"con_type=3\n"
		"use_nat=0\n"
		"nat_address=\n"

		"[rtp]\n"
		"audio_rtp_port=7078\n"
		"video_rtp_port=9078\n"
		"audio_jitt_comp=64\n"
		"video_jitt_comp=200\n"
		"audio_delay=0\n"

		"[sound]\n"
		"enabled=1\n"
		"dev_id=0\n"
		"rec_lev=50\n"
		"play_lev=85\n"
		"source=m\n"
		"local_ring=/usr/local/share/sounds/linphone/ring.wav\n"
		"remote_ring=/usr/local/share/sounds/linphone/ringback.wav\n"

		"[sip]\n"
		"sip_port=5060\n"
		"use_registrar=1\n"
		"username=" + phonenum_ + "\n"
		"hostname=" + host_ + "\n"
		"registrar=sip:" + phonenum_ + "@" + host_ + "\n"
		"passwd=\n"
		"expires=900\n"
		"as_proxy=1\n"
		"addr_of_rec=sip:" + phonenum_ + "@" + host_ + ":5060\n"

		"[video]\n"
		"enabled=1\n"
		"show_local=1\n"
		"source=IEEE 1394 Digital Camera\n"
		"device=/dev/video1394/0\n"

		"[audio_codec_0]\n"
		"mime=PCMU\n"
		"rate=8000\n"
		"enabled=1\n"

		"[audio_codec_1]\n"
		"mime=GSM\n"
		"rate=8000\n"
		"enabled=1\n"

		"[audio_codec_2]\n"
		"mime=PCMA\n"
		"rate=8000\n"
		"enabled=1\n"

		"[audio_codec_3]\n"
		"mime=speex\n"
		"rate=8000\n"
		"enabled=0\n"

		"[audio_codec_4]\n"
		"mime=speex\n"
		"rate=16000\n"
		"enabled=0\n"

		"[audio_codec_5]\n"
		"mime=1015\n"
		"rate=8000\n"
		"enabled=0\n"

		"[video_codec_0]\n"
		"mime=MPV\n"
		"rate=90000\n"
		"enabled=0\n"

		"[video_codec_1]\n"
		"mime=H263\n"
		"rate=90000\n"
		"enabled=1\n"

		"[address_book]\n"
		"entry_count=0\n";

		f << confstr;
		f.close();
		return LINPHONE_CONFIG_FILE_PATH;
}

};
