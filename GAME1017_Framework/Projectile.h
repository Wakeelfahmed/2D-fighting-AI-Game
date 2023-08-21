#pragma once
#include<SDL.h>
#ifndef __Projectile__
#define __Projectile__
class Projectile {
public:
	Projectile(SDL_Renderer* renderer, const char* spritePath, float x, float y, float width, float height, float velocityX, float velocityY, bool isOwnedByPlayer);
	void update(float deltaTime);
	void render();
	bool checkCollision(SDL_Rect& otherRect);
	bool checkCollision(SDL_FRect& otherRect);
	bool get_shouldBeDestroyed();
	void set_shouldBeDestroyed(bool);
	bool isOwnedByPlayer();
	SDL_Rect getRect();
	float mTargetX;
	float mTargetY;
	void setTargetPosition(float targetX, float targetY);

private:
	SDL_Renderer* mRenderer;
	SDL_Texture* mTexture;
	float mX;
	float mY;
	float mWidth;
	float mHeight;
	float mVelocityX;
	float mVelocityY;
	bool mIsOwnedByPlayer;

	bool mShouldBeDestroyed;

	const float mProjectileLifeTimeInSeconds = 3.0f;
	float mProjectileLifeTimer = 0.0f;
};

#endif