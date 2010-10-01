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

using namespace std;

enum AnimationType {WAIT_TO_CAST_SPELL, WAIT_TO_ATTACK, DEFEND, ATTACK, CAST_SPELL, HURT, DIE, EVADE};

class Command {
	public:
		Command(class FighterBase *caster = NULL);
		virtual void add_target(FighterBase *tg);
		virtual void set_attack(string attack_name);
		virtual string get_attack() {return attack_name;}
		virtual void execute();
	protected:
		FighterBase *caster;
		deque<FighterBase*> target;

		string attack_name;
		int calc_damage(int target_index); //MAX_DAMAGE + 1: MISS; MAX_DAMAGE + 2: BLOCK
		/*AnimationType wait_animation; //WAIT_TO_CAST_SPELL, WAIT_TO_ATTACK oder DEFEND
		AnimationType exec_animation; //CAST_SPELL oder ATTACK
		//Target führt entweder HURT, DIE oder EVADE aus
		//attack_animation <- Animation die die Attacke zeigt (zB Feuerball oder Esper)

		void(*damage_function)(FighterBase*, FighterBase*); <- kann alles aus attack_name gewonnen werden*/
};

#endif
