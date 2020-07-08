#ifndef NET_H
#define NET_H

#include <base/DetectPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#if PLATFORM == PLATFORM_WINDOWS
#include <winsock2.h>
#pragma comment( lib, "wsock32.lib" )

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#else
#error unknown platform!

#endif

enum {
   PROTOCOL_VERSION = 5,
   MAX_PACKET_SIZE = 1400,
   MAX_SNAPSHOT_SIZE = 4096
};

void wait_second( float seconds );
int InitializeSockets();
void ShutdownSockets();

#ifdef __cplusplus
}
#endif

#endif
