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
	buffer = IMGLOADER.create(SCREEN_W, SCREEN_H);
	#else
	buffer = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	#endif

	if(!buffer)
		MSG(Log::ERROR, "Game", "Konnte Doublebuffer nicht erzeugen");

	ScriptEngine::get_engine().push_pointer("game._ptr", this);
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
		IMGLOADER.destroy(buffer);
}

#ifdef ENABLE_DIALOG_MOVE_LOCK
void Game::set_move_lock(bool lock) {
	me->locked = lock;
}
#endif

void Game::speichern(std::string savefile) {
	MSG(Log::DEBUG, "Game", "Speichere " + savefile);

	if(!ScriptEngine::get_engine().do_string("game:save(\"Saves/" + savefile + ".lua\")"))
		MSG(Log::WARN, "Game", "Speichern von " + savefile + " fehlgeschlagen.");
}

int Game::luaf_load_legacy_map(lua_State *L) {
	MSG(Log::DEBUG, "Game", "luaf_load_legacy_map called.");
	if(lua_gettop(L) != 2)
		MSG(Log::WARN, "Game", "Falsche Parameterzahl für luaf_load_legacy_map.");
	//self-referenz wird nicht gebraucht
	lua_remove(L, 1);
	//Mapname
	std::string mapname = lua_tostring(L, 1);

	lua_getglobal(L, "game");
	lua_getfield(L, -1, "_ptr");
	lua_remove(L, -2);
	Game *game = (Game*)lua_touserdata(L, -1);

	game->m.laden(mapname, game);
	return 0;
}

