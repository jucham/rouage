#include "NetPiece.h"

#include <cassert>
#include <cstdio>
#include <string>

#include "../../base/Types.h"
#include "Common.h"
#include "Net.h"
#include "NetDataArray.h"

NetPiece::WordStats NetPiece::m_WordStats[NUM_STAT_TYPES];


NetPiece::NetPiece() : NetDataArray<>()
{

}

NetPiece::NetPiece(int comType, int dataType) : NetDataArray<>()
{
	m_Data[0] = comType;
	m_Data[1] = dataType;
	m_pCurData += 2;
}

void NetPiece::Reset() {
	NetDataArray<>::Reset();
	m_ClientId = INVALID_CLIENT_ID;
	m_FirstPacket = false;
}

void NetPiece::Print(int numBytesToPrint, bool interpretByte) const
{
	assert(numBytesToPrint < MAX_PACKET_SIZE);
	int i = 0;

	// communication type
	if (!interpretByte)
		printf("[%d] ", m_Data[i]);
	else if ( m_Data[i] == NO_AUTH_COM )
		printf("[NO_AUTH_COM] ");
	else if ( m_Data[i] == AUTH_COM )
		printf("[AUTH_COM] ");
	else
		printf("[%d] ", m_Data[i]);
	++i;

	//data type
	if (!interpretByte)
		printf("[%d] ", m_Data[i]);
	else if ( m_Data[i] == NET_MSG )
		printf("[NET_MSG] ");
	else if ( m_Data[i] == NET_INPUTS )
		printf("[NET_INPUTS] ");
	else if ( m_Data[i] == NET_SNAPSHOT )
		printf("[NET_SNAPSHOT] ");
	else
		printf("[%d] ", m_Data[i]);
	++i;

	for (i = 2; i < numBytesToPrint; i++)
	{
		printf("[%d] ", m_Data[i]);
	}
	printf("\n");
}

void NetPiece::Print(int byteStart, int byteEnd) const
{
	for (int i = byteStart; i < byteEnd; i++)
		printf("[%d] ", m_Data[i]);
	printf("\n");
}

void NetPiece::PrintRaw() const {

	const byte_t* pData = &m_Data[0];

	// communication type
	if ( *pData == NO_AUTH_COM )
		printf("[NO_AUTH_COM] ");
	else if ( *pData == AUTH_COM )
		printf("[AUTH_COM] ");
	else
		printf("[UNKNOWN_COM=%d] ", *pData);
	++pData;

	//data type
	if ( *pData == NET_MSG )
		printf("[NET_MSG] ");
	else if ( *pData == NET_INPUTS )
		printf("[NET_INPUTS] ");
	else if ( *pData == NET_SNAPSHOT )
		printf("[NET_SNAPSHOT] ");
	else
		printf("[UNKNOWN_DATA=%d] ", *pData);
	++pData;

	printf("[");

	for(int i=2; i<MAX_PACKET_SIZE; i++)
	{
	  printf("%c", *pData);
	  pData++;
	}
	printf("]\n");
}

bool NetPiece::isInput() const
{
	return m_Data[1] == NET_INPUTS;
}

void NetPiece::InitWordStats(int sampleToCollect) {
		m_WordStats[STAT_MSG].Init("MSG", "msg.huff", sampleToCollect);
		m_WordStats[STAT_INPUT].Init("INPUT", "input.huff", sampleToCollect);
		m_WordStats[STAT_SNAPSHOT].Init("SNAPSHOT", "snap.huff", sampleToCollect);
		m_WordStats[STAT_NOISE].Init("NOISE", "noise.huff", sampleToCollect);
}


