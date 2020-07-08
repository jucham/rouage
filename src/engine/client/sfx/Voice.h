#ifndef VOICE_H
#define VOICE_H

#include "Sound.h"
#include "Channel.h"
#include <base/Vector2D.h>

class Voice
{
public:
	static const int MAX_VOLUME = 255;

	Voice();
	const Sound* GetSound() const;
	void SetSound(const Sound* pSound);
	const Channel* getChannel() const;
	void SetChannel(Channel* pChannel);
	int Tick() const;
	void SetTick(int n);
	void DoTick();
	int GetVolume() const;
	void SetVolume(int vol);
	int Flags() const;
	void SetFlags(int flags);
	const Vector2D& Pos() const;
	void SetPos(const Vector2D& pos);
	void Free();
	bool IsFree() const;
	void SetLoopBehavior(bool b);
	bool HasLoopBehavior() const;
	int GetCustomId() const;
	void SetCustomId(int customId);

protected:
	const Sound* m_pSound;
	Channel*     m_pChannel;
	int          m_iTick;     // number of current sample
	int          m_iVolume;  // 0 to 255
	int          m_iFlags;
	Vector2D     m_vPos;
	unsigned int m_CustomId;
	bool         m_LoopBehavior;
};

#endif // VOICE_H
