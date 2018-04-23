#pragma once

#include "std.h"

#include "CContainer.h"

class CCube : public CContainer {
protected:
	D3DXMATRIX m_rotation,m_traslation;
public:
	CCube(float p_cubeSize,int p_dividing,CEvent * p_facesOnDown);
	virtual ~CCube();
	int fits(CFace *p_sender,const CSide *const p_sides[4]);
	virtual void update();
	inline void setRotation(D3DXMATRIX &p_rotation);
	inline const D3DXMATRIX &getRotation();
};

inline void CCube::setRotation(D3DXMATRIX &p_rotation)
{
	m_rotation=p_rotation;
}

inline const D3DXMATRIX &CCube::getRotation()
{
	return m_rotation;
}