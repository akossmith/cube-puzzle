#include "std.h"
#include "CGame.h"
#include "CGameObjectWithMesh.h"


CGameObjectWithMesh::CGameObjectWithMesh():m_mesh(NULL)
{
}

CGameObjectWithMesh::~CGameObjectWithMesh() 
{
	if(m_mesh)m_mesh->Release();
}

void CGameObjectWithMesh::draw() 
{
	if(m_mesh){
		CGame::instance()->getDevice()->SetTransform(D3DTS_WORLD,&m_transform);
		CGame::instance()->getDevice()->SetMaterial(&m_material);
		m_mesh->DrawSubset(0);
	}
}

