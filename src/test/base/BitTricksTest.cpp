#include <base/BitTricks.h>
#include <gtest/gtest.h>


TEST(CompressAndCopyBitsTest, OneByteFull) {

	// --- ARRANGE ---
	byte_t bits[] = {0,1,0,1,0,1,0,1};
	byte_t compBits = 0;

	const byte_t *pBits = &bits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopy8Bits(pBits, &compBits, 0, 8);

	// --- ASSERT ---
	ASSERT_EQ(0x55, compBits);
	ASSERT_EQ(0, cursor);
}

TEST(CompressAndCopyBitsTest, OneByteHalf) {

	// --- ARRANGE ---
	byte_t bits[] = {0,1,0,1,0,0,0,0};
	byte_t compBits = 0;

	const byte_t *pBits = &bits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopy8Bits(pBits, &compBits, 0, 4);

	// --- ASSERT ---
	ASSERT_EQ(0x50, compBits);
	ASSERT_EQ(4, cursor);
}


TEST(CompressAndCopyBitsTest, OneByteWithRandomCursor1) {

	// --- ARRANGE ---
	byte_t bits[] = {0,1,0,1};
	byte_t compBits = 0x50;

	const byte_t *pBits = &bits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopy8Bits(pBits, &compBits, 4, 4);

	// --- ASSERT ---
	ASSERT_EQ(0x55, compBits);
	ASSERT_EQ(0, cursor);
}


TEST(CompressAndCopyBitsTest, OneByteWithRandomCursor2) {

	// --- ARRANGE ---
	byte_t bits[] = {1,1};
	byte_t compBits = 0x80;

	const byte_t *pBits = &bits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopy8Bits(pBits, &compBits, 1, 2);

	// --- ASSERT ---
	ASSERT_EQ(0xE0, compBits);
	ASSERT_EQ(3, cursor);
}

TEST(CompressAndCopyBitsMultipleTest, Simple) {

	// --- ARRANGE ---
	byte_t bits[] = {1,0,1,0, 0,1,0,1};
	byte_t compBits[1] = {0};
	byte_t *pCompBits = &compBits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopyBits(&bits[0], &pCompBits, 8, 0);

	// --- ASSERT ---
	ASSERT_EQ(0xA5, compBits[0]);
	ASSERT_EQ(0, cursor);
}

TEST(CompressAndCopyBitsMultipleTest, OneCall) {

	// --- ARRANGE ---
	byte_t bits1[] = {1,0,1,0, 0,1,0,1, 1,1,1,1};
	byte_t compBits[2] = {0};
	byte_t *pCompBits = &compBits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopyBits(&bits1[0], &pCompBits, 12, 0);

	// --- ASSERT ---
	ASSERT_EQ(0xA5, compBits[0]);
	ASSERT_EQ(0xF0, compBits[1]);
	ASSERT_EQ(4, cursor);
}

TEST(CompressAndCopyBitsMultipleTest, OneCallVeryLongBitsWord) {

	// --- ARRANGE ---
	byte_t bits1[] = {0,0,0,1, 0,0,1,0, 0,0,1,1, 0,1,0,0, 0,1,0,1, 0,1,1,0, 1};
	byte_t compBits[4] = {0};
	byte_t *pCompBits = &compBits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopyBits(&bits1[0], &pCompBits, 25, 0);

	// --- ASSERT ---
	ASSERT_EQ(0x12, compBits[0]);
	ASSERT_EQ(0x34, compBits[1]);
	ASSERT_EQ(0x56, compBits[2]);
	ASSERT_EQ(0x80, compBits[3]);
	ASSERT_EQ(1, cursor);
}


TEST(CompressAndCopyBitsMultipleTest, ThreeCalls) {

	// --- ARRANGE ---
	byte_t bits1[] = {1,0,1,0, 0,1,0,1, 1,1,1,1};
	byte_t compBits[3] = {0};
	byte_t *pCompBits = &compBits[0];

	// --- ACT ---
	byte_t cursor = BitTricks::PackAndCopyBits(&bits1[0], &pCompBits, 12, 0);

	// --- ASSERT ---
	ASSERT_EQ(0xA5, compBits[0]);
	ASSERT_EQ(0xF0, compBits[1]);
	ASSERT_EQ(4, cursor);


	// --- ARRANGE ---
	byte_t bits2[] = {0,1,0};

	// --- ACT ---
	cursor = BitTricks::PackAndCopyBits(&bits2[0], &pCompBits, 3, cursor);

	// --- ASSERT ---
	ASSERT_EQ(0xA5, compBits[0]);
	ASSERT_EQ(0xF4, compBits[1]);
	ASSERT_EQ(7, cursor);


	// --- ARRANGE ---
	byte_t bits3[] = {1,0,1,1,1};

	// --- ACT ---
	cursor = BitTricks::PackAndCopyBits(&bits3[0], &pCompBits, 5, cursor);

	// --- ASSERT ---
	ASSERT_EQ(0xA5, compBits[0]);
	ASSERT_EQ(0xF5, compBits[1]);
	ASSERT_EQ(0x70, compBits[2]);
	ASSERT_EQ(4, cursor);
}

TEST(GetBytesNumberTest, One) {

	// --- ARRANGE ---
	int numBits[6] = {1, 7, 8, 12, 15, 17};
	int numBytes[6] = {0};

	// --- ACT ---
	for (int i = 0; i < 6; ++i) {
		numBytes[i] = BitTricks::GetBytesNumber(numBits[i]);
	}

	// --- ASSERT ---
	ASSERT_EQ(1, numBytes[0]);
	ASSERT_EQ(1, numBytes[1]);
	ASSERT_EQ(1, numBytes[2]);
	ASSERT_EQ(2, numBytes[3]);
	ASSERT_EQ(2, numBytes[4]);
	ASSERT_EQ(3, numBytes[5]);
}



