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
#include "attacks.h"

using namespace std;

enum PlayerSide {LEFT, MIDDLE, RIGHT};
enum AnimationType {WAIT_TO_CAST_SPELL, WAIT_TO_ATTACK, DEFEND, ATTACK, CAST_SPELL, HURT, DIE, EVADE};

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

class Fighter {
	public:
		Fighter(class Fight *f, Character c, string name, PlayerSide side, int dir);
		~Fighter();
		virtual void laden(string name);
		virtual void update();
		inline virtual int update_menu();
		virtual void draw(BITMAP *buffer, int x, int y);
		virtual void draw_status(BITMAP *buffer, int x, int y, int w, int h);
		inline virtual void draw_menu(BITMAP *buffer, int x, int y, int w, int h);
		virtual PlayerSide get_side() {return side;}
		int get_dir() {return direction;}
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
	protected:
		Fight *parent;
		friend Fight *get_parent(Fighter&);
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
				void draw(BITMAP *buffer, int x, int y, int w, int h);
				int update();
			protected:
				enum State {START, MENU, CHOOSE_TARGET, TARGETS_BY_ATTACK} state; 
				Fighter *fighter;
				BITMAP *pointer, *sub_bg;
				int auswahl;
				int pointer_position;
				int pointer_delta;
				int sub_auswahl;
				bool sub_open;
				deque< deque<string> > menu_items;

				class Command *c;
				PlayerSide target_side;
				int cur_target;
				static int mpause;
		} menu;
};

class Command {
	public:
		Command(Fighter *caster = NULL);
		virtual void add_target(Fighter *tg);
		virtual void set_attack(string attack_name);
		virtual string get_attack() {return attack_name;}
		virtual void execute();
	protected:
		Fighter *caster;
		deque<Fighter*> target;

		string attack_name;
		int calc_damage(int target_index); //MAX_DAMAGE + 1: MISS; MAX_DAMAGE + 2: BLOCK
		/*AnimationType wait_animation; //WAIT_TO_CAST_SPELL, WAIT_TO_ATTACK oder DEFEND
		AnimationType exec_animation; //CAST_SPELL oder ATTACK
		//Target führt entweder HURT, DIE oder EVADE aus
		//attack_animation <- Animation die die Attacke zeigt (zB Feuerball oder Esper)

		void(*damage_function)(Fighter*, Fighter*); <- kann alles aus attack_name gewonnen werden*/
};

class Fight {
	public:
		Fight(string dateiname = "defaultFight", Game *g = NULL);
		~Fight();
		int update();
		void draw(BITMAP *buffer);
		void enqueue_ready_fighter(Fighter *f);
		void enqueue_command(Command c);
		void block_comqueue(bool state) {command_is_executed = state;}
		int get_fighter_count(int side);
		int get_fighter_count(PlayerSide side);
		void add_fighter_target(Command &c, int fighter, int side);
		void add_fighter_target(Command &c, int fighter, PlayerSide side);
		inline void mark_fighter(int fighter, int side, bool mark);
		inline void mark_fighter(int fighter, PlayerSide side, bool mark);
		int get_index_of_fighter(Fighter*, PlayerSide);
		int get_side(Fighter*);
		PlayerSide get_PlayerSide(Fighter*);
		enum {FRIEND, ENEMY};
	private:
		Game *parent;
		enum FightType{NORMAL, BACK, PINCER, SIDE} type;
		long time;
		BITMAP *bg, *menu_bg, *auswahl;
		deque<Command> comqueue;
		bool command_is_executed;
		deque<Fighter*> fighters[2]; //Friends, Enemies
		deque<Fighter*> ready_fighters;
		bool marked_fighters[2][20];
};

#endif
