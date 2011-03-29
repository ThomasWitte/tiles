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
	animation_step = AnimationData::INITIALIZE;
	animation_ret = 0;
}

Command::~Command() {
	if(animation_step > AnimationData::INITIALIZE && attack_name != "") {
		AttackLib::Attack a = AttackLib::get_attack(attack_name);
		if(a.animation) {
			a.animation(AnimationData::DESTROY, &adata, NULL);
		}
	}		
}

void Command::add_target(FighterBase *tg) {
	target.push_back(tg);

	//Fighter in richtige Richtung drehen
	if(caster->get_side() < tg->get_side())
		caster->set_dir(1);
	else if(caster->get_side() > tg->get_side())
		caster->set_dir(0);
}

void Command::set_attack(string attack_name) {
	this->attack_name = attack_name;
	preparation_time = 1.5*GAME_TIMER_BPS; //Normale Attacken werden 1,5s vorbereitet
	caster->set_animation(FighterBase::WAIT_TO_ATTACK);

	AttackLib::Attack a = AttackLib::get_attack(attack_name);
	if(a.vulnerable_to_runic) { //Alle Zauber werden 3s vorbereitet
		preparation_time = 3*GAME_TIMER_BPS;
		caster->set_animation(FighterBase::WAIT_TO_CAST_SPELL);
	}
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
	AttackLib::Attack a = AttackLib::get_attack(attack_name);

	if(caster->lose_mp(a.mp_cost)) { //false, wenn nicht genug mp
		for(unsigned int i = 0; i < target.size(); i++) {
			target[i]->lose_health(calc_damage(i));
		}
	}

	caster->get_ready();
}

bool Command::prepare() {
	if(preparation_time > 0)
		preparation_time--;

	if(caster->get_status(Character::WOUND) == Character::SUFFERING)
		return false;
	return true;
}

bool Command::is_prepared() {
	if(preparation_time <= 0)
		return true;
	return false;
}

int Command::attack_animation() {
	AttackLib::Attack a = AttackLib::get_attack(attack_name);
	if(a.animation) {
		if(animation_step == AnimationData::INITIALIZE) {
			//adata vorbereiten
			caster->get_screen_position(&adata.caster.x, &adata.caster.y);
			for(unsigned int i = 0; i < target.size(); i++) {
				Position p;
				target[i]->get_screen_position(&p.x, &p.y);
				adata.targets.push_back(p);
			}

			animation_ret = a.animation(animation_step, &adata, NULL);
		}
		animation_step++;
		return animation_ret;
	}
	return -1;
}

void Command::draw_attack_animation(BITMAP *buffer) {
	AttackLib::Attack a = AttackLib::get_attack(attack_name);
	if(a.animation) {
		animation_ret = a.animation(animation_step, &adata, buffer);
	}
}

int Command::calc_damage(int target_index) {
	AttackLib::Attack a = AttackLib::get_attack(attack_name);
	return AttackLib::calc_damage(caster, target[target_index], a, (target.size() > 1));
}
