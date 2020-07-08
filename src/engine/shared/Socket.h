#ifndef SOCKET_H_
#define SOCKET_H_

#include "Address.h"

class Socket
{
public:
	Socket();
	~Socket();

	void Reset();
	bool Open(unsigned short port);
	void Close();
	bool IsOpen() const;
	bool Send(const Address & destination, const void * data, int size);
	int Receive(Address & sender, void * data, int size);

private:
	int socket;
};
#endif /* SOCKET_H_ */
