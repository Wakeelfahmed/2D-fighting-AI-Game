#pragma once
#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "GameObject.h"
#include "TiledLevel.h" // Could forward declare but TiledLevel.h doesn't include Enemy.h
#include "ActionNode.h" // ActionState in here.
#include"Projectile.h" // ActionState in here.

class DecisionTree; // Forward declaration.

class Enemy : public AnimatedSprite
{
protected:
	DecisionTree* GetTree();
public:
	Enemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath);
	void virtual Update() {}
	void virtual Update(bool Within_Close_Range);
	void virtual Update(bool Within_Close_Range, SDL_FRect& object1){}
	void Render();
	void SetDebugView() { m_isDebuging = !m_isDebuging; }
	// Action methods. Fill in for lab.
	void Idle();
	void Patrol();
	void MoveToRange();
	void virtual Attack();
	//void virtual Attack();
	void Flee();
	bool CheckCollision(const double dX, const double dY);
	bool GetDir() { return m_dir; }
	int GetHealth() { return m_health; }
	void TakeDamage();

	bool isCollidingWithPlayerProjectiles();
	bool isPlayerCollidingWithEnemyProjectiles();
	std::vector<Projectile*>* getProjectiles();
	void destroyProjectileAtIndex(int index);

	// Decision methods. Fill in for lab.
	ActionState GetActionState(); // Gives us an "enter" block for initialization
	void SetActionState(ActionState action);
	void virtual BuildTree() = 0;
	/*void set_Close_Range(bool Close_range) {
		GetTree()->GetRangeNode()->SetWithinRange(Close_range);
	}*/
protected:
	std::vector<Projectile*> mProjectiles;
	SDL_Texture* m_projTexture;
	float m_projectileShootTimer;
	const char* m_projectileImagePath = "../Assets/img/enemy-projectile.bmp";

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
	void Update(bool Within_Close_Range);
	void Update(bool Within_Close_Range, SDL_FRect& object1);
	RangedCombatEnemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath);
	void Attack();
private:
	void BuildTree();
	int mFramesTowait = 10;  // Number of frames to wait before destruction
	int mCurrentFrame;

};

#endif