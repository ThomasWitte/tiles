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

using namespace std;

struct Character {
	string name;
	int hp;
	int speed;
};

class Command {
	public:
		Command(long time);
		long get_time();
		virtual void execute();
	protected:
		long exec_time;
};

class Fighter {
	public:
		Fighter(class Fight *f, Character c, string name);
		~Fighter();
		virtual void laden(string name);
		virtual void update();
		inline virtual int update_menu();
		virtual void draw(BITMAP *buffer, int x, int y);
		virtual void draw_status(BITMAP *buffer, int x, int y, int w, int h);
		inline virtual void draw_menu(BITMAP *buffer, int x, int y, int w, int h);
	protected:
		Fight *parent;
		Character c;
		int atb;
		int step;

		struct FighterTileset {
			deque<BITMAP*> normal;
		} ts;

		class FighterMenu {
			public:
				FighterMenu();
				~FighterMenu();
				void set_items(string items[4]);
				void draw(BITMAP *buffer, int x, int y, int w, int h);
				int update();
			protected:
				BITMAP *pointer;
				int auswahl;
				int pointer_position;
				int pointer_delta;
				int pause;
				string menu_items[4];
		} menu;
};

class Fight {
	public:
		Fight(string dateiname = "defaultFight");
		~Fight();
		int update();
		void draw(BITMAP *buffer);
		void enqueue_ready_fighter(Fighter *f);
		enum PlayerSide {LEFT, MIDDLE, RIGHT};
	private:
		long time;
		BITMAP *bg, *menu_bg;
		deque<Command> comqueue;
		deque<Fighter*> fighters[3]; //PlayerSide
		deque<Fighter*> ready_fighters;
		PlayerSide side;
};

#endif
