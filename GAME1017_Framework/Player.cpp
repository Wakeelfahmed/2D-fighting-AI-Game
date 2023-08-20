#include "Player.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "RenderManager.h"
#include "TextureManager.h"
#include "Engine.h"
#define SPEED 2.0

class Enemy;

Player::Player(SDL_Rect s, SDL_FRect d, TiledLevel* level)
	:AnimatedSprite(s, d, STATE_IDLING), m_dir(0), m_level(level)
{
	SetAnimation(AnimState::STATE_IDLING, 1, 0, 1, 256);
	m_health = 30;
	m_healthDst = { m_dst.x,m_dst.y - 10,32,7 };
	m_curHealthDst = { m_dst.x + 1,m_dst.y - 9,static_cast<float>(m_health),5 };
}

void Player::Update()
{
	if (EVMA::GetMousePos().x > GetCenter().x)
		m_dir = 0;
	else m_dir = 1;

	switch (m_state)
	{
	case STATE_IDLING:
		if (EVMA::KeyHeld(SDL_SCANCODE_W) || EVMA::KeyHeld(SDL_SCANCODE_S) ||
			EVMA::KeyHeld(SDL_SCANCODE_A) || EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			SetAnimation(AnimState::STATE_RUNNING, 8, 1, 8, 256);
		}
		break;
	case STATE_RUNNING:
		if (EVMA::KeyReleased(SDL_SCANCODE_W) || EVMA::KeyReleased(SDL_SCANCODE_S) ||
			EVMA::KeyReleased(SDL_SCANCODE_A) || EVMA::KeyReleased(SDL_SCANCODE_D))
		{
			SetAnimation(AnimState::STATE_IDLING, 1, 0, 1, 256);
			// break; // Skip movement parsing below.
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_W))
		{
			if (m_dst.y > 0 && !CheckCollision(0.0, -SPEED))
			{
				m_dst.y += -SPEED;
			}
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_S))
		{
			if (m_dst.y < 768 - 32 && !CheckCollision(0.0, SPEED))
			{
				m_dst.y += SPEED;
			}
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_A))
		{
			if (m_dst.x > 0  && !CheckCollision(-SPEED, 0.0))
			{
				m_dst.x += -SPEED;
			}
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			if (m_dst.x < 1024 - 32 &&  !CheckCollision(SPEED, 0.0))
			{
				m_dst.x += SPEED;
			}
		}
		break;
	}
	m_healthDst = { m_dst.x,m_dst.y - 10,32,7 };
	m_curHealthDst = { m_dst.x + 1,m_dst.y - 9,static_cast<float>(m_health),5 };
	Animate();
}

void Player::Render()
{
	SDL_RenderCopyExF(REMA::GetRenderer(), TEMA::GetTexture("player"), GetSrc(), GetDst(), 0, 0, static_cast<SDL_RendererFlip>(m_dir));
	SDL_SetRenderDrawColor(REMA::GetRenderer(), 255, 255, 255, 255 );
	SDL_RenderDrawRectF(REMA::GetRenderer(), &m_healthDst);
	if(m_health <= 10)
		SDL_SetRenderDrawColor(REMA::GetRenderer(), 255, 0, 0, 255);
	else
		SDL_SetRenderDrawColor(REMA::GetRenderer(), 0, 0, 255, 255);
	SDL_RenderFillRectF(REMA::GetRenderer(), &m_curHealthDst);
}

bool Player::CheckCollision(const int dX, const int dY)
{
	int playerX = GetDst()->x / 32;
	int playerY = GetDst()->y / 32;
	SDL_Rect p = { static_cast<int>(GetDst()->x + dX + 8) , static_cast<int>(GetDst()->y + dY + 6), static_cast<int>(GetDst()->w - 16), static_cast<int>(GetDst()->h - 10) }; // Adjusted bounding box.
	Tile* tiles[4] = { m_level->GetLevel()[playerY][playerX],																				// Player's tile.
					   m_level->GetLevel()[playerY][(playerX + 1 == kCols ? kCols - 1 : playerX + 1)],										// Right tile.
					   m_level->GetLevel()[(playerY + 1 == kRows ? kRows - 1 : playerY + 1)][(playerX + 1 == kCols ? kCols - 1 : playerX + 1)],	// Bottom-Right tile.
					   m_level->GetLevel()[(playerY + 1 == kRows ? kRows - 1 : playerY + 1)][playerX] };										// Bottom tile.
	for (int i = 0; i < 4; i++)
	{
		SDL_Rect t = MAMA::ConvertFRect2Rect(*(tiles[i]->GetDst()));
		if (tiles[i]->IsObstacle() && SDL_HasIntersection(&p, &t))
		{ // Collision!
			return true;
			// Other potential code...
		}
	}
	return false;
}
