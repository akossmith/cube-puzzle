#pragma once

#include "std.h"

#include "CGameObjectWithMesh.h"
#include "CPickable.h"

class CGameObjectWithMeshPickable : public CGameObjectWithMesh,public CPickable {
public:
	virtual float picked(D3DXVECTOR3 p_origin,D3DXVECTOR3 p_direction)const;
};