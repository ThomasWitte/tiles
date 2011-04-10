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

#include <fstream>
#include <iostream>
#include <string>
#include "game.h"

Game::Game() : m("defaultLevel", this) {
	me = NULL;
	f = NULL;
	menu = NULL;
	last_action = frame = 0;
	
	buffer = NULL;

	#ifdef GP2X
	buffer = imageloader.create(SCREEN_W, SCREEN_H);
	#else
	buffer = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	#endif

	if(!buffer)
		cerr << "Konnte Doublebuffer nicht erzeugen" << endl;
}

Game::~Game() {
	if(f) {
		delete f;
		f = NULL;
	}
	if(menu) {
		delete menu;
		menu = NULL;
	}
	if(buffer)
		imageloader.destroy(buffer);
}

Game::Game(string spielstand) {
	me = NULL;
	last_action = 0;
	laden(spielstand);
}

#ifdef ENABLE_DIALOG_MOVE_LOCK
void Game::set_move_lock(bool lock) {
	me->locked = lock;
}
#endif

void Game::speichern(string spielstand) {
	if(me) {
		int x, y;
		me->get_position(x, y);
		char p[5];

		sprintf(p, "%i", x/m.get_tilesize());
		vars["position_x"] = p;
		sprintf(p, "%i", y/m.get_tilesize());
		vars["position_y"] = p;
	}

	ofstream file;
	spielstand.insert(0, "Saves/");
	file.open(spielstand.c_str(), ios_base::out);

	file << "< tiles 1.0 Savefile >" << endl;
	file << "[Level]" << endl << "Level " << m.get_level_name() << " ;;" << endl;
	file << "[Userdata]" << endl;

	for(map<string, string>::iterator i = vars.begin(); i != vars.end(); i++) {
		file << i->first << " " << i->second << " ;;" << endl; 
	}

	file << "[eof]" << endl;

	file.close();
}

void Game::laden(string spielstand) {
	if(f) {
		delete f;
		f = NULL;
	}
	if(menu) {
		delete menu;
		menu = NULL;
	}

	last_action = 0;
	for(int i = 0; i < 6; i++)
		events[i].resize(0);

	spielstand.insert(0, "Saves/");
	FileParser parser(spielstand, "Savefile");

	//Variablen laden
	vars.clear();
	deque<deque<string> > ret = parser.getsection("Userdata");
	for(unsigned int i = 0; i < ret.size(); i++)
		if(ret[i].size() > 1)
			vars[ret[i][0]] = ret[i][1];

	//Map laden
	m.laden(parser.getstring("Level", "Level"), this);

	//Events laden
	Event e;
	e.func = &Game::set_player_position;
	e.arg.push_back(vars["position_x"]);
	e.arg.push_back(vars["position_y"]);
	events[ON_LOAD].push_back(e);

	for(unsigned int i = 0; i < events[ON_LOAD].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = events[ON_LOAD][i].func;
		(this->*ptr)(&events[ON_LOAD][i]);
	}

	menu = new GameMenu(this);
	mode = MAP;
}

void Game::register_event(deque<string> ev) {
	Event e;
	EVENT typ;
	int index = 1;

	if(ev[0] == "on_load") {
		typ = ON_LOAD;
	} else if(ev[0] == "on_exit") {
		typ = ON_EXIT;
	} else if(ev[0] == "always") {
		typ = ALWAYS;
	} else if(ev[0] == "player_at") {
		typ = PLAYER_AT;
		e.x = atoi(ev[1].c_str());
		e.y = atoi(ev[2].c_str());
		index = 3;
	} else if(ev[0] == "on_action") {
		typ = ON_ACTION;
		e.x = atoi(ev[1].c_str());
		e.y = atoi(ev[2].c_str());
		index = 3;
	}

	if(ev[index] == "if") {
		Event f;
		f.func = &Game::if_function;
		f.arg.push_back(ev[index+1]);
		f.arg.push_back(ev[index+2]);
		f.arg.push_back(ev[index+3]);
		f.x = e.x;
		f.y = e.y;

		if(ev[index+1] == "var") {
			f.arg.push_back(ev[index+4]);
			index++;
		}
		if(ev[index+1] == "var") {
			f.arg.push_back(ev[index+4]);
			index++;
		}

		char ind[5];
		sprintf(ind, "%i", (int)events[EXTENDED_EVENTS].size());
		f.arg.push_back(ind);
		index += 5;
		events[typ].push_back(f);
		typ = EXTENDED_EVENTS;
	}

	if(ev[index] == "set_var") {
		e.func = &Game::set_var;
		e.arg.push_back(ev[index+1]);
		e.arg.push_back(ev[index+2]);
	} else if(ev[index] == "change_map") {
		e.func = &Game::change_map;
		e.arg.push_back(ev[index+1]);
	} else if(ev[index] == "start_fight") {
		e.func = &Game::start_fight;
		e.arg.push_back(ev[index+1]);
	} else if(ev[index] == "set_player_position") {
		e.func = &Game::set_player_position;
		e.arg.push_back(ev[index+1]);
		e.arg.push_back(ev[index+2]);
	} else if(ev[index] == "dialog") {
		e.func = &Game::dialog;
		for(unsigned int i = index+1; i < ev.size(); i++)
			e.arg.push_back(ev[i]);
	}

	events[typ].push_back(e);
}

