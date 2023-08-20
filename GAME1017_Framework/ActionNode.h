#pragma once
#ifndef __ACTIONNODE_H__
#define __ACTIONNODE_H__
#include "TreeNode.h"

enum class ActionState
{
	IDLE_STATE, PATROL_STATE, ATTACK_STATE, MoveToRange
};

class ActionNode : public TreeNode // Abstract class.
{
public:
	ActionNode() { m_isLeaf = true; }
	virtual ~ActionNode() = default;
	virtual void Action() = 0;
};
//class CloseCombatAction : public ActionNode
//{
//public:
//	CloseCombatAction(Enemy* agent);
//	virtual ~CloseCombatAction();
//
//	virtual void Action() override;
//private:
//
//};
//class RangedAttackAction : public ActionNode
//{
//public:
//	RangedAttackAction(Enemy* agent);
//	virtual ~RangedAttackAction();
//
//	virtual void Action() override;
//private:
//
//};

#endif