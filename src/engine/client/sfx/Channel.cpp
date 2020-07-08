#include "Channel.h"
#include <cassert>

Channel::Channel() :
   m_iVolume(0),
   m_iPanning(0)
{}

Channel::Channel(float volPercent, float panPercent) :
   m_iVolume( static_cast<int>(volPercent * 255) ),
   m_iPanning( static_cast<int>(panPercent * 255) )
{}


void Channel::SetVolume(float volPercent)
{
   assert ( volPercent >= 0.0f && volPercent <= 1.0f);
   m_iVolume = static_cast<int>(volPercent * 255);
}

int Channel::Volume() const
{
   return m_iVolume;
}

void Channel::SetPanning(float panPercent)
{
   assert ( panPercent >= 0.0f && panPercent <= 1.0f);
   m_iPanning = static_cast<int>(panPercent * 255);
}

int Channel::Panning() const
{
   return m_iPanning;
}

