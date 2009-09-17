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

#include "map.h"
#include <fstream>
#include <iostream>

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
			destroy_bitmap(tiles[i]);
}

void Tileset::load(string name) {
	for(int i = 0; i < MAX_TILES_PER_TILESET; i++) {
		if(tiles[i])
			destroy_bitmap(tiles[i]);
		tiles[i] = NULL;
		walk[i] = 0;
	}

	string prefix = "Tilesets/";
	prefix.append(name);
	prefix.append("/");

	string dateiname = prefix;
	dateiname.append(name);

	ifstream loadTS;
	loadTS.open(dateiname.c_str(), ios_base::in);
	string s, bild;
	int index, walkable;
	int state = -1;
	loadTS >> s;
	while(s != "[eof]") {
		if(s == "[Tileset]") {
			state = 0;
			loadTS >> s;
		}
		switch(state) {
			case 0: //Tileset
				loadTS >> bild >> walkable;
				index = atoi(s.c_str());
				if(index < MAX_TILES_PER_TILESET && index > -1) {
					dateiname = prefix;
					dateiname.append(bild);
					tiles[index] = load_bitmap(dateiname.c_str(), NULL);
					walk[index] = walkable;
				}
				loadTS >> s;
			break;
		}
	}
	loadTS.close();
	if(!tiles[0]) {
		tiles[0] = create_bitmap(16, 16);
		cout << "konnte Tileset nicht laden" << endl;
	} else
		cout << "tileset geladen" << endl;
}

int Tileset::is_walkable(int index) {
	return walk[index];
}

int Tileset::get_tilesize() {
	return tiles[0]->w;
}

BITMAP* Tileset::get_tile(int index) {
	return tiles[index];
}

