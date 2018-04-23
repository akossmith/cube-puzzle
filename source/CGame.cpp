#include "std.h"
#include "CGame.h"
#include "CArrow.h"
#include "CFace.h"
#include "CPickable.h"
#include "CEvent.h"
#include "CComponent.h"
#include "CSide.h"
#include "CText.h"

void CGame::CGameState::mouseDown(CPickable *p_involvedObject)
{
}

void CGame::CGameState::mouseUp(CPickable *p_involvedObject)
{
}

void CGame::CGameState::mouseMove()
{
}

void CGame::CGameStatePlaying::setSelected(CComponent * p_candidate) 
{
	if(m_selected)
			m_selected->onDeselect();
	if(p_candidate){
		m_selected=p_candidate;
		for(int i=0;i<8;i++){
			if(m_arrows[i].get()){
				m_arrows[i]->setParent(dynamic_cast<CFace *>(m_selected->getParent()));
			}
		}
		m_selected->onSelect();
	}else{
		m_selected=NULL;
		for(int i=0;i<8;i++){
			if(m_arrows[i].get()){
				m_arrows[i]->setParent(NULL);
				m_arrows[i]->hide();
			}
		}
	}
}

void CGame::CGameStatePlaying::mouseDown(CPickable *p_involvedObject)
{
	if(!p_involvedObject){
		m_lastX=CGame::instance()->getMessageHandler()->getCurrX();
		m_lastY=CGame::instance()->getMessageHandler()->getCurrY();
		m_deselectSelected=1;
	}
}

void CGame::CGameStatePlaying::mouseUp(CPickable *p_involvedObject)
{
	if(m_deselectSelected){
		setSelected(NULL);
		m_deselectSelected=0;
	}
	m_lastX=m_lastY=-1;
}

void CGame::CGameStatePlaying::mouseMove()
{
	D3DXMATRIX mat;
	if(m_lastX!=CGame::instance()->getMessageHandler()->getCurrX() || m_lastY!=CGame::instance()->getMessageHandler()->getCurrY())
		m_deselectSelected=0;
	if(!CGame::instance()->getMessageHandler()->getLButtonState())
		m_lastX=m_lastY=-1;
	if(m_lastX!=-1){
		D3DXMatrixRotationYawPitchRoll(&mat,-(CGame::instance()->getMessageHandler()->getCurrX()-m_lastX)/100.0f,
			-(CGame::instance()->getMessageHandler()->getCurrY()-m_lastY)/100.0f,0.0f);
		m_cube->setRotation(m_cube->getRotation()*mat);
		m_lastX=CGame::instance()->getMessageHandler()->getCurrX();
		m_lastY=CGame::instance()->getMessageHandler()->getCurrY();
	}
}

void CGame::CGameStatePlaying::relocateSelectedTo(CFace * p_dest) 
{
	if(m_selected && p_dest){
		if(m_selected->getState()==CComponent::STATEBeeingOut){
			if(!p_dest->getChild()){
				dynamic_cast<CFace *>(m_selected->getParent())->setChild(NULL);
				for(int i=0;i<8;i++)
					if(m_arrows[i].get())
						m_arrows[i]->setParent(p_dest);
				p_dest->setChild(m_selected);
				m_selected->setParent(p_dest);
			}
		}
	}
}

void CGame::CGameStatePlaying::draw() 
{
	int i=0;	
	int finished=1;
	if(m_repository.get())
		m_repository->draw();
	if(m_cube.get())
		m_cube->draw();
	for(i=0;i<8;i++){
		if(m_arrows[i].get())
			m_arrows[i]->draw();
	}
	if(m_newGameText.get())
		m_newGameText->draw();
	for(i=0;i<6;i++){
		if(m_cube->getFace(i)->getChild()){
			if(m_cube->getFace(i)->getChild()->getState()!=CComponent::STATEBeeingIn)
				finished=0;
		}else
			finished=0;
	}
	if(finished)
		if(MessageBox(CGame::instance()->m_hWnd,"Gratulálok.\nKíván új játékot kezdeni?","Gratulálok",MB_YESNO)==IDYES){
			delete CGame::instance()->m_currentState;
			CGame::instance()->m_currentState=NULL;
			CGame::instance()->m_currentState=new CGame::CGameStateIntro();
		}else
			exit(0);
}


