#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <SDL2/SDL.h>
#include <vector>
#include <base/Vector2D.h>
#include "Voice.h"
#include "Channel.h"

class SFXAsset;

enum
{
	SNDFLAG_LOOP=1,
	SNDFLAG_POS=2,
	SNDFLAG_ALL=3,
};

enum
{
	INVALID_VOICE_ID = -1,
	INVALID_CUSTOM_ID = 0
};

enum
{
	NUM_SAMPLES = 512,
	NUM_VOICES = 64,
	MAX_FRAMES = 1024
};

enum
{
	CHN_WORLD,
	CHN_GUI,
	CHN_MUSIC,
	NUM_CHANNELS
};


class SoundPlayer
{

public:
	static SoundPlayer & Instance() {
		static SoundPlayer sp;
		return sp;
	}
	~SoundPlayer();

	void UpdateVolume();
	int Play(int cid, int sid, int flags, const Vector2D& pos, int volume = Voice::MAX_VOLUME, unsigned int existingCustomId = INVALID_CUSTOM_ID, bool loop = false);
	int Play(int cid, int sid, int flags);
	void Stop(unsigned int vid);
	void SetListenerPos(const Vector2D *pPos); //FIXME put ref

private:

	SoundPlayer();
	SoundPlayer(const SoundPlayer &);
	SoundPlayer & operator=(const SoundPlayer &);

	void Init();
	void Mix(short *final_out, unsigned frames);
	friend void do_mix(SoundPlayer* sp, short* stream, int len);
	static short Int2Short(int i);
	unsigned int GetNextFreeCustomId();
	int GetVoiceIdFromCustomId(unsigned int customId) const;

	int                  m_SoundVol;
	const Vector2D 	     *m_pListenerPos;
	std::vector<Voice>   m_Voices;
	int                  m_NextVoice;
	std::vector<Channel> m_Channels;
	unsigned int         m_FreeCustomId;
};

void sdlcallback(void *unused, Uint8 *stream, int len);

#endif // SOUNDPLAYER_H
