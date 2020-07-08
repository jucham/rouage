#ifndef STATS_H_
#define STATS_H_

#include <cassert>

class StatCalc
{

public:
	StatCalc(int numSampleToCatch) :
			m_NumSamplesToCatch(numSampleToCatch), m_pCurSample(&m_Samples[0])
	{
		assert(numSampleToCatch <= MAX_SAMPLES);
	}

	void AddValue(float val)
	{
		*m_pCurSample++ = val;
	}

	bool AllSampleCollected() const
	{
		return (m_pCurSample - m_Samples) == m_NumSamplesToCatch;
	}

	float GetAverage()
	{
		m_pCurSample = &m_Samples[0];
		float total = 0;
		for (int i = 0; i < m_NumSamplesToCatch; i++)
		{
			total += m_Samples[i];
		}
		return total / m_NumSamplesToCatch;
	}

private:
	static const int MAX_SAMPLES = 100;
	float m_Samples[MAX_SAMPLES];
	int m_NumSamplesToCatch;
	float *m_pCurSample;

};

#endif /* STATS_H_ */
