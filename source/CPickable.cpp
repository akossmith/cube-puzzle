#include "CGame.h"
#include "CPickable.h"
#include "CEvent.h"
#include "std.h"

CPickable::CPickable():m_onDownEvent(NULL)
{
}

CPickable::~CPickable() 
{
	if(m_onDownEvent)
		delete m_onDownEvent;
}

void CPickable::subscribeToMsgs()
{
	CGame::instance()->getMessageHandler()->registerPickable(this);
}

void CPickable::unsubscribeFromMsgs()
{
	CGame::instance()->getMessageHandler()->unregisterPickable(this);
}

void CPickable::onDown() 
{
	if(m_onDownEvent)
		m_onDownEvent->execute(this);
}

void CPickable::onUp()
{
}

void CPickable::onOver() 
{
}

void CPickable::onOut() 
{
}

void CPickable::setOnDownEvent(CEvent *p_event)
{
	if(m_onDownEvent){
		delete m_onDownEvent;
		m_onDownEvent=NULL;
	}
	m_onDownEvent=p_event->clone();
}

float CPickable::picked(D3DXVECTOR3 p_origin,D3DXVECTOR3 p_direction) const 
{
	return INFINITEDISTANCE;
}

