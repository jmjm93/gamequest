#pragma once

#ifndef BULLET_H
#include <SDL.h>
#include "Constants.h"
#include <math.h>
#endif

enum ANGLE {
	LEFT,
	CENTER,
	RIGHT
};


const int BULLET_PLAYER_SPEED = 6;
const int BULLET_ENEMY_SPEED = 3;
const int MAX_BULLETS = 30000;
const int BULLET_SIZE = 5;





class Bullet
{
public:
	int id = -1;
	int team;
	int speed;
	float angle;
	SDL_Rect object;
	bool expired = false;
	Bullet();
	Bullet(int x, int y, Team team, int id, float ang);
	void refresh();
};

