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

AttackLib::Attack AttackLib::lib[] = {
	// pow = -1: Angriffskraft wird durch Characterwert bestimmt
	// hitr = 255: trifft immer
	// hitr = -1: Trefferrate wird durch Waffe oder Characterwert bestimmt

	//name			effect_func,	animation,		mp,		pow,	phys, 	ignd, 	unbl, 	bbst, 	rid,	vtr,	refl,	cob,	hitr,	elem, 	Target
	{"Antidot",		&unpoison,		&heal_ani,		3,		0,		false,	true,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE},
	{"Battle",		NULL,			NULL,			0,		-1,		true, 	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Bio",			&poison,		NULL,			26,		53,		false,	false,	false,	false,	true,	true,	true,	false,	120,	POISON,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"BioBlast",	&poiseiz,		NULL,			0,		60,		false,	false,	true, 	false,	true,	false,	false,	false,	0,		POISON,	SELF + FRIEND + ENEMY + MULTI},
	{"Bolt",		NULL,			NULL,			6,		20,		false,	false,	false,	false,	true,	true,	true,	false,	150,	BOLT,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"Bolt2",		NULL,			NULL,			22,		61,		false,	false,	false,	false,	true,	true,	true,	false,	150,	BOLT,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"Bolt3",		NULL,			NULL,			53,		120,	false,	false,	false,	false,	true,	true,	true,	false,	150,	BOLT,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"BoltBeam",	NULL,			NULL,			0,		62,		false,	false,	true, 	false,	true,	false,	false,	false,	0,		BOLT,	SELF + FRIEND + ENEMY + SINGLE},
	{"Break",		&petrify,		NULL,			25,		0,		false,	false,	false,	true,	true,	true,	true,	false,	120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE},
	{"Bserk",		&berserk,		NULL,			16,		0,		false,	false,	false,	false,	false,	true,	true,	false,	150,	NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Confuser",	&muddle,		NULL,			0,		0,		false,	false,	false,	false,	true,	false,	false,	false,	128,	NONE,	SELF + FRIEND + ENEMY + MULTI},
	{"Critical",	NULL,			NULL,			0,		24,		true,	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Cure",		NULL,			&heal_ani,		5,		10,		false,	true,	true,	false,	false,	true,	true,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"Cure2", 		NULL,			&heal_ani,		25,		28,		false,	true,	true,	false,	false,	true,	true,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"Cure3", 		NULL,			&heal_ani,		40,		66,		false,	true,	true,	false,	false,	true,	true,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"Demi",		&demi,			NULL,			33,		0,		false,	false,	false,	true,	true,	true,	false,	false,	120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE},
	{"Dispel",		&dispel,		NULL,			25,		0,		false,	false,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE},
	{"Doom",		&doom,			NULL,			35,		0,		false,	false,	false,	true,	true,	true,	true,	false,	95,		DEATH,	SELF + FRIEND + ENEMY + SINGLE},
	{"Drain", 		&drain,			NULL,			15,		0,		false,	false,	false,	false,	true,	true,	false,	false,	120,	NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"_Drain", 		NULL,			NULL,			0,		38,		false,	false,	false,	false,	true,	true,	false,	false,	255,	NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Fight",		NULL,			&fight_ani,		0,		-1,		true, 	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Fire",		NULL,			NULL,			4,		21,		false,	false,	false,	false,	true,	true,	true,	false,	150,	FIRE,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"Fire2",		NULL,			NULL,			20,		60,		false,	false,	false,	false,	true,	true,	true,	false,	150,	FIRE,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"Fire3",		NULL,			NULL,			51,		121,	false,	false,	false,	false,	true,	true,	true,	false,	150,	FIRE,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"FireBeam",	NULL,			NULL,			0,		60,		false,	false,	true, 	false,	true,	false,	false,	false,	0,		FIRE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Flare",		NULL,			NULL,			45,		60,		false,	true,	false,	false,	true,	true,	true,	false,	150,	NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Float",		&float_effect,	NULL,			17,		0,		false,	false,	true,	false,	false,	true,	false,	true,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE + MULTI},
	{"HealForce",	NULL,			&heal_ani,		0,		50,		false,	true, 	true, 	false,	false,	false,	false,	false,	0,		HEAL,	SELF + FRIEND + ENEMY + SINGLE},
	{"IceBeam",		NULL,			NULL,			0,		61, 	false,	false,	true, 	false,	true,	false,	false,	false,	0,		ICE,	SELF + FRIEND + ENEMY + SINGLE},
	{"TekMissile",	&seizure,		NULL,			0,		58, 	false,	true, 	true, 	false,	true,	false,	false,	false,	0,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"Tusk",		NULL,			NULL,			0,		30,		true,	false,	false,	false,	true,	false,	false,	false,	-1,		NONE,	SELF + FRIEND + ENEMY + SINGLE},
	{"X-Fer",		&death,			NULL,			0,		0,		false,	false,	false,	true, 	false,	false,	false,	false,	120,	DEATH,	SELF + FRIEND + ENEMY + SINGLE},
	{"NULL",		NULL,			NULL,			0,		0,		false,	false,	false,	false,	false,	false,	false,	false,	0,		NONE,	0}
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

int AttackLib::unpoison(FighterBase *caster, FighterBase *target) {
	if(target->get_status(Character::POISON) == Character::SUFFERING)
		target->set_status(Character::POISON, Character::NORMAL);
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

int AttackLib::petrify(FighterBase *caster, FighterBase *target) {
	if(target->get_status(Character::PETRIFY) != Character::IMMUNE)
		target->set_status(Character::PETRIFY, Character::SUFFERING);
	return 0;
}

int AttackLib::berserk(FighterBase *caster, FighterBase *target) {
	if(target->get_status(Character::BERSERK) != Character::IMMUNE)
		target->set_status(Character::BERSERK, Character::SUFFERING);
	return 0;
}

int AttackLib::demi(FighterBase *caster, FighterBase *target) {
	Character c = target->get_character();
	target->lose_health(c.curhp/4);
	return 0;
}

int AttackLib::dispel(FighterBase *caster, FighterBase *target) {
	if(target->get_status(Character::CLEAR) == Character::SUFFERING)
		target->set_status(Character::CLEAR, Character::NORMAL);
	if(target->get_status(Character::IMAGE) == Character::SUFFERING)
		target->set_status(Character::IMAGE, Character::NORMAL);
	if(target->get_status(Character::BERSERK) == Character::SUFFERING)
		target->set_status(Character::BERSERK, Character::NORMAL);
	if(target->get_status(Character::REGEN) == Character::SUFFERING)
		target->set_status(Character::REGEN, Character::NORMAL);
	if(target->get_status(Character::SLOW) == Character::SUFFERING)
		target->set_status(Character::SLOW, Character::NORMAL);
	if(target->get_status(Character::HASTE) == Character::SUFFERING)
		target->set_status(Character::HASTE, Character::NORMAL);
	if(target->get_status(Character::STOP) == Character::SUFFERING)
		target->set_status(Character::STOP, Character::NORMAL);
	if(target->get_status(Character::SHELL) == Character::SUFFERING)
		target->set_status(Character::SHELL, Character::NORMAL);
	if(target->get_status(Character::SAFE) == Character::SUFFERING)
		target->set_status(Character::SAFE, Character::NORMAL);
	if(target->get_status(Character::REFLECT) == Character::SUFFERING)
		target->set_status(Character::REFLECT, Character::NORMAL);
	if(target->get_status(Character::LIFE3) == Character::SUFFERING)
		target->set_status(Character::LIFE3, Character::NORMAL);
	if(target->get_status(Character::FLOAT) == Character::SUFFERING)
		target->set_status(Character::FLOAT, Character::NORMAL);
	return 0;
}

int AttackLib::doom(FighterBase *caster, FighterBase *target) {
	if(target->get_status(Character::ZOMBIE) != Character::SUFFERING)
		death(caster, target);
	else
		target->lose_health(-MAX_DAMAGE);
	return 0;
}

int AttackLib::drain(FighterBase *caster, FighterBase *target) {
	int dmg = calc_damage(caster, target, get_attack("_Drain"), false);

	Character ctarget = target->get_character();
	if(dmg > ctarget.curhp)
		dmg = ctarget.curhp;

	if(target->get_status(Character::ZOMBIE) == Character::SUFFERING)
		dmg *= -1;

	caster->lose_health(-dmg);
	target->lose_health(dmg);

	return 0;
}

int AttackLib::float_effect(FighterBase *caster, FighterBase *target) {
	if(target->get_status(Character::FLOAT) != Character::IMMUNE)
		target->set_status(Character::FLOAT, Character::SUFFERING);
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
	//6d
	if(a.physical && ctarget.defensive) {
		dmg /= 2;
	}
	
	if(!a.physical && ctarget.status[Character::MORPH] == Character::SUFFERING)
		dmg /= 2;

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

	if(dmg > MAX_DAMAGE) dmg = MAX_DAMAGE;
	if(dmg < -MAX_DAMAGE) dmg = -MAX_DAMAGE;

	//Trefferberechnung
	//Step0
	if(ctarget.status[Character::WOUND] && a.effect_function != AttackLib::revive && a.effect_function != AttackLib::full_revive) {
		return MAX_DAMAGE + 1;
	}
	//Step1
	if(a.physical && ctarget.status[Character::CLEAR] == Character::SUFFERING)
		return MAX_DAMAGE + 1;
	else if(!a.physical && ctarget.status[Character::CLEAR] == Character::SUFFERING)
		goto hit;
	//Step2
	if(ctarget.status[Character::WOUND] == Character::IMMUNE && a.effect_function == AttackLib::death)
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
	return dmg;
}