void CGame::CGameStatePlaying::update() 
{
	int i=0;
	if(m_repository.get())
		m_repository->update();
	if(m_cube.get())
		m_cube->update();
	for(i=0;i<6;i++){
		if(m_components[i].get())
			m_components[i]->update();
	}
	//nyilak láthatósága m_selected állapota szerint
	if(m_selected){
		for(int i=0;i<8;i++){
				if(m_arrows[i].get())
					m_arrows[i]->hide();
		}
		if(m_selected->getState()!=CComponent::STATEMoving){
			if(m_selected->getState()==CComponent::STATEBeeingIn){
				if(m_arrows[7].get())
					m_arrows[7]->show();
			}else{
				for(int i=0;i<6;i++){
					if(m_arrows[i].get())
						m_arrows[i]->show();
				}
				if(m_selected->fits() && m_arrows[6].get())
					m_arrows[6]->show();
			}
		}
	}
	if(m_newGameText.get())
		m_newGameText->update();
	for(i=0;i<8;i++){
		if(m_arrows[i].get())
			m_arrows[i]->update();
	}
}

#ifdef _DEBUG
DWORD g_veletlen; 
#endif

CGame::CGameStatePlaying::CGameStatePlaying(float p_cubeSize,int p_dividing,float p_maxComponentTranslationY):
	m_repository(NULL),m_cube(NULL),m_cubeSize(p_cubeSize),m_dividing(p_dividing),m_newGameText(NULL),
		m_maxComponentTranslationY(p_maxComponentTranslationY),m_selected(NULL),m_lastX(-1),m_lastY(-1),m_deselectSelected(0)
{
	int i=0,j=0;
	float unit=m_cubeSize/(m_dividing-2);
	
	class CFacesOnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CFacesOnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CFacesOnDown(*this);}
		virtual void execute(CPickable *p_sender){
			m_gameState->relocateSelectedTo(dynamic_cast<CFace *>(p_sender));//relocateSelectedTo kezeli az esetleges NULL-t
		}
	}facesOnDown(this);
	m_repository.reset(new CRepository(m_cubeSize,m_dividing,&facesOnDown));
	m_cube.reset(new CCube(m_cubeSize,m_dividing,&facesOnDown));

	//alkatrészek inicializálása:
#ifdef _DEBUG
	srand(g_veletlen=timeGetTime());
#else
	srand(timeGetTime());
