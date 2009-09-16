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

#ifndef SPRITE_H
#define SPRITE_H

#include <allegro.h>
#include <vector>
#include <string>
#include "object.h"

using namespace std;

class SpriteSet {
		vector<BITMAP*> walk[4];
		vector<BITMAP*> wait[4];
		string name;
	public:
		enum {UP, DOWN, LEFT, RIGHT};
		SpriteSet();
		SpriteSet(string name);
		~SpriteSet();
		void load(string name);
		BITMAP* get_sprite(int direction, bool walking, int frame);
		string get_name();
};

class Sprite : public BaseObject {
	protected:
		class BaseSteuerung {
			public:
				enum CMD {WAIT, LEFT, RIGHT, UP, DOWN, ACTION};
				virtual CMD command() {return WAIT;}
		};

		class PlayerSteuerung : public BaseSteuerung {
			public:
				virtual CMD command();
		};

		BaseSteuerung *strg;
		int current_direction;
		bool walking;
		int frame;
		SpriteSet *spriteset;

	public:
		enum STRG {NONE, PLAYER};
		enum {UP, DOWN, LEFT, RIGHT};
		bool action;
	
		Sprite(int x, int y, STRG s = NONE, SpriteSet *ss = NULL, bool solid = true, class Map *parent = NULL);
		~Sprite();
		virtual void draw(int xpos, int ypos, BITMAP *buffer);
		virtual int get_direction();
		virtual void update();
};

#endif
