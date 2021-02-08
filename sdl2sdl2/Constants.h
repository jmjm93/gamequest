#pragma once
//Screen dimension constants
const int SCREEN_WIDTH = 640*2;
const int SCREEN_HEIGHT = 480*2;


const int PC_SIZE = 30;
const int MAX_HEALTH = 20;

enum Team {
	PLAYER,
	ENEMY
};

const int GAME_LEVEL_THRESHOLDS[7] = { 0,50,150,300,600,1000,3000 };

enum GAMESTATE {
	RUNNING,
	GAMEOVER
};
