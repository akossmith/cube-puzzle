#pragma once

#include "std.h"
#include "CGameObjectWithMeshPickable.h"
#include "CComponent.h"

class CContainer;
class CSide;

class CFace : public CGameObjectWithMeshPickable,virtual public makeFinal<CFace> {
protected:
	CContainer *m_parent;
	CComponent *m_child;
	IDirect3DTexture9 *m_texture;
	D3DXMATRIX m_ownTransform;
	
	CFace(const CFace & source);//egyelõre tiltva
	CFace & operator=(const CFace & source);//egyelõre tiltva
	struct SVertex{
		SVertex(float p_x,float p_y,float p_z,float p_nx,float p_ny,float p_nz,float p_u,float p_v)
		{
			x=p_x;
			y=p_y;
			z=p_z;
			nx=p_nx;
			ny=p_ny;
			nz=p_nz;
			u=p_u;
			v=p_v;
		}
		SVertex(){};
		float x,y,z,nx,ny,nz,u,v;
	};
public:
	const static DWORD m_fvf;
	CFace(float p_cubeSize, int p_dividing,CContainer *p_container,const D3DXMATRIX &p_transform);
	virtual ~CFace();
	
	//ezzel lehet lekérdezni, h egy lapszélek illeszkedik-e
	int fits(const CSide *const p_sides[4]);
	virtual void update();
	virtual void draw();
	virtual void onOver();
	virtual void onOut();
	const CSide *getSide(int p_index) const;
	inline CComponent *getChild();
	void setChild(CComponent *value);
	CContainer *getParent(){return m_parent;}
	const D3DXMATRIX &getOwnTransform()const {return m_ownTransform;}
	void setOwnTransform(const D3DXMATRIX &p_transform){m_ownTransform=p_transform;}
};

inline CComponent *CFace::getChild() 
{
	  return m_child;
}
