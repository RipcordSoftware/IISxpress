#pragma once

template <typename T> class AutoArray
{

public:

	AutoArray() : m_pT(NULL), m_N(-1) {}
	explicit AutoArray(T* pT) : m_pT(pT), m_N(-1) {}
	explicit AutoArray(int N) : m_pT(new T[N]), m_N(N) {}

	~AutoArray() { delete[] m_pT; }

	AutoArray(AutoArray<T>& other) { m_pT = other.m_pT; m_N = other.m_N; other.m_pT = NULL; other.m_N = -1; }
	AutoArray<T>& operator =(AutoArray<T>& other) { Destroy(); m_pT = other.m_pT; other.m_pT = NULL; m_N = other.m_N; other.m_N = -1; return *this; }

	operator T*() { return m_pT; }

	bool operator !() { return m_pT == NULL; }

	void Destroy() { delete[] m_pT; m_pT = NULL; m_N = -1; }	

	int Size() { return m_N; }

	typedef T value_type;
	static const size_t elem_size = sizeof(T);

private:	

	T* m_pT;
	int m_N;
};
