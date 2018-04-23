#pragma once


#include "CContainer.h"

class CRepository : public CContainer {
  public:
	CRepository(float p_cubeSize,int p_dividing,CEvent * p_facesOnDown);
	virtual ~CRepository();

};
