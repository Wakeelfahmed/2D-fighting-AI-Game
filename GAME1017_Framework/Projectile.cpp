#include "Projectile.h"
#include<iostream>
Projectile::Projectile(SDL_Renderer* renderer, const char* spritePath, float x, float y, float width, float height, float velocityX, float velocityY, bool isOwnedByPlayer)
	: mRenderer(renderer), mX(x), mY(y), mWidth(width), mHeight(height), mVelocityX(velocityX), mVelocityY(velocityY), mIsOwnedByPlayer(isOwnedByPlayer)
{
	SDL_Surface* surface = SDL_LoadBMP(spritePath);
	mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
	SDL_FreeSurface(surface);
	mShouldBeDestroyed = false;
}
SDL_Rect Projectile::getRect()
{
	SDL_Rect rect = { (int)mX, (int)mY, (int)mWidth, (int)mHeight };
	return rect;
}
void Projectile::setTargetPosition(float targetX, float targetY) {
	mTargetX = targetX;
	mTargetY = targetY;

	// Calculate the direction vector to the target
	float directionX = mTargetX - mX;
	float directionY = mTargetY - mY;

	// Calculate the length of the direction vector
	float length = std::sqrt(directionX * directionX + directionY * directionY);

	// Normalize the direction vector
	if (length > 0.0f) {
		directionX /= length;
		directionY /= length;
	}

	// Set the velocity components based on the normalized direction vector
	mVelocityX = directionX * 1;//mProjectileSpeed;
	mVelocityY = directionY * 1; //mProjectileSpeed;
}
bool Projectile::shouldBeDestroyed()
{
	return mShouldBeDestroyed;
}
void Projectile::update(float deltaTime)
{
	mX += mVelocityX * deltaTime;
	mY += mVelocityY * deltaTime;

	// Depending on a projectile life timer decide if this projectile should be destroyed
	mProjectileLifeTimer += deltaTime;
	float distanceToTarget = std::sqrt((mTargetX - mX) * (mTargetX - mX) + (mTargetY - mY) * (mTargetY - mY));
	if (distanceToTarget <= mWidth * 0.5f) {
		mShouldBeDestroyed = true;
	}
}
void Projectile::render()
{
	SDL_Rect destRect = { (int)mX, (int)mY, (int)mWidth, (int)mHeight };
	SDL_RenderCopy(mRenderer, mTexture, nullptr, &destRect);
}
bool Projectile::checkCollision(SDL_Rect& otherRect)
{
	SDL_Rect myRect = { (int)mX, (int)mY, (int)mWidth, (int)mHeight };
	return SDL_HasIntersection(&myRect, &otherRect);
}
bool Projectile::isOwnedByPlayer() { return mIsOwnedByPlayer; }