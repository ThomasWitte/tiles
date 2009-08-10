#include "map.h"
#include <fstream>
#include <iostream>

BaseObject::BaseObject(int x, int y, Map *parent) {
	this->x = x;
	this->y = y;
	this->parent = parent;
}

void BaseObject::get_position(int &x, int &y) {
	x = this->x;
	y = this->y;
}

void BaseObject::set_position(int x, int y) {
	this->x = x;
	this->y = y;
}

void BaseObject::draw(int xpos, int ypos, BITMAP *buffer) {
}

void BaseObject::update() {
}

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
	if(!tiles[0])
		tiles[0] = create_bitmap(16, 16);
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

Map::Map(string dateiname) {
	tilemap = NULL;
	walkable = NULL;
	buffer = create_bitmap(SCREEN_W, SCREEN_H);
	objects.push_back(new BaseObject(0, 0, this));
	centre(0);

	ifstream levelfile;
	dateiname.insert(0, "Levels/");
	levelfile.open(dateiname.c_str(), ios_base::in);
	string s;
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
					tilemap = new int*[tilesx];
					for(int i = 0; i < tilesx; i++)
						tilemap[i] = new int[tilesy];

					walkable = new int*[tilesx];
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
				if(s == "bobj") { //BaseDbject
					levelfile >> curx >> cury;
					objects.push_back(new BaseObject(curx, cury, this));
				}
			break;

			case 2: //Events einlesen
			break;
		}
		levelfile >> s;
	}
	levelfile.close();
	cout << "map geladen" << endl;
}

Map::~Map() {
		for(int i = 0; i < tilesx; i++)
			delete tilemap[i];
		delete tilemap;

		for(int i = 0; i < tilesx; i++)
			delete walkable[i];
		delete walkable;
		
		for(int i = 0; i < objects.size(); i++)
			delete objects[i];

		destroy_bitmap(buffer);
}

bool Map::is_walkable(int x, int y) {
	x/=current_tileset.get_tilesize();
	y/=current_tileset.get_tilesize();
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
}

void Map::draw() {
	int camx, camy, xmin, ymin, xmax, ymax, ts, ox, oy, tileindex;
	ts = current_tileset.get_tilesize();
	objects[focus]->get_position(camx, camy);
	xmin = (camx - SCREEN_W/2) / ts;
	ymin = (camy - SCREEN_H/2) / ts;
	xmax = (camx + SCREEN_W/2) / ts + 1;
	ymax = (camy + SCREEN_H/2) / ts + 1;

	for(int x = xmin; x <= xmax; x++)
		for(int y = ymin; y <= ymax; y++) {
			tileindex = 0;
			if(x > -1 && x < tilesx && y > -1 && y < tilesy)
				tileindex = tilemap[x][y];
			blit(current_tileset.get_tile(tileindex), buffer, 0, 0,
				(x*ts)-(camx-SCREEN_W/2), (y*ts)-(camy-SCREEN_H/2), ts, ts);
		}

	for(int i = 0; i < objects.size(); i++) {
		objects[i]->get_position(ox, oy);
		ox -= (camx-SCREEN_W/2);
		oy -= (camy-SCREEN_H/2);
		if(ox > -(ts+5) && ox < SCREEN_W+5 &&
			oy > -(ts+5) && oy < SCREEN_H+5)
			objects[i]->draw(ox, oy, buffer);
	}
	blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}
