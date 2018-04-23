#pragma once

#include "std.h"
#include "CMessageHandler.h"
#include "CComponent.h"
#include "CRepository.h"
#include "CCube.h"
#include "CText.h"
#include <memory>
using namespace std;

class CArrow;
class CFace;
class CPickable;

class CGame {
  protected:
    class CGameState {
      protected:
      public:
    	virtual void draw() = 0;
		virtual void update() = 0;
		virtual ~CGameState(){};
		virtual void mouseDown(CPickable *p_involvedObject);
		virtual void mouseUp(CPickable *p_involvedObject);
		virtual void mouseMove();
    };
    
    //biztosan csak egy peldanya van, mert CGame-be van agyazva a deklaracioja, 
    //ezert mashol nem lehet peldanyt csinalni
    class CGameStatePlaying : public CGameState {
      protected:
    	auto_ptr<CComponent> m_components[6];
    	auto_ptr<CRepository> m_repository;
    	auto_ptr<CCube> m_cube;
    	CComponent *m_selected;
    	auto_ptr<CArrow> m_arrows[8];
		auto_ptr<CText> m_newGameText;
		int m_lastX,m_lastY;
		int m_deselectSelected;
    	//kozepso kocka oldalhosszusaga
    	const float m_cubeSize;
    	//lapok szeleinek beosztasa
    	const int m_dividing;
    	const float m_maxComponentTranslationY;

      public:
    	inline CComponent *getSelected();
    	void setSelected(CComponent * p_candidate);
    	void relocateSelectedTo(CFace * p_dest);
    	virtual void draw();
		virtual void update();
    	CGameStatePlaying(float p_cubeSize,int p_dividing,float p_maxComponentTranslationY);
    	virtual ~CGameStatePlaying();
		virtual void mouseDown(CPickable *p_involvedObject);
		virtual void mouseUp(CPickable *p_involvedObject);
		virtual void mouseMove();
    };
    
    class CGameStateIntro : public CGameState {
		CText *m_text,*m_title;
		CText *m_options[7];
		D3DMATERIAL9 m_material;
    public:
		CGameStateIntro();
		virtual ~CGameStateIntro();
    	virtual void draw();
		virtual void update();
    };

    CMessageHandler m_messageHandler;
	CGameState *m_currentState;
	IDirect3DDevice9 *m_D3DDevice;
	D3DPRESENT_PARAMETERS m_d3dpp;//presentation parameters; Reset()-nél kellhet
	D3DXMATRIX m_view,m_proj;//Reset() miatt
	D3DLIGHT9 m_light;//Reset() miatt
	D3DLIGHT9 m_light1;//Reset() miatt

  public:
	~CGame();

protected:
	HWND m_hWnd;
	//letrehozas tiltasa:
	CGame(HWND p_hWnd);
	//masolas tiltasa:
	CGame(const CGame & );
	//masolas tiltasa:
	CGame & operator=(const CGame & );
	void initD3D(HWND p_hWnd);
	void initSettings();//initD3D-nél, lostDevice-nál

public:
	static void init(HWND p_hWnd);
	static CGame *instance(HWND p_hWnd=NULL);
	inline CMessageHandler *getMessageHandler();
	IDirect3DDevice9 *getDevice(){ return m_D3DDevice;}
	void update();
	void draw();
	void mouseDown(CPickable *p_involvedObject);
	void mouseUp(CPickable *p_involvedObject);
	void mouseMove();

};
inline CComponent *CGame::CGameStatePlaying::getSelected() 
{
	return m_selected;
}

inline CMessageHandler *CGame::getMessageHandler() 
{
	return &m_messageHandler;
}
