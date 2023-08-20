#include "IdleAction.h"
#include "Enemy.h"
#include <iostream>

IdleAction::IdleAction(Enemy* agent)
{
	m_name = "Idle Action";
	SetAgent(agent);
}

IdleAction::~IdleAction() = default;

void IdleAction::Action()
{
	std::cout << "Performing " << m_name << std::endl;
	GetAgent()->Idle();
}

