#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <base/DetectPlatform.h>
#include "Net.h"
#include "Socket.h"

Socket::Socket() : socket(0)
{}

Socket::~Socket()
{
	Close();
}

void Socket::Reset()
{
	socket = 0;
}

bool Socket::Open(unsigned short port)
{
	assert(!IsOpen());

	// create socket

	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket <= 0)
	{
		printf("failed to create socket\n");
		socket = 0;
		return false;
	}

	// bind to port

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short) port);

	if (bind(socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0)
	{
		printf("failed to bind socket\n");
		Close();
		return false;
	}

	printf("socket opened on port %d\n", port);

	// set non-blocking io

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

	int nonBlocking = 1;
	if (fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
	{
		printf("failed to set non-blocking socket\n");
		Close();
		return false;
	}

#elif PLATFORM == PLATFORM_WINDOWS

	DWORD nonBlocking = 1;
	if ( ioctlsocket( socket, FIONBIO, &nonBlocking ) != 0 )
	{
		printf( "failed to set non-blocking socket\n" );
		Close();
		return false;
	}

#endif

	return true;
}

void Socket::Close()
{
	if (socket != 0)
	{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
		close(socket);
#elif PLATFORM == PLATFORM_WINDOWS
		closesocket( socket );
#endif
		socket = 0;
	}
}

bool Socket::IsOpen() const
{
	return socket != 0;
}

bool Socket::Send(const Address & destination, const void * data, int size)
{
	assert(data);
	assert(size > 0);

	if (socket == 0)
		return false;

	assert(destination.GetAddress() != 0);
	assert(destination.GetPort() != 0);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(destination.GetAddress());
	address.sin_port = htons((unsigned short) destination.GetPort());

	int sent_bytes = sendto(socket, (const char*) data, size, 0,
			(sockaddr*) &address, sizeof(sockaddr_in));

	return sent_bytes == size;
}

int Socket::Receive(Address & sender, void * data, int size)
{
	assert(data);
	assert(size > 0);

	if (socket == 0)
		return false;

#if PLATFORM == PLATFORM_WINDOWS
	typedef int socklen_t;
#endif

	sockaddr_in from;
	socklen_t fromLength = sizeof(from);

	int received_bytes = recvfrom(socket, (char*) data, size, 0,
			(sockaddr*) &from, &fromLength);

	if (received_bytes <= 0)
		return 0;

	unsigned int address = ntohl(from.sin_addr.s_addr);
	unsigned short port = ntohs(from.sin_port);

	sender = Address(address, port);

	return received_bytes;
}
