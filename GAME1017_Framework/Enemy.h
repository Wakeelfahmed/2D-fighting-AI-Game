#pragma once
#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "GameObject.h"
#include "TiledLevel.h" // Could forward declare but TiledLevel.h doesn't include Enemy.h
#include "ActionNode.h" // ActionState in here.

class DecisionTree; // Forward declaration.

class Enemy : public AnimatedSprite
{
protected:
	DecisionTree* GetTree();
public:
	Enemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath);
	void virtual Update();
	void Update(bool Within_Close_Range);
	void Render();
	void SetDebugView() { m_isDebuging = !m_isDebuging; }
	// Action methods. Fill in for lab.
	void Idle();
	void Patrol();
	void MoveToRange();
	void Attack();
	bool CheckCollision(const double dX, const double dY);
	bool GetDir() { return m_dir; }
	int GetHealth() { return m_health; }
	void TakeDamage();

	// Decision methods. Fill in for lab.
	ActionState GetActionState(); // Gives us an "enter" block for initialization
	void SetActionState(ActionState action);
	void virtual BuildTree() = 0;
	/*void set_Close_Range(bool Close_range) {
		GetTree()->GetRangeNode()->SetWithinRange(Close_range);
	}*/
protected:
	DecisionTree* m_tree;
	bool m_isIdling,
		m_isDebuging,
		m_hasLOS,
		m_isDetected;
	bool m_dir;
	// Decision properties. Fill in for lab.
	double m_accel,
		m_angle,
		m_dx, m_dy,
		m_vel, m_velMax,
		m_rotMax,
		m_detectionRad;
	SDL_FPoint m_patPoints[4]{ {3,1},{27,1},{27,22},{3,22} };
	SDL_FPoint m_losRay;
	SDL_FRect m_healthDst;
	SDL_FRect m_curHealthDst;
	int m_patPointsCounter;
	int m_health;
	TiledLevel* m_level;
	ActionState m_action;
private:
};

class CloseCombatEnemy : public Enemy
{
public:
	CloseCombatEnemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath);
private:
	void BuildTree();
};

class RangedCombatEnemy : public Enemy
{
public:
	RangedCombatEnemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath);

private:
	void BuildTree();
};

#endif
//RangedCombatEnemy::RangedCombatEnemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath)
//	: Enemy(s, d, level, startingpath)
//{
//	BuildTree(); // Call the specific BuildTree function for RCE
//}
//
//void RangedCombatEnemy::Update()
//{
//	GetTree()->MakeDecision(); // Implement MakeDecision() in Enemy class to traverse the decision tree
//	Animate(); // Implement the Animate() function
//}
//void CloseCombatEnemy::BuildTree()
//{
//	// Create and add root node
//	IdleCondition* idleCondition = new IdleCondition();
//	m_tree->SetIdleNode(idleCondition);
//	m_tree->GetNodes().push_back(idleCondition);
//
//	// Add left action (IdleAction)
//	TreeNode* idleAction = m_tree->AddNode(idleCondition, new IdleAction(this), TreeNodeType::LEFT_TREE_NODE);
//	m_tree->GetNodes().push_back(idleAction);
//
//	// Add right action (CloseCombatAction)
//	TreeNode* closeCombatAction = m_tree->AddNode(idleCondition, new CloseCombatAction(this), TreeNodeType::RIGHT_TREE_NODE);
//	m_tree->GetNodes().push_back(closeCombatAction);
//}

// Inside RangedCombatEnemy class
//void RangedCombatEnemy::BuildTree()
//{
//	// Create and add root node
//	IdleCondition* idleCondition = new IdleCondition();
//	m_tree->SetIdleNode(idleCondition);
//	m_tree->GetNodes().push_back(idleCondition);
//
//	// Add left action (IdleAction)
//	TreeNode* idleAction = m_tree->AddNode(idleCondition, new IdleAction(this), TreeNodeType::LEFT_TREE_NODE);
//	m_tree->GetNodes().push_back(idleAction);
//
//	// Add right action (RangedAttackAction)
//	TreeNode* rangedAttackAction = m_tree->AddNode(idleCondition, new RangedAttackAction(this), TreeNodeType::RIGHT_TREE_NODE);
//	m_tree->GetNodes().push_back(rangedAttackAction);
//}


