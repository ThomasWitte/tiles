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
#include "iohelper.h"
#include <iostream>

Map::Map() : tilesx(0), tilesy(0) {
	tilemap = NULL;
	walkable = NULL;

	objects.push_back(new BaseObject(0, 0, false, this));
	centre(0);

	laden("defaultLevel", NULL);
}

Map::Map(string dateiname, Game *parent) : tilesx(0), tilesy(0) {
	tilemap = NULL;
	walkable = NULL;
	this->parent = parent;

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

	for(unsigned int i = 0; i < objects.size(); i++)
		delete objects[i];

	for(unsigned int i = 0; i < sprites.size(); i++)
		delete sprites[i];

	for(unsigned int i = 0; i < animations.size(); i++)
		delete animations[i];

	for(unsigned int i = 0; i < dialoge.size(); i++)
		IMGLOADER.destroy(dialoge[i].dlg);
}

bool Map::is_walkable(int x, int y) {
	x/=current_tileset.get_tilesize();
	y/=current_tileset.get_tilesize();

	for(unsigned int i = 0; i < objects.size(); i++) {
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
	return false;
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

	for(unsigned int i = 1; i < objects.size(); i++) //alle außer dem ersten objekt löschen
		delete objects[i];
	objects.resize(1);
	centre(0);

	for(unsigned int i = 0; i < sprites.size(); i++)
		delete sprites[i];
	sprites.resize(0);

	for(unsigned int i = 0; i < animations.size(); i++)
		delete animations[i];
	animations.resize(0);

	for(unsigned int i = 0; i < dialoge.size(); i++)
		IMGLOADER.destroy(dialoge[i].dlg);
	dialoge.resize(0);

	dateiname.insert(0, "Levels/");

	FileParser parser(dateiname, "Level");
	deque<string> ret;
	deque<deque<string> > retall;

	//Map-Abschnitt
	current_tileset.load(parser.getstring("Map", "Tileset"));

	ret = parser.get("Map", "Size");
	tilesx = atoi(ret[0].c_str());
	tilesy = atoi(ret[2].c_str());

	tilemap = new int*[tilesx];
	for(int i = 0; i < tilesx; i++)
		tilemap[i] = new int[tilesy];

	walkable = new int*[tilesx];
	for(int i = 0; i < tilesx; i++)
		walkable[i] = new int[tilesy];

	ret = parser.get("Map", "Data");
	int index = 0;

	for(int cury = 0; cury < tilesy; cury++) {
		for(int curx = 0; curx < tilesx; curx++) {
			tilemap[curx][cury] = atoi(ret[index].c_str());
			walkable[curx][cury] = current_tileset.is_walkable(tilemap[curx][cury]);
			index++;
		}
	}

	retall = parser.getall("Object", "bobj");
	for(unsigned int i = 0; i < retall.size(); i++)
		objects.push_back(new BaseObject(
			(atoi(retall[i][0].c_str())+0.5)*current_tileset.get_tilesize(),
			(atoi(retall[i][1].c_str())+0.5)*current_tileset.get_tilesize(), false, this));

	retall = parser.getall("Object", "sprite");
	for(unsigned int n = 0; n < retall.size(); n++) {
		if(retall[n][2] == "DefaultSprite" && parent->get_var("Map.DefaultSprite") != "") {
			sprites.push_back(new SpriteSet(parent->get_var("Map.DefaultSprite")));
		} else {
			sprites.push_back(new SpriteSet(retall[n][2]));
		}
		int index = sprites.size()-1;

		if(retall[n][3] == "player") {
			objects.push_back(new Sprite(
				(atoi(retall[n][0].c_str())+0.5)*current_tileset.get_tilesize(),
				(atoi(retall[n][1].c_str())+0.5)*current_tileset.get_tilesize(), Sprite::PLAYER, sprites[index], true, this));
			centre(objects.size()-1);
			parent->set_player((Sprite*)objects[objects.size()-1]);
		} else {
			objects.push_back(new Sprite(
				(atoi(retall[n][0].c_str())+0.5)*current_tileset.get_tilesize(),
				(atoi(retall[n][1].c_str())+0.5)*current_tileset.get_tilesize(), Sprite::NONE, sprites[index], true, this));
		}
	}

	retall = parser.getall("Object", "object");
	for(unsigned int n = 0; n < retall.size(); n++) {
		int index = -1;
		for(unsigned int i = 0; i < animations.size(); i++) {
			if(animations[i]->get_name() == retall[n][2]) index = i;
		}
		if(index == -1) {
			animations.push_back(new Animation(retall[n][2]));
			index = animations.size()-1;
		}
		objects.push_back(new Object(
			(atoi(retall[n][0].c_str())+0.5)*current_tileset.get_tilesize(),
			(atoi(retall[n][1].c_str())+0.5)*current_tileset.get_tilesize(), animations[index], true, this));
	}

	retall = parser.getsection("Event");
	for(unsigned int i = 0; i < retall.size(); i++)
		parent->register_event(retall[i]);

	MSG(Log::INFO, "Map", dateiname + " geladen");
}

int Map::get_tilesize() {
	return current_tileset.get_tilesize();
}

void Map::centre(unsigned int index) {
	if(index < objects.size())
		focus = index;
	else
		focus = 0;
}

void Map::update() {
	for(unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->update();
	}

	if(dialoge.size() > 0) {
#ifdef ENABLE_DIALOG_MOVE_LOCK
		parent->set_move_lock(true);
#endif
		dialoge[0].min_frames--;
		dialoge[0].max_frames--;

		bool action = false;
		for(unsigned int i = 0; i < objects.size(); i++) {
			if(objects[i]->action) {
				action = true;
			}
			objects[i]->action = false;
		}

		if(dialoge[0].min_frames < 0) {
			if(action) {
				IMGLOADER.destroy(dialoge[0].dlg);
				dialoge.pop_front();
				parent->action();
			}
		}
		if(dialoge[0].max_frames < 0) {
			IMGLOADER.destroy(dialoge[0].dlg);
			dialoge.pop_front();
		}
	} else {
#ifdef ENABLE_DIALOG_MOVE_LOCK
		parent->set_move_lock(false);
#endif
	}
}

void Map::draw(BITMAP *buffer) {
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

	for(unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->get_position(ox, oy);
		ox -= (camx-buffer->w/2);
		oy -= (camy-buffer->h/2);
		if(ox > -(ts*15) && ox < buffer->w+ts &&
			oy > -(ts+5) && oy < buffer->h+15*ts)
			objects[i]->draw(ox, oy, buffer);
	}

	if(dialoge.size() > 0) {
		blit(dialoge[0].dlg, buffer, 0, 0, 0, buffer->h - dialoge[0].dlg->h, dialoge[0].dlg->w, dialoge[0].dlg->h);
	}

}

string Map::get_level_name() {
	return map_name;
}

void Map::show_dialog(Dlg d) {
	dialoge.push_back(d);
}
