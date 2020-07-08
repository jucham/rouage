#ifndef FRAMECOUNTER_H_INCLUDED
#define FRAMECOUNTER_H_INCLUDED

class FrameCounter
{
   private:
      int m_iDelay; // time between each fps update in milliseconds
      int m_iFps;
      int m_iLastTime;
      int m_iFrameCount;
      int m_iLastFrameCount;

   public:

      FrameCounter(int delay, int lastTime) :
         m_iDelay(delay),
         m_iFps(0),
         m_iLastTime(lastTime),
         m_iFrameCount(0),
         m_iLastFrameCount(0)
      {}

      FrameCounter()
      {
          FrameCounter(100, 0);
      }

      const int& Fps() const
      {
         return m_iFps;
      }

      void updateFps(int currentTime)
      {
         m_iFrameCount++;

         if (currentTime - m_iLastTime >=  m_iDelay)
         {
            m_iLastTime = currentTime;
            m_iLastFrameCount = m_iFrameCount;
            m_iFrameCount = 0;
            m_iFps = static_cast<int>( m_iLastFrameCount * (1000.0f / m_iDelay) );
         }
      }
};

#endif // FRAMECOUNTER_H_INCLUDED
