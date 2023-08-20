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
	void Update(bool withinrange) {}
	void Update(SDL_FRect& object1) {}
	void Render();
	bool GetDir() { return m_dir; }
	void Take_Damage() {
		bool static already_detected = false;
		if (m_health > 0 && already_detected == false) {
			m_health -= 1;
			already_detected = true;
		}
		else if (already_detected) {
			already_detected = false;
		}

	}
private:
	int m_health;
	bool m_dir;
	TiledLevel* m_level;
	SDL_FRect m_healthDst;
	SDL_FRect m_curHealthDst;
	bool CheckCollision(const int dX, const int dY);
};

#endif

