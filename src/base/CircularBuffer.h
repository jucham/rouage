#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <cstdlib>
#include <cassert>
#include <cstdio>

template<typename T, size_t S>
class CircularBuffer {

private:
	T m_Array[S];
	unsigned int m_Size;
	int m_Last;

public:
	CircularBuffer() {Reset();}

	void Reset() {
		m_Size = 0;
		m_Last = -1;
	}

	void Add(T &e) {
		m_Last++;
		m_Last %= S;
		m_Array[m_Last] = e;
		if (m_Size < S)
			m_Size++;
	}

	T & operator[](unsigned int i) {
		assert(i < S);
		return m_Array[(m_Last+1 + (S-m_Size) + i) % S];
	}

	const T & operator[](unsigned int i) const {
		assert(i < S);
		return m_Array[(m_Last+1 + (S-m_Size) + i) % S];
	}

	T Last() const {
		return m_Array[m_Last];
	}

	T & Last() {
		return m_Array[m_Last];
	}

	int Size() const {
		return m_Size;
	}

	void ToString()  const {
		for (int i = 0; i < S; ++i) {
			std::printf("m_Array[%d] = %d\n", i, m_Array[i]);
		}
		std::printf("m_Last is %d\n", m_Last);

		for (int i = 0; i < S; ++i) {
			std::printf("operator[%d] = %d\n", i, operator[](i));
		}
		std::printf("Last() is %d\n", Last());
	}

};

#endif /* CIRCULARBUFFER_H_ */