Map::Map() : tilesx(0), tilesy(0) {
	tilemap = NULL;
	walkable = NULL;

	buffer = NULL;

	#ifdef GP2X
	buffer = create_bitmap(SCREEN_W, SCREEN_H);
	#else
	buffer = create_bitmap(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	#endif

	if(!buffer)
		cerr << "Konnte Doublebuffer nicht erzeugen" << endl;

	objects.push_back(new BaseObject(0, 0, false, this));
	centre(0);

	laden("defaultLevel", NULL);
}

Map::Map(string dateiname, Game *parent) : tilesx(0), tilesy(0) {
	tilemap = NULL;
	walkable = NULL;
	this->parent = parent;

	buffer = NULL;

	#ifdef GP2X
	buffer = create_bitmap(SCREEN_W, SCREEN_H);
	#else
	buffer = create_bitmap(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	#endif

	if(!buffer)
		cerr << "Konnte Doublebuffer nicht erzeugen" << endl;

	objects.push_back(new BaseObject(0, 0, false, this));
	centre(0);

	laden(dateiname, parent);
}

Map::~Map() {
	if(tilemap)
		for(int i = 0; i < tilesx; i++)
			delete [] tilemap[i];
		delete [] tilemap;

	if(walkable)
		for(int i = 0; i < tilesx; i++)
			delete [] walkable[i];
		delete [] walkable;

	for(int i = 0; i < objects.size(); i++)
		delete objects[i];

	for(int i = 0; i < sprites.size(); i++)
		delete sprites[i];

	for(int i = 0; i < animations.size(); i++)
		delete animations[i];

	for(int i = 0; i < dialoge.size(); i++)
		destroy_bitmap(dialoge[i].dlg);

	if(buffer)
		destroy_bitmap(buffer);
}

bool Map::is_walkable(int x, int y) {
	x/=current_tileset.get_tilesize();
	y/=current_tileset.get_tilesize();

	for(int i = 0; i < objects.size(); i++) {
		if(objects[i]->is_solid()) {
			int xp, yp;
			objects[i]->get_position(xp, yp);
			xp/=current_tileset.get_tilesize();
			yp/=current_tileset.get_tilesize();
			if(x == xp && y == yp) {
				return false;
			}
		}
	}
	
	if(x < 0 || y < 0 || x >= tilesx || y >= tilesy) return false;

	if(walkable[x][y] == 0)
		return false;
	if(walkable[x][y] == 1)
		return true;
}

void Map::laden(string dateiname, Game *parent) {
	map_name = dateiname;
	this->parent = parent;

	if(tilemap) {
		for(int i = 0; i < tilesx; i++)
			delete [] tilemap[i];
		delete [] tilemap;
	}

	if(walkable) {
		for(int i = 0; i < tilesx; i++)
			delete [] walkable[i];
		delete [] walkable;
	}

	for(int i = 1; i < objects.size(); i++) //alle außer dem ersten objekt löschen
		delete objects[i];
	objects.resize(1);
	centre(0);

	for(int i = 0; i < sprites.size(); i++)
		delete sprites[i];
	sprites.resize(0);

	for(int i = 0; i < animations.size(); i++)
		delete animations[i];
	animations.resize(0);

	for(int i = 0; i < dialoge.size(); i++)
		destroy_bitmap(dialoge[i].dlg);
	dialoge.resize(0);

	ifstream levelfile;
	dateiname.insert(0, "Levels/");
	levelfile.open(dateiname.c_str(), ios_base::in);
	string s, s2, s3;
	vector<string> parameter;
	int curx = 0;
	int cury = 0;
	int state = 3;

	levelfile >> s;
	while(s != "[eof]") {
		if(s == "[Map]") {state = 0; levelfile >> s; }
		if(s == "[Object]") {state = 1; levelfile >> s; }
		if(s == "[Event]") {state = 2; levelfile >> s; }
		switch(state) {
			case 0: //Map einlesen
				if(s == "Tileset") {
					levelfile >> s;
					current_tileset.load(s);
				} else if(s == "Size") {
					levelfile >> tilesx >> s >> tilesy;

					tilemap = new (int*[tilesx]);
					for(int i = 0; i < tilesx; i++)
						tilemap[i] = new int[tilesy];

					walkable = new (int*[tilesx]);
					for(int i = 0; i < tilesx; i++)
						walkable[i] = new int[tilesy];

					cury = 0;
					curx = 0;
				} else {
					if(curx >= tilesx) {
						curx = 0;
						cury++;
					}
					tilemap[curx][cury] = atoi(s.c_str());
					walkable[curx][cury] = current_tileset.is_walkable(tilemap[curx][cury]);
					curx++;
				}
			break;

			case 1: //Objekte einlesen
				if(s == "bobj") { //BaseObject
					levelfile >> curx >> cury;
					curx = curx - curx%current_tileset.get_tilesize() + current_tileset.get_tilesize()/2;
					cury = cury - cury%current_tileset.get_tilesize() + current_tileset.get_tilesize()/2;
					objects.push_back(new BaseObject(curx, cury, false, this));
				}

				if(s == "sprite") { //Sprite
					levelfile >> curx >> cury >> s3 >> s2;
					curx = curx - curx%current_tileset.get_tilesize() + current_tileset.get_tilesize()/2;
					cury = cury - cury%current_tileset.get_tilesize() + current_tileset.get_tilesize()/2;
					int index = -1;
					for(int i = 0; i < sprites.size(); i++) {
						if(sprites[i]->get_name() == s3) index = i;
					}
					if(index == -1) {
						sprites.push_back(new SpriteSet(s3));
						index = sprites.size()-1;
					}

					if(s2 == "player") {
						objects.push_back(new Sprite(curx, cury, Sprite::PLAYER, sprites[index], true, this));
						centre(objects.size()-1);
						parent->set_player((Sprite*)objects[objects.size()-1]);
					} else {
						objects.push_back(new Sprite(curx, cury, Sprite::NONE, sprites[index], true, this));
					}
				}

				if(s == "object") { //Object
					levelfile >> curx >> cury >> s2;
					curx = curx - curx%current_tileset.get_tilesize() + current_tileset.get_tilesize()/2;
					cury = cury - cury%current_tileset.get_tilesize() + current_tileset.get_tilesize()/2;
					int index = -1;
					for(int i = 0; i < animations.size(); i++) {
						if(animations[i]->get_name() == s2) index = i;
					}
					if(index == -1) {
						animations.push_back(new Animation(s2));
						index = animations.size()-1;
					}
					objects.push_back(new Object(curx, cury, animations[index], true, this));
				}
			break;

			case 2: //Events einlesen
				parameter.resize(0);
				while(s != ";") {
					parameter.push_back(s);
					levelfile >> s;
				}
				parent->register_event(parameter);
			break;
		}
		levelfile >> s;
	}
	levelfile.close();
	cout << "map geladen" << endl;
}

int Map::get_tilesize() {
	return current_tileset.get_tilesize();
}

void Map::centre(int index) {
	if(index < objects.size())
		focus = index;
	else
		focus = 0;
}

void Map::update() {
	for(int i = 0; i < objects.size(); i++) {
		objects[i]->update();
	}

	if(dialoge.size() > 0) {
		dialoge[0].min_frames--;
		dialoge[0].max_frames--;

		bool action = false;
		for(int i = 0; i < objects.size(); i++) {
			if(objects[i]->action) {
				action = true;
			}
			objects[i]->action = false;
		}

		if(dialoge[0].min_frames < 0) {
			if(action) {
				destroy_bitmap(dialoge[0].dlg);
				dialoge.pop_front();
				parent->action();
			}
		}
		if(dialoge[0].max_frames < 0) {
			destroy_bitmap(dialoge[0].dlg);
			dialoge.pop_front();
		}
	}
}

void Map::draw() {
	int camx, camy, xmin, ymin, xmax, ymax, ts, ox, oy, tileindex;
	
	ts = current_tileset.get_tilesize();
	objects[focus]->get_position(camx, camy);
	xmin = (camx - buffer->w/2) / ts - 1;
	ymin = (camy - buffer->h/2) / ts - 1;
	xmax = (camx + buffer->w/2) / ts + 1;
	ymax = (camy + buffer->h/2) / ts + 1;

	for(int x = xmin; x <= xmax; x++)
		for(int y = ymin; y <= ymax; y++) {
			tileindex = 0;
			if(x > -1 && x < tilesx && y > -1 && y < tilesy)
				tileindex = tilemap[x][y];
			blit(current_tileset.get_tile(tileindex), buffer, 0, 0,
				(x*ts)-(camx-buffer->w/2), (y*ts)-(camy-buffer->h/2), ts, ts);
		}

	for(int i = 0; i < objects.size(); i++) {
		objects[i]->get_position(ox, oy);
		ox -= (camx-buffer->w/2);
		oy -= (camy-buffer->h/2);
		if(ox > -(ts+5) && ox < buffer->w+5 &&
			oy > -(ts+5) && oy < buffer->h+5)
			objects[i]->draw(ox, oy, buffer);
	}

	if(dialoge.size() > 0) {
		blit(dialoge[0].dlg, buffer, 0, 0, 0, buffer->h - dialoge[0].dlg->h, dialoge[0].dlg->w, dialoge[0].dlg->h);
	}

	#ifdef GP2X
	blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	#else
	stretch_blit(buffer, screen, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y, 0, 0, SCREEN_W, SCREEN_H);
	#endif
}

string Map::get_level_name() {
	return map_name;
}

void Map::show_dialog(Dlg d) {
	dialoge.push_back(d);
}
