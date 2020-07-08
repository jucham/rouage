#ifndef TIME_H_
#define TIME_H_

#include <cstdio>
#include <sys/time.h>
#include <unistd.h>

class Time {

public:
	static void init();
	static float sec();
	static int ms();

private:
	static timeval m_StartTime;
};

inline float Time::sec()
{
	timeval endTime;
	long seconds, useconds;
	double duration;
	gettimeofday(&endTime, NULL);
	seconds  = endTime.tv_sec  - m_StartTime.tv_sec;
	useconds = endTime.tv_usec - m_StartTime.tv_usec;
	duration = seconds + useconds/1000000.0;
	return duration;
}

inline int Time::ms()
{
	return static_cast<int>(sec()*1000.0f);
}



#endif /* TIME_H_ */
