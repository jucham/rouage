#include <engine/shared/Huffman.h>
#include <gtest/gtest.h>
#include <base/utils.h>


TEST(CompressDecompressTest, ShortData) {

	// --- ARRANGE ---
	double patternFrequencies[Huffman::NUM_LITERALS];

	for (int i=0; i<Huffman::NUM_LITERALS; i++) {
		patternFrequencies[i] = 0.05 / (Huffman::NUM_LITERALS-4);
	}
	patternFrequencies[101] = 0.05;
	patternFrequencies[102] = 0.2;
	patternFrequencies[103] = 0.3;
	patternFrequencies[104] = 0.4;

	Huffman huffman(patternFrequencies);

	byte_t uncompData_Input[4096] = {0};
	byte_t uncompData_Output[4096] = {0};
	byte_t compData_Output[1024] = {0};

	byte_t *pUncompData_Input = &uncompData_Input[0];
	byte_t *pUncompData_Output = &uncompData_Output[0];
	byte_t *pCompData_Output = &compData_Output[0];

	int numBytes = 5;
	uncompData_Input[0] = 101; // 1101
	uncompData_Input[1] = 102; // 111
	uncompData_Input[2] = 103; // 10
	uncompData_Input[3] = 104; // 0
	uncompData_Input[4] = 201; // 1100 1101 1111

	// --- ACT ---
	int numBits = huffman.Compress(pUncompData_Input, pCompData_Output, numBytes);
	huffman.Uncompress(pCompData_Output, pUncompData_Output, numBits);

	// --- ASSERT ---
	ASSERT_EQ(22, numBits);
	ASSERT_EQ(0xDF, pCompData_Output[0]);
	ASSERT_EQ(0x33, pCompData_Output[1]);
	ASSERT_EQ(0x7C, pCompData_Output[2]);
	for (int i = 0; i < 5; ++i) {
		ASSERT_EQ(pUncompData_Input[i], pUncompData_Output[i]);
	}
}

TEST(CompressDecompressTest, LongData) {

	// --- ARRANGE ---
	double patternFrequencies[Huffman::NUM_LITERALS];

	for (int i=0; i<Huffman::NUM_LITERALS; i++) {
		patternFrequencies[i] = 0.05 / (Huffman::NUM_LITERALS-4);
	}
	patternFrequencies[101] = 0.05;
	patternFrequencies[102] = 0.2;
	patternFrequencies[103] = 0.3;
	patternFrequencies[104] = 0.4;

	Huffman huffman(patternFrequencies);

	byte_t uncompData_Input[4096] = {0};
	byte_t uncompData_Output[4096] = {0};
	byte_t compData_Output[1024] = {0};

	byte_t *pUncompData_Input = &uncompData_Input[0];
	byte_t *pUncompData_Output = &uncompData_Output[0];
	byte_t *pCompData_Output = &compData_Output[0];

	int numBytes = 2000;

	std::pair<int,int> intervals[numBytes];
	intervals[0].first = 0;
	intervals[0].second = numBytes-1;
	int curNumIntervals = 1;

	for (int i = 0; i < numBytes; ++i) {

		// pick interval and value index
		int interIndex = RandInt(0, curNumIntervals-1);
		std::pair<int,int> &inter = intervals[interIndex];
		int valIndex = RandInt(inter.first, inter.second);
		// MaJ Intervals
		if (inter.first == inter.second) {
			std::swap(intervals[interIndex], intervals[curNumIntervals-1]);
			--curNumIntervals;
		} else if (valIndex == inter.first ) {
			++inter.first;
		} else if (valIndex == inter.second) {
			--inter.second;
		} else {
			// on crée un nouvel intervalle valIndex+1 à n
			intervals[curNumIntervals].first = valIndex+1;
			intervals[curNumIntervals].second = inter.second;
			++curNumIntervals;
			//on configure l'intervalle choisit pour aller de 0 à valIndex-1
			inter.second = valIndex-1;
		}

		// ---

		if (i < 800) {
			uncompData_Input[valIndex] = 104;
		} else if (i < 1400) {
			uncompData_Input[valIndex] = 103;
		} else if (i < 1800) {
			uncompData_Input[valIndex] = 102;
		} else if (i < 1900) {
			uncompData_Input[valIndex] = 101;
		} else {
			uncompData_Input[valIndex] = 201;
		}
	}

	// --- ACT ---
	int numBits = huffman.Compress(pUncompData_Input, pCompData_Output, numBytes);
	int numUncompBytes = huffman.Uncompress(pCompData_Output, pUncompData_Output, numBits);

	printf("numUncompBytes = %d\n", numUncompBytes);
	ASSERT_EQ(2000, numUncompBytes);

	double compressionRatio = numBytes*8.0 / numBits;
	double spaceSavings = 1.0 - (1.0/compressionRatio);

//	printf("%d bits (%d bytes) compressed in %d bits (%d bytes).\n", numBytes*8, numBytes, numBits, numBits/8);
//	printf("Compression ratio : %0.1lf.\n", compressionRatio);
//	printf("Space savings : %0.2lf %%.\n", spaceSavings);


	// --- ASSERT ---
	for (int i = 0; i < numBytes; ++i) {
		ASSERT_EQ(pUncompData_Input[i], pUncompData_Output[i]);
	}

}
