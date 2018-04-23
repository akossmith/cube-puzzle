#pragma once

#include "std.h"


class CEvent;

class CPickable { 
  protected:
	CEvent *m_onDownEvent;
	void subscribeToMsgs();//mi van, ha kétszer meghívják?mi van, ha elfelejtik?
	void unsubscribeFromMsgs();
  public:
	CPickable();
	virtual ~CPickable();
	
	virtual void onDown();
	virtual void onUp();
	virtual void onOver();
	virtual void onOut();
	virtual float picked(D3DXVECTOR3 p_origin,D3DXVECTOR3 p_direction) const;
	void setOnDownEvent(CEvent *p_event);
};