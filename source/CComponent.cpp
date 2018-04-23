#include "std.h"

#include "CGame.h"
#include "CComponent.h"
#include "CFace.h"
#include "CSide.h"

const DWORD CComponent::m_fvf=D3DFVF_XYZ|D3DFVF_NORMAL;

void CComponent::update()
{
	DWORD currTime=timeGetTime();
	float dRotation=0.0f;
	DWORD deltaTime=currTime-m_lastTime;
	D3DXMATRIX m0,m1;
	D3DXMatrixIdentity(&m1);
	if(m_currState==STATEMoving){
		if(m_currMovementType==MOVEMENTTranslY){
			m_translationY+=m_currMovementDir*(m_maxTranslationY-m_minTranslationY)/250*deltaTime;
			if(m_currMovementDir==1 && m_translationY>=m_maxTranslationY){
				m_translationY=m_maxTranslationY;
				m_currState=STATEBeeingOut;
			}else if(m_currMovementDir==-1 && m_translationY<=m_minTranslationY){
				m_translationY=m_minTranslationY;
				m_currState=STATEBeeingIn;
			}
		}else{
			dRotation=m_currMovementDir*D3DX_PI/2/250*deltaTime;
			if(abs(m_currRotation+dRotation) 
				>= abs(m_currMovementDir*D3DX_PI/2*(m_currMovementType!=MOVEMENTTurnY ? 2:1))){//x és z körüli forgatásnál pi-kben gondolkodunk(? operator)
				m_currState=STATEBeeingOut;
				dRotation=m_currMovementDir*D3DX_PI/2*(m_currMovementType!=MOVEMENTTurnY ? 2:1)-m_currRotation;
				m_currRotation=0.0f;
			}else{
				m_currRotation+=dRotation;
			}
			switch(m_currMovementType){
				case MOVEMENTTurnX:
					D3DXMatrixRotationX(&m1,dRotation);
					break;
				case MOVEMENTTurnY:
					D3DXMatrixRotationY(&m1,dRotation);
					break;
				case MOVEMENTTurnZ:
					D3DXMatrixRotationZ(&m1,dRotation);
					break;
				default:
					MYASSERT(false);
			}
			m_rotation=m_rotation*m1;
		}
	}
	D3DXMatrixTranslation(&m0,0.0f,m_translationY,0.0f);
	if(m_parent)
		m_transform=m_rotation*m0*m_parent->getTransform();
	else
		m_transform=m_rotation*m0;
	m_lastTime=currTime;

	m_fits=dynamic_cast<CFace *>(m_parent)->fits(m_sides);
}

const CSide *CComponent::getSide(int p_num) const 
{
	return m_sides[p_num];
}

CComponent::EnumState CComponent::getState() 
{
	return m_currState;
}

void CComponent::rotateSides(int p_movementDir, CComponent::EnumMovementType p_movementType)
{	
	CSide tempSide((*m_sides[0]).size());
	switch(p_movementType){
		case MOVEMENTTurnX:
			tempSide=(*m_sides[0]);
			(*m_sides[0])=(*m_sides[2]).reverse();
			(*m_sides[2])=tempSide.reverse();
			(*m_sides[1])=m_sides[1]->reverse();
			(*m_sides[3])=m_sides[3]->reverse();
			break;
		case MOVEMENTTurnY:
			if(p_movementDir==1){
				tempSide=(*m_sides[0]);
				(*m_sides[0])=(*m_sides[3]);
				(*m_sides[3])=(*m_sides[2]);
				(*m_sides[2])=(*m_sides[1]);
				(*m_sides[1])=tempSide;
			}else{
				tempSide=(*m_sides[0]);
				(*m_sides[0])=(*m_sides[1]);
				(*m_sides[1])=(*m_sides[2]);
				(*m_sides[2])=(*m_sides[3]);
				(*m_sides[3])=tempSide;
			}
			break;
		case MOVEMENTTurnZ:
			tempSide=(*m_sides[1]);
			(*m_sides[1])=(*m_sides[3]).reverse();
			(*m_sides[3])=tempSide.reverse();
			(*m_sides[0])=m_sides[0]->reverse();
			(*m_sides[2])=m_sides[2]->reverse();
			break;
	}
}

