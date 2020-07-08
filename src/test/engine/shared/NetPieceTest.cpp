#include <engine/shared/NetPiece.h>
#include <game/client/Camera.h>
#include <gtest/gtest.h>


namespace {


class NetPieceTest : public ::testing::Test {

public:
	static const int NUM_WORD = NetPiece::WordStats::NUM_WORD;

protected:

	// You can remove any or all of the following functions if its body
	// is empty.
	NetPieceTest() {
		//NetPiece::InitWordStats(100);
	}

	virtual ~NetPieceTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for NetPiece.
};


TEST(WordStatsTest, AllsOneOccurrenceOneFeed) {

	// --- ARRANGE ---
	NetPiece::WordStats ws;
	ws.Init("TEST", "", 1);

	// --- ACT ---
	int dataSize = 4;
	int wordOccurences[NetPieceTest::NUM_WORD] = {0};
	for (int i=0; i<NetPieceTest::NUM_WORD; i++) {
		wordOccurences[i] = 1;
	}
	ws.Feed(wordOccurences, NetPieceTest::NUM_WORD);
	ws.ComputeAverages();

	// --- ASSERT ---
	const double *pFrequencies = ws.GetFrequencies();
	for (int w; w<NetPieceTest::NUM_WORD; w++) {
		ASSERT_DOUBLE_EQ(1.0/NetPieceTest::NUM_WORD, pFrequencies[w]);
	}

}

TEST(WordStatsTest, RandomOccurrencesOneFeed) {

	// --- ARRANGE ---
	NetPiece::WordStats ws;
	ws.Init("TEST", "", 1);

	// --- ACT ---
	int dataSize = 4;
	int wordOccurences[NetPieceTest::NUM_WORD] = {0};
	for (int i=1; i<dataSize; i++) {
		wordOccurences[i] = i;
	}
	ws.Feed(wordOccurences, dataSize);
	ws.ComputeAverages();

	// --- ASSERT ---
	const double *pFrequencies = ws.GetFrequencies();
	for (int w=0; w<dataSize; w++) {
		EXPECT_DOUBLE_EQ((double)w/dataSize, pFrequencies[w]);
	}
}


TEST(WordStatsTest, RandomOccurrencesMultipleFeeds) {

	// --- ARRANGE ---
	NetPiece::WordStats ws;
	// membre const
	ws.Init("TEST", "", 1);

	// --- ACT ---
	int dataSize = 3;

	int wordOccurences1[NetPieceTest::NUM_WORD] = {0,1,2};
	ws.Feed(wordOccurences1, dataSize);

	int wordOccurences2[NetPieceTest::NUM_WORD] = {2,0,1};
	ws.Feed(wordOccurences2, dataSize);

	int wordOccurences3[NetPieceTest::NUM_WORD] = {1,2,0};
	ws.Feed(wordOccurences3, dataSize);

	ws.ComputeAverages();
	double frequencies1[256];
	std::memcpy(frequencies1, ws.GetFrequencies(), 256*sizeof(double));

	int wordOccurences4[NetPieceTest::NUM_WORD] = {0,1,2};
	ws.Feed(wordOccurences4, dataSize);

	ws.ComputeAverages();
	double frequencies2[256];
	std::memcpy(frequencies2, ws.GetFrequencies(), 256*sizeof(double));

	// --- ASSERT ---
	for (int w=0; w<dataSize; w++) {
		ASSERT_DOUBLE_EQ(1/3.0, frequencies1[w]);
	}
	ASSERT_DOUBLE_EQ(1/6.0, frequencies2[0]); // (0.33 + 0/3) / 2
	ASSERT_DOUBLE_EQ(1/3.0, frequencies2[1]); // (0,33 + 1/3) / 2
	ASSERT_DOUBLE_EQ(0.5, frequencies2[2]); // (0,33 + 2/3) / 2

}


}  // namespace


