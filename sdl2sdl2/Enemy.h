#pragma once

#ifndef ENEMY_H
#include <SDL.h>
#include "Constants.h"
#include "SpriteMap.h"
#endif



const int MAX_ENEMIES = 10000;
const int ENEMY_SIZE = PC_SIZE / 2;
const int ENEMY_SPEED = 2;
const int ENEMY_HEALTH = 2;
const int ENEMY_SPAWN_PERIOD = 15;
const int ENEMY_ANIM_PERIOD = 64;

enum EnemyState {
	IDLE,
	HEADING_LEFT,
	SHOOTING_LEFT,
	HEADING_RIGHT,
	SHOOTING_RIGHT,
	DYING,
	DEAD
};

class Enemy
{
public:
	int id;
	int state = IDLE;
	SDL_Rect object;
	SDL_Texture* texture;
	SpriteMap deadAnim;
	Enemy();
	Enemy(int x, int y, int id, SDL_Texture* texture, SpriteMap deadTexture);
	void refresh();
	bool isDead();
	void render(SDL_Renderer* renderContext);
	void hit();

private:
	int stateCounter = 0;
	int health = ENEMY_HEALTH;
};

