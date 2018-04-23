#pragma once

#include "std.h"

class CFace;

#include "CGameObjectWithMeshPickable.h"

class CArrow : public CGameObjectWithMeshPickable,virtual public makeFinal<CArrow> {
	CFace *m_parent;
	D3DXMATRIX m_ownTransform;
	int m_visible;
public:
	CArrow(const char *p_filename, float p_cubeSize, int p_dividing,const D3DXMATRIX &p_transform);
	virtual ~CArrow();

	virtual void draw();
	virtual void update();
	virtual void onOver();
	virtual void onOut();
	virtual float picked(D3DXVECTOR3 p_origin,D3DXVECTOR3 p_direction)const;
	void setParent(CFace *p_parent);
	CFace *getParent(){return m_parent;}
	void show();
	void hide();
	int getVisibility();
};
