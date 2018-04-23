#include "std.h"
#include "CRepository.h"
//#include "CGame.h"
CRepository::CRepository(float p_cubeSize,int p_dividing,CEvent * p_facesOnDown) 
{
	D3DXMATRIX rotation,translation,   temp;
	D3DXMatrixRotationYawPitchRoll(&m_transform,0.0f,D3DX_PI/2-0.4f,0.0f);
	D3DXMatrixTranslation(&temp,-0.6f,-8.0f,20.0f);
	m_transform=m_transform*temp;
	D3DXMatrixRotationYawPitchRoll(&rotation,0.0f,-D3DX_PI/2,0.0f);
	for(int i=0;i<4;i++){	
		//vízszintes:
		D3DXMatrixTranslation(&translation,-12.0f+i*8.0f,0.0f,0.0f);
		m_faces[i]=new CFace(p_cubeSize,p_dividing,this,rotation*translation);
	}
	D3DXMatrixTranslation(&translation,-12.0f,8.0f,0.0f);
	m_faces[4]=new CFace(p_cubeSize,p_dividing,this,rotation*translation);
	D3DXMatrixTranslation(&translation,-12.0f+3*8.0f,8.0f,0.0f);
	m_faces[5]=new CFace(p_cubeSize,p_dividing,this,rotation*translation);

	for(int i=0;i<6;i++){
		if(m_faces[i])
			m_faces[i]->setOnDownEvent(p_facesOnDown);
	}
}

CRepository::~CRepository() 
{
	//az m_faces törlését megcsinálja az örökölt dtor.
}

//fits() mindig return 0