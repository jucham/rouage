#ifndef NETDATAARRAY_H_
#define NETDATAARRAY_H_

#include <cassert>
#include <cstdio>
#include <cstring>
#include <base/utils.h>
#include <base/Types.h>
#include "Net.h"
#include <base/tri_logger.hpp>
#include <engine/external/half/half.hpp>

template <size_t S=MAX_PACKET_SIZE>
class NetDataArray
{
public:
	NetDataArray();
	void Reset();
	void AddInt16(int i);
	int GetInt16();
	void AddInt32(int i);
	int GetInt32();
	void AddFloat(float f);
	float GetFloat();
	void AddString(const char *pStr);
	const char * GetString();
	void AddBytes(const void *pData, int size);
	const byte_t * GetBytes(int size);
	void AddByte(const void *pData);
	void AddByte(const byte_t b);
	const byte_t GetByte();
	void Rewind(int bytes = -1);
	const byte_t* GetData() const;

	void GetData(byte_t *pData) const {
		std::memcpy(pData, m_Data, GetSize());
	}

	void Add(const NetDataArray &nda);

	int GetSize() const {
		return m_pCurData - m_Data;
	}

	void Print(int numBytesToPrint = 10) const;

protected:

	byte_t m_Data[S];
	byte_t *m_pCurData;
	byte_t *m_pEndData;

	template<typename T>
	int PackValue32(void *buf, T x);

	template<typename T>
	T UnpackValue32(const void *buf, int *unpackedBytes);

	int PackValue16(void *buf, uint16_t x)
	{
		unsigned char *b = reinterpret_cast<unsigned char*>(buf);
		unsigned char *p = reinterpret_cast<unsigned char*>(&x);

		// little endian, we swap to conform to MSB
#ifdef ARCH_LITTLE_ENDIAN
		b[0] = p[1];
		b[1] = p[0];
#else
		b[0] = p[0];
		b[1] = p[1];
#endif

		return 2;

	}

	uint16_t UnpackValue16(const void *buf, int *unpackedBytes = 0)
	{
		if ( unpackedBytes )
			*unpackedBytes += 2;
		const unsigned char *b = (const unsigned char *) buf;
		uint16_t temp = 0;
		temp = ((b[0] << 8) | b[1]);
		return temp;
	}

};

template <size_t S>
inline const byte_t* NetDataArray<S>::GetData() const
{
	return m_Data;
}


// pack method for storing data in network,
// big endian, byte order (MSB first)
// returns number of bytes packed
template <size_t S>
template<typename T>
inline int NetDataArray<S>::PackValue32(void *buf, T x)
{
	unsigned char *b = reinterpret_cast<unsigned char*>(buf);
	unsigned char *p = reinterpret_cast<unsigned char*>(&x);

	// little endian, we swap to conform to MSB
#ifdef ARCH_LITTLE_ENDIAN
	b[0] = p[3];
	b[1] = p[2];
	b[2] = p[1];
	b[3] = p[0];
#else
	b[0] = p[0];
	b[1] = p[1];
	b[2] = p[2];
	b[3] = p[3];
#endif

	return 4;

}


// unpack method for retrieving data in network byte,
//   big endian, order (MSB first)
// increments index i by the number of bytes unpacked
template <size_t S>
template<typename T>
inline T NetDataArray<S>::UnpackValue32(const void *buf, int *unpackedBytes)
{
	if ( unpackedBytes )
		*unpackedBytes += 4;
	const unsigned char *b = (const unsigned char *) buf;
	uint32_t temp = 0;
	temp = ((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]);
	return *(reinterpret_cast<T*>(&temp));
}

template <size_t S>
NetDataArray<S>::NetDataArray()
{
	Reset();
}

template <size_t S>
void NetDataArray<S>::Reset()
{
	std::memset(m_Data, 0, S);
	m_pCurData = m_Data;
	m_pEndData = m_Data + S;
}

template <size_t S>
void NetDataArray<S>::AddInt16(int i)
{
	int packedBytes = PackValue16(m_pCurData, i);
	m_pCurData += packedBytes;
}

