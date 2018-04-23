#pragma once

#pragma comment(lib,"d3d9.lib")

#pragma comment(lib,"winmm.lib")
#ifdef _DEBUG
#define D3D_DEBUG_INFO
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <D3d9.h>
#include <D3dx9.h>
#include <mmsystem.h> //WIN32_LEAN_AND_MEAN miatt

#define INFINITEDISTANCE FLT_MAX

template <class T>
class makeFinal{
	friend T;
	makeFinal(){}
	~makeFinal(){}
};

class EError{
public:
	char text[100];
	EError(const char *p_text){	strncpy(text,p_text,100);}
};

#ifndef _DEBUG
#define MYASSERT(X)
#else
#define MYASSERT(X) \
	if(!(X))__asm int 3;
#endif