void CComponent::rotateImmediately(int p_movementDir, CComponent::EnumMovementType p_movementType) 
{
	D3DXMATRIX m1;
	if(p_movementDir!=1 && p_movementDir!=-1 || p_movementType!=MOVEMENTTurnX &&
		p_movementType!=MOVEMENTTurnY && p_movementType!=MOVEMENTTurnZ || m_currState!=STATEBeeingOut)
		return;
	//m_sides célállapota:
	rotateSides(p_movementDir,p_movementType);
	switch(p_movementType){
		case MOVEMENTTurnX:
			D3DXMatrixRotationX(&m1,p_movementDir*D3DX_PI);
			break;
		case MOVEMENTTurnY:
			D3DXMatrixRotationY(&m1,p_movementDir*D3DX_PI/2);
			break;
		case MOVEMENTTurnZ:
			D3DXMatrixRotationZ(&m1,p_movementDir*D3DX_PI);
			break;
		default:
			MYASSERT(false);
	}
	m_rotation=m_rotation*m1;
	if(++m_transformCounter >= 100){//ha m_rotation már eléggé eltorzult a kerekítési pontatlanságok miatt
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				m_rotation(i,j)=floor(m_rotation(i,j)+0.5f);//mivel m_rotation csak -1,0,1-et tart.
		m_transformCounter=0;
	}
}

void CComponent::move(int p_movementDir, CComponent::EnumMovementType p_movementType) 
{
	if(p_movementDir!=1 && p_movementDir!=-1 || p_movementType!=MOVEMENTTranslY && p_movementType!=MOVEMENTTurnX &&
		p_movementType!=MOVEMENTTurnY && p_movementType!=MOVEMENTTurnZ)
		return;
	if(m_currState==STATEBeeingIn && p_movementType==MOVEMENTTranslY && p_movementDir==1 || 
	m_currState==STATEBeeingOut && (p_movementType==MOVEMENTTranslY && p_movementDir==-1 || p_movementType!=MOVEMENTTranslY)){
		m_currMovementType=p_movementType;
		m_currState=STATEMoving;
		m_currMovementDir=p_movementDir;
		m_lastTime=timeGetTime();
		if(p_movementType!=MOVEMENTTranslY){
			m_currRotation=0.0f;
			//m_sides célállapota:
			rotateSides(p_movementDir,p_movementType);
		}
		if(++m_transformCounter >= 100){//ha m_rotation már eléggé eltorzult a kerekítési pontatlanságok miatt
			for(int i=0;i<4;i++)
				for(int j=0;j<4;j++)
					m_rotation(i,j)=floor(m_rotation(i,j)+0.5f);//mivel m_rotation csak -1,0,1-et tart.
			m_transformCounter=0;
		}
	}
}

void CComponent::setParent(CFace *p_parent) 
{
	m_parent=p_parent;
}


void CComponent::onOver()
{
	m_material.Diffuse=D3DXCOLOR(m_material.Diffuse)*(5.0f/3.0f);
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
}

void CComponent::onOut()
{
	m_material.Diffuse=D3DXCOLOR(m_material.Diffuse)*(3.0f/5.0f);
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
}

void CComponent::onSelect()
{
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(40,210,10))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
}

void CComponent::onDeselect()
{
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(150,150,10))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
}

