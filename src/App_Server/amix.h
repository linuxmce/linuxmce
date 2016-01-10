#ifndef MasterMix_H
#define MasterMix_H

/** @file amix.h
A part of App_Server
*/
#include <alsa/asoundlib.h>

/** @class MasterMix
The Master sound mixer for app_server.
*/
class MasterMix
{
	private:
		snd_mixer_t * m_MixerHandle;
		snd_mixer_elem_t * m_MixerElem;

		long m_VolMin, m_VolMax;
		bool m_bUsable;

		void GetMixerHandle();
		void GetMasterMixerElem();
		void GetVolumeLimits();
		void CloseMixerHandle();

	public:
        /** Standard constructor. */
		MasterMix();

        /** Standard Destructor. */
		~MasterMix();

        /** Returns the minimum volume level. */
		long GetVolMin();

        /** Returns the maximum volume level. */
		long GetVolMax();

        /** Returns the volume level. */
		long GetVolume();

        /** Normalize the volume to 0-100 range */
		int GetVolumePercent();

        /** Return off/on status of mixer. */
		int IsOn();

        /** Set the absolute volume level. */
		void SetVolume(long Volume);

        /** Set the volume to 0-100 % of max.
        @param Percent is a number in the range of 0 to 100.
        */
		void SetVolumePercent(int Percent);

        /** Turn the mixer on or off( 0 ) */
		void SetOn(int On);
};

#endif /*MasterMix_H*/
