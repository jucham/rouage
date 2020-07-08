#ifndef CHANNEL_H
#define CHANNEL_H


class Channel
{
   public:
      Channel();
      Channel(float volPercent, float panPercent);

      void SetVolume(float volPercent);
      int Volume() const;

      void SetPanning(float panPercent);
      int Panning() const;

   protected:
      int m_iVolume;
      int m_iPanning;
   private:
};

#endif // CHANNEL_H