#endif
	class CRandomValue{
	public:
		char operator()(unsigned upperLimit){
			return (char)(rand()/((double)RAND_MAX+0.5)*upperLimit);
		}
	}randomValue;
	char *a[12];
	CSide *s[6][4];
	for(i=0;i<6;i++)
		for(j=0;j<4;j++)
			s[i][j]=new CSide(m_dividing);
	for(i=0;i<12;i++)
		a[i]=new char[m_dividing];
	//a tizenkét él bitmintája:
	for(i=0;i<12;i++)
		for(j=0;j<m_dividing;j++)
			a[i][j]=randomValue(2);
	(*s[4][3])=~((*s[0][0])=a[0]).reverse();
	(*s[1][3])=~((*s[0][2])=a[1]).reverse();
	(*s[3][0])=~((*s[1][1])=a[2]).reverse();
	(*s[4][1])=~((*s[3][2])=a[3]).reverse();
	(*s[4][0])=~((*s[2][3])=a[4]).reverse();
	(*s[2][1])=~((*s[1][2])=a[5]).reverse();
	(*s[5][1])=~((*s[1][0])=a[6]).reverse();
	(*s[5][3])=~((*s[4][2])=a[7]).reverse();
	(*s[2][0])=~((*s[0][3])=a[8]).reverse();
	(*s[3][3])=~((*s[2][2])=a[9]).reverse();
	(*s[5][0])=~((*s[3][1])=a[10]).reverse();
	(*s[5][2])=~((*s[0][1])=a[11]).reverse();

	(*s[0][1])[0]=(*s[0][0])[m_dividing-1]=
		(*s[4][3])[0]=(*s[4][2])[m_dividing-1]=
		(*s[5][3])[0]=(*s[5][2])[m_dividing-1]=0;
	if((*s[0][1])[1] || (*s[0][0])[m_dividing-2])
        (*s[0][1])[0]=(*s[0][0])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[4][3])[0]=(*s[4][2])[m_dividing-1]=1;
				break;
			case 1:
				(*s[5][3])[0]=(*s[5][2])[m_dividing-1]=1;
				break;
		}
	}
	(*s[0][0])[0]=(*s[0][3])[m_dividing-1]=
        (*s[4][0])[0]=(*s[4][3])[m_dividing-1]=
		(*s[2][0])[0]=(*s[2][3])[m_dividing-1]=0;
	if((*s[0][0])[1] || (*s[0][3])[m_dividing-2])
        (*s[0][0])[0]=(*s[0][3])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[4][0])[0]=(*s[4][3])[m_dividing-1]=1;
				break;
			case 1:
				(*s[2][0])[0]=(*s[2][3])[m_dividing-1]=1;
				break;
		}
	}
	(*s[4][1])[0]=(*s[4][0])[m_dividing-1]=
		(*s[3][3])[0]=(*s[3][2])[m_dividing-1]=
		(*s[2][3])[0]=(*s[2][2])[m_dividing-1]=0;
	if((*s[4][1])[1] || (*s[4][0])[m_dividing-2])
        (*s[4][1])[0]=(*s[4][0])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[3][3])[0]=(*s[3][2])[m_dividing-1]=1;
				break;
			case 1:
				(*s[2][3])[0]=(*s[2][2])[m_dividing-1]=1;
				break;
		}
	}
	(*s[5][0])[0]=(*s[5][3])[m_dividing-1]=
		(*s[4][2])[0]=(*s[4][1])[m_dividing-1]=
		(*s[3][2])[0]=(*s[3][1])[m_dividing-1]=0;
	if((*s[5][0])[1] || (*s[5][3])[m_dividing-2])
        (*s[5][0])[0]=(*s[5][3])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[4][2])[0]=(*s[4][1])[m_dividing-1]=1;
				break;
			case 1:
				(*s[3][2])[0]=(*s[3][1])[m_dividing-1]=1;
				break;
		}
	}
	/*4:*/	
	(*s[0][2])[0]=(*s[0][1])[m_dividing-1]=
		(*s[1][0])[0]=(*s[1][3])[m_dividing-1]=
		(*s[5][2])[0]=(*s[5][1])[m_dividing-1]=0;
	if((*s[0][2])[1] || (*s[0][1])[m_dividing-2])
        (*s[0][2])[0]=(*s[0][1])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[1][0])[0]=(*s[1][3])[m_dividing-1]=1;
				break;
			case 1:
				(*s[5][2])[0]=(*s[5][1])[m_dividing-1]=1;
				break;
		}
	}
	(*s[0][3])[0]=(*s[0][2])[m_dividing-1]=
		(*s[1][3])[0]=(*s[1][2])[m_dividing-1]=
		(*s[2][1])[0]=(*s[2][0])[m_dividing-1]=0;
	if((*s[0][3])[1] || (*s[0][2])[m_dividing-2])
        (*s[0][3])[0]=(*s[0][2])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[1][3])[0]=(*s[1][2])[m_dividing-1]=1;
				break;
			case 1:
				(*s[2][1])[0]=(*s[2][0])[m_dividing-1]=1;
				break;
		}
	}
	(*s[2][2])[0]=(*s[2][1])[m_dividing-1]=
		(*s[3][0])[0]=(*s[3][3])[m_dividing-1]=
		(*s[1][2])[0]=(*s[1][1])[m_dividing-1]=0;
	if((*s[2][2])[1] || (*s[2][1])[m_dividing-2])
        (*s[2][2])[0]=(*s[2][1])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[3][0])[0]=(*s[3][3])[m_dividing-1]=1;
				break;
			case 1:
				(*s[1][2])[0]=(*s[1][1])[m_dividing-1]=1;
				break;
		}
	}
	(*s[1][1])[0]=(*s[1][0])[m_dividing-1]=
		(*s[5][1])[0]=(*s[5][0])[m_dividing-1]=
		(*s[3][1])[0]=(*s[3][0])[m_dividing-1]=0;
	if((*s[1][1])[1] || (*s[1][0])[m_dividing-2])
        (*s[1][1])[0]=(*s[1][0])[m_dividing-1]=1;
	else{
		switch(randomValue(2)){
			case 0:
				(*s[5][1])[0]=(*s[5][0])[m_dividing-1]=1;
				break;
			case 1:
				(*s[3][1])[0]=(*s[3][0])[m_dividing-1]=1;
				break;
		}
	}

	class CEventComponentOnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CEventComponentOnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CEventComponentOnDown(*this);}
		virtual void execute(CPickable *p_sender){
			m_gameState->setSelected(dynamic_cast<CComponent *>(p_sender));//setSelected kezeli NULL-t
		}
	} componentOnDown(this);
	for(i=0;i<6;i++){
		while(m_components[j=randomValue(6)].get());
		m_components[j].reset(new CComponent(m_cubeSize,m_dividing,s[j],m_maxComponentTranslationY));
		m_components[j]->setOnDownEvent(&componentOnDown);
		m_components[j]->setParent(m_repository->getFace(i));
		m_repository->getFace(i)->setChild(m_components[j].get());
		m_components[j]->rotateImmediately( ( randomValue(2) ? -1 : 1),(CComponent::EnumMovementType)randomValue(3));
	}
	
	for(i=0;i<6;i++)
		for(j=0;j<4;j++)
			delete s[i][j];
	for(i=0;i<12;i++)
		delete[] a[i];

	
	//nyilak inicializálása:
	D3DXMATRIX temp1,temp2;
	class CArrow0OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow0OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow0OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(-1,CComponent::MOVEMENTTurnY);
		}
	}arrow0OnDown(this);
	D3DXMatrixTranslation(&temp1,m_cubeSize+unit*0.8f,m_maxComponentTranslationY,0.0f);
	m_arrows[0].reset(new CArrow("curvedArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[0]->setOnDownEvent(&arrow0OnDown);

	class CArrow1OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow1OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow1OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(1,CComponent::MOVEMENTTurnY);
		}
	}arrow1OnDown(this);
	D3DXMatrixTranslation(&temp1,-m_cubeSize-unit*0.8f,m_maxComponentTranslationY,0.0f);
	D3DXMatrixRotationZ(&temp2,D3DX_PI);
	temp1=temp2*temp1;
	m_arrows[1].reset(new CArrow("curvedArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[1]->setOnDownEvent(&arrow1OnDown);

	class CArrow2OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow2OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow2OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(1,CComponent::MOVEMENTTurnX);
		}
	}arrow2OnDown(this);
	D3DXMatrixRotationYawPitchRoll(&temp1,0.0f,D3DX_PI/4,D3DX_PI/2);
	D3DXMatrixTranslation(&temp2,0.0f,m_maxComponentTranslationY+unit+2.0f,unit+1.5f);
	temp1=temp1*temp2;
	m_arrows[2].reset(new CArrow("curvedArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[2]->setOnDownEvent(&arrow2OnDown);
	
	class CArrow3OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow3OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow3OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(-1,CComponent::MOVEMENTTurnX);
		}
	}arrow3OnDown(this);
	D3DXMatrixRotationYawPitchRoll(&temp1,D3DX_PI,D3DX_PI/4,D3DX_PI/2);
	D3DXMatrixTranslation(&temp2,0.0f,m_maxComponentTranslationY+unit+2.0f,-unit-1.5f);
	temp1=temp1*temp2;
	m_arrows[3].reset(new CArrow("curvedArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[3]->setOnDownEvent(&arrow3OnDown);

	class CArrow4OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow4OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow4OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(-1,CComponent::MOVEMENTTurnZ);
		}
	}arrow4OnDown(this);
	D3DXMatrixRotationYawPitchRoll(&temp1,D3DX_PI/2,D3DX_PI/4,D3DX_PI/2);
	D3DXMatrixTranslation(&temp2,unit+1.5f,m_maxComponentTranslationY+unit+2.0f,0.0f);
	temp1=temp1*temp2;
	m_arrows[4].reset(new CArrow("curvedArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[4]->setOnDownEvent(&arrow4OnDown);

	class CArrow5OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow5OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow5OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(1,CComponent::MOVEMENTTurnZ);
		}
	}arrow5OnDown(this);
	D3DXMatrixRotationYawPitchRoll(&temp1,-D3DX_PI/2,D3DX_PI/4,D3DX_PI/2);
	D3DXMatrixTranslation(&temp2,-unit-1.5f,m_maxComponentTranslationY+unit+2.0f,0.0f);
	temp1=temp1*temp2;
	m_arrows[5].reset(new CArrow("curvedArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[5]->setOnDownEvent(&arrow5OnDown);

	class CArrow6OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow6OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow6OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(-1,CComponent::MOVEMENTTranslY);
		}
	}arrow6OnDown(this);
	D3DXMatrixRotationYawPitchRoll(&temp1,0.0f,D3DX_PI,0.0f);
	D3DXMatrixTranslation(&temp2,0.0f,m_maxComponentTranslationY-1.0f,0.0f);
	temp1=temp1*temp2;
	m_arrows[6].reset(new CArrow("smallArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[6]->setOnDownEvent(&arrow6OnDown);

	class CArrow7OnDown:public CEvent{
		CGameStatePlaying *m_gameState;
	public:
		CArrow7OnDown(CGameStatePlaying *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new CArrow7OnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(m_gameState->getSelected())
				m_gameState->getSelected()->move(1,CComponent::MOVEMENTTranslY);
		}
	}arrow7OnDown(this);
	D3DXMatrixTranslation(&temp1,0.0f,unit+0.5f,0.0f);
	m_arrows[7].reset(new CArrow("smallArrow.X",m_cubeSize,m_dividing,temp1));
	m_arrows[7]->setOnDownEvent(&arrow7OnDown);

	//"új játék kezdése" felirat
	class CNewGameOnDown:public CEvent{
	public:
		CNewGameOnDown(){}
		virtual CEvent *clone(){return new CNewGameOnDown(*this);}
		virtual void execute(CPickable *p_sender){
			if(MessageBox(CGame::instance()->m_hWnd,"Biztosan új játékot kezd?","Megerõsítés",MB_YESNO)==IDYES){
				delete CGame::instance()->m_currentState;
				CGame::instance()->m_currentState=NULL;
				CGame::instance()->m_currentState=new CGame::CGameStateIntro();
			}
		}
	}newOnDown;
	D3DXMatrixTranslation(&temp1,-7.5f,-5.5f,-13.0f);
	m_newGameText.reset(new CText("Új játék kezdése",0.5f,1,0));
	m_newGameText->setTransform(temp1);
	m_newGameText->setOnDownEvent(&newOnDown);
}

CGame::CGameStatePlaying::~CGameStatePlaying() 
{
}

CGame::CGameStateIntro::CGameStateIntro():m_text(NULL),m_title(NULL)
{
	D3DXMATRIX m1,m2;
	m_title=new CText("Új játék",0.5f,0,1);
	m_text=new CText("Lapok széleinek beosztása:",0.5f,0,1);
	
	D3DXMatrixTranslation(&m1,-1.8f,2.0f,0.0f);
	D3DXMatrixScaling(&m2,2.5f,2.5f,1.0f);
	m_title->setTransform(m1*m2);

	D3DXMatrixTranslation(&m1,-5.3f,0.7f,0.0f);
	D3DXMatrixScaling(&m2,2.0f,2.0f,1.0f);
	m_text->setTransform(m1*m2);

	class COptionOnDown:public CEvent{
		CGameStateIntro *m_gameState;
	public:
		COptionOnDown(CGameStateIntro *p_gameState):m_gameState(p_gameState){}
		virtual CEvent *clone(){return new COptionOnDown(*this);}
		virtual void execute(CPickable *p_sender){
			int dividing=0;
			float cubeSize=0.0f;
			for(int i=0;i<7 && m_gameState->m_options[i]!=p_sender;i++);
			dividing=i+3;
			if(dividing<5)
				cubeSize=2.0f;
			else
				cubeSize=3.0f;
			delete CGame::instance()->m_currentState;
			CGame::instance()->m_currentState=NULL;
			CGame::instance()->m_currentState=new CGame::CGameStatePlaying(cubeSize,dividing,cubeSize+1.5f*cubeSize/(dividing-2));
		}
	}optionOnDown(this);
	D3DXMatrixScaling(&m2,3.0f,3.0f,1.0f);
	char a[3];
	for(int i=0;i<7;i++){
		itoa(i+3,a,10);
		m_options[i]=new CText(a,0.7f,1,0);
		D3DXMatrixTranslation(&m1,-10.0f+i*3.0f,-3.0f,0.0f);
		m_options[i]->setTransform(m2*m1);
		m_options[i]->setOnDownEvent(&optionOnDown);
	}
}

CGame::CGameStateIntro::~CGameStateIntro()
{
	if(m_text)
		delete m_text;
	if(m_title)
		delete m_title;
	for(int i=0;i<7;i++)
		if(m_options[i])
			delete m_options[i];
}

void CGame::CGameStateIntro::draw() 
{
	if(m_text)
		m_text->draw();
	if(m_title)
		m_title->draw();
	for(int i=0;i<7;i++)
		if(m_options[i])
			m_options[i]->draw();
}

void CGame::CGameStateIntro::update() 
{
	if(m_text)
		m_text->update();
	if(m_title)
		m_title->update();	
	for(int i=0;i<7;i++)
		if(m_options[i])
			m_options[i]->update();
}

void CGame::mouseDown(CPickable *p_involvedObject)
{
	m_currentState->mouseDown(p_involvedObject);
}

void CGame::mouseUp(CPickable *p_involvedObject)
{
	m_currentState->mouseUp(p_involvedObject);
}

void CGame::mouseMove()
{
	m_currentState->mouseMove();
}

//letrehozas tiltasa(private),inicializalas
CGame::CGame(HWND p_hWnd):m_D3DDevice(NULL),m_hWnd(p_hWnd),m_currentState(NULL)
{
	if(!p_hWnd)throw EError("Hiba lépett fel, ezért a program leáll");
	initD3D(p_hWnd);
	m_currentState=new CGameStateIntro();
}

CGame::~CGame() 
{
	if(m_currentState)
		delete m_currentState;
	if(m_D3DDevice)m_D3DDevice->Release();
}

//masolas tiltasa:
CGame::CGame(const CGame & ) 
{
}

//masolas tiltasa:
CGame & CGame::operator=(const CGame & ) 
{
	return *this;
}


CGame *CGame::instance(HWND p_hWnd)
{
	static CGame game(p_hWnd);//statikus lokalis valtozoval singleton
	return &game;
}

void CGame::init(HWND p_hWnd)
{
	instance(p_hWnd);
}

void CGame::initD3D(HWND p_hWnd)
{
	IDirect3D9 *id3d=0;
	RECT rect;
	D3DCAPS9 d3dcaps;
	GetClientRect(p_hWnd,&rect);

	m_d3dpp.AutoDepthStencilFormat=D3DFMT_D16;
	m_d3dpp.BackBufferCount=1;
	m_d3dpp.BackBufferFormat=D3DFMT_X8R8G8B8;
	m_d3dpp.BackBufferHeight=rect.bottom;
	m_d3dpp.BackBufferWidth=rect.right;
	m_d3dpp.EnableAutoDepthStencil=true;
	m_d3dpp.Flags=D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_d3dpp.FullScreen_RefreshRateInHz=0;
	m_d3dpp.hDeviceWindow=p_hWnd;
	m_d3dpp.MultiSampleQuality=0;
	m_d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	m_d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
	m_d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	m_d3dpp.Windowed=true;
	id3d=Direct3DCreate9(D3D_SDK_VERSION);
	if(!id3d) throw EError("Hiba lépett fel a DirectX inicializálásakor, ezért a program leáll");
	id3d->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&d3dcaps);
	if(FAILED(id3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,p_hWnd,
		(d3dcaps.VertexProcessingCaps ? D3DCREATE_HARDWARE_VERTEXPROCESSING:D3DCREATE_SOFTWARE_VERTEXPROCESSING)
		,&m_d3dpp,&m_D3DDevice))){
			id3d->Release();
			throw EError("Hiba lépett fel a DirectX inicializálásakor, ezért a program leáll");
		}
	id3d->Release();

	D3DXMatrixLookAtLH(&m_view,&D3DXVECTOR3(0.0f,0.0f,-40.0f),&D3DXVECTOR3(0,0,0),&D3DXVECTOR3(0.0f,1.0f,0.0f));
	D3DXMatrixPerspectiveFovLH(&m_proj,D3DX_PI/7.0f,(float)rect.right/(float)rect.bottom,1.0f,1000.0f);
	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = rect.right;
	vp.Height = rect.bottom;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;
	m_D3DDevice->SetViewport(&vp);

	ZeroMemory(&m_light,sizeof(D3DLIGHT9));
	m_light.Diffuse=D3DXCOLOR(D3DCOLOR_ARGB(255,255,255,255))*1.0f;
	m_light.Ambient=D3DXCOLOR(D3DCOLOR_ARGB(255,255,255,255))*0.9f;
	m_light.Specular=D3DXCOLOR(D3DCOLOR_ARGB(255,255,255,255))*0.5f;
	m_light.Attenuation0=1.1f;
	m_light.Range=1000.0f;
	m_light.Position=D3DXVECTOR3(-25,0,-25);
	m_light.Type=D3DLIGHT_POINT;

	ZeroMemory(&m_light1,sizeof(D3DLIGHT9));
	m_light1.Diffuse=D3DXCOLOR(D3DCOLOR_ARGB(255,255,255,255))*1.0f;
	m_light1.Ambient=D3DXCOLOR(D3DCOLOR_ARGB(255,255,255,255))*0.9f;
	m_light1.Specular=D3DXCOLOR(D3DCOLOR_ARGB(255,255,255,255))*0.5f;
	m_light1.Attenuation0=1.1f;
	m_light1.Range=1000.0f;
	m_light1.Position=D3DXVECTOR3(25,0,-25);
	m_light1.Type=D3DLIGHT_POINT;

	initSettings();
}

