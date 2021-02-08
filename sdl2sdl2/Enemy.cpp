#include "Enemy.h"


Enemy::Enemy() {}

Enemy::Enemy(int x, int y, int id, SDL_Texture* texture, SpriteMap deadTexture) {
	SDL_Rect rect = {x, y, ENEMY_SIZE, ENEMY_SIZE};
		this->object = rect;
		this->id = id;
		this->state = HEADING_LEFT;
		this->texture = texture;
		this->deadAnim = deadTexture;
	}

	bool Enemy::isDead() {
		return state == DEAD;
	}

	void Enemy::hit() {
		health--;
		if (health == 0) {
			state = DYING;
			stateCounter = 0;
		}
	}

	void Enemy::refresh() {
		if(state != DYING && state != DEAD){
		switch (state) {
			case HEADING_LEFT:
				object.x -= ENEMY_SPEED;
				if (stateCounter == ENEMY_ANIM_PERIOD/2) state = SHOOTING_LEFT;
				break;
			case SHOOTING_LEFT:
				state = HEADING_RIGHT;
				break;
			case HEADING_RIGHT:
				object.x += ENEMY_SPEED;
				if (stateCounter == ENEMY_ANIM_PERIOD) {
					state = SHOOTING_RIGHT;
					stateCounter = 0;
				}
				break;
			case SHOOTING_RIGHT:
				state = HEADING_LEFT;
				break;
			}
		}

		if (state == DYING) {
			if (stateCounter == 40) {
				state = DEAD;
				stateCounter = 0;
			}
		}

		if(state != DEAD) stateCounter++;
	}

	void Enemy::render(SDL_Renderer* renderContext) {
		if(state != DEAD && state != DYING) SDL_RenderCopy(renderContext, this->texture, NULL, &this->object);
		if (state == DYING) {
			SDL_RenderCopy(renderContext, deadAnim.fetchTexture(), &deadAnim.fetchSprite(stateCounter) , &this->object);
		}
	}

