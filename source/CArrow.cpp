
#include "CArrow.h"
#include "CGame.h"

void CArrow::draw()
{
	if(m_visible)
		CGameObjectWithMesh::draw();
}

void CArrow::update()
{
	if(m_visible)
		if(m_parent)
			m_transform=m_ownTransform*m_parent->getTransform();
}

float CArrow::picked(D3DXVECTOR3 p_origin,D3DXVECTOR3 p_direction)const
{
	if(!m_visible)
		return INFINITEDISTANCE;
	else
		return CGameObjectWithMeshPickable::picked(p_origin,p_direction);
}

void CArrow::onOver()
{
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(50,50,220));
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
}

void CArrow::onOut()
{
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(0,0,180));
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
}

void CArrow::setParent(CFace *p_parent)
{
	m_parent=p_parent;
}

void CArrow::show()
{
	m_visible=1;
}

void CArrow::hide()
{
	m_visible=0;
}

int CArrow::getVisibility()
{
	return m_visible;
}

CArrow::CArrow(const char *p_filename, float p_cubeSize, int p_dividing,const D3DXMATRIX &p_transform):m_visible(0),m_parent(NULL)
{
	m_ownTransform=p_transform;
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(0,0,180));
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.0f;
	m_material.Specular=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*1.0f;
	m_material.Emissive=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*0.0f;
	m_material.Power=80.0f;

	D3DXLoadMeshFromX(p_filename,D3DXMESH_VB_MANAGED|D3DXMESH_IB_MANAGED,CGame::instance()->getDevice(),NULL,NULL,NULL,NULL,&m_mesh);
	if(!m_mesh)
		throw EError("A program sérült");
	subscribeToMsgs();
}

CArrow::~CArrow() 
{
	//mesh->Release() inherited
	unsubscribeFromMsgs();
}

