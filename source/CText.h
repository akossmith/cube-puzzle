#pragma once

#include "std.h"
#include "CGameObjectWithMeshPickable.h"

class CText : public CGameObjectWithMeshPickable,virtual public makeFinal<CText>{
	int m_static;
public:
	CText(const char *p_text,float p_depth,int p_bold,int p_static);
	virtual ~CText();
	virtual void onOver();
	virtual void onOut();
	void setTransform(const D3DXMATRIX &p_transform){m_transform=p_transform;}
};
