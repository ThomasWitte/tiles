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
#include <string>
#include "config.h"

using namespace std;

class BaseObject {
	protected:
		int x;
		int y;
		class Map *parent;
	public:
		BaseObject(int x, int y, Map *parent = NULL);
		virtual void get_position(int &x, int &y);
		virtual void set_position(int x, int y);
		virtual void draw(int xpos, int ypos, BITMAP *buffer);
		virtual void update();
};

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

class Map {
	private:
		vector<BaseObject*> objects;
		string map_name;
		int **tilemap;
		int **walkable;
		int tilesx;
		int tilesy;
		int focus;
		BITMAP *buffer;
		Tileset current_tileset;
	public:
		Map();
		Map(string dateiname);
		~Map();
		bool is_walkable(int x, int y);
		void laden(string dateiname);
		void centre(int index = 0);
		void update();
		void draw();
		string get_level_name();
};

#endif /* MAP_H */
