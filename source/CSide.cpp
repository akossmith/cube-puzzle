#include "std.h"
#include "CSide.h"

CSide::CSide():m_values(0),m_size(0)
{
}

CSide::CSide(int p_size):m_values(0),m_size(p_size)
{
	m_values=new char[p_size];
	ZeroMemory(m_values,p_size);
}

CSide::CSide(int p_size,char *p_values):m_size(p_size)
{
	m_values=new char[p_size];
	CopyMemory(m_values,p_values,p_size);
}

CSide::~CSide() 
{
	if(m_values)delete[] m_values;
}

CSide::CSide(const CSide &p_source) 
{
	m_size=p_source.m_size;
	m_values=new char[m_size];
	CopyMemory(m_values,p_source.m_values,m_size);
}

CSide &CSide::operator=(const CSide &p_source)
{
	if(&p_source!=this){
		m_size=p_source.m_size;
		if(m_values)delete[] m_values;
		m_values=NULL;
		m_values=new char[m_size];
		CopyMemory(m_values,p_source.m_values,m_size);
	}
	return *this;
}

CSide &CSide::operator=(char *p_source)
{
	CopyMemory(m_values,p_source,m_size);
	return *this;
}

int CSide::operator==(const CSide &p_other) const 
{
	for(int i=0;i<m_size;i++)
		if(p_other[i]!=m_values[i])return 0;
	return 1;
}

int CSide::operator!=(const CSide &p_other) const 
{
	for(int i=0;i<m_size;i++)
		if(p_other[i]!=m_values[i])return 1;
	return 0;
}

int CSide::conflicts(const CSide &p_other) const 
{
	for(int i=0;i<m_size && i<p_other.m_size;i++)
		if(p_other[i] && m_values[i])return 1;
	return 0;
}

CSide CSide::reverse() const 
{
	CSide temp(m_size);
	for(int i=0;i<m_size;i++){
		temp.m_values[i]=m_values[m_size-1-i];
	}
	return temp;
}

CSide CSide::operator~() const 
{
	CSide temp(m_size);
	for(int i=0;i<m_size;i++)
		temp.m_values[i]=1-m_values[i];
	return temp;
}

char CSide::operator[](int p_index) const 
{
	return m_values[p_index];
}

char &CSide::operator[](int p_index) 
{
	return m_values[p_index];
}

