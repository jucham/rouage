#include "Net.h"

// platform independent wait for n seconds

#if PLATFORM == PLATFORM_WINDOWS
void wait_second( float seconds )
{
    Sleep( (int) ( seconds * 1000.0f ) );
}
#else
#include <unistd.h>
void wait_second(float seconds)
{
    usleep((int) (seconds * 1000000.0f));
}
#endif

int InitializeSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    int result = WSAStartup(MAKEWORD(2,2), &WsaData);
	if(result == 0)
		return 1;
	return 0;
#else
    return 1;
#endif
}

void ShutdownSockets()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif
}
