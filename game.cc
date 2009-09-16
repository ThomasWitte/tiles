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
}

Game::~Game() {
}

Game::Game(string spielstand) {
	me = NULL;
	laden(spielstand);
}

void Game::speichern(string spielstand) {
	ofstream file;
	spielstand.insert(0, "Saves/");
	file.open(spielstand.c_str(), ios_base::out);

	file << "[level]" << endl << m.get_level_name() << endl;
	file << "[userdata]" << endl;

	for(map<string, string>::iterator i = vars.begin(); i != vars.end(); i++) {
		file << "var " << i->first << " " << i->second << endl; 
	}

	file << "[eof]" << endl;

	file.close();
}

void Game::laden(string spielstand) {
	for(int i = 0; i < 6; i++)
		events[i].resize(0);

	ifstream savefile;
	spielstand.insert(0, "Saves/");
	savefile.open(spielstand.c_str(), ios_base::in);

	vars.clear();

	string input, input2;
	int state = 0;
	savefile >> input;

	while(input != "[eof]") {
		if(input == "[level]") {state = 1; savefile >> input; }
		if(input == "[userdata]") {state = 2; savefile >> input; }

		switch(state) {
			case 1:
				m.laden(input, this);
				state = 0;
			break;

			case 2:
				if(input == "var") {
					savefile >> input >> input2;
					vars[input] = input2;
				}
			break;
		}

		savefile >> input;
	}
	
	savefile.close();

	for(int i = 0; i < events[ON_LOAD].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = events[ON_LOAD][i].func;
		(this->*ptr)(&events[ON_LOAD][i]);
	}
}

void Game::register_event(vector<string> ev) {
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

	if(ev[index] == "set_var") {
		e.func = &Game::set_var;
		e.arg.push_back(ev[index+1]);
		e.arg.push_back(ev[index+2]);
	} else if(ev[index] == "change_map") {
		e.func = &Game::change_map;
		e.arg.push_back(ev[index+1]);
	}

	events[typ].push_back(e);
}

void Game::set_var(Event *e) {
	vars[e->arg[0]] = e->arg[1];
}

void Game::change_map(Event *e) {
	for(int i = 0; i < events[ON_EXIT].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = events[ON_EXIT][i].func;
		(this->*ptr)(&events[ON_EXIT][i]);
	}
	vars["last_map"] = m.get_level_name();

	m.laden(e->arg[0], this);
}

void Game::update() {
	if(me) {
		int x, y;
		me->get_position(x, y);
		x /= m.get_tilesize();
		y /= m.get_tilesize();

		for(int i = 0; i < events[PLAYER_AT].size(); i++) 
			if(x == events[PLAYER_AT][i].x && y == events[PLAYER_AT][i].y) {
				void (Game::*ptr) (Event*);
				ptr = events[PLAYER_AT][i].func;
				(this->*ptr)(&events[PLAYER_AT][i]);
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
			for(int i = 0; i < events[ON_ACTION].size(); i++) 
				if(x == events[ON_ACTION][i].x && y == events[ON_ACTION][i].y) {
					void (Game::*ptr) (Event*);
					ptr = events[ON_ACTION][i].func;
					(this->*ptr)(&events[ON_ACTION][i]);
			}
		}

	}

	for(int i = 0; i < events[ALWAYS].size(); i++) {
		void (Game::*ptr) (Event*);
		ptr = events[ALWAYS][i].func;
		(this->*ptr)(&events[ALWAYS][i]);
	}

	m.update();
}

void Game::draw() {
	m.draw();
}

