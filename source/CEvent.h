#pragma once


class CPickable;

class CEvent {
  public:
	virtual void execute(CPickable *p_sender);
	virtual CEvent *clone()=0;
	virtual ~CEvent(){}
};