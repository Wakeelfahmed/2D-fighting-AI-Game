#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "GameObject.h"
#include "TiledLevel.h"

class Player : public AnimatedSprite
{
public:
	Player(SDL_Rect s, SDL_FRect d, TiledLevel* level);
	void Update();
	void Render();
	bool GetDir() { return m_dir; }
private:
	bool m_dir;
	TiledLevel* m_level;
	SDL_FRect m_healthDst;
	SDL_FRect m_curHealthDst;
	int m_health;
	bool CheckCollision(const int dX, const int dY);
	void Take_Damage() { m_health -= 5; }
};

#endif

