/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#ifndef SOURCE_CPP
#include "Constants.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <time.h>
#include <list>

#include "Bullet.h"
#include "Enemy.h"
#include "SpriteMap.h"

#endif

//The window we'll be rendering to
SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Renderer* renderer = NULL;
SDL_Texture* screenTexture = NULL;
SDL_Texture* ufoTexture = NULL;
SDL_Texture* mcTexture = NULL;
SDL_Texture* hpTexture = NULL;
SDL_Texture* explosionTexture = NULL;
SpriteMap explosionSpritemap;
SDL_Texture* enemyBullet = NULL;
SDL_Texture* playerBullet = NULL;

bool initScreen(){
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		window = SDL_CreateWindow("GAMEQUEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		screenSurface = SDL_GetWindowSurface(window);

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


		IMG_Init(IMG_INIT_PNG);
		TTF_Init();
		SDL_ShowCursor(SDL_DISABLE);
		return true;
	}
}

SDL_Surface* loadMedia(const char * path)
{
	//Loading success flag
	bool success = true;
	SDL_Surface* ans = NULL;
	//Load splash image
	ans = IMG_Load(path);
	if (ans == NULL) printf("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
	ans = SDL_ConvertSurface(ans, screenSurface->format, 0);
	return ans;
}

SDL_Texture* loadTexture(const char* path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}



SDL_Texture* textTexture;
TTF_Font* gFont;
SDL_Color color;

void loadText(std::string text)
{
	SDL_DestroyTexture(textTexture);
	textTexture = NULL;
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
}

void renderImg(SDL_Surface* image)
{
	SDL_BlitSurface(image, NULL, screenSurface, NULL);
	SDL_UpdateWindowSurface(window);
}


void closeSDLInstance() {
	SDL_DestroyTexture(screenTexture);
	SDL_DestroyTexture(mcTexture);
	SDL_DestroyTexture(hpTexture);
	SDL_DestroyTexture(ufoTexture);
	SDL_DestroyTexture(textTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_ShowCursor(SDL_ENABLE);
	window = NULL;
	renderer = NULL;
	IMG_Quit();
	SDL_Quit();
}



// PC properties
int posx = SCREEN_WIDTH / 2;
int posy = SCREEN_HEIGHT / 2;
SDL_Rect pc = { posx, posy, PC_SIZE, PC_SIZE };
int health = MAX_HEALTH;
int killCount = 0;

//enemy properties
Enemy enemies[MAX_ENEMIES];
int enemy_num = 0;


// bullet array
Bullet bullets[MAX_BULLETS];
std::list<Bullet*> bulletSuscriptors; 
//Bullet bullets[SCREEN_WIDTH][SCREEN_HEIGHT];
int bullet_num = 0;

int posmatrix[SCREEN_WIDTH][SCREEN_HEIGHT]; // spatial matrix storing the idx of the bullet based on their position


int frameCount = 0;


int GAME_LEVEL = 0;
GAMESTATE GAME_STATE = RUNNING;

void updateLevel() {
	if (GAME_LEVEL < 7 && killCount > GAME_LEVEL_THRESHOLDS[GAME_LEVEL]) {
		GAME_LEVEL++;
		printf("%d\n", GAME_LEVEL);
	}
}
/*
void shoot(int x, int y, Team team, float angle) {
	if (bullet_num == MAX_BULLETS) return;
	Bullet newB = Bullet(x, y, team, bullet_num, angle);
	bullets[bullet_num] = newB;
	bullet_num++;
}


void removeBullet(int idx) {
	bullet_num--;
	bullets[idx].expired = true;
	posmatrix[bullets[idx].object.x][bullets[idx].object.y] = -1;
	for (int i = idx; i + 1 < MAX_BULLETS; i++) bullets[i] = bullets[i + 1];
}*/
   
void shoot(int x, int y, Team team, float angle) {
	if (bullet_num == MAX_BULLETS) return;
	Bullet newB = Bullet(x, y, team, bullet_num, angle);
	bullet_num++;
	bool found = false, firstIt = false;
	for (int i = bullet_num; !found; i++) {
		if (i == MAX_BULLETS) i = 0;
		if (bullets[i].expired) {
			newB.id = i;
			bullets[i] = newB;
			if(x > 0 && y > 0 && x < SCREEN_WIDTH && y < SCREEN_HEIGHT) posmatrix[x][y] = i;
			bulletSuscriptors.push_front(&bullets[i]);
			found = true;
		}
		firstIt = true;
	}
}

void removeBullet(int idx) {
	bullet_num--;
	bullets[idx].expired = true;
}


void spawn() {
	int xo = rand() % SCREEN_WIDTH;
	int yo = rand() % (SCREEN_HEIGHT / 2);
	Enemy newE = Enemy(xo, yo, enemy_num, ufoTexture, explosionSpritemap);
	enemies[enemy_num] = newE;
	enemy_num++;
}

int specialMoves = 0;
void kill(int idx) {
	enemy_num--;
	for (int i = idx; i + 1 < MAX_ENEMIES; i++) enemies[i] = enemies[i + 1];
	killCount++;
	if (killCount % 500 == 0) specialMoves++;
	updateLevel();

} 

// returns -1 if not hit, otherwise id of the bullet that hit
int checkHit(SDL_Rect item) {
	for (int i = item.x > 0 ? item.x : 0; i < item.w + item.x && i < SCREEN_WIDTH; i++) {
		for (int j = item.y > 0 ? item.y : 0; j < item.h + item.y && j < SCREEN_HEIGHT; j++) {
			if (posmatrix[i][j] > -1) {
				return posmatrix[i][j];
			}
		}
	}
	return -1;
}


void drawEnemies() {
	int period = ENEMY_SPAWN_PERIOD;
	switch (GAME_LEVEL) {
		case 1:
			period = floor(ENEMY_SPAWN_PERIOD * 0.94);
			break;
		case 2:
			period = floor(ENEMY_SPAWN_PERIOD * 0.81);
			break;
		case 3:
			period = floor(ENEMY_SPAWN_PERIOD * 0.67);
			break;
		case 4:
			period = floor(ENEMY_SPAWN_PERIOD * 0.44);
			break;
		case 5:
			period = floor(ENEMY_SPAWN_PERIOD * 0.33);
			break;
		case 6:
			period = floor(ENEMY_SPAWN_PERIOD * 0.15);
			break;
		case 7:
			period = floor(ENEMY_SPAWN_PERIOD * 0.05);
			break;
	}
	if (period < 1) period = 1;
	if (frameCount % period == 0 && enemy_num < MAX_ENEMIES) {
		spawn();
	//	for (int i = 0; i < diffFactor; i++) spawn();
	}

	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
	for (int i = 0; i < enemy_num; i++) {
		enemies[i].refresh();
		if (enemies[i].state == SHOOTING_LEFT || enemies[i].state == SHOOTING_RIGHT) {
			shoot(enemies[i].object.x, enemies[i].object.y, Team::ENEMY, enemies[i].state == SHOOTING_LEFT ? 225 : -45);
		}
		int id = checkHit(enemies[i].object);
		if (id > -1 && !bullets[id].expired && bullets[id].team == PLAYER) {
			enemies[i].hit();
			if (enemies[i].isDead()) {
				kill(i);
				i--;
			}
		}
		else enemies[i].render(renderer);
		//SDL_RenderCopy(renderer, ufoTexture, NULL, &enemies[i].object);
	}
}
/*
void drawBullets() {
	for (int i = 0; i < MAX_BULLETS; i++) {
		int prevx = bullets[i].object.x;
		int prevy = bullets[i].object.y;
		bullets[i].refresh();
		if(prevx > 0 && prevx < SCREEN_WIDTH && prevy > 0 && prevy < SCREEN_HEIGHT) posmatrix[prevx][prevy] = -1;
		if (bullets[i].expired) removeBullet(i);
		else {
			posmatrix[bullets[i].object.x][bullets[i].object.y] = i;
			//SDL_RenderFillRect(renderer, &bullets[i].object);
			SDL_RenderCopyEx(renderer, bullets[i].team == PLAYER ? playerBullet : enemyBullet, NULL, &bullets[i].object, bullets[i].angle, NULL, SDL_FLIP_NONE);
		}
	}
}*/

void drawBullets() {
	std::list<Bullet>::iterator it;
	for (auto it = bulletSuscriptors.begin(); it != bulletSuscriptors.end();) {
			int prevx = (*it)->object.x;
			int prevy = (*it)->object.y;
			(*it)->refresh();
			if (prevx > 0 && prevx < SCREEN_WIDTH && prevy > 0 && prevy < SCREEN_HEIGHT) posmatrix[prevx][prevy] = -1;
			if ((*it)->expired) {
				removeBullet((*it)->id);
				it = bulletSuscriptors.erase(it);
			}
			else {
				posmatrix[(*it)->object.x][(*it)->object.y] = (*it)->id;
				//SDL_RenderFillRect(renderer, &bullets[i].object);
				SDL_RenderCopyEx(renderer, (*it)->team == PLAYER ? playerBullet : enemyBullet, NULL, &(*it)->object, (*it)->angle, NULL, SDL_FLIP_NONE);
				++it;
			}
		
	}
}

void drawHud() {
	SDL_Rect hud = { SCREEN_WIDTH - 300, 0, 300, 50 };
	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);
	SDL_RenderFillRect(renderer, &hud);
	SDL_Rect hp_icon = { hud.x + 15, hud.y + 5, 20, 20 };
	SDL_RenderCopy(renderer, hpTexture, NULL, &hp_icon);
	SDL_Rect hp_bar = { hp_icon.x + 30, hp_icon.y + 5, 250 * health / MAX_HEALTH, 10 };
	SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, &hp_bar);

	
	std::stringstream oss;
	oss<< "Enemies killed: " << killCount;
	std::string textStr = oss.str();
	loadText(textStr);
	SDL_Rect text_rect = { hp_icon.x, hp_icon.y + 20, 250, 20 };
	SDL_RenderCopy(renderer, textTexture, NULL, &text_rect);
}

