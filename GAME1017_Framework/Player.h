#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "GameObject.h"
#include "TiledLevel.h"
#include "SoundManager.h"

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
		//if (m_health > 0)
		m_health -= 5;
	}
	void set_health(int m_health) { this->m_health = m_health; }
	int get_health() const { return m_health; }
	void set_AnimationState(AnimState newm_state) { m_state = newm_state; }
	AnimState get_AnimationState() const { return m_state; }
	void set_Animation(AnimState STATE, const unsigned short frameMax, const unsigned short spriteMin, const unsigned short spriteMax, const int srcY) {
		SetAnimation(STATE, frameMax, spriteMin, spriteMax, srcY);  // Adjust frames and duration
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

