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

#ifndef COMMAND_H
#define COMMAND_H

#include "config.h"
#include <allegro.h>
#include <string>
#include <deque>
#include "fighter_base.h"
#include "attacks.h"
#include "attack_animations.h"

using namespace std;

class Command {
	public:
		Command(class FighterBase *caster = NULL);
		~Command();
		void add_target(FighterBase *tg);
		void set_attack(string attack_name);
		string get_attack() {return attack_name;}
		bool is_target(FighterBase *tgt);
		bool is_caster(FighterBase *c);
		bool prepare();
		bool is_prepared();
		int attack_animation();
		void draw_attack_animation(BITMAP *buffer);
		
		void execute();
	protected:
		FighterBase *caster;
		deque<FighterBase*> target;

		string attack_name;
		int calc_damage(int target_index); //MAX_DAMAGE + 1: MISS; MAX_DAMAGE + 2: BLOCK
		int preparation_time;
		int animation_step, animation_ret;
		AnimationData adata;
};

#endif
