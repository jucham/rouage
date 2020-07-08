#ifndef SOUND_H
#define SOUND_H


class Sound
{
   public:
      Sound(short* pData,
            int numSamples,
            int sampleRate,
            int numChannels);

      ~Sound();

      short* Data() const;
      short Data(int i) const ;
      int NumSamples() const;
      int SampleRate() const;
      int NumChannels() const;

      void setNumSamples(int numSamples);


   protected:
      short*   m_pData;
      int      m_iNumSamples;
      int      m_iSampleRate;
      int      m_iNumChannels;

};

#endif // SOUND_H
