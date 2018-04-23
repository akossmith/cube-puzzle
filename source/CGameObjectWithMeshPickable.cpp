#include "CGameObjectWithMeshPickable.h"

float CGameObjectWithMeshPickable::picked(D3DXVECTOR3 p_origin,D3DXVECTOR3 p_direction)const
{
	BOOL hit;
	D3DXMATRIX mat;
	float dist=INFINITEDISTANCE;
	if(m_mesh){
		D3DXMatrixInverse(&mat,NULL,&m_transform);
		D3DXVec3TransformNormal(&p_direction,&p_direction,&mat);
		D3DXVec3TransformCoord(&p_origin,&p_origin,&mat);
		D3DXIntersect(m_mesh,&p_origin,&p_direction,&hit,NULL,NULL,NULL,&dist,NULL,NULL);
	}
	return dist;
}