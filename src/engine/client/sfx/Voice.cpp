#include "Voice.h"

const int Voice::MAX_VOLUME;

Voice::Voice() :
    		  m_pSound(0),
    		  m_pChannel(0),
    		  m_iTick(0),  
    		  m_iVolume(MAX_VOLUME), 
    		  m_iFlags(0),
    		  m_vPos(0,0),
    		  m_CustomId(0),
    		  m_LoopBehavior(false)
{}


const Sound* Voice::GetSound() const {return m_pSound;}
void Voice::SetSound(const Sound* pSound) {m_pSound = pSound;}

const Channel* Voice::getChannel() const {return m_pChannel;}
void Voice::SetChannel(Channel* pChannel) {m_pChannel = pChannel;}

int Voice::Tick() const {return m_iTick;}
void Voice::SetTick(int n) { m_iTick = n;}
void Voice::DoTick() { m_iTick += 1; }

void Voice::SetVolume(int vol)
{
	Clamp(vol, 0, MAX_VOLUME);
	m_iVolume = vol;
}

int Voice::GetVolume() const {return m_iVolume;}

int Voice::Flags() const {return m_iFlags;}
void Voice::SetFlags(int flags) {m_iFlags = flags;}

const Vector2D& Voice::Pos() const {return m_vPos;}
void Voice::SetPos(const Vector2D& pos) {m_vPos = pos;}

void Voice::Free() {
	m_pSound = 0;
	m_CustomId = 0;
}
bool Voice::IsFree() const {return m_pSound == 0;}

void Voice::SetLoopBehavior(bool b) {m_LoopBehavior = b;}
bool Voice::HasLoopBehavior() const {return m_LoopBehavior;}
int Voice::GetCustomId() const {return m_CustomId;}
void Voice::SetCustomId(int customId) {m_CustomId = customId;}
