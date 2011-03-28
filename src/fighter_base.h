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

#ifndef FIGHTER_BASE_H
#define FIGHTER_BASE_H

#include "config.h"
#include <allegro.h>
#include <string>
#include <deque>

using namespace std;

enum PlayerSide {LEFT, MIDDLE, RIGHT};

struct Character {
	enum {NORMAL, WEAK, ABSORB, IMMUNE, RESISTANT, SUFFERING};
	enum {DARK, ZOMBIE, POISON, MTEK, CLEAR, IMP, PETRIFY, WOUND, CONDEMNED, NEAR_FATAL, IMAGE, MUTE, BERSERK, MUDDLE, SEIZURE, SLEEP, DANCE, REGEN, SLOW, HASTE, STOP, SHELL, SAFE, REFLECT, MORPH};

	string name;
	bool defensive;
	int hp;
	int curhp;
	int mp;
	int curmp;
	int speed;
	int vigor;
	int stamina;
	int mpower;
	int apower;
	int mdefense;
	int adefense;
	int mblock;
	int ablock;
	int xp;
	int levelupxp;
	int level;
	int hitrate; // wird von waffe vorgegeben, bei kampf ohne waffe gild dieser wert
	int elements[11];
	int status[25];
};

class FighterBase {
	public:
		enum AnimationType {NORMAL, WAIT_TO_CAST_SPELL, WAIT_TO_ATTACK, DEFEND, ATTACK, ATTACK_IN_PROGRESS, RETURN, HURT, DIE, EVADE};

		struct MenuEntry {
			string text;
			deque<MenuEntry> submenu; 
		};

		FighterBase() {}
		FighterBase(class Fight *f, Character c, string name, PlayerSide side, int dir) {}
		virtual ~FighterBase() {}
		virtual void laden(string) = 0;
		virtual void update() = 0;
		virtual void animate() = 0;
		virtual void draw(BITMAP *buffer, int x, int y) = 0;
		virtual void draw_status(BITMAP *buffer, int x, int y, int w, int h) = 0;
		virtual PlayerSide get_side() = 0;
		virtual int get_dir() = 0;
		virtual void set_dir(int dir) = 0;
		virtual void get_ready() = 0;
		virtual bool is_monster() = 0;
		virtual bool is_friend() = 0;
		virtual Character get_character() = 0;
		virtual void override_character(Character) = 0;
		virtual void lose_health(int) = 0;
		virtual void show_text(string text, int color, int frames) = 0;
		virtual int get_status(int status) = 0;
		virtual void set_status(int status, int state) = 0;
		virtual string get_spritename() = 0;
		virtual void set_animation(AnimationType) = 0;
		virtual MenuEntry* get_menu_entry(string name) = 0;
		virtual void get_screen_position(int *x, int *y) = 0;
};

#include "fight.h"

#endif