bool shooting = false;
void drawPC(int x, int y) {
	posx = x; posy = y;
	pc.x = x; pc.y = y;

	if (shooting) {
		shoot(pc.x + PC_SIZE/2, pc.y, PLAYER, 135);
		shoot(pc.x + PC_SIZE / 2, pc.y, PLAYER, 90);
		shoot(pc.x + PC_SIZE / 2, pc.y, PLAYER, 45);
	}
	int id = checkHit(pc);
	if (id > -1 && !bullets[id].expired && bullets[id].team == ENEMY) {
		health--;
		if (health == -1) {
			health = 0;
			GAME_STATE = GAMEOVER;
		}
		bullets[id].expired = true;
	}
	SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderCopy(renderer, mcTexture, NULL, &pc);
}



void printBulletIdPositions(int id) {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			if (posmatrix[i][j] == id) {
				printf("[%d][%d]\n", i, j);
			}
		}
	}
}

void shootSpecialMove() {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		shoot(i, SCREEN_HEIGHT, PLAYER, 90);
		shoot(SCREEN_WIDTH - i, SCREEN_HEIGHT, PLAYER, 90);
	}
}

void drawGameoverScreen() {


	SDL_Rect text = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8 };
	std::stringstream oss;
	oss << "Game over";
	std::string textStr = oss.str();
	loadText(textStr);
	SDL_RenderCopy(renderer, textTexture, NULL, &text);

	oss.str(std::string());

	SDL_Rect text2 = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8 };
	oss << "Score: " << killCount;
	std::string textStr2 = oss.str();
	loadText(textStr2);
	SDL_RenderCopy(renderer, textTexture, NULL, &text2);

	SDL_Texture* sadTexture = loadTexture("sprites/sad.png");
	SDL_Rect textRect = { (SCREEN_WIDTH / 2) - 64, (SCREEN_HEIGHT / 2) - 128, 128, 128 };
	SDL_RenderCopy(renderer, sadTexture, NULL, &textRect);
	SDL_DestroyTexture(sadTexture);
}