void CGame::update()
{
	//mások update-jének meghívása:
	m_currentState->update();
	m_messageHandler.sendMessages();//az obj.-okhoz viszonyított egérpozíció megváltozahatott az update()-ek miatt
}

void CGame::draw()
{
	m_D3DDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0xff000000,1.0f,0);
	m_D3DDevice->BeginScene();	
	m_currentState->draw();
	m_D3DDevice->EndScene();
	if(m_D3DDevice->Present(NULL,NULL,NULL,NULL)==D3DERR_DEVICELOST){
		while(m_D3DDevice->TestCooperativeLevel()!=D3DERR_DEVICENOTRESET)
			Sleep(100);
		m_D3DDevice->Reset(&m_d3dpp);
		initSettings();
	};
}

void CGame::initSettings()
{
	m_D3DDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
	m_D3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	m_D3DDevice->SetRenderState(D3DRS_SPECULARENABLE,true);//??????????????????????????
	m_D3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_D3DDevice->SetTransform(D3DTS_VIEW,&m_view);
	m_D3DDevice->SetTransform(D3DTS_PROJECTION,&m_proj);
	m_D3DDevice->SetLight(0,&m_light);
	m_D3DDevice->LightEnable(0,true);
	m_D3DDevice->SetLight(1,&m_light1);
	m_D3DDevice->LightEnable(1,true);
}