template <size_t S>
int NetDataArray<S>::GetInt16() {
	int unpackedBytes = 0;
	uint16_t i = UnpackValue16(m_pCurData, &unpackedBytes);
	m_pCurData += unpackedBytes;
	return i;
}

template <size_t S>
void NetDataArray<S>::AddInt32(int i)
{
	int packedBytes = PackValue32<int>(m_pCurData, i);
	m_pCurData += packedBytes;
}

template <size_t S>
int NetDataArray<S>::GetInt32() {
	int unpackedBytes = 0;
	int i = UnpackValue32<int>(m_pCurData, &unpackedBytes);
	m_pCurData += unpackedBytes;
	return i;
}

//#define USE_16BITS_FLOAT
#if defined(USE_16BITS_FLOAT)
template <size_t S>
void NetDataArray<S>::AddFloat(float f)
{
	half_float::half hf(f);
	int packedBytes = PackValue16(m_pCurData, hf.data_);
	m_pCurData += packedBytes;
}

template <size_t S>
float NetDataArray<S>::GetFloat()
{
	int unpackedBytes = 0;
	uint16_t i = UnpackValue16(m_pCurData, &unpackedBytes);
	m_pCurData += unpackedBytes;
	half_float::half hf;
	hf.data_ = i;
	return static_cast<float>(hf);
}

#else
template <size_t S>
void NetDataArray<S>::AddFloat(float f)
{
	int packedBytes = PackValue32(m_pCurData, f);
	m_pCurData += packedBytes;
}

template <size_t S>
float NetDataArray<S>::GetFloat() {
	int unpackedBytes = 0;
	float f = UnpackValue32<float>(m_pCurData, &unpackedBytes);
	m_pCurData += unpackedBytes;
	return f;
}
#endif

template <size_t S>
void NetDataArray<S>::AddString(const char *pStr)
{
	while (*pStr)
	{
		*m_pCurData++ = *pStr++;

		if ( m_pCurData >= m_pEndData )
		{
			printf("string exceeds size allowed by buffer\n");
			break;
		}
	}
	*m_pCurData++ = 0;
}

template <size_t S>
const char * NetDataArray<S>::GetString()
{
	char *pStr = reinterpret_cast<char*>(m_pCurData);
	while (*m_pCurData) // skip the string
	{
		m_pCurData++;
		if ( m_pCurData == m_pEndData )
		{
			printf("problem when reading a string in buffer\n");
			return "Huston?";
		}
	}
	m_pCurData++;

	return pStr;
}

template <size_t S>
void NetDataArray<S>::AddBytes(const void *pData, int size)
{
	assert(m_pCurData + size <= m_pEndData);

	std::memcpy(m_pCurData, pData, size);
	m_pCurData += size;
}

template <size_t S>
const byte_t * NetDataArray<S>::GetBytes(int size)
{
	assert(m_pCurData + size < m_pEndData);

	byte_t *pData = m_pCurData;
	m_pCurData += size;
	return pData;
}

template <size_t S>
void NetDataArray<S>::AddByte(const void *pData)
{
	AddBytes(pData, 1);
}

template <size_t S>
void NetDataArray<S>::AddByte(const byte_t b)
{
	AddBytes(&b, 1);
}

template <size_t S>
const byte_t NetDataArray<S>::GetByte()
{
	const byte_t *pByte = GetBytes(1);
	return *pByte;
}

template <size_t S>
void NetDataArray<S>::Rewind(int bytes)
{
	assert(bytes <= GetSize());
	if (bytes < 0) {
		m_pCurData = m_Data;
	} else {
		m_pCurData -= bytes;
	}
}

template <size_t S>
void NetDataArray<S>::Add(const NetDataArray &nda)
{
	assert (GetSize() + nda.GetSize() <= S);

	std::memcpy(m_pCurData, nda.m_Data, nda.GetSize());
	m_pCurData += nda.GetSize();
}

template <size_t S>
void NetDataArray<S>::Print(int numBytesToPrint) const
{
	for (int i = 0; i < numBytesToPrint; i++)
	{
		printf("[%d] ", m_Data[i]);
	}
	printf("\n");
}


#endif
