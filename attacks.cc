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

#include "attacks.h"

AttackLib::Attack AttackLib::get_attack(string name) {
	Attack lib[] = {
		// pow = -1: Angriffskraft wird durch Characterwert bestimmt
		// hitr = 1000: trifft immer
		// hitr = -1: Trefferrate wird durch Waffe oder Characterwert bestimmt

		//name			effect_func,		pow,phys, ignd, unbl, bbst, hitr,	elem, Target
		{"Fight",		NULL,				-1,	true, false,false,false,-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
		{"BoltBeam",	NULL,				62,	false,false,true, false,0,		BOLT,	SELF + FRIEND + ENEMY + SINGLE},
		{"BioBlast",	&poiseiz,			60,	false,false,true, false,0,		POISON,	SELF + FRIEND + ENEMY + MULTI},
		{"Confuser",	&muddle,			0,	false,false,false,false,128,	NONE,	SELF + FRIEND + ENEMY + MULTI},
		{"FireBeam",	NULL,				60,	false,false,true, false,0,		FIRE,	SELF + FRIEND + ENEMY + SINGLE},
		{"HealForce",	NULL,				50,	false,true, true, false,0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE},
		{"IceBeam",		NULL,				61, false,false,true, false,0,		ICE,	SELF + FRIEND + ENEMY + SINGLE},
		{"TekMissile",	&seizure,			58, false,true, true, false,0,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
		{"X-Fer",		&death,				0,	false,false,false,true, 120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE},
		{"NULL",		NULL,				0,	false,false,false,false,0,		NONE,	0}
	};

	for(int i = 0; lib[i].name != "NULL"; i++)
		if(lib[i].name == name)
			return lib[i];

	return lib[0];
}

int AttackLib::poison(Fighter *caster, Fighter *target) {
	//Fügt Poison zu
	if(target->get_status(Character::POISON) != Character::IMMUNE)
		target->set_status(Character::POISON, Character::SUFFERING);
	return 0;
}

int AttackLib::seizure(Fighter *caster, Fighter *target) {
	//Fügt Seizure zu
	if(target->get_status(Character::SEIZURE) != Character::IMMUNE)
		target->set_status(Character::SEIZURE, Character::SUFFERING);
	return 0;
}

int AttackLib::poiseiz(Fighter *caster, Fighter *target) {
	//Fügt Poison und Seizure zu
	poison(caster, target);
	seizure(caster, target);
	return 0;
}

int AttackLib::death(Fighter *caster, Fighter *target) {
	//Tötet…
	if(target->get_status(Character::WOUND) != Character::IMMUNE)
		target->set_status(Character::WOUND, Character::SUFFERING);
	return 0;
}

int AttackLib::muddle(Fighter *caster, Fighter *target) {
	//Verwirrt
	if(target->get_status(Character::MUDDLE) != Character::IMMUNE)
		target->set_status(Character::MUDDLE, Character::SUFFERING);
	return 0;
}
