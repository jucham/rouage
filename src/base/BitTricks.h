#ifndef BITTRICKS_H_
#define BITTRICKS_H_

#include <cassert>
#include <cstdio>
#include <base/Types.h>

class BitTricks
{

public:

	/*
	 * PackAndCopyBits --- Copy bits from an array where each bit
	 * is stored on 1 byte to a packed array where each bit is stored on 1 bit.
	 *
	 * pBits : a pointer to source array where each bit is stored on 1 byte.
	 * ppPackedBits : a pointer of pointer to destination array where each bit
	 *  is stored on 1 bit.
	 * cursor : an index (goes 0 to 7) which is location where to begin to copy
	 *  bits in pPackedBits.
	 * numBits : number of bits to copy
	 *
	 * return new location of cursor.
	 */
	static byte_t PackAndCopyBits(const byte_t *pBits,  byte_t **ppPackedBits, short numBits, byte_t cursor) {

		do {
			// 8 bits or more to copy
			if (numBits >= 8 && cursor == 0) {
				cursor = PackAndCopy8Bits(pBits, *ppPackedBits, cursor, 8);
				pBits += 8;
				numBits -= 8;
			}
			// less than 8 bits to copy
			else {
				int freeBits = 8 - cursor;

				// there is enough space
				if (numBits <= freeBits) {
					cursor = PackAndCopy8Bits(pBits, *ppPackedBits, cursor, numBits);
					pBits += numBits;
					numBits = 0;
				}
				// copy what we can and the rest will be done to the next iteration
				else {
					cursor = PackAndCopy8Bits(pBits, *ppPackedBits, cursor, freeBits);
					pBits += freeBits;
					numBits -= freeBits;
				}
			}

			// we have to go to next byte
			if (cursor == 0) {
				(*ppPackedBits)++;
			}

		} while (numBits > 0);

		return cursor;
	}

	/*
	 * PackAndCopy8Bits --- Copy from 1 to 8 bits from an array where each bit
	 * is stored on 1 byte to a packed array where each bit is stored on 1 bit.
	 *
	 * pBits : a pointer to source array where each bit is stored on 1 byte.
	 * pPackedBits : a pointer to destination array where each bit is stored on
	 *  1 bit.
	 * cursor : an index (goes 0 to 7) which is location where to begin to copy
	 *  bits in pPackedBits.
	 * numBits : number of bits to copy.
	 *
	 * return new location of cursor.
	 */
	static int PackAndCopy8Bits(const byte_t *pBits, byte_t *pPackedBits, byte_t cursor, byte_t numBits) {

		assert(numBits > 0 && numBits <= 8);

		for (int i=cursor; i < cursor+numBits; ++i) {
			*pPackedBits |= *pBits << (7-i);
			pBits++;
		}
		return (cursor+numBits)%8;
	}

	/*
	 * GetBytesNumber(int numBits) --- get the number of byte for a bit quantity
	 *
	 * numBits : number of bits
	 */
	static int GetBytesNumber(int numBits) {

		assert(numBits >= 0);

		return ((numBits%8==0) ? numBits/8 : numBits/8+1);
	}

	/*
	 * PrintByteAsBits --- Print a byte as a sequence of bits
	 *
	 * b : byte to print
	 */
	static void PrintByteAsBits(byte_t b) {
		for (int i = 7; i >= 0; --i) {
			printf("%d", (b>>i)&1);
		}
		printf("\n");
	}

};

#endif /* BITTRICKS_H_ */
