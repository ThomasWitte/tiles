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

#ifndef ATTACKLIB_H
#define ATTACKLIB_H

#include <string>
#include "fighter_base.h"

using namespace std;

class AttackLib {
	public:
		enum {SELF = 1, FRIEND = 2, ENEMY = 4, MULTI = 8, SINGLE = 16, MULTI_BOTH_SIDES = 32, DEAD = 64, RANDOM = 128};
		enum Element {NONE, HEAL, DEATH, BOLT, ICE, FIRE, WATER, WIND, EARTH, POISON, PEARL};
		struct Attack {
			string name;
			int (*effect_function)(class FighterBase*, FighterBase*);
			int (*animation)(int step, int caster_x, int caster_y, vector<int> *target_x, vector<int> *target_y);
			int power;
			bool physical;
			bool ign_def;
			bool unblock;
			bool block_by_stamina;
			bool retarget_if_dead;
			bool vulnerable_to_runic;
			bool cast_outside_battle;
			int hit_rate;
			Element element;
			int possible_targets;
		};
		static Attack get_attack(string name);
		static int death(FighterBase *caster, FighterBase *target);
		static int full_revive(FighterBase *caster, FighterBase *target);
		static int revive(FighterBase *caster, FighterBase *target);
	protected:
		static int poison(FighterBase *caster, FighterBase *target);
		static int seizure(FighterBase *caster, FighterBase *target);
		static int poiseiz(FighterBase *caster, FighterBase *target);
		static int muddle(FighterBase *caster, FighterBase *target);
		//statische overridemethoden
};

#endif
