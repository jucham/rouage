#include "Sound.h"

Sound::Sound(short* pData,
            int numSamples,
            int sampleRate,
            int numChannels) :
   m_pData(pData),
   m_iNumSamples(numSamples),
   m_iSampleRate(sampleRate),
   m_iNumChannels(numChannels)
{}

Sound::~Sound()
{
   delete[] m_pData;
}

short* Sound::Data() const {return m_pData;}
short Sound::Data(int i) const {return m_pData[i];}
int Sound::NumSamples() const {return m_iNumSamples;}
int Sound::SampleRate() const {return m_iSampleRate;}
int Sound::NumChannels() const {return m_iNumChannels;}

void Sound::setNumSamples(int numSamples)
{
   m_iNumSamples = numSamples;
}