int Game::luaf_game_run(lua_State *L) {
	if(lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
		MSG(Log::WARN, "Game", "Falsche Parameterzahl für luaf_game_run.");
	lua_getfield(L, 1, "_ptr");
	Game *game = (Game*)lua_touserdata(L, -1);
	lua_pop(L, 2);

	timecounter = 0;
#ifdef ENABLE_FRAME_COUNTER
	int drawn_frames = 0;
#endif

	bool needs_redraw = false;	
	bool ende = false;
	
	while(!ende) {
		while(timecounter) {
			timecounter--;
			needs_redraw = true;
			if(timecounter > MAX_FRAMESKIP) {
				timecounter = 0;	
				break;
			}
			game->update();
		}

#ifdef ENABLE_FRAME_COUNTER
		if(framecounter >= GAME_TIMER_BPS) {//1 Sekunde vergangen
			framecounter = 0;
			MSG(Log::INFO, "main", to_string(drawn_frames) + " fps");
			drawn_frames = 0;
		}
#endif
	
		if(needs_redraw) {
			needs_redraw = false;
			game->draw();
#ifdef ENABLE_FRAME_COUNTER
			drawn_frames++;
#endif
		} else {
			sched_yield();
		}

		if(key[MENU_KEY]) {
			switch(Menu::pause_menu()) {
				case Menu::SAVE:
					game->speichern(Menu::save_menu());
				break;
				case Menu::ENDE:
					ende = true;
				break;
				case Menu::EXIT:
					ScriptEngine::get_engine().do_string("exit = true");
					ende = true;
				break;
			}
		}
	}
	return 0;
}

int Game::luaf_game_cleanup(lua_State *L) {
	if(lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
		MSG(Log::WARN, "Game", "Falsche Parameterzahl für luaf_game_cleanup.");
	lua_getfield(L, 1, "_ptr");
	Game *game = (Game*)lua_touserdata(L, -1);
	lua_pop(L, 2);

	if(game->f) {
		delete game->f;
		game->f = NULL;
	}
	if(game->menu) {
		delete game->menu;
		game->menu = NULL;
	}

	game->last_action = 0;
	for(int i = 0; i < 6; i++)
		game->events[i].resize(0);

	game->mode = MAP;

	return 0;
}

int Game::luaf_game_exec_events(lua_State *L) {
	if(lua_gettop(L) != 2 || !lua_isuserdata(L, 1) || !lua_isstring(L, 2))
		MSG(Log::WARN, "Game", "Falsche Parameterzahl für luaf_game_cleanup.");
	lua_getfield(L, 1, "_ptr");
	Game *game = (Game*)lua_touserdata(L, -1);
	std::string type = lua_tostring(L, 2);
	lua_pop(L, 3);


	int x = ON_LOAD;
	if(type == "on_load") {
		//Gefällt mir nicht, aber wohin damit?
		game->menu = new GameMenu(game);
		x = ON_LOAD;
	} else if(type == "on_exit") {
		x = ON_EXIT;
	} else if(type == "always") {
		x = ALWAYS;
	} else if(type == "player_at") {
		x = PLAYER_AT;
	} else if(type == "on_action") {
		x = ON_ACTION;
	}

	for(unsigned int i = 0; i < game->events[x].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = game->events[x][i].func;
		(game->*ptr)(&(game->events[x][i]));
	}

	return 0;
}

int Game::luaf_game_main_menu_dlg(lua_State *L) {
	if(lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
		MSG(Log::WARN, "Game", "Falsche Parameterzahl für luaf_game_main_menu_dlg.");
	lua_getfield(L, 1, "_ptr");
	//Game *game = (Game*)lua_touserdata(L, -1);
	lua_pop(L, 2);

	switch(Menu::main_menu()) {
		case Menu::GAME:
		lua_pushnumber(L, 1);
		return 1;

		case Menu::EXIT:
		lua_pushnumber(L, 0);
		return 0;
	}

	lua_pushnil(L);
	return 1;
}

int Game::luaf_game_choose_savefile_dlg(lua_State *L) {
	if(lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
		MSG(Log::WARN, "Game", "Falsche Parameterzahl für luaf_game_choose_savefile_dlg.");
	lua_getfield(L, 1, "_ptr");
	//Game *game = (Game*)lua_touserdata(L, -1);
	lua_pop(L, 2);

	lua_pushstring(L, Menu::load_menu().c_str());
	return 1;
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
	ScriptEngine::get_engine().set_string(e->arg[0], e->arg[1]);
}

std::deque<std::string> Game::get_var_list(std::string str) {
	std::deque<std::string> ret;
	ret.push_back(str + "Funktion");
	ret.push_back(str + "zur");
	ret.push_back(str + "Zeit");
	ret.push_back(str + "entfernt");

	return ret;
}

void Game::change_map(Event *e) {
	for(unsigned int i = 0; i < events[ON_EXIT].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = events[ON_EXIT][i].func;
		(this->*ptr)(&events[ON_EXIT][i]);
	}
	ScriptEngine::get_engine().do_string("game.data.last_map = game:get_active_map().level_name");
	
	string map_to_load = e->arg[0];

	for(int i = 0; i < 6; i++)
		events[i].resize(0);

	BITMAP *start = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	BITMAP *ziel = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);

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
	
	BITMAP *start = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
	BITMAP *ziel = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);

	blit(buffer, start, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);

	f->draw(ziel);
	mode = BLENDE;
	b.init(start, ziel, Blende::ZOOM, FIGHT, GAME_TIMER_BPS/2);
}

void Game::dialog(Event *e) {
	Dlg d;
	d.dlg = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y/3);
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
			e.arg[i] = ScriptEngine::get_engine().get_string(e.arg[i+1], "nil");
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

				BITMAP *start = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				BITMAP *ziel = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
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

				BITMAP *start = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				BITMAP *ziel = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				blit(buffer, start, 0, 0, 0, 0, PC_RESOLUTION_X, PC_RESOLUTION_Y);
				m.draw(ziel);
				mode = BLENDE;
				b.init(start, ziel, Blende::REV_ZOOM, MAP, GAME_TIMER_BPS/2);
			}
		break;
		case MENU:
			if(menu->update() == 0) {//Menü wurde geschlossen
				mode = MAP;

				BITMAP *start = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
				BITMAP *ziel = IMGLOADER.create(PC_RESOLUTION_X, PC_RESOLUTION_Y);
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

void Game::set_var(string key, string val) {
	MSG(Log::DEBUG, "Game", key + "<-" + val);
	ScriptEngine::get_engine().set_string(key, val);
}

void Game::set_var(string key, double val) {
	MSG(Log::DEBUG, "Game", key + "<-" + to_string(val));
	ScriptEngine::get_engine().set_string(key, to_string(val));
}

string Game::get_var(string key) {
	string ret = ScriptEngine::get_engine().get_string(key);
	MSG(Log::DEBUG, "Game", key + "->" + ret);
	return ret;
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
				IMGLOADER.destroy(start);
				IMGLOADER.destroy(ziel);
				return mode;
			}
		break;
		case ZOOM:
			versatz += delta;
			if(versatz >= PC_RESOLUTION_Y) {
				IMGLOADER.destroy(start);
				IMGLOADER.destroy(ziel);
				return mode;
			}
		break;
		case REV_ZOOM:
			versatz -= delta;
			if(versatz <= 0) {
				IMGLOADER.destroy(start);
				IMGLOADER.destroy(ziel);
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
				IMGLOADER.destroy(start);
				IMGLOADER.destroy(ziel);
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
