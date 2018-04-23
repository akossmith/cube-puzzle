#include "CGame.h"
#include "CMessageHandler.h"
#include "CPickable.h"
#include "CGameObjectWithMesh.h"
#include <algorithm>
#include "std.h"
using namespace std;

CMessageHandler::CMessageHandler():m_previouslyOver(NULL),m_lButtonState(MOUSEBUTTONUP)
{
}

CMessageHandler::~CMessageHandler() 
{
}

void CMessageHandler::saveMouseState(int p_x,int p_y,MouseButtonState p_button)
{
	m_mouseX=p_x;
	m_mouseY=p_y;
	m_lButtonState=p_button;
}

void CMessageHandler::messageUp(int p_x, int p_y) 
{
	CPickable *p=NULL;
	saveMouseState(p_x,p_y,MOUSEBUTTONUP);
	if(p=pickPickable(p_x,p_y))
		p->onUp();
	CGame::instance()->mouseUp(p);
}

void CMessageHandler::messageDown(int p_x, int p_y) 
{
	CPickable *p=NULL;
	saveMouseState(p_x,p_y,MOUSEBUTTONDOWN);
	if(p=pickPickable(p_x,p_y))
		p->onDown();
	CGame::instance()->mouseDown(p);
}

void CMessageHandler::messageMove(int p_x, int p_y,int p_lButtonState) 
{
	CPickable *p;
	saveMouseState(p_x,p_y,(MouseButtonState)p_lButtonState);
	if(p=pickPickable(p_x,p_y)){
		if(p!=m_previouslyOver){
			if(m_previouslyOver){
				m_previouslyOver->onOut();
			}
			if(m_lButtonState!=MOUSEBUTTONDOWN){
				m_previouslyOver=p;
				p->onOver();
			}else
				m_previouslyOver=NULL;
		}
	}else{
		if(m_previouslyOver)
			m_previouslyOver->onOut();
		m_previouslyOver=NULL;
	}
	CGame::instance()->mouseMove();
}

void CMessageHandler::sendMessages()
{
	//csak move-ot kézbesítünk,hogy kattintással kapcsolatos események csak kattintáskor következhessenek be
	//akkor ne, amikor lenyomott balgomb mellett egy obj az egérkurzol alá kerül
	messageMove(m_mouseX,m_mouseY,m_lButtonState);
}

void CMessageHandler::registerPickable(CPickable *p_observer) 
{
	if(p_observer)
		m_pickables.push_back(p_observer);
}

void CMessageHandler::unregisterPickable(CPickable *p_observer) 
{
	if(p_observer){
		if(p_observer==m_previouslyOver)
			m_previouslyOver=NULL;
		m_pickables.remove(p_observer);
	}
}

namespace{

class isPicked{
	float m_distance;
	CPickable * m_ret;
	D3DXVECTOR3 m_direction,m_origin;
public:
	isPicked(const D3DXVECTOR3 &p_origin,const D3DXVECTOR3 &p_direction):m_ret(NULL),
		m_distance(INFINITEDISTANCE)
	{
		m_direction=p_direction;
		m_origin=p_origin;
	}
	void operator()(CPickable *&p)
	{
		float tempDist;
		if((tempDist=p->picked(m_origin,m_direction))<m_distance){
			m_distance=tempDist;
			m_ret=p;
		};
	}
	operator CPickable *()
	{
		return m_ret;
	}
};

}//namespace

CPickable *CMessageHandler::pickPickable(int x,int y)//ez dönti el, h kicsoda picked x,y alapján
{
	D3DXVECTOR3 origin,direction;
	D3DVIEWPORT9 vp;
	D3DXMATRIX mat;
	if(m_pickables.size()>0){
		CGame::instance()->getDevice()->GetViewport(&vp);
		CGame::instance()->getDevice()->GetTransform(D3DTS_PROJECTION, &mat);
		direction.x = ((( 2.0f*x) / (vp.Width-1)) - 1.0f)/mat(0, 0);
		direction.y = (((-2.0f*y) / (vp.Height-1)) + 1.0f)/mat(1, 1);
		direction.z=1.0f;
		origin.x=0.0f;
		origin.y=0.0f;
		origin.z=0.0f;
		CGame::instance()->getDevice()->GetTransform(D3DTS_VIEW,&mat);
		D3DXMatrixInverse(&mat,NULL,&mat);
		D3DXVec3TransformNormal(&direction,&direction,&mat);
		D3DXVec3TransformCoord(&origin,&origin,&mat);
		return for_each(m_pickables.begin(),m_pickables.end(),isPicked(origin,direction));
	}
	return NULL;
}