CComponent::CComponent(float p_cubeSize, int p_dividing,const CSide *const p_sides[4], float p_maxTranslationY):m_parent(NULL),
m_maxTranslationY(p_maxTranslationY),m_currState(CComponent::STATEBeeingOut),m_translationY(p_maxTranslationY),m_currRotation(0.0f),
m_transformCounter(0),m_fits(0)
{
	DWORD numvertices=0,numfaces=0;
	int i=0,j=0;
	SVertex *vBData=0;
	WORD *iBData=0;
	unsigned int vBPos=0,iBPos=0;
	float unit=p_cubeSize/(p_dividing-2);
	m_minTranslationY=unit/2;

	D3DXMatrixIdentity(&m_rotation);
	m_material.Diffuse=D3DXCOLOR(D3DCOLOR_XRGB(150,150,10))*1.0f;
	m_material.Ambient=D3DXCOLOR(m_material.Diffuse)*0.1f;
	m_material.Specular=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*0.0f;
	m_material.Emissive=D3DXCOLOR(D3DCOLOR_XRGB(255,255,255))*0.0f;
	m_material.Power=0.0f;
	//sides másolása
	for(i=0;i<4;i++)
		m_sides[i]=new CSide(*p_sides[i]);

	//numvertices,numfaces kiszámítása
	//oldallapok:
	for(i=0;i<4;i++){
		if((*p_sides[i])[1]){
			numvertices+=2;
		}
		for(j=2;j<p_dividing;j++){
			if((*p_sides[i])[j]!=(*p_sides[i])[j-1]){
				numvertices+=2;
				if(!(*p_sides[i])[j])numfaces+=1;//átmenet 1-bõl 0-ba -> lap befejezése
			}
		}
		if((*p_sides[i])[p_dividing-1]){//félbehagyott befejezése
			numvertices+=2;
			numfaces++;
		}
	}
	//középsõ oldallapok:
	numfaces+=1;
	numvertices+=4;
	numfaces*=2;//csak az egyik oldalt számoltam
	numvertices*=2;//csak az egyik oldalt számoltam
	//szélek:
	for(i=0;i<4;i++){
		if(!(*m_sides[i])[0]) j=2;
		else j=1;
		numvertices+=2;
		for(;j<p_dividing-1;j++){
			if((*p_sides[i])[j]!=(*p_sides[i])[j-1]){//ha szintváltás
				numvertices+=2+4+2;
				numfaces+=2;
			}
		}
		if((*p_sides[i])[p_dividing-1]!=(*p_sides[i])[p_dividing-2]){//ha szintváltás
			numvertices+=2+4;
			numfaces+=2;
			if((*p_sides[i])[p_dividing-1] || (*p_sides[(i+1)%4])[1]){
				numvertices+=4;
				numfaces++;
			}
		}else{
			numvertices+=2;
			numfaces++;
		}
	}
	numfaces*=2;//1 téglalap=2 háromszög
	///numvertices,numfaces kiszámítása

	D3DXCreateMeshFVF(numfaces,numvertices,D3DXMESH_IB_MANAGED|D3DXMESH_VB_MANAGED,
		m_fvf,CGame::instance()->getDevice(),&m_mesh);
	m_mesh->LockVertexBuffer(0,(void **)&vBData);
	m_mesh->LockIndexBuffer(0,(void **)&iBData);

	//középsõ oldallapok (metszéspontvizsgálat!!!):
	vBData[vBPos++]=SVertex(-p_cubeSize/2,unit/2,p_cubeSize/2,0,1,0);
	vBData[vBPos++]=SVertex(p_cubeSize/2,unit/2,p_cubeSize/2,0,1,0);
	vBData[vBPos++]=SVertex(p_cubeSize/2,unit/2,-p_cubeSize/2,0,1,0);
	vBData[vBPos++]=SVertex(-p_cubeSize/2,unit/2,-p_cubeSize/2,0,1,0);
	iBData[iBPos++]=0;iBData[iBPos++]=1;iBData[iBPos++]=2;
	iBData[iBPos++]=0;iBData[iBPos++]=2;iBData[iBPos++]=3;
	vBData[vBPos++]=SVertex(-p_cubeSize/2,-unit/2,p_cubeSize/2,0,-1,0);
	vBData[vBPos++]=SVertex(p_cubeSize/2,-unit/2,p_cubeSize/2,0,-1,0);
	vBData[vBPos++]=SVertex(p_cubeSize/2,-unit/2,-p_cubeSize/2,0,-1,0);
	vBData[vBPos++]=SVertex(-p_cubeSize/2,-unit/2,-p_cubeSize/2,0,-1,0);
	iBData[iBPos++]=5;iBData[iBPos++]=4;iBData[iBPos++]=6;
	iBData[iBPos++]=6;iBData[iBPos++]=4;iBData[iBPos++]=7;
	//oldallapok:
	generateFacesNY(p_cubeSize,p_dividing,1,p_sides,vBData,iBData,vBPos,iBPos);
	generateFacesNY(p_cubeSize,p_dividing,-1,p_sides,vBData,iBData,vBPos,iBPos);
	//szélek:
	generateFacesNXZ(p_cubeSize,p_dividing,p_sides,vBData,iBData,vBPos,iBPos);
	
	m_mesh->UnlockIndexBuffer();
	m_mesh->UnlockVertexBuffer();
	subscribeToMsgs();//nem veszélyes, mert final
}

