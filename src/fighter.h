/*  Copyright 2009-2010 Thomas Witte

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

#ifndef FIGHTER_H
#define FIGHTER_H

#include "config.h"
#include <allegro.h>
#include <string>
#include <deque>
#include "fighter_base.h"
#include "fight.h"
#include "command.h"
#include "attacks.h"

using namespace std;

class Fighter : public FighterBase {
	public:
		Fighter(class Fight *f, Character c, string name, PlayerSide side, int dir);
		~Fighter();
		virtual void laden(string name);
		virtual void update();
		virtual inline void animate();
		virtual void draw(BITMAP *buffer, int x, int y);
		virtual void draw_status(BITMAP *buffer, int x, int y, int w, int h);
		virtual PlayerSide get_side() {return side;}
		int get_dir() {return direction;}
		int set_dir(int dir) {direction = dir%2;}
		virtual void get_ready();
		virtual bool is_monster() {return !is_friend();}; // nicht final
		virtual bool is_friend();
		Character get_character();
		void override_character(Character);
		virtual void lose_health(int);
		void show_text(string text, int color, int frames);
		int get_status(int status);
		void set_status(int status, int state); //status zB Character::DARK, state: Character::NORMAL, IMMUNE oder SUFFERING
		string get_spritename() {return spritename;}
		virtual MenuEntry *get_menu_entry(string name) {return menu.get_menu_entry(name, NULL);}
		virtual void set_animation(AnimationType type) {current_animation = type; step = 0;}

	protected:
		Fight *parent;
		friend Fight *get_parent(Fighter&);
		AnimationType current_animation;
		Character c;
		int atb;
		int itc;
		int poisoncounter;
		int step;
		PlayerSide side;
		int direction; //Blickrichtung 0 = links
		string texttoshow;
		int textremframes;
		int textcol;
		string spritename;

		struct FighterTileset {
			deque<BITMAP*> normal;
		} ts;

		class FighterMenu {
			public:
				FighterMenu();
				~FighterMenu();
				void set_parent(Fighter *fighter);
				void set_items(deque< deque<string> > items);
				MenuEntry *get_menu_entry(string name, MenuEntry *e);
			protected:
				Fighter *fighter;
	
				MenuEntry menu;
		} menu;
};

class Hero : public Fighter {
	public:
		Hero(class Fight *f, Character c, string name, PlayerSide side, int dir);
		virtual bool is_monster() {return false;}
		virtual int get_xp(int);
		virtual void draw(BITMAP *buffer, int x, int y);
};

class Monster : public Fighter {
	public:
		Monster(class Fight *f, Character c, string name, PlayerSide side, int dir);
		virtual bool is_monster() {return true;}
		virtual void update();
		virtual void draw(BITMAP *buffer, int x, int y);
		virtual void laden(string name);

		struct Treasure {
			int xp;
			int gp;
			deque<string> dropped_items;
			deque<string> stolen_items;
			int morph;
			deque<string> morph_items;
		};

		virtual Treasure treasure();
	protected:
		Treasure t;
};

#endif
