#pragma once


#include "CGameObject.h"
#include "CFace.h"

class CContainer : public CGameObject {
  protected:
	CFace *m_faces[6];

  public:
	CContainer();
	virtual ~CContainer();
	void draw();
	void update();
	virtual int fits(CFace *p_sender,const CSide *const p_sides[4]);
	inline CFace *getFace(int p_index);
	void setTransform(const D3DXMATRIX & p_transform);
};

inline CFace *CContainer::getFace(int p_index) 
{
	return m_faces[p_index];
}