void Game::set_var(Event *e) {
	vars[e->arg[0]] = e->arg[1];
}

void Game::change_map(Event *e) {
	for(unsigned int i = 0; i < events[ON_EXIT].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = events[ON_EXIT][i].func;
		(this->*ptr)(&events[ON_EXIT][i]);
	}
	vars["last_map"] = m.get_level_name();

	string map_to_load = e->arg[0];

	for(int i = 0; i < 6; i++)
		events[i].resize(0);

	BITMAP *start = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	BITMAP *ziel = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);

	blit(buffer, start, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
	m.laden(map_to_load, this);

	for(unsigned int i = 0; i < events[ON_LOAD].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = events[ON_LOAD][i].func;
		(this->*ptr)(&events[ON_LOAD][i]);
	}
	m.draw(ziel);
	mode = BLENDE;
	b.init(start, ziel, Blende::STREIFEN, MAP, GAME_TIMER_BPS/3);
}

void Game::start_fight(Event *e) {
	f = new Fight(e->arg[0], this);
	
	BITMAP *start = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	BITMAP *ziel = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);

	blit(buffer, start, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);

	f->draw(ziel);
	mode = BLENDE;
	b.init(start, ziel, Blende::ZOOM, FIGHT, GAME_TIMER_BPS/2);
}

void Game::dialog(Event *e) {
	Dlg d;
	d.dlg = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y/3);
	for(int i = 0; i < d.dlg->h; i++) {
		line(d.dlg, 0, i, d.dlg->w, i, makecol(i, i, 255-i));
	}
	rect(d.dlg, 3, 3, d.dlg->w-4, d.dlg->h-4, makecol(255, 255, 255));
	d.min_frames = GAME_TIMER_BPS*0.25; //15 frames = 0.25s
	d.max_frames = GAME_TIMER_BPS*10;

	int zeile = 15;
	int spalte = 15;
	for(unsigned int i = 0; i < e->arg.size(); i++) {
		if(spalte + text_length(font, e->arg[i].c_str()) > d.dlg->w - 15) {
			spalte = 15;
			zeile = zeile + text_height(font) + 3;
		}
		textout_ex(d.dlg, font, e->arg[i].c_str(), spalte, zeile, makecol(255, 255, 255), -1);
		spalte = spalte + text_length(font, e->arg[i].c_str()) + 5;
	}

	m.show_dialog(d);
}

void Game::if_function(Event *e2) {
	Event e = *e2;
	bool do_it = false;

	for(unsigned int i = 0; i < e.arg.size(); i++)
		if(e.arg[i] == "var") {
			e.arg[i] = vars[e.arg[i+1]];
			if(e.arg[i] == "") e.arg[i] = "nil";
			e.arg.erase(e.arg.begin()+i+1);
		}

	if(e.arg[1] == "=" && e.arg[0] == e.arg[2]) do_it = true;
	else if(e.arg[1] == "!=" && e.arg[0] != e.arg[2]) do_it = true;
	else if(e.arg[1] == ">" && atof(e.arg[0].c_str()) > atof(e.arg[2].c_str())) do_it = true;
	else if(e.arg[1] == "<" && atof(e.arg[0].c_str()) < atof(e.arg[2].c_str())) do_it = true;

	if(do_it) {
		void (Game::*ptr) (Event*);
		ptr = events[EXTENDED_EVENTS][atoi(e.arg[3].c_str())].func;
		(this->*ptr)(&events[EXTENDED_EVENTS][atoi(e.arg[3].c_str())]);
		action();
	}
}

