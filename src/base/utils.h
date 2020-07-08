#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include "DetectPlatform.h"
#include "Types.h"

#if PLATFORM == PLATFORM_MAC
#include "CoreFoundation/CoreFoundation.h"
#endif

enum OrientHV
{
	HORIZ, VERT
};
enum OrientTBLR
{
	TOP, BOTTOM, LEFT, RIGHT
};

//a few useful constants
const unsigned int MaxUnsignedInt = (std::numeric_limits<unsigned int>::max)();
const float MaxDouble = (std::numeric_limits<float>::max)();
const float MinDouble = (std::numeric_limits<float>::min)();
const float MaxFloat = (std::numeric_limits<float>::max)();
const float MinFloat = (std::numeric_limits<float>::min)();
const float Pi = 3.14159f;
const float TwoPi = Pi * 2;
const float HalfPi = Pi / 2;
const float QuarterPi = Pi / 4;

//compares two real numbers. Returns true if they are equal
inline bool isEqual(float a, float b)
{
	if ( std::fabs(a - b) < 1E-12 )
	{
		return true;
	}

	return false;
}

//returns a random integer between x and y
inline int RandInt(int x, int y)
{
	assert(y >= x && "<RandInt>: y is less than x");
	return rand() % (y - x + 1) + x;
}

//returns a random float between zero and 1
inline float RandFloat()
{
	return ((rand()) / (RAND_MAX + 1.0));
}

inline float RandInRange(float x, float y)
{
	return x + RandFloat() * (y - x);
}

//clamps the first argument between the second two
template<class T, class U, class V>
inline void Clamp(T& arg, const U& minVal, const V& maxVal)
{
	assert((minVal < maxVal) && "<Clamp>MaxVal < MinVal!");

	if ( arg < (T) minVal )
	{
		arg = (T) minVal;
	}

	if ( arg > (T) maxVal )
	{
		arg = (T) maxVal;
	}
}

inline bool LineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float *x, float *y)
{
	float mua, mub;
	float denom, numera, numerb;

	denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
	numera = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
	numerb = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);

	/* Is the intersection along the the segments */
	mua = numera / denom;
	mub = numerb / denom;
	if ( mua < 0 || mua > 1 || mub < 0 || mub > 1 )
	{
		*x = 50;
		*y = 50;
		return false;
	}

	*x = x1 + mua * (x2 - x1);
	*y = y1 + mua * (y2 - y1);
	return true;
}

#define BUFFER_OFFSET_BYTE(i) ((GLubyte*)0 + (i) )
#define BUFFER_OFFSET_SHORT(i) ((GLushort*)0 + (i) )

inline int GetInt(std::string &s) {
	return std::atoi(s.c_str());
}

inline bool IsEqualString(const char *s1, const char *s2) {
	return (std::strcmp(s1,s2)==0);
}


inline void XOR(const byte_t *a1, const byte_t *a2, int len, byte_t *dst) {
	for (int i=0; i<len; ++i) {
		dst[i] = *(a1++) ^ *(a2++);
	}
}

inline void RLEWrite_Bit(int bit, int totalBits, byte_t **dst) {
	// kind of bit
	if (bit == 0)
		*(*dst)++ = 'z';
	else
		*(*dst)++ = 'o';

	// qty
	char numBitsStr[6]; // max num is 99999
	sprintf(numBitsStr, "%d", totalBits);
	char *str = numBitsStr;
	while (*str) {
		*(*dst)++ = *str++;
	}
}

inline void RLECompress_Bit(const byte_t *src, int len, byte_t *dst) {

	int curBit = -1;
	int totalCurBit = 0;

	// for each byte of src
	for (int i = 0; i < len; ++i) {

		byte_t curByte = *src++;

		// for the 8 bits of the byte
		for (int j=0; j<8; ++j) {

			int bit = (curByte >> (7-j)) & 1;

			if (curBit == -1) {
				curBit = bit;
			}

			// if changing bit
			if (bit != curBit) {
				// we write total number of same bit in the buffer
				RLEWrite_Bit(curBit, totalCurBit, &dst);
				//reset current total
				totalCurBit = 0;
				// the new current bit becomes ones' complement of the current bit
				curBit = (curBit)?0:1;
			}

			++totalCurBit;
		}
	}

	RLEWrite_Bit(curBit, totalCurBit, &dst);

}


#if PLATFORM == PLATFORM_MAC
inline void ChangeDirectoryMacosx() {
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
		printf("error on CFURLGetFileSystemRepresentation\n");
	CFRelease(resourcesURL);
	chdir(path);
}
#endif

#endif // UTILS_H_INCLUDED


