#pragma once
#ifndef SPRITEMAP_H
#include <SDL.h>
#include "Constants.h"
#include <SDL_image.h>
#endif


class SpriteMap
{
private:
	SDL_Texture* sprite_map;
	int sprite_w;
	int sprite_h;
	int map_w;
	int map_h;
	int output_w;
	int output_h;

public:
	SpriteMap();
	SpriteMap(SDL_Texture* spritemap, int w, int h);
	SDL_Rect fetchSprite(int idx);
	SDL_Texture* fetchTexture();
	void setOutputDimensions(int x, int y);
};

