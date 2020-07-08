#include <base/CircularBuffer.h>
#include <gtest/gtest.h>


TEST(AddTest, Void) {

	// --- ARRANGE ---
	CircularBuffer<int, 10> cb;

	// --- ASSERT ---
	ASSERT_EQ(cb.Size(), 0);
}

TEST(AddTest, FullFill) {

	// --- ARRANGE ---
	CircularBuffer<int, 10> cb;

	// --- ACT ---
	for (int i = 0; i < 10; ++i) {
		cb.Add(i);
	}

	// --- ASSERT ---
	ASSERT_EQ(cb.Size(), 10);
	for (int i = 0; i < 10; ++i)
		ASSERT_EQ(i, cb[i]);
}

TEST(AddTest, DoubleFullFill) {

	// --- ARRANGE ---
	CircularBuffer<int, 10> cb;

	// --- ACT ---
	for (int i = 0; i < 20; ++i) {
		cb.Add(i);
	}

	cb.ToString();

	// --- ASSERT ---
	ASSERT_EQ(cb.Size(), 10);
	for (int i = 0; i < 9; ++i)
		ASSERT_EQ(i+10, cb[i]);
}

TEST(AddTest, FullFillPlusOne) {

	// --- ARRANGE ---
	CircularBuffer<int, 10> cb;

	// --- ACT ---
	for (int i = 0; i < 11; ++i) {
		cb.Add(i);
	}

	cb.ToString();

	// --- ASSERT ---
	ASSERT_EQ(cb.Size(), 10);
	for (int i = 1; i < 9; ++i)
		ASSERT_EQ(i+1, cb[i]);
	ASSERT_EQ(10, cb[9]);

	ASSERT_EQ(10, cb.Last());

}

TEST(AddTest, HalfFill) {

	// --- ARRANGE ---
	CircularBuffer<int, 10> cb;

	// --- ACT ---
	for (int i = 0; i < 5; ++i) {
		cb.Add(i);
	}

	// --- ASSERT ---
	ASSERT_EQ(cb.Size(), 5);
	for (int i = 0; i < 5; ++i)
		ASSERT_EQ(i, cb[i]);
}
