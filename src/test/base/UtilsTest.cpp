#include <base/utils.h>
#include <gtest/gtest.h>


TEST(XORTest, Test1) {

	// --- ARRANGE ---
	const int len = 3;
	const byte_t a1[len] = {0x11,0x22,0x33};
	const byte_t a2[len] = {0x33,0x22,0x11};
	byte_t a1_XOR_a2[len] = {0};

	// --- ACT ---
	XOR(a1, a2, len, a1_XOR_a2);

	// --- ASSERT ---
	byte_t a1_XOR_a2_expected[len] = {0x22, 0x00, 0x22};

	for (int i = 0; i < len; ++i) {
		ASSERT_EQ(a1_XOR_a2_expected[i], a1_XOR_a2[i]);
	}
}

TEST(XORTest, Test2) {
	// --- ARRANGE ---
	const int len = 3;
	const byte_t a1[len] = {0xAA,0xAA,0xAA};
	const byte_t a2[len] = {0x55,0x55,0x55};
	byte_t a1_XOR_a2[len] = {0};

	// --- ACT ---
	XOR(a1, a2, len, a1_XOR_a2);

	// --- ASSERT ---
	byte_t a1_XOR_a2_expected[len] = {0xFF, 0xFF, 0xFF};

	for (int i = 0; i < len; ++i) {
		ASSERT_EQ(a1_XOR_a2_expected[i], a1_XOR_a2[i]);
	}
}

TEST(RLETest, TestAllZero) {
	// --- ARRANGE ---
	const int LEN = 1400;
	const int COMP_LEN = 6;
	const byte_t src[LEN] = {0};
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'z','1','1','2','0','0'};
	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}

TEST(RLETest, TestAllOne) {
	// --- ARRANGE ---
	const int LEN = 1400;
	const int COMP_LEN = 6;
	byte_t src[LEN] = {0};
	for (int i = 0; i < LEN; ++i) { src[i] = 0xFF; }
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'o','1','1','2','0','0'};
	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}

TEST(RLETest, Test1) {
	// --- ARRANGE ---
	const int len = 3;
	const int compLen = 8;
	const byte_t src[len] = {0x0F,0xF0,0x0F};
	byte_t dst[compLen] = {0};

	// --- ACT ---
	RLECompress_Bit(src, len, dst);

	// --- ASSERT ---
	const byte_t dst_expected[compLen] = {'z','4','o','8','z','8','o','4'};

	for (int i = 0; i < len; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}


TEST(RLETest, Test2) {
	// --- ARRANGE ---
	const int LEN = 100;
	const int COMP_LEN = 6;
	byte_t src[LEN] = {0};
	src[99] = 1;
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'z','7','9','9','o','1'};

	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}


TEST(RLETest, Test3) {
	// --- ARRANGE ---
	const int LEN = 100;
	const int COMP_LEN = 6;
	byte_t src[LEN] = {0};
	src[99] = 3;
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'z','7','9','8','o','2'};

	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}

TEST(RLETest, Test4) {
	// --- ARRANGE ---
	const int LEN = 100;
	const int COMP_LEN = 8;
	byte_t src[LEN] = {0};
	src[99] = 2;
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'z','7','9','8','o','1', 'z', '1'};

	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}

TEST(RLETest, Test5) {
	// --- ARRANGE ---
	const int LEN = 100;
	const int COMP_LEN = 8;
	byte_t src[LEN] = {0};
	src[0] = 128;
	src[99] = 1;
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'o','1','z','7','9','8','o','1'};

	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}

TEST(RLETest, Test6) {
	// --- ARRANGE ---
	const int LEN = 10;
	const int COMP_LEN = 8;
	byte_t src[LEN] = {0};
	src[5] = 128;
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'z','4','0','o','1','z','3','9'};

	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}

TEST(RLETest, Test7) {
	// --- ARRANGE ---
	const int LEN = 1;
	const int COMP_LEN = 16;
	byte_t src[LEN] = {0};
	src[0] = 0x55;
	byte_t dst[COMP_LEN] = {0};

	// --- ACT ---
	RLECompress_Bit(src, LEN, dst);

	// --- ASSERT ---
	const byte_t dst_expected[COMP_LEN] = {'z','1','o','1','z','1','o','1','z','1','o','1','z','1','o','1'};

	for (int i = 0; i < COMP_LEN; ++i) {
		ASSERT_EQ(dst_expected[i], dst[i]);
	}
}
