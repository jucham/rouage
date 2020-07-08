#include "Time.h"

timeval Time::m_StartTime;

void Time::init() {
	gettimeofday(&m_StartTime, NULL);
}



