/*  Copyright 2011 Thomas Witte

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

#ifndef TILESET_H
#define TILESET_H

#include <allegro.h>
#include <string>
#include "config.h"

using namespace std;

class Tileset {
		BITMAP *tiles[MAX_TILES_PER_TILESET];
		int walk[MAX_TILES_PER_TILESET];
	public:
		Tileset();
		~Tileset();
		void load(string dateiname);
		int is_walkable(int index);
		void toggle_walkable(int index);
		int get_tilesize();
		BITMAP* get_tile(int index);
};

#endif
