
#include "Bullet.h"



	Bullet::Bullet() {}

	Bullet::Bullet(int x, int y, Team team, int id, float ang) {
		SDL_Rect rect = { x, y, BULLET_SIZE, BULLET_SIZE };
		this->object = rect;
		this->team = team;
		this->id = id;
		this->angle = ang;
		this->speed = team == PLAYER ? BULLET_PLAYER_SPEED : BULLET_ENEMY_SPEED;
	}


	void Bullet::refresh() {
		int alphax = speed * cos(angle * 6.28 / 360);
		int alphay = speed * sin(angle * 6.28 / 360);
		object.x += alphax;
		object.y -= alphay; // y plane in monitor is inverted
		if (object.x < 0) {
			object.x = 0;
			expired = true;
		}
		if (object.x >= SCREEN_WIDTH) {
			object.x = SCREEN_WIDTH - 1;
			expired = true;
		}
		if (object.y < 0) {
			object.y = 0;
			expired = true;
		}
		if (object.y >= SCREEN_HEIGHT) {
			object.y = SCREEN_HEIGHT - 1;
			expired = true;
		}
	}