CComponent::~CComponent() 
{
	for(int i=0;i<4;i++)
		if(m_sides[i])delete m_sides[i];
	unsubscribeFromMsgs();
}

void CComponent::generateFacesNXZ(float p_cubeSize,int p_dividing,const CSide *const p_sides[4],
		SVertex *vBData,WORD *iBData, unsigned int &vBPos,unsigned int &iBPos)
{
	int i=0,j=0;
	float unit=p_cubeSize/(p_dividing-2);
	class CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y/*-1,1*/,float p_x,float p_z,float nx,float nz)=0;
	};
	class CCalcSVertexAutoPtr{
		CCalcSVertex *m;
	public:
		SVertex operator()(float p_unit,int p_y,float p_x,float p_z,float p_nx,float p_nz){
			return (*m)(p_unit,p_y,p_x,p_z,p_nx,p_nz);
		}
		CCalcSVertexAutoPtr(CCalcSVertex *p):m(p){}
		~CCalcSVertexAutoPtr(){delete m;}
	};
	class CCalcSVertex0:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y,float p_x,float p_z,float p_nx,float p_nz){
			return SVertex(-p_x,p_y*p_unit/2,-p_z,-p_nx,0.0f,-p_nz);
		}
	};
	class CCalcSVertex1:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y,float p_x,float p_z,float p_nx,float p_nz){
			return SVertex(-p_z,p_y*p_unit/2,p_x,-p_nz,0.0f,p_nx);
		}
	};
	class CCalcSVertex2:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y,float p_x,float p_z,float p_nx,float p_nz){
			return SVertex(p_x,p_y*p_unit/2,p_z,p_nx,0.0f,p_nz);
		}
	};
	class CCalcSVertex3:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y,float p_x,float p_z,float p_nx,float p_nz){
			return SVertex(p_z,p_y*p_unit/2,-p_x,p_nz,0.0f,-p_nx);
		}
	};
	static CCalcSVertexAutoPtr calcFunctions[]={new CCalcSVertex0(),new CCalcSVertex1(),new CCalcSVertex2(),new CCalcSVertex3()};
	for(i=0;i<4;i++){
		//új kezdése az elején, ha 1-gyel kezdõdik,
		// ha 0-val, akkor a másodiktól indulunk, mivel az elsõt megcsinálta az elõzõ ciklus
		if(!(*m_sides[i])[0]) j=2;
		else j=1;
		vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+unit*(j-2),p_cubeSize/2+(*p_sides[i])[j-1]*unit,0.0f,1.0f);
		vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+unit*(j-2),p_cubeSize/2+(*p_sides[i])[j-1]*unit,0.0f,1.0f);
		for(;j<p_dividing-1;j++){
			if((*p_sides[i])[j]!=(*p_sides[i])[j-1]){//ha szintváltás
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j-1]*unit,0.0f,1.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j-1]*unit,0.0f,1.0f);
				iBData[iBPos++]=vBPos-4;
				iBData[iBPos++]=vBPos-3;
				iBData[iBPos++]=vBPos-2;
				iBData[iBPos++]=vBPos-2;
				iBData[iBPos++]=vBPos-3;
				iBData[iBPos++]=vBPos-1;
				if((*p_sides[i])[j]){//szintváltás fölfelé
					vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j-1]*unit,-1.0f,0.0f);
					vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j-1]*unit,-1.0f,0.0f);
					vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j]*unit,-1.0f,0.0f);
					vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j]*unit,-1.0f,0.0f);
				}else{				//szintváltás lefelé
					vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j-1]*unit,1.0f,0.0f);
					vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j-1]*unit,1.0f,0.0f);
					vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j]*unit,1.0f,0.0f);
					vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j]*unit,1.0f,0.0f);
				}
				iBData[iBPos++]=vBPos-4;
				iBData[iBPos++]=vBPos-3;
				iBData[iBPos++]=vBPos-2;
				iBData[iBPos++]=vBPos-2;
				iBData[iBPos++]=vBPos-3;
				iBData[iBPos++]=vBPos-1;
				//új lapot kezdünk
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j]*unit,0.0f,1.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+(*p_sides[i])[j]*unit,0.0f,1.0f);
			}
		}
		if((*p_sides[i])[p_dividing-1]!=(*p_sides[i])[p_dividing-2]){//ha szintváltás
			vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,0.0f,1.0f);
			vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,0.0f,1.0f);
			iBData[iBPos++]=vBPos-4;
			iBData[iBPos++]=vBPos-3;
			iBData[iBPos++]=vBPos-2;
			iBData[iBPos++]=vBPos-2;
			iBData[iBPos++]=vBPos-3;
			iBData[iBPos++]=vBPos-1;
			if((*p_sides[i])[p_dividing-1]){//szintváltás fölfelé
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,-1.0f,0.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,-1.0f,0.0f);
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,-1.0f,0.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,-1.0f,0.0f);
			}else{				//szintváltás lefelé
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,1.0f,0.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,1.0f,0.0f);
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,1.0f,0.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,1.0f,0.0f);
			}
			iBData[iBPos++]=vBPos-4;
			iBData[iBPos++]=vBPos-3;
			iBData[iBPos++]=vBPos-2;
			iBData[iBPos++]=vBPos-2;
			iBData[iBPos++]=vBPos-3;
			iBData[iBPos++]=vBPos-1;
			//új lapot csinálunk,ha az utolsó egy,vagy az utsó 0,de köv 2.-a 1
			if((*p_sides[i])[p_dividing-1] || (*p_sides[(i+1)%4])[1]){
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,0.0f,1.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(p_dividing-2)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,0.0f,1.0f);
				vBData[vBPos++]=calcFunctions[i](unit,1,-p_cubeSize/2+(p_dividing-1)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,0.0f,1.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,-p_cubeSize/2+(p_dividing-1)*unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,0.0f,1.0f);
				iBData[iBPos++]=vBPos-4;
				iBData[iBPos++]=vBPos-3;
				iBData[iBPos++]=vBPos-2;
				iBData[iBPos++]=vBPos-2;
				iBData[iBPos++]=vBPos-3;
				iBData[iBPos++]=vBPos-1;
			}
		}else{
			//utolsó lezárása
			if((*p_sides[i])[p_dividing-1] || (*p_sides[(i+1)%4])[1]){
				vBData[vBPos++]=calcFunctions[i](unit,1,p_cubeSize/2+unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,0.0f,1.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,p_cubeSize/2+unit,p_cubeSize/2+(*p_sides[i])[p_dividing-1]*unit,0.0f,1.0f);	
			}else{
				vBData[vBPos++]=calcFunctions[i](unit,1,p_cubeSize/2,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,0.0f,1.0f);
				vBData[vBPos++]=calcFunctions[i](unit,-1,p_cubeSize/2,p_cubeSize/2+(*p_sides[i])[p_dividing-2]*unit,0.0f,1.0f);
			}
			iBData[iBPos++]=vBPos-4;
			iBData[iBPos++]=vBPos-3;
			iBData[iBPos++]=vBPos-2;
			iBData[iBPos++]=vBPos-2;
			iBData[iBPos++]=vBPos-3;
			iBData[iBPos++]=vBPos-1;
		}
	}
}

