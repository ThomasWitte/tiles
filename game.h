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

#ifndef GAME_H
#define GAME_H

#include <map>
#include <vector>
#include <string>
#include "map.h"

struct Event {
	vector<string> arg;
	void (Game::*func) (Event*);
	int x, y;
};

class Game {
	public:
		Game();
		Game(string);
		~Game();

		void speichern(string spielstand);
		void laden(string spielstand);
		void register_event(vector<string> ev);
		void update();
		void draw();
		void set_player(class Sprite *s) {me = s;}
	protected:
		enum EVENT {ON_LOAD, ON_EXIT, ALWAYS, PLAYER_AT, ON_ACTION, EXTENDED_EVENTS};
		vector<Event> events[6]; //Anzahl siehe ^

		Sprite *me;
		Map m;
		map<string, string> vars;

		void set_var(Event *e);
		void change_map(Event *e);
};

#endif
