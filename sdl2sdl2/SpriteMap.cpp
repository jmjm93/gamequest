#include "SpriteMap.h"


SpriteMap::SpriteMap(){}

SpriteMap::SpriteMap(SDL_Texture* spritemap, int w, int h) {
	this->sprite_map = spritemap;
	this->sprite_w = w;
	this->sprite_h = h;
	int x, y;
	SDL_QueryTexture(spritemap, NULL, NULL, &x, &y);
	this->map_w = x;
	this->map_h = y;
	this->output_h = h;
	this->output_w = w;
}

void SpriteMap::setOutputDimensions(int x, int y) {
	output_w = x;
	output_h = y;
}
SDL_Rect SpriteMap::fetchSprite(int idx)
{
	SDL_Rect currentSprite;
	if (sprite_w == 0) sprite_w = 1;
	int sprites_per_row = map_w / sprite_w;
	currentSprite.x = sprite_w * (idx % sprites_per_row);
	currentSprite.y = sprite_h * floor(idx / sprites_per_row);
	currentSprite.w = output_w;
	currentSprite.h = output_h;
	return currentSprite;
}

SDL_Texture* SpriteMap::fetchTexture()
{
	return sprite_map;
}