void CComponent::generateFacesNY(float p_cubeSize,int p_dividing,int p_y,const CSide *const p_sides[4],
								 SVertex *vBData,WORD *iBData, unsigned int &vBPos,unsigned int &iBPos)
{
	int i=0,j=0;
	float unit=p_cubeSize/(p_dividing-2);
	class CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y/*-1,1*/,float p_x,float p_z)=0;
	};
	class CCalcSVertexAutoPtr{
		CCalcSVertex *m;
	public:
		SVertex operator()(float p_unit,int p_y/*-1,1*/,float p_x,float p_z){
			return (*m)(p_unit,p_y,p_x,p_z);
		}
		CCalcSVertexAutoPtr(CCalcSVertex *p):m(p){}
		~CCalcSVertexAutoPtr(){delete m;}
	};
	class CCalcSVertex0:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y/*-1,1*/,float p_x,float p_z){
			return SVertex(-p_x,p_y*p_unit/2,-p_z,0.0f,(float)p_y,0.0f);
		}
	};
	class CCalcSVertex1:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y/*-1,1*/,float p_x,float p_z){
			return SVertex(-p_z,p_y*p_unit/2,p_x,0.0f,(float)p_y,0.0f);
		}
	};
	class CCalcSVertex2:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y/*-1,1*/,float p_x,float p_z){
			return SVertex(p_x,p_y*p_unit/2,p_z,0.0f,(float)p_y,0.0f);
		}
	};
	class CCalcSVertex3:public CCalcSVertex{
	public:	virtual SVertex operator()(float p_unit,int p_y/*-1,1*/,float p_x,float p_z){
			return SVertex(p_z,p_y*p_unit/2,-p_x,0.0f,(float)p_y,0.0f);
		}
	};
	static CCalcSVertexAutoPtr calcFunctions[]={new CCalcSVertex0,new CCalcSVertex1,new CCalcSVertex2,new CCalcSVertex3};
	for(i=0;i<4;i++){
		if((*p_sides[i])[1]){
			vBData[vBPos++]=calcFunctions[i](unit,p_y,-p_cubeSize/2,p_cubeSize/2);
			vBData[vBPos++]=calcFunctions[i](unit,p_y,-p_cubeSize/2,p_cubeSize/2+unit); 
		}
		for(j=2;j<p_dividing;j++){
			if((*p_sides[i])[j]!=(*p_sides[i])[j-1]){
				//mindenképpen kell két új vertex
				vBData[vBPos++]=calcFunctions[i](unit,p_y,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2);
				vBData[vBPos++]=calcFunctions[i](unit,p_y,-p_cubeSize/2+(j-1)*unit,p_cubeSize/2+unit); 
				if(!(*p_sides[i])[j]){				//átmenet 1-bõl 0-ba -> lap befejezése
					iBData[iBPos++]=vBPos-(p_y==1 ? 4 : 3);//?: más körüljárási irány y-tól függõen
					iBData[iBPos++]=vBPos-(p_y==1 ? 3 : 4);
					iBData[iBPos++]=vBPos-2;
					iBData[iBPos++]=vBPos-(p_y==1 ? 2 : 3);
					iBData[iBPos++]=vBPos-(p_y==1 ? 3 : 2);
					iBData[iBPos++]=vBPos-1;
				}
			}
		}
		if((*p_sides[i])[p_dividing-1]){//félbehagyott befejezése
            vBData[vBPos++]=calcFunctions[i](unit,p_y,-p_cubeSize/2+(p_dividing-1)*unit,p_cubeSize/2);
			vBData[vBPos++]=calcFunctions[i](unit,p_y,-p_cubeSize/2+(p_dividing-1)*unit,p_cubeSize/2+unit); 
			iBData[iBPos++]=vBPos-(p_y==1 ? 4 : 3);
			iBData[iBPos++]=vBPos-(p_y==1 ? 3 : 4);
			iBData[iBPos++]=vBPos-2;
			iBData[iBPos++]=vBPos-(p_y==1 ? 2 : 3);
			iBData[iBPos++]=vBPos-(p_y==1 ? 3 : 2);
			iBData[iBPos++]=vBPos-1;
		}
	}
}
