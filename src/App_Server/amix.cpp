#include <alsa/asoundlib.h>
#include <math.h>
#include "amix.h"

static void error(const char *fmt,...)
{
	va_list va;

	va_start(va, fmt);
	fprintf(stderr, "amixer: ");
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
	va_end(va);
}

MasterMix::MasterMix()
{
	GetMixerHandle();
	GetMasterMixerElem();
	GetVolumeLimits();
}

MasterMix::~MasterMix()
{
	CloseMixerHandle();
}

long MasterMix::GetVolMin()
{
	return m_VolMin;
}

long MasterMix::GetVolMax()
{
	return m_VolMax;
}

void MasterMix::GetVolumeLimits()
{
	if (! m_bUsable)
	{
		m_VolMin = m_VolMax = 0;
		return;
	}

	// get min and max limits
	if (snd_mixer_selem_has_playback_volume(m_MixerElem))
	{
		if (snd_mixer_selem_has_playback_volume(m_MixerElem))
		{
			snd_mixer_selem_get_playback_volume_range(m_MixerElem, &m_VolMin, &m_VolMax);
		}
	}
}

long MasterMix::GetVolume()
{
	if (! m_bUsable)
		return 0;

	long Volume;
	if (!snd_mixer_selem_has_common_volume(m_MixerElem))
	{
		if (snd_mixer_selem_has_playback_volume(m_MixerElem))
		{
			snd_mixer_selem_get_playback_volume(m_MixerElem, (snd_mixer_selem_channel_id_t)0, &Volume);
			return Volume;
		}
	}

	return 0;
}

int MasterMix::GetVolumePercent()
{
	return m_VolMax != 0 ? (GetVolume() * 100 / m_VolMax) : m_VolMax;
}

int MasterMix::IsOn()
{
	if (! m_bUsable)
		return 0;

	int IsOn;
	if (!snd_mixer_selem_has_common_switch(m_MixerElem))
	{
		if (snd_mixer_selem_has_playback_switch(m_MixerElem))
		{
			snd_mixer_selem_get_playback_switch(m_MixerElem, (snd_mixer_selem_channel_id_t)0, &IsOn);
			return IsOn;
		}
	}

	return 0;
}

void MasterMix::SetVolume(long Volume)
{
	if (! m_bUsable)
		return;

	snd_mixer_selem_set_playback_volume(m_MixerElem, (snd_mixer_selem_channel_id_t)0, Volume);
	snd_mixer_selem_set_playback_volume(m_MixerElem, (snd_mixer_selem_channel_id_t)1, Volume);
}

void MasterMix::SetVolumePercent(int Percent)
{
	SetVolume(lrintl(Percent / 100.0 * m_VolMax));
}

void MasterMix::SetOn(int On)
{
	if (! m_bUsable)
		return;

	snd_mixer_selem_set_playback_switch(m_MixerElem, (snd_mixer_selem_channel_id_t)0, On);
	snd_mixer_selem_set_playback_switch(m_MixerElem, (snd_mixer_selem_channel_id_t)1, On);
}

void MasterMix::GetMixerHandle()
{
	int err;
	m_bUsable = false;

	if ((err = snd_mixer_open(&m_MixerHandle, 0)) < 0)
	{
		error("Mixer %s open error: %s", "default", snd_strerror(err));
		return;
	}
	if ((err = snd_mixer_attach(m_MixerHandle, "default")) < 0)
	{
		error("Mixer attach %s error: %s", "default", snd_strerror(err));
		snd_mixer_close(m_MixerHandle);
		return;
	}
	if ((err = snd_mixer_selem_register(m_MixerHandle, NULL, NULL)) < 0)
	{
		error("Mixer register error: %s", snd_strerror(err));
		snd_mixer_close(m_MixerHandle);
		return;
	}
	err = snd_mixer_load(m_MixerHandle);
	if (err < 0)
	{
		error("Mixer %s load error: %s", "default", snd_strerror(err));
		snd_mixer_close(m_MixerHandle);
		return;
	}

	m_bUsable = true;
}

void MasterMix::CloseMixerHandle()
{
	if (m_bUsable)
	{
		snd_mixer_close(m_MixerHandle);
		m_bUsable = false;
	}
}

void MasterMix::GetMasterMixerElem()
{
	if (! m_bUsable)
		return;
	
	/*
	snd_mixer_t *handle;
	*/
	snd_mixer_selem_id_t *sid;
	snd_mixer_elem_t *elem;
	snd_mixer_selem_id_alloca(&sid);

	// find our mixer (Master/Front)
	m_MixerElem = NULL;
	for (elem = snd_mixer_first_elem(m_MixerHandle); elem; elem = snd_mixer_elem_next(elem))
	{
		snd_mixer_selem_get_id(elem, sid);
		const char * MixerName = snd_mixer_selem_id_get_name(sid);
		if (strcmp(MixerName, "Master") == 0 || strcmp(MixerName, "Front") == 0)
		{
			m_MixerElem = elem;
			break;
		}
	}
	
	if (m_MixerElem == NULL)
	{
		error("No suitable volume mixer found");
		CloseMixerHandle();
	}
}
