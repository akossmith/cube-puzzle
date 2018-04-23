#pragma once

#include "std.h"
#include "CPickable.h"

class CGameObject {
protected:
	D3DXMATRIX m_transform;//az alkalmazott transformot kell tárolnia az egéresemény-vizsgálathoz

public:
	CGameObject();
	virtual ~CGameObject();
	inline const D3DXMATRIX &getTransform() const;//alkalmazott transf.-ot adja vissza
	virtual void draw(){};
	virtual void update(){};
};

inline const D3DXMATRIX &CGameObject::getTransform() const 
{
	return m_transform;
}