void Game::set_player_position(Event *e) {
	if(me)
		me->set_position(atoi(e->arg[0].c_str())*m.get_tilesize()+m.get_tilesize()/2,
						 atoi(e->arg[1].c_str())*m.get_tilesize()+m.get_tilesize()/2);
}

void Game::update() {
	switch(mode) {
		case MAP:
			m.update();
			if(key[INGAME_MENU_KEY]) { //GameMenu öffnen
				mode = MENU;
				clear_keybuf();

				BITMAP *start = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				BITMAP *ziel = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				blit(buffer, start, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
				menu->draw(ziel);
				mode = BLENDE;
				b.init(start, ziel, Blende::SCHIEBEN, MENU, GAME_TIMER_BPS/4);
			}
			if(last_action)
				key[ACTION_KEY] = 0;

			if(me) {
				int x, y;
				me->get_position(x, y);
				x /= m.get_tilesize();
				y /= m.get_tilesize();

				if(x!=lastx || y!=lasty) {
					for(unsigned int i = 0; i < events[PLAYER_AT].size(); i++) 
						if(x == events[PLAYER_AT][i].x && y == events[PLAYER_AT][i].y) {
							void (Game::*ptr) (Event*);
							ptr = events[PLAYER_AT][i].func;
							(this->*ptr)(&events[PLAYER_AT][i]);
						}
					lastx = x;
					lasty = y;
					set_var("Game.Steps", atoi(get_var("Game.Steps").c_str())+1);

					//Giftschaden
					string chars = get_var("CharactersInBattle");
					int pos = chars.find_first_of(";");
					while(pos != (int)string::npos) {
						string curchar = chars.substr(0, pos);
						if(get_var(curchar + ".status" + to_string((int)Character::POISON)) == "suffering") {
							int curhp = atoi(get_var(curchar + ".curhp").c_str());
							curhp -= atoi(get_var(curchar + ".hp").c_str())/32;
							if(curhp < 1) curhp = 1;
							set_var(curchar + ".curhp", curhp);
						}
						chars.erase(0, pos+1);
						pos = chars.find_first_of(";");
					}
				}
				switch(me->get_direction()) {
					case Sprite::UP:
						y--;
					break;
					case Sprite::DOWN:
						y++;
					break;
					case Sprite::LEFT:
						x--;
					break;
					case Sprite::RIGHT:
						x++;
					break;
				}

				if(me->action) {
					me->action = false;
					for(unsigned int i = 0; i < events[ON_ACTION].size(); i++) 
						if(x == events[ON_ACTION][i].x && y == events[ON_ACTION][i].y) {
							void (Game::*ptr) (Event*);
							ptr = events[ON_ACTION][i].func;
							(this->*ptr)(&events[ON_ACTION][i]);
					}
					action();
				}
			}

			for(unsigned int i = 0; i < events[ALWAYS].size(); i++) {
				void (Game::*ptr) (Event*);
				ptr = events[ALWAYS][i].func;
				(this->*ptr)(&events[ALWAYS][i]);
			}

			last_action--;
		break; //case MAP
		case FIGHT:
			if(f->update() == 0) {//Kampfende
				mode = MAP;
				delete f;
				f = NULL;

				BITMAP *start = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				BITMAP *ziel = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				blit(buffer, start, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
				m.draw(ziel);
				mode = BLENDE;
				b.init(start, ziel, Blende::REV_ZOOM, MAP, GAME_TIMER_BPS/2);
			}
		break;
		case MENU:
			if(menu->update() == 0) {//Menü wurde geschlossen
				mode = MAP;

				BITMAP *start = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				BITMAP *ziel = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				blit(buffer, start, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
				m.draw(ziel);
				mode = BLENDE;
				b.init(start, ziel, Blende::SCHIEBEN, MAP, GAME_TIMER_BPS/10);
			}
		break;
		case BLENDE:
			mode = b.update();
		break;
	}
	frame++;
	if(frame > GAME_TIMER_BPS) {
		frame = 0;
		inc_playtime(1);
	}
}

void Game::draw() {
	switch(mode) {
		case MAP:
			m.draw(buffer);
		break;
		case FIGHT:
			f->draw(buffer);
		break;
		case MENU:
			menu->draw(buffer);
		break;
		case BLENDE:
			b.draw(buffer);
		break;
	}

	#ifdef GP2X
	blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	#else
	stretch_blit(buffer, screen, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y, 0, 0, SCREEN_W, SCREEN_H);
	#endif
}

void Game::set_var(string key, int val) {
	vars[key] = to_string(val);
}

string Game::get_var(string key) {
	if(vars.find(key) != vars.end())
		return vars[key];
	return "";
}

void Game::inc_playtime(int seconds) {
	string pt = get_var("Game.Playtime");
	int s = 0, min = 0, h = 0;

	if(pt.find_last_of(":") != string::npos) {
		s = seconds + atoi(pt.substr(pt.find_last_of(":")+1).c_str());
		pt.erase(pt.find_last_of(":"));
	}
	if(pt.find_last_of(":") != string::npos) {
		min = (s/60) + atoi(pt.substr(pt.find_last_of(":")+1).c_str());
		pt.erase(pt.find_last_of(":"));
	}

	h = (min/60) + atoi(pt.c_str());
	s = s%60;
	min = min%60;

	char temp[15];
	sprintf(temp, "%i:%2i:%2i", h, min, s);
	pt = temp;
	set_var("Game.Playtime", pt);
}

Game::Blende::Blende() {
	start = ziel = dest = NULL;
	type = SCHIEBEN;
	mode = Game::MAP;
	versatz = 0;
}

Game::Blende::~Blende() {
}

void Game::Blende::init(BITMAP* s, BITMAP *z, BLEND_TYPE t, Game::GAME_MODE m, int frames) {
	start = s;
	ziel = z;
	type = t;
	mode = m;

	BITMAP *temp = NULL;

	switch(type) {
		case SCHIEBEN:
			delta = PC_RESOLUTION_Y/frames;
			versatz = PC_RESOLUTION_Y;
		break;
		case REV_ZOOM:
			delta = PC_RESOLUTION_Y/frames;
			versatz = PC_RESOLUTION_Y;
			temp = start;
			start = ziel;
			ziel = temp;
		break;
		case ZOOM:
			delta = PC_RESOLUTION_Y/frames;
			versatz = 0;
		break;
		case STREIFEN:
			delta = PC_RESOLUTION_Y/frames;
			versatz = 0;
		break;
	}
}

Game::GAME_MODE Game::Blende::update() {
	switch(type) {
		case SCHIEBEN:
			versatz -= delta;
			if(versatz <= 0) {
				imageloader.destroy(start);
				imageloader.destroy(ziel);
				return mode;
			}
		break;
		case ZOOM:
			versatz += delta;
			if(versatz >= PC_RESOLUTION_Y) {
				imageloader.destroy(start);
				imageloader.destroy(ziel);
				return mode;
			}
		break;
		case REV_ZOOM:
			versatz -= delta;
			if(versatz <= 0) {
				imageloader.destroy(start);
				imageloader.destroy(ziel);
				return mode;
			}
		break;
		case STREIFEN:
			for(int i = 0; i < 8; i++) {
				rectfill(start, i*PC_RESOLUTION_X/8, versatz, (2*i+1)*PC_RESOLUTION_X/16-1, versatz+delta, makecol(255, 0, 255));
				rectfill(start, (2*i+1)*PC_RESOLUTION_X/16, PC_RESOLUTION_Y-versatz-delta, (i+1)*PC_RESOLUTION_X/8-1, PC_RESOLUTION_Y-versatz, makecol(255,0,255));
			}
			versatz += delta;
			if(versatz >= PC_RESOLUTION_Y) {
				imageloader.destroy(start);
				imageloader.destroy(ziel);
				return mode;
			}
		break;
	}
	return Game::BLENDE;
}

void Game::Blende::draw(BITMAP *buffer) {
	switch(type) {
		case SCHIEBEN:
			blit(start, buffer, 0, 0, 0, PC_RESOLUTION_Y - versatz, PC_RESOLUTION_X, versatz);
			blit(ziel, buffer, 0, versatz, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y - versatz);
		break;
		case REV_ZOOM:
		case ZOOM:
			blit(start, buffer, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
			stretch_blit(ziel, buffer, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y, (PC_RESOLUTION_X-(versatz*PC_RESOLUTION_X)/PC_RESOLUTION_Y)/2, (PC_RESOLUTION_Y-versatz)/2, (versatz*PC_RESOLUTION_X)/PC_RESOLUTION_Y, versatz);
		break;
		case STREIFEN:
			blit(ziel, buffer, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
			masked_blit(start, buffer, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
		break;
	}
}
