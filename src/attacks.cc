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

		//name			effect_func,	animation,		pow,	phys, 	ignd, 	unbl, 	bbst, 	rid,	vtr,	cob,	hitr,	elem, 	Target
		{"Fight",		NULL,			NULL,			-1,		true, 	false,	false,	false,	true,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
		{"Critical",	NULL,			NULL,			24,		true,	false,	false,	false,	true,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
		{"Tusk",		NULL,			NULL,			30,		true,	false,	false,	false,	true,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
		{"BoltBeam",	NULL,			NULL,			62,		false,	false,	true, 	false,	true,	false,	false,	0,		BOLT,	SELF + FRIEND + ENEMY + SINGLE},
		{"BioBlast",	&poiseiz,		NULL,			60,		false,	false,	true, 	false,	true,	false,	false,	0,		POISON,	SELF + FRIEND + ENEMY + MULTI},
		{"Confuser",	&muddle,		NULL,			0,		false,	false,	false,	false,	true,	false,	false,	128,	NONE,	SELF + FRIEND + ENEMY + MULTI},
		{"FireBeam",	NULL,			NULL,			60,		false,	false,	true, 	false,	true,	false,	false,	0,		FIRE,	SELF + FRIEND + ENEMY + SINGLE},
		{"HealForce",	NULL,			NULL,			50,		false,	true, 	true, 	false,	false,	false,	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE},
		{"IceBeam",		NULL,			NULL,			61, 	false,	false,	true, 	false,	true,	false,	false,	0,		ICE,	SELF + FRIEND + ENEMY + SINGLE},
		{"TekMissile",	&seizure,		NULL,			58, 	false,	true, 	true, 	false,	true,	false,	false,	0,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
		{"X-Fer",		&death,			NULL,			0,		false,	false,	false,	true, 	false,	false,	false,	120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE},
		{"NULL",		NULL,			NULL,			0,		false,	false,	false,	false,	false,	false,	false,	0,		NONE,	0}
	};

	for(int i = 0; lib[i].name != "NULL"; i++)
		if(lib[i].name == name)
			return lib[i];

	return lib[0];
}

int AttackLib::full_revive(FighterBase *caster, FighterBase *target) {
	Character c = target->get_character();
	if(c.status[Character::WOUND] == Character::SUFFERING) {
		target->set_status(Character::WOUND, Character::NORMAL);
		target->lose_health(c.hp);
	}
	return 0;
}

int AttackLib::revive(FighterBase *caster, FighterBase *target) {
	Character c = target->get_character();
	if(c.status[Character::WOUND] == Character::SUFFERING) {
		target->set_status(Character::WOUND, Character::NORMAL);
		target->lose_health(-c.hp/4);
	}
	return 0;
}

int AttackLib::poison(FighterBase *caster, FighterBase *target) {
	//Fügt Poison zu
	if(target->get_status(Character::POISON) != Character::IMMUNE)
		target->set_status(Character::POISON, Character::SUFFERING);
	return 0;
}

int AttackLib::seizure(FighterBase *caster, FighterBase *target) {
	//Fügt Seizure zu
	if(target->get_status(Character::SEIZURE) != Character::IMMUNE)
		target->set_status(Character::SEIZURE, Character::SUFFERING);
	return 0;
}

int AttackLib::poiseiz(FighterBase *caster, FighterBase *target) {
	//Fügt Poison und Seizure zu
	poison(caster, target);
	seizure(caster, target);
	return 0;
}

int AttackLib::death(FighterBase *caster, FighterBase *target) {
	//Tötet…
	if(target->get_status(Character::WOUND) != Character::IMMUNE)
		target->set_status(Character::WOUND, Character::SUFFERING);
	return 0;
}

int AttackLib::muddle(FighterBase *caster, FighterBase *target) {
	//Verwirrt
	if(target->get_status(Character::MUDDLE) != Character::IMMUNE)
		target->set_status(Character::MUDDLE, Character::SUFFERING);
	return 0;
}
