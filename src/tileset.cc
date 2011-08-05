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

#include "tileset.h"
#include "iohelper.h"
#include <deque>

Tileset::Tileset() {
	for(int i = 0; i < MAX_TILES_PER_TILESET; i++) {
		tiles[i] = NULL;
		walk[i] = 0;
	}
	load("defaultTileset");
}

Tileset::~Tileset() {
	for(int i = 0; i < MAX_TILES_PER_TILESET; i++)
		if(tiles[i])
			IMGLOADER.destroy(tiles[i]);
}

void Tileset::load(string name) {
	for(int i = 0; i < MAX_TILES_PER_TILESET; i++) {
		if(tiles[i])
			IMGLOADER.destroy(tiles[i]);
		tiles[i] = NULL;
		walk[i] = 0;
	}

	string prefix = string("Tilesets/") + name + string("/");

	FileParser parser(prefix + name, "Tileset");

	deque<deque<string> > ret = parser.getsection("Tileset");
	
	int index;
	for(unsigned int i = 0; i < ret.size(); i++) {
		index = atoi(ret[i][0].c_str());
		if(index < MAX_TILES_PER_TILESET && index > -1) {
			tiles[index] = IMGLOADER.load(prefix + ret[i][1]);
			walk[index] = atoi(ret[i][2].c_str());
		}
	}

	if(!tiles[0]) {
		tiles[0] = IMGLOADER.create(16, 16);
		MSG(Log::ERROR, "Tileset", "Tileset " + name + " konnte nicht geladen werden.");
	} else
		MSG(Log::DEBUG, "Tileset", "Tileset " + name + " geladen.");
}

int Tileset::is_walkable(int index) {
	return walk[index];
}

void Tileset::toggle_walkable(int index) {
	walk[index] = !walk[index];
}

int Tileset::get_tilesize() {
	return tiles[0]->w;
}

BITMAP* Tileset::get_tile(int index) {
	return tiles[index];
}
