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

template<>
int AttackLib::inflict<Character::WOUND>(FighterBase *caster, FighterBase *target) {
	if(target->get_status(Character::ZOMBIE) != Character::SUFFERING && !target->get_special(Character::UNDEAD)) {
		if(target->get_status(Character::WOUND) != Character::IMMUNE) {
			target->set_status(Character::WOUND, Character::SUFFERING);
		}
	} else {
		target->lose_health(-MAX_DAMAGE);
	}
	return 0;
}

template<>
int AttackLib::inflict<Character::CONDEMNED>(FighterBase *caster, FighterBase *target) {
	if(	target->get_status(Character::CONDEMNED) != Character::IMMUNE &&
		target->get_status(Character::WOUND) != Character::SUFFERING) {
		target->set_status(Character::CONDEMNED, Character::SUFFERING);
		if(target->condemnedcounter < 1) {
			int level = caster->get_character().level;
			target->condemnedcounter = 79-level-random()%level;
			if(target->condemnedcounter < 20)
				target->condemnedcounter = 20;
		}
	}
	return 0;
}

AttackLib::Attack AttackLib::lib[] = {
	// pow = -1: Angriffskraft wird durch Characterwert bestimmt
	// hitr = 255: trifft immer
	// hitr = -1: Trefferrate wird durch Waffe oder Characterwert bestimmt

	//name			effect_func,					animation,		mp,		pow,	phys, 	ignd, 	unbl, 	bbst, 	rid,	vtr,	refl,	cob,	hitr,	elem, 	Target										Description
	{"Antidot",		&heal<Character::POISON>,		&heal_ani,		3,		0,		false,	true,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Cures poisoning"},
	{"Axe",			NULL,							NULL,			0,		32,		true,	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Battle",		NULL,							NULL,			0,		-1,		true, 	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Bio",			&inflict<Character::POISON>,	NULL,			26,		53,		false,	false,	false,	false,	true,	true,	true,	false,	120,	POISON,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Poison-elemental attack"},
	{"BioBlast",	&poiseiz,						NULL,			0,		60,		false,	false,	true, 	false,	true,	false,	false,	false,	0,		POISON,	SELF + FRIEND + ENEMY + MULTI,				""},
	{"Bolt",		NULL,							NULL,			6,		20,		false,	false,	false,	false,	true,	true,	true,	false,	150,	BOLT,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Lightning-elemental attack"},
	{"Bolt2",		NULL,							NULL,			22,		61,		false,	false,	false,	false,	true,	true,	true,	false,	150,	BOLT,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Lightning-elemental attack"},
	{"Bolt3",		NULL,							NULL,			53,		120,	false,	false,	false,	false,	true,	true,	true,	false,	150,	BOLT,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Lightning-elemental attack"},
	{"BoltBeam",	NULL,							NULL,			0,		62,		false,	false,	true, 	false,	true,	false,	false,	false,	0,		BOLT,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Break",		&inflict<Character::PETRIFY>,	NULL,			25,		0,		false,	false,	false,	true,	true,	true,	true,	false,	120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE,				"Turns an enemy to stone"},
	{"Bserk",		&inflict<Character::BERSERK>,	NULL,			16,		0,		false,	false,	false,	false,	false,	true,	true,	false,	150,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Casts Berserk on target"},
	{"Confuser",	&inflict<Character::MUDDLE>,	NULL,			0,		0,		false,	false,	false,	false,	true,	false,	false,	false,	128,	NONE,	SELF + FRIEND + ENEMY + MULTI,				""},
	{"Counter",		NULL,							NULL,			0,		18,		true,	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Critical",	NULL,							NULL,			0,		24,		true,	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Cure",		NULL,							&heal_ani,		5,		10,		false,	true,	true,	false,	false,	true,	true,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Recovers HP"},
	{"Cure2", 		NULL,							&heal_ani,		25,		28,		false,	true,	true,	false,	false,	true,	true,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Recovers HP"},
	{"Cure3", 		NULL,							&heal_ani,		40,		66,		false,	true,	true,	false,	false,	true,	true,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Recovers HP"},
	{"Demi",		&partdmg<50>,					NULL,			33,		0,		false,	false,	false,	true,	true,	true,	false,	false,	120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE,				"Cuts an enemy's HP in half"},
	{"Dispel",		&dispel,						NULL,			25,		0,		false,	false,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Maintains poor status"},
	{"Doom",		&inflict<Character::WOUND>,		NULL,			35,		0,		false,	false,	false,	true,	true,	true,	true,	false,	95,		DEATH,	SELF + FRIEND + ENEMY + SINGLE,				"Dispatches an enemy"},
	{"Drain", 		&drain,							NULL,			15,		0,		false,	false,	false,	false,	true,	true,	false,	false,	120,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Drains HP from an enemy"},
	{"_Drain", 		NULL,							NULL,			0,		38,		false,	false,	false,	false,	false,	true,	false,	false,	255,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Fatal",		&inflict<Character::WOUND>,		NULL,			0,		0,		true,	false,	false,	true, 	false,	false,	false,	false,	-1,		DEATH,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Fight",		NULL,							&fight_ani,		0,		-1,		true, 	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Fire",		NULL,							NULL,			4,		21,		false,	false,	false,	false,	true,	true,	true,	false,	150,	FIRE,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Fire-elemental attack"},
	{"Fire2",		NULL,							NULL,			20,		60,		false,	false,	false,	false,	true,	true,	true,	false,	150,	FIRE,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Fire-elemental attack"},
	{"Fire3",		NULL,							NULL,			51,		121,	false,	false,	false,	false,	true,	true,	true,	false,	150,	FIRE,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Fire-elemental attack"},
	{"FireBeam",	NULL,							NULL,			0,		60,		false,	false,	true, 	false,	true,	false,	false,	false,	0,		FIRE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Flare",		NULL,							NULL,			45,		60,		false,	true,	false,	false,	true,	true,	true,	false,	150,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Barrier-piercing attack"},
	{"Float",		&inflict<Character::FLOAT>,		NULL,			17,		0,		false,	false,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Makes target float"},
	{"Haste",		&inflict<Character::HASTE>,		NULL,			10,		0,		false,	false,	true,	false,	false,	true,	true,	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Raises battle speed"},
	{"Haste2",		&inflict<Character::HASTE>,		NULL,			38,		0,		false,	false,	true,	false,	false,	true,	true,	false,	0,		HEAL,	SELF + FRIEND + ENEMY + MULTI,				"Speeds up entire party"},
	{"HealForce",	NULL,							&heal_ani,		0,		50,		false,	true, 	true, 	false,	false,	false,	false,	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Ice",			NULL,							NULL,			5,		22,		false,	false,	false,	false,	true,	true,	true,	false,	150,	ICE,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Ice-elemental attack"},
	{"Ice2",		NULL,							NULL,			21,		62,		false,	false,	false,	false,	true,	true,	true,	false,	150,	ICE,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Ice-elemental attack"},
	{"Ice3",		NULL,							NULL,			52,		122,	false,	false,	false,	false,	true,	true,	true,	false,	150,	ICE,	SELF + FRIEND + ENEMY + SINGLE + MULTI,		"Ice-elemental attack"},
	{"IceBeam",		NULL,							NULL,			0,		61, 	false,	false,	true, 	false,	true,	false,	false,	false,	0,		ICE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Inviz",		&inflict<Character::CLEAR>,		NULL,			0,		0,		true,	false,	false,	false,	false,	false,	false,	false,	-1,		NONE,	SELF,										""},
	{"Imp",			&toggle<Character::IMP>,		NULL,			10,		0,		false,	false,	false,	false,	false,	true,	true,	true,	100,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Changes to/from \"Imp\""},
	{"Life",		&revive,						NULL,			30,		0,		false,	false,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + DEAD,		"Recovers life"},
	{"Life2",		&full_revive,					NULL,			60,		0,		false,	false,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + DEAD,		"Restores life and HP/MP"},
	{"Life3", 		&inflict<Character::LIFE3>,		NULL,			50,		0,		false,	false,	true,	false,	false,	true,	false,	false,	0,		HEAL,	SELF + FRIEND + SINGLE,						"Protects from wound"},
	{"Merton",		NULL,							NULL,			85,		138,	false,	true,	true,	false,	false,	false,	false,	false,	0,		FIRE,	SELF + FRIEND + ENEMY + MULTI_BOTH_SIDES,	"Unfocused piercing fire attack"},
	{"Meteor",		NULL,							NULL,			62,		62,		false,	true,	true,	false,	false,	false,	false,	false,	0,		NONE, 	ENEMY + MULTI_BOTH_SIDES,					"Damages multiple enemies"},
	{"Muddle",		&inflict<Character::MUDDLE>,	NULL,			8,		0,		false,	false,	false,	false,	false,	true,	true,	false,	94,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Confuses target"},
	{"Mute",		&inflict<Character::MUTE>,		NULL,			8,		0,		false,	false,	false,	false,	false,	true,	true,	false,	100,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Silences target"},
	{"Osmose", 		&osmose,						NULL,			1,		0,		false,	false,	false,	false,	true,	true,	false,	false,	150,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Drains HP from an enemy"},
	{"_Osmose", 	NULL,							NULL,			0,		26,		false,	false,	false,	false,	true,	false,	false,	false,	255,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Pearl",		NULL,							NULL,			40,		108,	false,	false,	false,	false,	true,	true,	true,	false,	150,	PEARL,	SELF + FRIEND + ENEMY + SINGLE,				"Pearl-elemental attack"},
	{"Poison",		&inflict<Character::POISON>,	NULL,			3,		25,		false,	false,	false,	false,	true,	true,	true,	false,	100,	POISON,	SELF + FRIEND + ENEMY + SINGLE,				"Poisons an enemy"},
	{"Quake",		NULL,							NULL,			50,		111,	false,	true,	true,	false,	false,	false,	false,	false,	0,		EARTH,	SELF + FRIEND + ENEMY + MULTI_BOTH_SIDES,	"Unfocused Earth-elemental attack"},
	{"Quartr",		&partdmg<75>,					NULL,			48,		0,		false,	false,	false,	true,	true,	true,	false,	false,	100,	DEATH,	ENEMY + MULTI_BOTH_SIDES,					"Cuts an enemy's HP by 3/4"},
	{"Rasp",		&rasp,							NULL,			12,		0,		false,	false,	false,	false,	true,	true,	true,	false,	150,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Damages MP"},
	{"_Rasp", 		NULL,							NULL,			0,		10,		false,	false,	false,	false,	false,	true,	false,	false,	255,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Regen",		&inflict<Character::REGEN>,		&heal_ani,		10,		0,		false,	false,	true,	false,	false,	true,	true,	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Gradually recovers HP"},
	{"Remedy",		&remedy,						NULL,			15,		0,		false,	false,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Cures status ailments"},
	{"Rflect",		&inflict<Character::REFLECT>,	NULL,			22,		0,		false,	false,	true,	false,	false,	true,	true,	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Casts a magic barrier on target"},
	{"Safe",		&inflict<Character::SAFE>,		NULL,			12,		0,		false,	false,	true,	false,	false,	true,	true, 	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Raises defense"},
	{"Shell",		&inflict<Character::SHELL>,		NULL,			15,		0,		false,	false,	true,	false,	false,	true,	true, 	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE,				"Raises magic defense"},
	{"Sleep",		&inflict<Character::SLEEP>,		NULL,			5,		0,		false,	false,	false,	false,	false,	true,	true,	false,	111,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Puts target to sleep"},
	{"Slow",		&inflict<Character::SLOW>,		NULL,			5,		0,		false, 	false, 	false, 	false, 	false, 	true,	true, 	false, 	120,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Slows battle speed"},
	{"Slow2",		&inflict<Character::SLOW>,		NULL,			26,		0,		false, 	false, 	false, 	false, 	false, 	true,	true, 	false,	150,	NONE,	SELF + FRIEND + ENEMY + MULTI,				"Slows multiple enemies"},
	{"Stop",		&inflict<Character::STOP>,		NULL,			10,		0,		false,	false, 	false, 	false, 	false, 	true,	true, 	false,	100,	NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Stops an enemy"},
	{"TekMissile",	&inflict<Character::SEIZURE>,	NULL,			0,		58, 	false,	true, 	true, 	false,	true,	false,	false,	false,	0,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Tusk",		NULL,							NULL,			0,		30,		true,	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"Ultima",		NULL,							NULL,			80,		150,	false,	true,	true,	false,	false,	true,	false,	false,	0,		NONE,	ENEMY + MULTI_BOTH_SIDES,					"Damages multiple enemies"},
	{"Vanish",		&toggle<Character::CLEAR>,		NULL,			18,		0,		false,	false,	true, 	false, 	false,	true,	false,	false,	0,		NONE,	SELF + FRIEND + ENEMY + SINGLE,				"Renders target invisible"},
	{"WWind",		&partdmg<94>,					NULL,			75,		0,		false,	false,	false,	true,	false,	false,	false,	false,	100,	DEATH,	SELF + FRIEND + ENEMY + MULTI_BOTH_SIDES,	"Unfocused near-fatal attack"},
	{"X-Fer",		&inflict<Character::WOUND>,		NULL,			0,		0,		false,	false,	false,	true, 	false,	false,	false,	false,	120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE,				""},
	{"X-Zone",		&inflict<Character::WOUND>,		NULL,			53,		0,		false,	false,	false,	true,	false,	true,	false,	false,	85,		DEATH,	ENEMY + MULTI_BOTH_SIDES,					"Sends an enemy into the X-Zone"},
	{"NULL",		NULL,							NULL,			0,		0,		false,	false,	false,	false,	false,	false,	false,	false,	0,		NONE,	0,											""}
	//name			effect_func,					animation,		mp,		pow,	phys, 	ignd, 	unbl, 	bbst, 	rid,	vtr,	refl,	cob,	hitr,	elem, 	Target										Description
};

AttackLib::Attack AttackLib::get_attack(string name) {
	for(int i = 0; lib[i].name != "NULL"; i++)
		if(lib[i].name == name)
			return lib[i];

	return lib[0];
}

int AttackLib::full_revive(FighterBase *caster, FighterBase *target) {
	Character c = target->get_character();
	if(c.status[Character::WOUND] == Character::SUFFERING) {
		target->set_status(Character::WOUND, Character::NORMAL);
		target->lose_health(-MAX_DAMAGE);
	} else if(c.status[Character::ZOMBIE] == Character::SUFFERING || c.special[Character::UNDEAD]) {
		kill(caster, target);
	}
	return 0;
}

int AttackLib::revive(FighterBase *caster, FighterBase *target) {
	Character c = target->get_character();
	if(c.status[Character::WOUND] == Character::SUFFERING) {
		target->set_status(Character::WOUND, Character::NORMAL);
		target->lose_health(-c.hp/8);
	} else if(c.status[Character::ZOMBIE] == Character::SUFFERING || c.special[Character::UNDEAD]) {
		kill(caster, target);
	}
	return 0;
}

int AttackLib::poiseiz(FighterBase *caster, FighterBase *target) {
	//Fügt Poison und Seizure zu
	inflict<Character::POISON>(caster, target);
	inflict<Character::SEIZURE>(caster, target);
	return 0;
}

int AttackLib::dispel(FighterBase *caster, FighterBase *target) {
	heal<Character::CLEAR>(caster, target);
	heal<Character::IMAGE>(caster, target);
	heal<Character::BERSERK>(caster, target);
	heal<Character::REGEN>(caster, target);
	heal<Character::SLOW>(caster, target);
	heal<Character::HASTE>(caster, target);
	heal<Character::STOP>(caster, target);
	heal<Character::SHELL>(caster, target);
	heal<Character::SAFE>(caster, target);
	heal<Character::REFLECT>(caster, target);
	heal<Character::LIFE3>(caster, target);
	heal<Character::FLOAT>(caster, target);
	return 0;
}

int AttackLib::remedy(FighterBase *caster, FighterBase *target) {
	heal<Character::DARK>(caster, target);
	heal<Character::POISON>(caster, target);
	heal<Character::PETRIFY>(caster, target);
	heal<Character::MUTE>(caster, target);
	heal<Character::MUDDLE>(caster, target);
	heal<Character::SEIZURE>(caster, target);
	heal<Character::SLEEP>(caster, target);
	heal<Character::SLOW>(caster, target);
	heal<Character::STOP>(caster, target);
	return 0;
}

int AttackLib::drain(FighterBase *caster, FighterBase *target) {
	int dmg = calc_damage(caster, target, get_attack("_Drain"), false);

	Character ctarget = target->get_character();
	if(dmg > ctarget.curhp)
		dmg = ctarget.curhp;

	if(target->get_status(Character::ZOMBIE) == Character::SUFFERING || target->get_special(Character::UNDEAD))
		dmg *= -1;

	caster->lose_health(-dmg);
	target->lose_health(dmg);

	return 0;
}

int AttackLib::osmose(FighterBase *caster, FighterBase *target) {
	int dmg = calc_damage(caster, target, get_attack("_Osmose"), false);

	Character ctarget = target->get_character();
	if(dmg > ctarget.curmp)
		dmg = ctarget.curmp;

	if(target->get_status(Character::ZOMBIE) == Character::SUFFERING || target->get_special(Character::UNDEAD))
		dmg *= -1;

	caster->lose_mp(-dmg);
	target->lose_mp(dmg);

	return 0;
}

int AttackLib::rasp(FighterBase *caster, FighterBase *target) {
	int dmg = calc_damage(caster, target, get_attack("_Rasp"), false);
	target->lose_mp(dmg);

	return 0;
}

int AttackLib::kill(FighterBase *caster, FighterBase *target) {
	target->set_status(Character::WOUND, Character::SUFFERING);
	return 0;
}

int AttackLib::calc_damage(FighterBase *caster, FighterBase *target, Attack a, bool multitarget) {
	int dmg = 0;
	bool character = !caster->is_monster();
	Character ccaster = caster->get_character();
	Character ctarget = target->get_character();
	//Step1
	if(a.physical == true) {
		int bpower = a.power;
		if(a.power < 0) bpower = ccaster.apower;

		if(character) {
			int vigor2 = 2*ccaster.vigor;
			if(vigor2 > 255) vigor2 = 255;
			int bpower = a.power;
			if(a.power < 0) bpower = ccaster.apower;
			//if equipped with gauntlet… 1c
			dmg = bpower + ((ccaster.level * ccaster.level * (vigor2+bpower)) / 256) * 3/2;
			//1e
			//1f
		} else { //Monster
			dmg = ccaster.level * ccaster.level * (bpower * 4 + ccaster.vigor) / 256;
		}
	} else { //Magische Attacke
		if(character) {
			dmg = a.power * 4 + (ccaster.level * ccaster.mpower * a.power / 32);
		} else {
			dmg = a.power * 4 + (ccaster.level * (ccaster.mpower * 3/2)  * a.power / 32);
		}
	}
	//Step2
	//Step3
	if((!a.physical) && (a.possible_targets & AttackLib::SINGLE) && multitarget)
		dmg /= 2;
	//Step4
	if(a.name == "Fight" && ccaster.defensive)
		dmg /= 2;
	//Step5
	int dmg_multiplier = 0;
	if(random()%32 == 0 && a.name == "Fight") dmg_multiplier += 2;
	if(ccaster.status[Character::MORPH] == Character::SUFFERING) dmg_multiplier += 2;
	if(a.physical && ccaster.status[Character::BERSERK] == Character::SUFFERING) dmg_multiplier++;
	dmg += (dmg/2) * dmg_multiplier;
	//Step6
	dmg = (dmg * (random()%32 + 224) / 256) + 1;
	if(a.physical)
		dmg = (dmg * (255 - ctarget.adefense) / 256) + 1;
	else
		dmg = (dmg * (255 - ctarget.mdefense) / 256) + 1;

	if((a.physical && ctarget.status[Character::SAFE] == Character::SUFFERING) ||
		(!a.physical && ctarget.status[Character::SHELL] == Character::SUFFERING))
		dmg = (dmg * 170/256) + 1;
	//6e
	if(a.physical && ctarget.defensive) {
		dmg /= 2;
	}
	//6f
	if(!a.physical && ctarget.status[Character::MORPH] == Character::SUFFERING)
		dmg /= 2;
	//6g
	if((a.element != AttackLib::HEAL) && !caster->is_monster() && !target->is_monster())
		dmg /= 2;
	//Step7
	if(a.physical && ((target->get_dir() == 0 && caster->get_side() > target->get_side()) || //Ziel schaut nach links 
					  (target->get_dir() == 1 && caster->get_side() < target->get_side()))) { //Ziel schaut nach rechts
		dmg *= 1.5;
	}
	//Step8
	if(ctarget.status[Character::PETRIFY] == Character::SUFFERING)
		dmg = 0;
	//Step9
	//9a
	if(ctarget.elements[a.element] == Character::ABSORB)
		dmg *= -1;
	if(ctarget.elements[a.element] == Character::IMMUNE)
		dmg = 0;
	if(ctarget.elements[a.element] == Character::WEAK)
		dmg *= 2;
	if(ctarget.elements[a.element] == Character::RESISTANT)
		dmg /= 2;

	//Undead are harmed by heal
	if(a.element == AttackLib::HEAL && ctarget.status[Character::ZOMBIE] == Character::SUFFERING)
		dmg *= -1;

	if(dmg > MAX_DAMAGE) dmg = MAX_DAMAGE;
	if(dmg < -MAX_DAMAGE) dmg = -MAX_DAMAGE;

	//Trefferberechnung
	//Step0
	if(ctarget.status[Character::WOUND] && a.effect_function != AttackLib::revive && a.effect_function != AttackLib::full_revive) {
		return MAX_DAMAGE + 1;
	}
	//Earth Attacks never hit floating targets
	if(a.element == EARTH && ctarget.status[Character::FLOAT] == Character::SUFFERING)
		return MAX_DAMAGE + 1;

	//Step1
	if(a.physical && ctarget.status[Character::CLEAR] == Character::SUFFERING)
		return MAX_DAMAGE + 1;
	else if(!a.physical && ctarget.status[Character::CLEAR] == Character::SUFFERING) {
		//Magic removes Clear status
		if(a.effect_function != &toggle<Character::CLEAR>)
			heal<Character::CLEAR>(caster, target);

		goto hit;
	}
	//Step2
	if(ctarget.status[Character::WOUND] == Character::IMMUNE && a.effect_function == &inflict<Character::WOUND>)
		return MAX_DAMAGE + 1;
	//Step3
	if(a.unblock) goto hit;
	//Step4
	if(!a.block_by_stamina) {
		//4a
		if(	ctarget.status[Character::SLEEP] == Character::SUFFERING ||
			ctarget.status[Character::PETRIFY] == Character::SUFFERING ||
			ctarget.status[Character::STOP] == Character::SUFFERING)
			goto hit;
		//4b
		if(a.physical && ((target->get_dir() == 0 && caster->get_side() > target->get_side()) || //Ziel schaut nach links 
						  (target->get_dir() == 1 && caster->get_side() < target->get_side()))) {  //Ziel schaut nach rechts
			target->set_dir((target->get_dir()+1)%2);
			goto hit;
		}
		//4c
		if(a.hit_rate == 255)
			goto hit;
		//4d
		if(a.physical && ctarget.status[Character::IMAGE] == Character::SUFFERING) {
			if(random()%4 == 0)
				target->set_status(Character::IMAGE, Character::NORMAL);
			return MAX_DAMAGE + 1;
		}
		//4e
		int bval;
		if(a.physical)
			bval = (255-ctarget.ablock*2) + 1;
		else
			bval = (255-ctarget.mblock*2) + 1;
		if(bval < 1) bval = 1;
		if(bval > 255) bval = 255;
		int hitr;
		if(a.hit_rate == -1) hitr = ccaster.hitrate;
		else hitr = a.hit_rate;

		//Penalties to Hit Rate
		//Poison
		if(ctarget.status[Character::POISON] == Character::SUFFERING)
			hitr *= 0.75;
		//Seizure
		if(ctarget.status[Character::SEIZURE] == Character::SUFFERING)
			hitr *= 0.75;
		//Zombie
		if(ctarget.status[Character::ZOMBIE] == Character::SUFFERING)
			hitr *= 1.25;
		//Dark
		if(ctarget.status[Character::DARK] == Character::SUFFERING)
			hitr *= 1.25;
		if(ccaster.status[Character::DARK] == Character::SUFFERING)
			hitr *= 0.5;
		//Near Fatal
		if(ctarget.status[Character::NEAR_FATAL] == Character::SUFFERING)
			hitr *= 0.75;

		if((hitr*bval/256) > (random()%100)) {
			goto hit;
		} else {
			return MAX_DAMAGE + 1;
		}
	} else {
	//Step5
		int bval;
		if(a.physical)
			bval = (255-ctarget.ablock*2) + 1;
		else
			bval = (255-ctarget.mblock*2) + 1;
		if(bval < 1) bval = 1;
		if(bval > 255) bval = 255;
		int hitr;
		if(a.hit_rate == -1) hitr = ccaster.hitrate;
		else hitr = a.hit_rate;
		if((hitr*bval/256) > (random()%100)) {
			if(ctarget.stamina > (random()%128))
				return MAX_DAMAGE + 2;
			else
				goto hit;
		} else {
			return MAX_DAMAGE + 1;
		}
	}

	hit:

	if(a.effect_function != NULL)
		dmg += a.effect_function(caster, target);
	if(ccaster.status[Character::ZOMBIE] == Character::SUFFERING) {
		if(random()%16 == 0)
			inflict<Character::POISON>(caster, target);
		if(random()%16 == 0)
			inflict<Character::DARK>(caster, target);
	}
	return dmg;
}
