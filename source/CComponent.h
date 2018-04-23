#pragma once

#include "std.h"

#include "CGameObjectWithMeshPickable.h"
#include "CComponent.h"
#include "CSide.h"

class CFace;

class CComponent : public CGameObjectWithMeshPickable ,virtual public makeFinal<CComponent>{
public:
    enum EnumMovementType {
        MOVEMENTTurnX =0,
    	MOVEMENTTurnY =1,
    	MOVEMENTTurnZ =2,
    	MOVEMENTTranslY =3
    };
    enum EnumState {
        STATEBeeingIn,
    	STATEBeeingOut,
    	STATEMoving
    };
	const static DWORD m_fvf;
protected:
	CFace *m_parent;
	DWORD m_lastTime;
	struct SVertex{
		SVertex(float p_x,float p_y,float p_z,float p_nx,float p_ny,float p_nz)
		{
			x=p_x;
			y=p_y;
			z=p_z;
			nx=p_nx;
			ny=p_ny;
			nz=p_nz;
		}
		SVertex(){};
		float x,y,z,nx,ny,nz;
	};
	CSide *m_sides[4];
	EnumState m_currState;
	EnumMovementType m_currMovementType;
	int m_currMovementDir;
	float m_translationY;
	D3DXMATRIX m_rotation;
	float m_currRotation;
	int m_transformCounter;
	float m_maxTranslationY,m_minTranslationY;
	int m_fits;

	CComponent(const CComponent & source){};
	CComponent & operator=(const CComponent & source){};
public:
	CComponent(float p_cubeSize, int p_dividing,const CSide *const p_sides[4], float p_maxTranslationY);
	virtual ~CComponent();

	virtual void update();
	const CSide *getSide(int p_num) const;
	EnumState getState();
	void rotateImmediately(int p_movementDir, CComponent::EnumMovementType p_movementType) ;
	void move(int p_movementDir, EnumMovementType p_movementType);
	void setParent(CFace *p_parent);
	CFace *getParent(){return m_parent;}
	virtual void onOver();
	virtual void onOut();
	void onSelect();
	void onDeselect();
	int fits(){return m_fits;}
private:
	void rotateSides(int p_movementDir, CComponent::EnumMovementType p_movementType);
	void generateFacesNY(float p_cubeSize,int p_dividing,int p_y,const CSide *const p_sides[4],
		SVertex *vBData,WORD *iBData, unsigned int &vBPos,unsigned int &iBPos);
	void generateFacesNXZ(float p_cubeSize,int p_dividing,const CSide *const p_sides[4],
		SVertex *vBData,WORD *iBData, unsigned int &vBPos,unsigned int &iBPos);
};
