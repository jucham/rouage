#ifndef NETPIECE_H_
#define NETPIECE_H_

#include <cassert>
#include <fstream>
#include <iostream>
#include <engine/shared/Common.h>
#include <base/utils.h>
#include "NetDataArray.h"

class NetPiece : public NetDataArray<>
{
public:

	enum {
		STAT_MSG,
		STAT_INPUT,
		STAT_SNAPSHOT,
		STAT_NOISE,
		NUM_STAT_TYPES
	};

	NetPiece();
	NetPiece(int comType, int dataType);

	void Reset();

	void Print(int numBytesToPrint = 10, bool interpretByte = true) const;
	void Print(int byteStart, int byteEnd) const;
	void PrintRaw() const;

	bool isInput() const;

	void SetClientId(int clientId)
	{
		m_ClientId = clientId;
	}

	int GetClientId() const {
		return m_ClientId;
	}

	void SetAsFirstPacket() {
		m_FirstPacket = true;
	}

	bool isFirstPacket() const {
		return m_FirstPacket;
	}

	// to be call after data retrieving so that GetSize() returns the right value
	void FeedStats() {

		const byte_t* pData = &m_Data[0];
		int wordOccurences[256] = {0};

		// count first byte (communication type)
		wordOccurences[*pData]++;
		++pData;

		//data type
		WordStats *pWordStats = 0;
		if ( *pData == NET_MSG )
			pWordStats = &m_WordStats[STAT_MSG];
		else if ( *pData == NET_INPUTS )
			pWordStats = &m_WordStats[STAT_INPUT];
		else if ( *pData == NET_SNAPSHOT )
			pWordStats = &m_WordStats[STAT_SNAPSHOT];
		else
			pWordStats = &m_WordStats[STAT_NOISE];

		// count second byte (data type)
		wordOccurences[*pData]++;
		++pData;

		const int dataSize = GetSize();

		// get occurence for each character
		for(int i=2; i<dataSize; i++) {
			wordOccurences[*pData]++;
			++pData;
		}

		// compute the frequency in percent and add this to the rest
		pWordStats->Feed(wordOccurences, dataSize);

		if (pWordStats->AllSamplesCollected()) {
			pWordStats->ComputeAverages();
			pWordStats->Save();
		}
	}

	static void InitWordStats(int sampleToCollect);

	class WordStats {

	public:

		static const int NUM_WORD = 256;

		void Init(const char *type, const char *filename, int countMax) {
			std::strcpy(this->type, type);
			std::strcpy(this->filename, filename);
			this->countMax = countMax;
			firstCompute = true;

			for(int i=0; i<NUM_WORD; i++) {
				frequencies[i] = 0.0;
			}

			Reset();
		}

		void Feed(const int *pWordOccurences, int dataSize) {
			for (int i=0; i<NUM_WORD; i++) {
				frequencies[i] += static_cast<double>(pWordOccurences[i]) / dataSize;
			}
			count += 1;
		}

		bool AllSamplesCollected() const {return count >= countMax;}

		void ComputeAverages() {

			for(int i=0; i<NUM_WORD; i++) {
				if (firstCompute) {
					frequencies[i] /= count;
				} else {
					frequencies[i] /= (count+1);
				}
			}

			firstCompute = false;
			Reset();
		}

		void Save() {
			printf("Save %s stats\n", type);
			std::ofstream stream;
			stream.open(filename, std::ofstream::out);

			char str[100];
			for(int i=0; i<256; i++) {
				std::sprintf(str, "%d %0.6lf\n", i, frequencies[i]);
				stream << str;
			}
			stream.close();
		}

		const double * GetFrequencies() const {
			return frequencies;
		}

		void Print() const {
			for(int i=0; i<256; i++) {
				printf("[%0.2lf]", frequencies[i]);
			}
			printf("\n");
		}


	private:
		char type[10];
		int count; // count the sample treated
		int countMax;
		double frequencies[NUM_WORD]; // sum of sampled frequencies for each of 256 words
		char filename[50];
		bool firstCompute;

		void Reset() {
			count = 0;
		}

	};

	const WordStats & GetWordStats(int type) {
		assert(type >= STAT_MSG && type < NUM_STAT_TYPES);
		return m_WordStats[type];
	}

private:

	int m_ClientId;
	bool m_FirstPacket;

	static WordStats m_WordStats[NUM_STAT_TYPES];

};

#endif /* NETPIECE_H_ */