bool mainLoop() {
	bool quit = 0;
	SDL_Event e;
	int mousex = 0, mousey = 0, flags = 0;
	while (!quit)
	{
		SDL_PollEvent(&e);
		switch (e.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_MOUSEMOTION:
			flags = SDL_GetMouseState(&mousex, &mousey);
			break;
		case SDL_SCANCODE_ESCAPE:
			quit = true;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if(e.button.button == SDL_BUTTON_LEFT){
				shooting = true;
			}
			if (e.button.button == SDL_BUTTON_RIGHT) {
				shootSpecialMove();
			}

			break;
		case SDL_MOUSEBUTTONUP:
			shooting = false;
			break;
		}

		Uint32 msecs = SDL_GetTicks();
		float fps = frameCount / (msecs / 1000.f);
		printf("%f\n", fps);
		if(fps < 60){

		frameCount++;
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
		switch (GAME_STATE) {
			case RUNNING:
				drawPC(mousex, mousey);
				drawBullets();
				drawEnemies();
				drawHud();
				break;
			case GAMEOVER:
				drawGameoverScreen();
				break;
		}
		SDL_RenderPresent(renderer);
		}
	}
	return 1;
}

int main(int argc, char* args[])
{

	//The surface contained by the window
	
	initScreen();

	srand(time(NULL));
	screenTexture = loadTexture("sprites/bg.png");
	mcTexture = loadTexture("sprites/ship.png");
	hpTexture = loadTexture("sprites/mc.png");
	ufoTexture = loadTexture("sprites/Ufo_idk.png");
	gFont = TTF_OpenFont("fonts/lazy.ttf", 28);
	color = { 0xFF, 0xFF, 0xFF };
	explosionTexture = loadTexture("sprites/explosion.png");
	explosionSpritemap = SpriteMap(explosionTexture, 128, 128);
	playerBullet = loadTexture("sprites/bluebullet.png");
	enemyBullet = loadTexture("sprites/redbullet.png");
	

	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			posmatrix[i][j] = -1;
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++) {
		Bullet newb = Bullet(0, 0, PLAYER, -1, 0);
		bullets[i] = newb;
		bullets[i].expired = true;
	}

	mainLoop();

	closeSDLInstance();
	return 0;
}