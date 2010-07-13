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

using namespace std;

class AttackLib {
	public:
		enum {SELF = 1, FRIEND = 2, ENEMY = 4, MULTI = 8, SINGLE = 16, MULTI_BOTH_SIDES = 32, DEAD = 64};
		enum Element {NONE, HEAL, DEATH, BOLT, ICE, FIRE, WATER, WIND, EARTH, POISON, PEARL};
		struct Attack {
			string name;
			int (*effect_function)(class Fighter*, Fighter*);
			int power;
			bool physical;
			bool ign_def;
			bool unblock;
			bool block_by_stamina;
			int hit_rate;
			Element element;
			int possible_targets;
		};
		static Attack get_attack(string name);
		static int death(Fighter *caster, Fighter *target);
	protected:
		static int poison(Fighter *caster, Fighter *target);
		static int seizure(Fighter *caster, Fighter *target);
		static int poiseiz(Fighter *caster, Fighter *target);
		static int muddle(Fighter *caster, Fighter *target);
		//statische overridemethoden
};

#include "fight.h"

#endif
