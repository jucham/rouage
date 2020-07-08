#include <game/client/Camera.h>
#include <base/Stats.h>
#include <gtest/gtest.h>

TEST(GetAverageTest, Misc) {

	StatCalc ac(10);
	for (int i = 0; i < 10; i++)
	{
		ac.AddValue(i);
	}
	float avg = ac.GetAverage();
	ASSERT_FLOAT_EQ(4.5f, avg);

	StatCalc ac2(100);
	for (int i = 0; i < 100; i++)
	{
		ac2.AddValue(123);
	}
	avg = ac2.GetAverage();
	ASSERT_FLOAT_EQ(123.0f, avg);
}



