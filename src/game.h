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
#include <deque>
#include <string>
#include "menu.h"
#include "map.h"
#include "fight.h"
#include "fighter_base.h"
#include "game_menu.h"
#include "iohelper.h"
#include "script_engine.h"

struct Event {
	deque<string> arg;
	void (Game::*func) (Event*);
	int x, y;
};

class Game {
	public:
		Game();
		~Game();

		static int luaf_load_legacy_map(lua_State *L);
		static int luaf_game_run(lua_State *L);
		static int luaf_game_cleanup(lua_State *L);
		static int luaf_game_exec_events(lua_State *L);
		static int luaf_game_main_menu_dlg(lua_State *L);
		static int luaf_game_choose_savefile_dlg(lua_State *L);

		void register_event(deque<string> ev);
		void update();
		void draw();
		void set_player(class Sprite *s) {me = s;}
		void action() {last_action = GAME_TIMER_BPS/4;}
		void set_var(string key, string val);
		void set_var(string key, double val);
		std::deque<std::string> get_var_list(std::string str);
		string get_var(string key);
#ifdef ENABLE_DIALOG_MOVE_LOCK
		void set_move_lock(bool lock);
#endif
	protected:
		enum EVENT {ON_LOAD, ON_EXIT, ALWAYS, PLAYER_AT, ON_ACTION, EXTENDED_EVENTS};
		enum GAME_MODE {FIGHT, MAP, BLENDE, MENU};

		class Blende {
			public:
				enum BLEND_TYPE {SCHIEBEN, ZOOM, REV_ZOOM, STREIFEN};
				Blende();
				~Blende();

				void init(BITMAP* s, BITMAP *z, BLEND_TYPE t, Game::GAME_MODE m, int frames);
				void draw(BITMAP *buffer);
				Game::GAME_MODE update();
			protected:
				BITMAP *start, *ziel, *dest;
				BLEND_TYPE type;
				Game::GAME_MODE mode;
				int delta, versatz;
		};

		deque<Event> events[6]; //Anzahl siehe bei enum EVENT

		BITMAP *buffer;
		Sprite *me;
		int last_action, lastx, lasty, frame;
		Map m;
		Blende b;
		class Fight *f;
		class GameMenu *menu;
		GAME_MODE mode;

		void set_var(Event *e);
		void change_map(Event *e);
		void start_fight(Event *e);
		void dialog(Event *e);
		void set_player_position(Event *e);
		void if_function(Event *e);

		void inc_playtime(int seconds);
};

#endif
