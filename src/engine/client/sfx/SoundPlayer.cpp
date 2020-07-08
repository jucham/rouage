#include "SoundPlayer.h"

#include <cassert>
#include <SDL2/SDL.h>
#include <base/utils.h>
#include <iostream>
#include "SFXAsset.h"
#include "Sound.h"

SoundPlayer::SoundPlayer() :
m_SoundVol(100),
m_pListenerPos(0),
m_Voices(NUM_VOICES),
m_NextVoice(0),
m_Channels(NUM_CHANNELS),
m_FreeCustomId(INVALID_CUSTOM_ID)
{
	Init();

	m_Channels[CHN_WORLD].SetVolume(0.3f);
	m_Channels[CHN_WORLD].SetPanning(1.0f);
	m_Channels[CHN_GUI].SetVolume(1.0f);
	m_Channels[CHN_GUI].SetPanning(0.0f);
	m_Channels[CHN_MUSIC].SetVolume(0.2f);
	m_Channels[CHN_MUSIC].SetPanning(0.0f);
}

SoundPlayer::~SoundPlayer()
{
	SDL_CloseAudio();
}

void SoundPlayer::Init()
{
	SDL_AudioSpec format;
	SDL_LockAudio();

	format.freq = 44100;
	format.format = AUDIO_S16;
	format.channels = 2;
	format.samples = 512;
	format.callback = sdlcallback;
	format.userdata = this;

	// Open the audio device and start playing sound!
	if(SDL_OpenAudio(&format, 0) < 0)
	{
		printf("unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_PauseAudio(0);
	UpdateVolume();
}

void SoundPlayer::UpdateVolume()
{
	int wanted_volume = 100;
	if(wanted_volume != m_SoundVol)
	{
		SDL_LockAudio();
		m_SoundVol = wanted_volume;
		SDL_UnlockAudio();
	}
}

void sdlcallback(void *userdata, Uint8 *stream, int len)
{
	do_mix( reinterpret_cast<SoundPlayer*> (userdata), (short*)stream, len );
}

void do_mix(SoundPlayer *pSoundPlayer, short *stream, int len)
{
	// len is 2048 so make that at 512, then len/2/2
	pSoundPlayer->Mix( stream, len/2/2 );
}

void SoundPlayer::Mix(short *final_out, unsigned frames)
{
	int mix_buffer[MAX_FRAMES*2] = {0};
	SDL_LockAudio();

	// computes all voices ( a voice = a sound )
	for(int i = 0; i < NUM_VOICES; i++)
	{
		Voice &voice = m_Voices[i];

		if( !voice.IsFree() )
		{
			int *pMixBuf = mix_buffer;

			const Sound *pSound = voice.GetSound();
			short *pData = pSound->Data();

			int step       = pSound->NumChannels(); // setup input sources
			short *in_l    = &pData[voice.Tick()*step];
			short *in_r    = &pData[voice.Tick()*step+1];

			// number of remaining samples
			int end = pSound->NumSamples() - voice.Tick();

			// same volume both sides
			int rvol = voice.GetVolume();
			int lvol = rvol;

			// last piece of samples could be less (less than 512 i.e.) then set it.
			if(static_cast<int>(frames) < end)
				end = frames;

			/// check if we have a mono sound
			if(pSound->NumChannels() == 1)
			{
				in_r = in_l;
			}

			// volume calculation
			if( (voice.Flags()&SNDFLAG_POS) && voice.getChannel()->Panning())
			{
				const int range = 30; // range is the limit in meters to ear something
				int dist = static_cast<int> ( Vec2DDistance(*m_pListenerPos, voice.Pos()) );
				int dx = static_cast<int> (voice.Pos().x - m_pListenerPos->x);
				int absDx = (dx<0) ? -dx : dx; // absolute value of distance on x axis

				if(dist < range)
				{
					// compute panning
					if(dx > 0)
						lvol = ((range-absDx)*lvol)/range;
					else
						rvol = ((range-absDx)*rvol)/range;

					// falloff
					lvol = (lvol*(range-dist))/range;
					rvol = (rvol*(range-dist))/range;
				}
				else
				{
					lvol = 0;
					rvol = 0;
				}
			}

			// process all frames (512 samples or less for the last frame)
			for(int s=0; s<end; s++)
			{
				*pMixBuf++ += (*in_l)*lvol;
				*pMixBuf++ += (*in_r)*rvol;
				in_l += step;
				in_r += step;
				voice.DoTick();
			}

			// free voice if not used any more
			if(voice.Tick() == pSound->NumSamples())
			{
				if (voice.HasLoopBehavior()) 
				{
					voice.SetTick(0);
				}
				else
				{
					voice.Free();
				}
			}

		}
	} // END VOICES

	SDL_UnlockAudio();

	// mix voices
	for (int i = 0; i < static_cast<int>(frames); i++)
	{
		int vl = ((mix_buffer[2 * i] * m_SoundVol) / 101) >> 8;
		int vr = ((mix_buffer[2 * i + 1] * m_SoundVol) / 101) >> 8;

		final_out[2 * i] = Int2Short(vl);
		final_out[2 * i + 1] = Int2Short(vr);
	}
}

short SoundPlayer::Int2Short(int i)
{
	if(i > 0x7fff)
		return 0x7fff;
	else if(i < -0x7fff)
		return -0x7fff;
	return i;
}

int SoundPlayer::Play(int cid, int sid, int flags, const Vector2D& pos, int volume, unsigned int existingCustomId, bool loop)
{
	int vid = GetVoiceIdFromCustomId(existingCustomId);

	// sound already exists
	if (vid != INVALID_VOICE_ID) {
		m_Voices[vid].SetPos(pos);
		return existingCustomId;
	}

	SDL_LockAudio();

	// search for voice
	for(int i=0; i<NUM_VOICES; i++)
	{
		vid = (m_NextVoice + i) % NUM_VOICES;
		if(m_Voices[vid].IsFree())
		{
			m_NextVoice = vid+1;
			break;
		}
	}

	unsigned int customId = INVALID_CUSTOM_ID;

	// voice found, use it
	if(vid != INVALID_VOICE_ID)
	{
		m_Voices[vid].SetSound( SFXAsset::Instance().GetSound(sid) );
		m_Voices[vid].SetChannel(  &m_Channels[cid] );
		m_Voices[vid].SetTick(0);
		m_Voices[vid].SetVolume(volume);
		m_Voices[vid].SetFlags(flags);
		m_Voices[vid].SetPos(pos);
		customId = GetNextFreeCustomId();
		m_Voices[vid].SetCustomId(customId);
		m_Voices[vid].SetLoopBehavior(loop);
	}

	SDL_UnlockAudio();

	return customId;
}


int SoundPlayer::Play(int cid, int sid, int flags)
{
	Vector2D pos(0,0);
	return Play(cid, sid, flags, pos);
}

void SoundPlayer::Stop(unsigned int customId)
{
	assert(customId != INVALID_CUSTOM_ID);
	SDL_LockAudio();
	for (int i = 0; i < NUM_VOICES; ++i) {
		if (m_Voices[i].GetCustomId() == customId) {
			m_Voices[i].Free();
			break;
		}
	}
	SDL_UnlockAudio();
}

void SoundPlayer::SetListenerPos(const Vector2D *pPos)
{
	m_pListenerPos = pPos;
}

unsigned int SoundPlayer::GetNextFreeCustomId() {
	if (m_FreeCustomId == MaxUnsignedInt)
		m_FreeCustomId = 1;
	else
		m_FreeCustomId++;
	return m_FreeCustomId;
}

int SoundPlayer::GetVoiceIdFromCustomId(unsigned int customId) const {

	if (customId == INVALID_CUSTOM_ID)
		return INVALID_VOICE_ID;

	for (int i = 0; i < NUM_VOICES; ++i) {
		if (m_Voices[i].GetCustomId() == customId) {
			return i;
		}
	}
	return INVALID_VOICE_ID;
}

