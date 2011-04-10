/*  Copyright 2009 Thomas Witte

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sprite.h"
#include "iohelper.h"
#include <iostream>

SpriteSet::SpriteSet() {
}

SpriteSet::SpriteSet(string name) {
	load(name);
}

SpriteSet::~SpriteSet() {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < walk[i].size(); j++)
			imageloader.destroy(walk[i][j]);
		for(int j = 0; j < wait[i].size(); j++)
			imageloader.destroy(wait[i][j]);
	}
}

string SpriteSet::get_name() {
	return name;
}

void SpriteSet::load(string name) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < walk[i].size(); j++)
			imageloader.destroy(walk[i][j]);
		walk[i].resize(0);
		for(int j = 0; j < wait[i].size(); j++)
			imageloader.destroy(wait[i][j]);
		wait[i].resize(0);
	}

	this->name = name;
	string prefix = "Objects/";
	prefix.append(name);
	prefix.append("/");

	string dateiname = prefix;
	dateiname.append(name);

	FileParser parser(dateiname, "Sprite");
	string directions[] = {"up", "down", "left", "right"};
	deque< deque<string> > ret;

	for(int i = 0; i < 4; i++) {
		ret = parser.getsection(string("walk_") + directions[i]);
		for(int j = 0; j < ret.size(); j++)
			walk[i].push_back(imageloader.load(prefix + ret[j][0]));
		ret = parser.getsection(string("wait_") + directions[i]);
		for(int j = 0; j < ret.size(); j++)
			wait[i].push_back(imageloader.load(prefix + ret[j][0]));
	}
}

BITMAP* SpriteSet::get_sprite(int direction, bool walking, int frame) {
	frame /= SPRITE_ANIMATION_SPEED;
	if(walking)
		return walk[direction][frame%walk[direction].size()];
	else
		return wait[direction][frame%wait[direction].size()];
}

Sprite::BaseSteuerung::CMD Sprite::PlayerSteuerung::command() {
	CMD ret = WAIT;

	if(key[DIR_UP]) ret = UP;
	else if (key[DIR_DOWN]) ret = DOWN;
	else if (key[DIR_LEFT]) ret = LEFT;
	else if (key[DIR_RIGHT]) ret = RIGHT;
	else if (key[ACTION_KEY]) ret = ACTION;

	return ret;
}

Sprite::Sprite(int x, int y, STRG s, SpriteSet *ss, bool solid, Map *parent) : BaseObject(x, y, solid, parent) {
	switch(s) {
		case NONE:
			strg = new BaseSteuerung();
		break;
		case PLAYER:
			strg = new PlayerSteuerung();
		break;
	}
	spriteset = ss;
	walking = false;
	action = false;
	current_direction = UP;
	frame = 0;
}

Sprite::~Sprite() {
	delete strg;
}

void Sprite::draw(int xpos, int ypos, BITMAP *buffer) {
	BITMAP *sprite = spriteset->get_sprite(current_direction, walking, frame);
	draw_sprite(buffer, sprite, xpos-parent->get_tilesize()/2, ypos-sprite->h+parent->get_tilesize()/2);
}

int Sprite::get_direction() {
	return current_direction;
}

void Sprite::update() {
#ifdef ENABLE_DIALOG_MOVE_LOCK
	int cdir = current_direction;
	int wk = true;
#endif
	frame++;

	if((x+parent->get_tilesize()/2)%parent->get_tilesize() == 0 && (y+parent->get_tilesize()/2)%parent->get_tilesize() == 0) {
		walking = false;
#ifdef ENABLE_DIALOG_MOVE_LOCK
		wk = false;
#endif
		switch(strg->command()) {
			case BaseSteuerung::UP:
				current_direction = UP;
				if(parent->is_walkable(x, y-parent->get_tilesize()))
					walking = true;
			break;
			case BaseSteuerung::DOWN:
				current_direction = DOWN;
				if(parent->is_walkable(x, y+parent->get_tilesize()))
					walking = true;
			break;
			case BaseSteuerung::LEFT:
				current_direction = LEFT;
				if(parent->is_walkable(x-parent->get_tilesize(), y))
					walking = true;
			break;
			case BaseSteuerung::RIGHT:
				current_direction = RIGHT;
				if(parent->is_walkable(x+parent->get_tilesize(), y))
					walking = true;
			break;
			case BaseSteuerung::ACTION:
				action = true;
			break;
		}
	}

#ifdef ENABLE_DIALOG_MOVE_LOCK
	if(locked) {
		walking = wk;
		current_direction = cdir;
	}
#endif

	if(walking)
		switch(current_direction) {
			case UP:
				y--;
			break;
			case DOWN:
				y++;
			break;
			case LEFT:
				x--;
			break;
			case RIGHT:
				x++;
			break;
		}
}
