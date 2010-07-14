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

#ifndef FIGHT_H
#define FIGHT_H

#include <allegro.h>
#include <string>
#include <deque>
#include "config.h"
#include "game.h"
#include "command.h"
#include "attacks.h"
#include "fighter_base.h"

using namespace std;

class Fight {
	public:
		Fight(string dateiname = "defaultFight", Game *g = NULL);
		~Fight();
		int update();
		void draw(BITMAP *buffer);
		void enqueue_ready_fighter(FighterBase *f);
		void enqueue_command(class Command c);
		void block_comqueue(bool state) {command_is_executed = state;}
		int get_fighter_count(int side);
		int get_fighter_count(PlayerSide side);
		void add_fighter_target(Command &c, int fighter, int side);
		void add_fighter_target(Command &c, int fighter, PlayerSide side);
		void mark_fighter(int fighter, int side, bool mark);
		void mark_fighter(int fighter, PlayerSide side, bool mark);
		int get_index_of_fighter(FighterBase*, PlayerSide);
		int get_team(FighterBase*);
		int get_team(int fighter, PlayerSide side);
		void destroy_fighter(FighterBase*);
		PlayerSide get_PlayerSide(FighterBase*);
		enum {FRIEND, ENEMY};
	private:
		Game *parent;
		enum FightType{NORMAL, BACK, PINCER, SIDE} type;
		long time;
		BITMAP *bg, *menu_bg, *auswahl;
		deque<Command> comqueue;
		bool command_is_executed;
		deque<FighterBase*> fighters[2]; //Friends, Enemies
		deque<FighterBase*> ready_fighters;
		bool marked_fighters[2][20];
};

#endif
