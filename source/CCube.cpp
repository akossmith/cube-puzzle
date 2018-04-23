#include "CCube.h"
#include "CGame.h"
CCube::CCube(float p_cubeSize,int p_dividing,CEvent * p_facesOnDown) 
{
	D3DXMATRIX rotation,translation;

	D3DXMatrixTranslation(&translation,0,p_cubeSize/2,0.0f);
	m_faces[0]=new CFace(p_cubeSize,p_dividing,this,translation);
	D3DXMatrixRotationYawPitchRoll(&rotation,D3DX_PI/2,0.0f,D3DX_PI/2);
	m_faces[1]=new CFace(p_cubeSize,p_dividing,this,translation*rotation);
	D3DXMatrixRotationYawPitchRoll(&rotation,0.0f,D3DX_PI/2,-D3DX_PI/2);
	m_faces[2]=new CFace(p_cubeSize,p_dividing,this,translation*rotation);
	D3DXMatrixRotationYawPitchRoll(&rotation,0,D3DX_PI,0.0f);
	m_faces[3]=new CFace(p_cubeSize,p_dividing,this,translation*rotation);
	D3DXMatrixRotationYawPitchRoll(&rotation,-D3DX_PI/2,0.0f,D3DX_PI/2);
	m_faces[4]=new CFace(p_cubeSize,p_dividing,this,translation*rotation);
	D3DXMatrixRotationYawPitchRoll(&rotation,0.0f,-D3DX_PI/2,D3DX_PI/2);
	m_faces[5]=new CFace(p_cubeSize,p_dividing,this,translation*rotation);

	for(int i=0;i<6;i++){
		if(m_faces[i])
			m_faces[i]->setOnDownEvent(p_facesOnDown);
	}

	D3DXMatrixTranslation(&m_traslation,0.0f,2.0f,0.0f);
	D3DXMatrixIdentity(&m_rotation);
}

CCube::~CCube() 
{
	//az m_faces törlését megcsinálja az örökölt dtor.
}

void CCube::update()
{
	m_transform=m_rotation*m_traslation;
	CContainer::update();
}

int CCube::fits(CFace *p_sender,const CSide *const p_sides[4]) 
{
	int i=0,findex=-1;
	while(i<6 && p_sender!=m_faces[i])i++;
	if(i==6)
		return 0;
	findex=i;
	if(findex%2){
		if(m_faces[(findex+4)%6]->getSide(1) && p_sides[0] && (*m_faces[(findex+4)%6]->getSide(1)).reverse().conflicts(*p_sides[0])
			||m_faces[(findex+2)%6]->getSide(0) && p_sides[1] && (*m_faces[(findex+2)%6]->getSide(0)).reverse().conflicts(*p_sides[1])
			||m_faces[(findex+1)%6]->getSide(1) && p_sides[2] && (*m_faces[(findex+1)%6]->getSide(1)).reverse().conflicts(*p_sides[2])
			||m_faces[(findex+5)%6]->getSide(2) && p_sides[3] && (*m_faces[(findex+5)%6]->getSide(2)).reverse().conflicts(*p_sides[3]))
			return 0;
	}else{
		if(m_faces[(findex+4)%6]->getSide(3) && p_sides[0] && (*m_faces[(findex+4)%6]->getSide(3)).reverse().conflicts(*p_sides[0])
			||m_faces[(findex+5)%6]->getSide(2) && p_sides[1] && (*m_faces[(findex+5)%6]->getSide(2)).reverse().conflicts(*p_sides[1])
			||m_faces[(findex+1)%6]->getSide(3) && p_sides[2] && (*m_faces[(findex+1)%6]->getSide(3)).reverse().conflicts(*p_sides[2])
			||m_faces[(findex+2)%6]->getSide(0) && p_sides[3] && (*m_faces[(findex+2)%6]->getSide(0)).reverse().conflicts(*p_sides[3]))
			return 0;
	}
	return 1;
}

