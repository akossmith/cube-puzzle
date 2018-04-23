#pragma once

#include "std.h"

#include "CGameObject.h"

class CGameObjectWithMesh : public CGameObject {
  protected:
	D3DMATERIAL9 m_material;
	ID3DXMesh *m_mesh;

  public:
	CGameObjectWithMesh();
	virtual ~CGameObjectWithMesh();

	virtual void draw();
};
