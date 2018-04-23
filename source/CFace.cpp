#include "std.h"
#include "CGame.h"
#include "CFace.h"
#include "CContainer.h"
#include "CSide.h"

const DWORD CFace::m_fvf=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;

CFace::CFace(float p_cubeSize, int p_dividing,CContainer *p_container,const D3DXMATRIX &p_transform):
	m_texture(NULL),m_child(NULL),m_parent(NULL)
{
	SVertex *vBData=0;
	WORD *iBData=0;
	float unit=p_cubeSize/(p_dividing-2);

	m_parent=p_container;
	
	m_ownTransform=p_transform;
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(150,150,150))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
	m_material.Specular=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*0.0f;
	m_material.Emissive=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*0.0f;
	m_material.Power=0.0f;

	D3DXCreateTextureFromFile(CGame::instance()->getDevice(),"faceTexture.png",&m_texture);

	D3DXCreateMeshFVF(2,4,D3DXMESH_IB_MANAGED|D3DXMESH_VB_MANAGED,m_fvf,CGame::instance()->getDevice(),&m_mesh);
	m_mesh->LockVertexBuffer(0,(void **)&vBData);
	m_mesh->LockIndexBuffer(0,(void **)&iBData);

	vBData[0]=SVertex(-p_cubeSize/2,0,p_cubeSize/2,0,1,0,0,0);
	vBData[1]=SVertex(p_cubeSize/2,0,p_cubeSize/2,0,1,0,1,0);
	vBData[2]=SVertex(p_cubeSize/2,0,-p_cubeSize/2,0,1,0,1,1);
	vBData[3]=SVertex(-p_cubeSize/2,0,-p_cubeSize/2,0,1,0,0,1);
	iBData[0]=0;iBData[1]=1;iBData[2]=2;
	iBData[3]=0;iBData[4]=2;iBData[5]=3;
	
	m_mesh->UnlockIndexBuffer();
	m_mesh->UnlockVertexBuffer();
	subscribeToMsgs();// nem veszélyes, mert final
}

CFace::~CFace() 
{
	if(m_texture)m_texture->Release();
	//mesh release-e inherited
	unsubscribeFromMsgs();
}

CFace::CFace(const CFace & source) //private
{
}

CFace & CFace::operator=(const CFace & source) //private
{
	return *this;
}

//ezzel lehet lekérdezni, hogy a gyerek illeszkedik-e
int CFace::fits(const CSide *const p_sides[4]) 
{
	return (m_parent ? m_parent->fits(this,p_sides) : 0);
}

void CFace::update()
{
	if(m_parent)
		m_transform=m_ownTransform*m_parent->getTransform();
	else
		m_transform=m_ownTransform;
	if(m_child)
		m_child->update();
}

void CFace::draw()
{
	if(m_texture)
		CGame::instance()->getDevice()->SetTexture(0,m_texture);
	CGameObjectWithMesh::draw();
	if(m_texture)
		CGame::instance()->getDevice()->SetTexture(0,NULL);
	if(m_child)
		m_child->draw();
}

void CFace::onOver()
{
	if(!m_child){
		m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(170,170,170))*1.0f;
		m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*3.0f;
	}
}

void CFace::onOut()
{
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(150,150,150))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
}

const CSide *CFace::getSide(int p_index) const 
{
	if(!m_child || m_child->getState()!=CComponent::STATEBeeingIn)
		return NULL;
	return m_child->getSide(p_index);

}

void CFace::setChild(CComponent *value) 
{
	m_child=value;
}

