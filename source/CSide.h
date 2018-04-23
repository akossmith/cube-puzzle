#pragma once

#include "std.h"

class CSide{
protected:
	//a konstruktorban lefoglalok egy megfelelo meretu char tombot
	char *m_values;
	int m_size;
public:
	CSide();
	CSide(int p_size);
	CSide(int p_size,char *p_values);
	~CSide();
	CSide(const CSide &p_source);
	CSide & operator=(const CSide &p_source);
	CSide & operator=(char p_source[]);
	inline int size();
	int operator==(const CSide &p_other) const;
	int operator!=(const CSide &p_other) const; 
	int conflicts(const CSide &p_other) const; 
	CSide operator~() const;
	CSide reverse() const;
	char operator[](int p_index) const;
	char &operator[](int p_index);
};

inline int CSide::size()
{
	return m_size;
}