#include "std.h"
#include "CGameObject.h"

CGameObject::CGameObject()
{
	D3DXMatrixIdentity(&m_transform);
}

CGameObject::~CGameObject() 
{
}