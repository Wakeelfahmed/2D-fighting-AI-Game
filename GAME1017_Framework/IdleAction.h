#pragma once
#ifndef __IDLEACTION_H__
#define __IDLEACTION_H__
#include "ActionNode.h"

class IdleAction : public ActionNode
{
public:
	IdleAction(Enemy* agent);
	virtual ~IdleAction();

	virtual void Action() override;
private:

};

#endif 
