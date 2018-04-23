#pragma once

#include <list>
using namespace std;
class CPickable;

class CMessageHandler {
public:
	enum MouseButtonState {MOUSEBUTTONUP=0,MOUSEBUTTONDOWN=1};
protected:
	list<CPickable *> m_pickables;
	int m_mouseX;
	int m_mouseY;
	MouseButtonState m_lButtonState;
	CPickable *m_previouslyOver;

	CPickable *pickPickable(int x,int y);
	void saveMouseState(int p_x,int p_y,MouseButtonState p_button);
public:
	CMessageHandler();
	~CMessageHandler();
	inline int getCurrX();
	inline int getCurrY();
	inline MouseButtonState getLButtonState();
	void messageUp(int p_x, int p_y);
	void messageDown(int p_x, int p_y);
	void messageMove(int p_x, int p_y,int p_lButtonState);
	void registerPickable(CPickable *p_observer);
	void unregisterPickable(CPickable *p_observer) ;
	void sendMessages();
};

inline int CMessageHandler::getCurrX()
{
	return m_mouseX;
}

inline int CMessageHandler::getCurrY()
{
	return m_mouseY;
}

inline CMessageHandler::MouseButtonState CMessageHandler::getLButtonState()
{
	return m_lButtonState;
}
