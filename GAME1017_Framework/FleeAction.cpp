#include "FleeAction.h"
#include "Enemy.h"
#include <iostream>

FleeAction::FleeAction(Enemy* agent)
{
	m_name = "Flee Action";
	SetAgent(agent);
}

FleeAction::~FleeAction() = default;

void FleeAction::Action()
{
	std::cout << "Performing " << m_name << std::endl;
	//GetAgent()->Flee();
}

CloseCombatAction::CloseCombatAction(Enemy* agent)
{
	m_name = "Close Combat Action";
	SetAgent(agent);
}

CloseCombatAction::~CloseCombatAction() = default;

void CloseCombatAction::Action()
{
	std::cout << "Performing " << m_name << std::endl;
	//GetAgent()->Flee();
}

