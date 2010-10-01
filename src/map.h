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

#ifndef MAP_H
#define MAP_H

#include <allegro.h>
#include <vector>
#include <deque>
#include <string>
#include "config.h"
#include "object.h"

using namespace std;

class Tileset {
		BITMAP *tiles[MAX_TILES_PER_TILESET];
		int walk[MAX_TILES_PER_TILESET];
	public:
		Tileset();
		~Tileset();
		void load(string dateiname);
		int is_walkable(int index);
		int get_tilesize();
		BITMAP* get_tile(int index);
};

struct Dlg {
	BITMAP *dlg;
	int min_frames, max_frames;
};

class Map {
	private:
		vector<class BaseObject*> objects;
		vector<class SpriteSet*> sprites;
		vector<class Animation*> animations;
		deque<Dlg> dialoge;

		string map_name;
		int **tilemap;
		int **walkable;
		int tilesx;
		int tilesy;
		int focus;
		Tileset current_tileset;
		class Game *parent;
	public:
		Map();
		Map(string dateiname, Game *parent);
		~Map();
		bool is_walkable(int x, int y);
		void laden(string dateiname, Game *parent);
		void centre(int index = 0);
		void update();
		void draw(BITMAP *buffer);
		int get_tilesize();
		string get_level_name();
		void show_dialog(Dlg d);
};

#include "game.h"

#endif /* MAP_H */
