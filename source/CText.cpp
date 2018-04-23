#include "CGame.h"
#include "CText.h"

CText::CText(const char *p_text,float p_depth,int p_bold,int p_static):m_static(p_static)
{
	LOGFONT lf;
	HDC hdc;
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(100,200,70))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
	m_material.Specular=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*0.0f;
	m_material.Emissive=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*0.0f;
	m_material.Power=0.0f;

	ZeroMemory(&lf,sizeof(LOGFONT));
	lf.lfCharSet=DEFAULT_CHARSET;
	strcpy(lf.lfFaceName,"Arial");
	lf.lfWeight=(p_bold ? FW_HEAVY:FW_NORMAL);

	hdc=CreateCompatibleDC(NULL);
	HFONT hFont=CreateFontIndirect(&lf);
	SelectObject(hdc,hFont);
	D3DXCreateText(CGame::instance()->getDevice(),hdc,p_text,0.01f,p_depth,&m_mesh,NULL,NULL);
	DeleteObject(hFont);
	DeleteDC(hdc);
	if(!m_static)
		subscribeToMsgs();
}

void CText::onOver()
{
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(140,250,110))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
	CGameObjectWithMeshPickable::onOver();
}

void CText::onOut()
{
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(100,200,70))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
	CGameObjectWithMeshPickable::onOut();
}

CText::~CText()
{
	if(!m_static)
		unsubscribeFromMsgs();
}