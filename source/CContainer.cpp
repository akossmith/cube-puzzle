
#include "CContainer.h"
#include "CFace.h"

CContainer::CContainer() 
{
	for(int i=0;i<6;i++){
		m_faces[i]=NULL;
	}
}

CContainer::~CContainer() 
{
	for(int i=0;i<6;i++){
		if(m_faces[i]){
			delete m_faces[i];
		}
	}
}

void CContainer::draw() 
{
	for(int i=0;i<6;i++)
		if(m_faces[i])
			m_faces[i]->draw();
}

void CContainer::update()
{
	for(int i=0;i<6;i++)
		if(m_faces[i])
			m_faces[i]->update();
}

int CContainer::fits(CFace *p_sender,const CSide *const p_sides[4]) 
{
	return 0;
}

void CContainer::setTransform(const D3DXMATRIX & p_transform) 
{
}

