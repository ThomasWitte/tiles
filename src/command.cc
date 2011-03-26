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
#include "command.h"
#include <iostream>
#include <sstream>

Command::Command(FighterBase *caster) {
	this->caster = caster;
	attack_name = "Verteid.";
	preparation_time = 0;
}

void Command::add_target(FighterBase *tg) {
	target.push_back(tg);
}

void Command::set_attack(string attack_name) {
	this->attack_name = attack_name;
	preparation_time = GAME_TIMER_BPS; //Normale Attacken werden 1s vorbereitet

	AttackLib::Attack a = AttackLib::get_attack(attack_name);
	if(a.vulnerable_to_runic) //Alle Zauber werden 2s vorbereitet
		preparation_time = 2*GAME_TIMER_BPS;
}

bool Command::is_target(FighterBase *tgt) {
	for(unsigned int i = 0; i < target.size(); i++) {
		if(tgt == target[i])
			return true;
	}
	return false;
}

bool Command::is_caster(FighterBase *c) {
	return (c == caster);
}

void Command::execute() {
cout << "caster: " << caster->get_spritename() << endl;
cout << "attack: " << attack_name << endl;

	for(unsigned int i = 0; i < target.size(); i++) {
		target[i]->lose_health(calc_damage(i));
	}
	caster->get_ready();
}

void Command::prepare() {
	if(preparation_time > 0)
		preparation_time--;
}

bool Command::is_prepared() {
	if(preparation_time <= 0)
		return true;
	return false;
}

int Command::calc_damage(int target_index) {
	int dmg = 0;
	bool character = !caster->is_monster();
	AttackLib::Attack a = AttackLib::get_attack(attack_name);
	Character ccaster = caster->get_character();
	Character ctarget = target[target_index]->get_character();
	//Step1
	if(a.physical == true) {
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
			dmg = ccaster.level * ccaster.level * (ccaster.apower * 4 + ccaster.vigor) / 256;
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
	if((!a.physical) && (a.possible_targets & AttackLib::SINGLE) && (target.size() > 1))
		dmg /= 2;
	//Step4
	if(attack_name == "Fight" && ccaster.defensive)
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
	if(a.physical && ctarget.defensive)
		dmg /= 2;
	
	if(!a.physical && ctarget.status[Character::MORPH] == Character::SUFFERING)
		dmg /= 2;

	if((a.element != AttackLib::HEAL) && !caster->is_monster() && !target[target_index]->is_monster())
		dmg /= 2;
	//Step7
	if(a.physical && ((target[target_index]->get_dir() == 0 && caster->get_side() > target[target_index]->get_side()) || //Ziel schaut nach links 
					  (target[target_index]->get_dir() == 1 && caster->get_side() < target[target_index]->get_side())))  //Ziel schaut nach rechts
		dmg *= 1.5;
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
		if(a.physical && ((target[target_index]->get_dir() == 0 && caster->get_side() > target[target_index]->get_side()) || //Ziel schaut nach links 
						  (target[target_index]->get_dir() == 1 && caster->get_side() < target[target_index]->get_side())))  //Ziel schaut nach rechts
			goto hit;
		//4c
		if(a.hit_rate == 255)
			goto hit;
		//4d
		if(a.physical && ctarget.status[Character::IMAGE] == Character::SUFFERING) {
			if(random()%4 == 0)
				target[target_index]->set_status(Character::IMAGE, Character::NORMAL);
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
		a.effect_function(caster, target[target_index]);
	return dmg;
}
