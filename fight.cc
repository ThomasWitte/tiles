/*  Copyright 2009 Thomas Witte

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
#include "fight.h"
#include "iohelper.h"
#include <iostream>
#include <sstream>

int Fighter::FighterMenu::mpause = 0;

Command::Command(Fighter *caster) {
	this->caster = caster;
	attack_name = "Verteid.";
}

void Command::add_target(Fighter *tg) {
	target.push_back(tg);
}

void Command::set_attack(string attack_name) {
	this->attack_name = attack_name;
}

void Command::execute() {
	for(int i = 0; i < target.size(); i++) {
		target[i]->lose_health(calc_damage(i));
	}
	caster->get_ready();
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

Fighter::Fighter(Fight *f, Character c, string name, PlayerSide side, int dir) {
	parent = f;
	this->c = c;
	atb = 0;
	itc = 0;
	step = 0;
	poisoncounter = 0;
	this->side = side;
	direction = dir;
	texttoshow = "";
	textremframes = 0;
	textcol = 0;
	
	laden(name);

	menu.set_parent(this);
}

Fighter::~Fighter() {
	for(int i = 0; i < ts.normal.size(); i++)
		imageloader.destroy(ts.normal[i]);
}

void Fighter::laden(string name) {
	string path;
	path = string("Fights/Fighters/") + name + string("/");

	FileParser parser(path + name, "Fighter");

	deque< deque<string> > ret = parser.getsection("normal");
	for(int i = 0; i < ret.size(); i++)
		ts.normal.push_back(imageloader.load(path + ret[i][0]));

	//alle anderen bilder auch mit normal belegen

	c.defensive = true;
	if(parser.getvalue("Fighter", "defensive", 0) == 0) c.defensive = false;
	c.name = parser.getstring("Fighter", "name", c.name);
	c.hp = parser.getvalue("Fighter", "hp", c.hp);
	c.curhp = c.hp;
	c.mp = parser.getvalue("Fighter", "mp", c.mp);
	c.curmp = c.mp;
	c.speed = parser.getvalue("Fighter", "speed", c.speed);
	c.vigor = parser.getvalue("Fighter", "vigor", c.vigor);
	c.stamina = parser.getvalue("Fighter", "stamina", c.stamina);
	c.mpower = parser.getvalue("Fighter", "mpower", c.mpower);
	c.apower = parser.getvalue("Fighter", "apower", c.apower);
	c.mdefense = parser.getvalue("Fighter", "mdefense", c.mdefense);
	c.adefense = parser.getvalue("Fighter", "adefense", c.adefense);
	c.mblock = parser.getvalue("Fighter", "mblock", c.mblock);
	c.ablock = parser.getvalue("Fighter", "ablock", c.ablock);
	c.level = parser.getvalue("Fighter", "level", c.level);
	c.hitrate = parser.getvalue("Fighter", "hitrate", c.hitrate);

	//Elemente
	string elements[] = {"none", "Heal", "Death", "Bolt", "Ice", "Fire", "Water", "Wind", "Earth", "Poison", "Pearl"};
	string rt;
	for(int i = 0; i < 11; i++) {
		rt = parser.getstring("Elements", elements[i], "normal");
		if(rt == "weak") c.elements[i] = Character::WEAK;
		else if(rt == "absorb") c.elements[i] = Character::ABSORB;
		else if(rt == "immune") c.elements[i] = Character::IMMUNE;
		else if(rt == "resist") c.elements[i] = Character::RESISTANT;
		else c.elements[i] = Character::NORMAL;
	}

	//Statuse
	string status[] = {"Dark", "Zombie", "Poison", "MTek", "Clear", "Imp", "Petrify", "Wound", "Condemned", "NearFatal", "Image", "Mute", "Berserk", "Muddle", "Seizure", "Sleep", "Dance", "Regen", "Slow", "Haste", "Stop", "Shell", "Safe", "Reflect", "Morph"};
	for(int i = 0; i < 25; i++) {
		rt = parser.getstring("Status", status[i], "normal");
		if(rt == "immune") c.status[i] = Character::IMMUNE;
		else if(rt == "suffering") c.status[i] = Character::SUFFERING;
		else c.status[i] = Character::NORMAL;
	}

	deque< deque<string> > menu_items = parser.getsection("Menu");
	menu.set_items(menu_items);
}

void Fighter::update() {
	if(c.curhp <= 0) {
		c.status[Character::WOUND] = Character::SUFFERING;
		atb = 0;
	} else if(c.curhp < c.hp/8) c.status[Character::NEAR_FATAL] = Character::SUFFERING;
	else c.status[Character::NEAR_FATAL] = Character::NORMAL;

	if(c.status[Character::WOUND] != Character::SUFFERING ||
		c.status[Character::STOP] != Character::SUFFERING ||
		c.status[Character::SLEEP] != Character::SUFFERING ||
		c.status[Character::PETRIFY] != Character::SUFFERING)

		if(atb < 65536)
			if(is_monster()) {
				if(c.status[Character::HASTE] == Character::SUFFERING) // Berechnungen in der Algorithms FAQ sind offensichtlich falsch…
					atb += 63*(c.speed+20) / 16;
				else if(c.status[Character::SLOW] == Character::SUFFERING)
					atb += 24*(c.speed+20) / 16;
				else
					atb += 3*(c.speed+20);
			} else {
				if(c.status[Character::HASTE] == Character::SUFFERING)
					atb += 63*(c.speed+20) / 16;
				else if(c.status[Character::SLOW] == Character::SUFFERING)
					atb += 24*(c.speed+20) / 16;
				else
					atb += 3*(c.speed+20);
			}
	if(atb > 65536) {
		atb = 65536;
		parent->enqueue_ready_fighter(this);
	}

	if(c.status[Character::HASTE] == Character::SUFFERING)
		itc += 3;
	else if(c.status[Character::SLOW] == Character::SUFFERING)
		itc += 1;
	else
		itc += 2;

	if(itc > 255) {
		itc = 0;
		if(c.status[Character::POISON] != Character::SUFFERING)
			poisoncounter = 0;
		if(c.status[Character::POISON] == Character::SUFFERING && random()%8 == 0) {
			int dmg = (c.hp * c.stamina / 1024) + 2;
			if(dmg > 255) dmg = 255;
			dmg = dmg * (random()%32 + 224) / 256;
			if(!is_monster()) dmg /= 2;
			dmg *= (poisoncounter + 1);
			lose_health(dmg);

			poisoncounter++;
			if(poisoncounter > 7) poisoncounter = 0;
			//Giftschaden
		}
		if(c.status[Character::SEIZURE] == Character::SUFFERING && random()%4 == 0) {
			int dmg = (c.hp * c.stamina / 1024) + 2;
			if(dmg > 255) dmg = 255;
			dmg = dmg * (random()%32 + 224) / 256;
			if(!is_monster()) dmg /= 2;
			lose_health(dmg);
			//Seizureschaden
		}
		if(c.status[Character::REGEN] == Character::SUFFERING && random()%4 == 0) {
			int dmg = (c.hp * c.stamina / 1024) + 2;
			if(dmg > 255) dmg = 255;
			dmg = dmg * (random()%32 + 224) / -256;
			lose_health(dmg);
			//hp durch regen
		}
	}

	step++;
}

int Fighter::update_menu() {
	return menu.update();
}

void Fighter::draw(BITMAP *buffer, int x, int y) {
	int index = (step/SPRITE_ANIMATION_SPEED)%ts.normal.size();
	//spiegeln, wenn direction = 0(linkss)
	if(direction == 0) {
		draw_sprite_h_flip(buffer, ts.normal[index], x-ts.normal[index]->w/2, y-ts.normal[index]->h/2);
	} else {
		draw_sprite(buffer, ts.normal[index], x-ts.normal[index]->w/2, y-ts.normal[index]->h/2);
	}
	if(textremframes) {
		textremframes--;
		textout_ex(buffer, font, texttoshow.c_str(), x-10, y-25+textremframes/2, textcol, -1);
	}
}

void Fighter::draw_status(BITMAP *buffer, int x, int y, int w, int h) {
	textout_ex(buffer, font, c.name.c_str(), x+5, y+h/2-text_height(font)/2, COL_WHITE, -1);
	char text[10];
	sprintf(text, "%i", c.curhp);
	if(c.status[Character::NEAR_FATAL] == Character::SUFFERING)
		textout_right_ex(buffer, font, text, x+w*2/3, y+h/2-text_height(font)/2, COL_YELLOW, -1);
	else if(c.status[Character::WOUND] == Character::SUFFERING)
		textout_right_ex(buffer, font, text, x+w*2/3, y+h/2-text_height(font)/2, COL_RED, -1);
	else
		textout_right_ex(buffer, font, text, x+w*2/3, y+h/2-text_height(font)/2, COL_WHITE, -1);
	rect(buffer, x+w*2/3+2, y+h/2-4, x+w-3, y+h/2+3, COL_WHITE);

	int color;
	if(atb < 65536) {
		if(c.status[Character::HASTE] == Character::SUFFERING)
			color = COL_LIGHT_BLUE;
		else if(c.status[Character::SLOW] == Character::SUFFERING ||
				c.status[Character::STOP] == Character::SUFFERING)
			color = COL_RED;
		else
			color = COL_WHITE;
	} else {
		color = COL_YELLOW;
	}

	rectfill(buffer, x+w*2/3+4, y+h/2-2, x+w*2/3+4+(atb*(w/3-8))/65536, y+h/2+1, color);
}

void Fighter::draw_menu(BITMAP *buffer, int x, int y, int w, int h) {
	menu.draw(buffer, x, y, w, h);
}

void Fighter::get_ready() {
	atb = 0;
}

bool Fighter::is_friend() {
	if(parent->get_side(this) == Fight::FRIEND) return true;
	return false;
}

Character Fighter::get_character() {
	return c;
}

void Fighter::override_character(Character o) {
	if(o.name != "") c.name = o.name;
	c.defensive = o.defensive;
	if(o.hp >= 0) c.hp = o.hp;
	if(o.curhp >= 0) c.curhp = o.curhp;
	if(o.mp >= 0) c.mp = o.mp;
	if(o.curmp >= 0) c.curmp = o.curmp;
	if(o.speed >= 0) c.speed = o.speed;
	if(o.vigor >= 0) c.vigor = o.vigor;
	if(o.stamina >= 0) c.stamina = o.stamina;
	if(o.mpower >= 0) c.mpower = o.mpower;
	if(o.apower >= 0) c.apower = o.apower;
	if(o.mdefense >= 0) c.mdefense = o.mdefense;
	if(o.adefense >= 0) c.adefense = o.adefense;
	if(o.mblock >= 0) c.mblock = o.mblock;
	if(o.ablock >= 0) c.ablock = o.ablock;
	if(o.xp >= 0) c.xp = o.xp;
	if(o.levelupxp >= 0) c.levelupxp = o.levelupxp;
	if(o.level >= 0) c.level = o.level;
	if(o.hitrate >= 0) c.hitrate = o.hitrate;
	for(int i = 0; i < 11; i++)
		if(o.elements[i] != Character::NORMAL) {
			for(int j = 0; j < 11; j++)
				c.elements[i] = o.elements[i];
			break;
		}
	for(int i = 0; i < 25; i++)
		if(o.status[i] != Character::NORMAL) {
			for(int j = 0; j < 25; j++)
				c.status[i] = o.status[i];
			break;
		}
}

void Fighter::lose_health(int hp) {
	if(hp == MAX_DAMAGE+1) {
		show_text("Miss", COL_WHITE, GAME_TIMER_BPS/2);
	} else if(hp == MAX_DAMAGE+2) {
		show_text("Block", COL_WHITE, GAME_TIMER_BPS/2);
	} else if(hp < 0) {
		stringstream s;
		s << (hp*-1);
		show_text(s.str(), COL_GREEN, GAME_TIMER_BPS/2);
	} else {
		stringstream s;
		s << hp;
		show_text(s.str(), COL_WHITE, GAME_TIMER_BPS/2);
	}

	if(hp > MAX_DAMAGE) hp = 0;
	c.curhp -= hp;
	if(c.curhp < 0) c.curhp = 0;
	if(c.curhp > c.hp) c.curhp = c.hp;
}

void Fighter::show_text(string text, int color, int frames) {
	texttoshow = text;
	textcol = color;
	textremframes = frames;
}

int Fighter::get_status(int status) {
	if(status >= 25 || status < 0) return -1;
	return c.status[status];
}

void Fighter::set_status(int status, int state) {
	if(status >= 25 || status < 0 || !(state == Character::NORMAL || state == Character::IMMUNE || state == Character::SUFFERING)) return;
	c.status[status] = state;
}

Fighter::FighterMenu::FighterMenu() {
	pointer = imageloader.load("Images/auswahl.tga");
	if(!pointer)
		cout << "Images/auswahl.tga konnte nicht geladen werden" << endl;
	sub_bg = imageloader.load("Images/sub_bg.tga");
	if(!sub_bg)
		cout << "Images/sub_bg.tga konnte nicht geladen werden" << endl;

	pointer_position = 0;
	pointer_delta = 1;
	auswahl = 0;
	fighter = NULL;
	sub_auswahl = 0;
	sub_open = false;
	state = MENU;
}

Fight *get_parent(Fighter& f) {
	return f.parent;
}

void Fighter::FighterMenu::set_parent(Fighter *fighter) {
	this->fighter = fighter;
}

Fighter::FighterMenu::~FighterMenu() {
	imageloader.destroy(pointer);
	imageloader.destroy(sub_bg);
}

void Fighter::FighterMenu::set_items(deque< deque<string> > items) {
	menu_items = items;
}

int Fighter::FighterMenu::update() {
	switch(state) {
	case MENU:
	if(mpause < 0) {
		if(key[KEY_UP]) {
			if(sub_open) {
				sub_auswahl--;
				if(sub_auswahl <= 0) sub_auswahl = menu_items[auswahl].size()-1;
			} else {
				auswahl--;
				auswahl = auswahl%menu_items.size();
			}
			mpause = GAME_TIMER_BPS/10;
		} else if(key[KEY_DOWN]) {
			if(sub_open) {
				sub_auswahl++;
				if(sub_auswahl >= menu_items[auswahl].size()) sub_auswahl = 1;
			} else {
				auswahl++;
				auswahl = auswahl%menu_items.size();
			}
			mpause = GAME_TIMER_BPS/10;
		} else if(key[KEY_ENTER]) {
			mpause = GAME_TIMER_BPS/10;
			if(menu_items[auswahl].size() > 1 && !sub_open) {
				//Submenü öffnen
				sub_auswahl = 1;
				sub_open = true;
			} else {
				state = TARGETS_BY_ATTACK;
				sub_open = false;
			}
		} else if(key[KEY_BACKSPACE]) {
			if(sub_open) sub_open = false;
			mpause = GAME_TIMER_BPS/10;
		}
	} else mpause--;
	pointer_position += pointer_delta;
	if(pointer_position >= 5 || pointer_position <= -5)
		pointer_delta *= -1;
	break;

	case TARGETS_BY_ATTACK:
	//Starttargets werden anhand der gewählten Attacke ausgesucht…
	if(get_parent(*fighter)->get_fighter_count(Fight::ENEMY)) {
		target_side = Fight::ENEMY;
		cur_target = 0;
	} else {
		target_side = Fight::FRIEND;
		cur_target = 0;
	}
	state = CHOOSE_TARGET;
	//kein break, um sofort den state zu wechseln

	case CHOOSE_TARGET:
	if(mpause < 0) {
		int fc = get_parent(*fighter)->get_fighter_count(target_side);
		if(key[KEY_UP]) {
			cur_target--;
			if(cur_target < 0) cur_target = fc-1;
			mpause = GAME_TIMER_BPS/10;
		} else if(key[KEY_DOWN]) {
			cur_target++;
			if(cur_target >= fc) cur_target = 0;
			mpause = GAME_TIMER_BPS/10;
		} else if(key[KEY_LEFT] || key[KEY_RIGHT]) {
			if(get_parent(*fighter)->get_fighter_count((target_side+1)%2)) {
				cur_target = 0;
				target_side = (target_side+1)%2;
			}
			mpause = GAME_TIMER_BPS/10;
		} else if(key[KEY_ENTER]) {
			Command c(fighter);
			get_parent(*fighter)->add_fighter_target(c, cur_target, target_side);
			string attack = "Verteid.";
			if(menu_items[auswahl].size() > 1) {
				attack = menu_items[auswahl][sub_auswahl];
			} else {
				attack = menu_items[auswahl][0];
			}			
			c.set_attack(attack);
			get_parent(*fighter)->enqueue_command(c);
			state = MENU;
			mpause = GAME_TIMER_BPS/10;
			for(int h = 0; h < 2; h++)
				for(int i = 0; i < get_parent(*fighter)->get_fighter_count(h); i++)
					get_parent(*fighter)->mark_fighter(i, h, false);
			return 0;
		} else if(key[KEY_BACKSPACE]) {
			state = MENU;
			mpause = GAME_TIMER_BPS/10;
			for(int h = 0; h < 2; h++)
				for(int i = 0; i < get_parent(*fighter)->get_fighter_count(h); i++)
					get_parent(*fighter)->mark_fighter(i, h, false);
		}

	for(int h = 0; h < 2; h++)
		for(int i = 0; i < get_parent(*fighter)->get_fighter_count(h); i++)
			get_parent(*fighter)->mark_fighter(i, h, false);
	get_parent(*fighter)->mark_fighter(cur_target, target_side, true);
	} else mpause--;
	break;
	}
	return 1; //0 = schließen
}

void Fighter::FighterMenu::draw(BITMAP *buffer, int x, int y, int w, int h) {
	for(int i = 0; i < menu_items.size(); i++) {
		textout_ex(buffer, font, menu_items[i][0].c_str(), x+26, y+(i*h/4)+5, makecol(255,255,255), -1);
	}
	
	if(sub_open) {
		masked_blit(sub_bg, buffer, 0, 0, x+w-5, y+h-sub_bg->h, sub_bg->w, sub_bg->h);
		masked_blit(pointer, buffer, 0, 0, x+5, y+auswahl*h/4+5, pointer->w, pointer->h);
		masked_blit(pointer, buffer, 0, 0, x+w+pointer_position, y+h-sub_bg->h+5+(sub_auswahl-1)*sub_bg->h/15, pointer->w, pointer->h);
		for(int i = 1; i < menu_items[auswahl].size(); i++)
			textout_ex(buffer, font, menu_items[auswahl][i].c_str(), x+w-5+26, y+h-sub_bg->h+5+(i-1)*sub_bg->h/15, makecol(255,255,255), -1);
	} else {
		masked_blit(pointer, buffer, 0, 0, x+5+pointer_position, y+auswahl*h/4+5, pointer->w, pointer->h);
	}
}

Fight::Fight(string dateiname) {
	bg = NULL;
	command_is_executed = false;
	ifstream datei;
	string input;
	
	FileParser parser(string("Fights/") + dateiname, "Fight");

	bg = imageloader.load(string("Fights/Images/") + parser.getstring("Fight", "Background"));
	auswahl = imageloader.load("Images/auswahl_hor.tga");

	for(int i = 0; i < 20; i++) {
		marked_fighters[0][i] = false;
		marked_fighters[1][i] = false;
	}

	bool types_enabled[4];
	for(int i = 0; i < 4; i++) {
		types_enabled[i] = false;
	}

	int type = parser.getvalue("Fight", "Type");

	if(type >= 8) {
		types_enabled[4] = true;
		type -= 8;
	}
	if(type >= 4) {
		types_enabled[3] = true;
		type -= 4;
	}
	if(type >= 2) {
		types_enabled[2] = true;
		type -= 2;
	}
	if(type >= 1) {
		types_enabled[1] = true;
	}
		
	int i;
	while(1) {
		i = random()%255;

		if(i<208 && types_enabled[1]) {
			type = NORMAL; // typ normal
			break;
		} else if(i<216 && i>=208 && types_enabled[2]) {
			type = BACK; // typ back
			break;
		} else if(i<224 && i>=216 && types_enabled[3]) {
			type = PINCER; //typ pincer
			break;
		} else if(i>=224 && types_enabled[4]) {
			type = SIDE; // typ side
			break;
		}
	}

	deque< deque<string> > ret = parser.getall("Fighter", "Enemy");

	PlayerSide side;
	int dir;

	for(int i = 0; i < ret.size(); i++) {
		Character c = {"Enemy", false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		for(int j = 0; j < 11; j++)
			c.elements[j] = Character::NORMAL;
		for(int j = 0; j < 25; j++)
			c.status[j] = Character::NORMAL;

		switch(type) {
			case NORMAL:
				side = LEFT;
				dir = 1; //rechts
				break;
			case BACK:
				side = RIGHT;
				dir = 0;
				break;
			case PINCER:
				if(random()%2 == 0) {
					side = RIGHT;
					dir = 0;
				} else {
					side = LEFT;
					dir = 1;
				}
				break;
			case SIDE:
				side = LEFT;
				dir = 0;
				break;
		}
		c.vigor = random()%8 + 56;
		fighters[ENEMY].push_back(new Fighter(this, c, ret[i][0], side, dir));
	}
	time = 0;

	menu_bg = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y/3);
	for(int i = 0; i < menu_bg->h; i++) {
		line(menu_bg, 0, i, menu_bg->w, i, makecol(i, i, 255-i));
	}
	vline(menu_bg, menu_bg->w/3, 3, menu_bg->h-4, makecol(255, 255, 255));
	rect(menu_bg, 3, 3, menu_bg->w-4, menu_bg->h-4, makecol(255, 255, 255));

	//Party hinzufügen (noch nicht final) am ende sollte ein fighter.override_character(c) stehen…

	Character c = {"Enemy", false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(int i = 0; i < 11; i++)
		c.elements[i] = Character::NORMAL;
	for(int i = 0; i < 25; i++)
		c.status[i] = Character::NORMAL;

	c.hp = 9876;
	c.name = "test";
	c.speed = 57;

	switch(type) {
		case NORMAL:
			side = RIGHT;
			dir = 0;
			break;
		case BACK:
			side = LEFT;
			dir = 0;
			break;
		case PINCER:
			side = MIDDLE;
			dir = random()%2;
			break;
		case SIDE:
			side = RIGHT;
			dir = 0;
			break;
	}

	fighters[FRIEND].push_back(new Fighter(this, c, "Mario", side, dir));
	fighters[FRIEND].push_back(new Fighter(this, c, "Luigi", side, (dir+1)%2));
}

Fight::~Fight() {
	imageloader.destroy(bg);
	imageloader.destroy(menu_bg);
	imageloader.destroy(auswahl);
	for(int i = 0; i < 2; i++) 
		for(int j = 0; j < fighters[i].size(); j++) {
			delete fighters[i][j];
		}
}

void Fight::draw(BITMAP *buffer) {
	int x, y;
	int sz[3]; for(int i = 0; i < 3; i++) sz[i] = 0;
	int szd[3]; for(int i = 0; i < 3; i++) szd[i] = 0;
	stretch_blit(bg, buffer, 0, 0, bg->w, bg->h, 0, 0, buffer->w, buffer->h);
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++) {
			sz[fighters[i][j]->get_side()]++;
		}
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++) {
			x = PC_RESOLUTION_X/8 + PC_RESOLUTION_X/8 * 3 * fighters[i][j]->get_side();
			y = (2*PC_RESOLUTION_Y/3) / (sz[fighters[i][j]->get_side()]+1) * (szd[fighters[i][j]->get_side()]+1);
			szd[fighters[i][j]->get_side()]++;
			
			fighters[i][j]->draw(buffer, x, y);
			if(marked_fighters[i][j])
				masked_blit(auswahl, buffer, 0, 0, x, y-25, auswahl->w, auswahl->h);
		}
	blit(menu_bg, buffer, 0, 0, 0, 2*PC_RESOLUTION_Y/3, PC_RESOLUTION_X, PC_RESOLUTION_Y);

	for(int i = 0; i < fighters[FRIEND].size(); i++) {
		fighters[FRIEND][i]->draw_status(buffer, PC_RESOLUTION_X/3+2, 2*PC_RESOLUTION_Y/3+i*((PC_RESOLUTION_Y/3)/fighters[FRIEND].size()), 2*PC_RESOLUTION_X/3-4, (PC_RESOLUTION_Y/3)/fighters[FRIEND].size());
	}

	int current_menu = -1;
	for(int i = 0; i < ready_fighters.size(); i++) {
		bool end = false;
		for(int j = 0; j < fighters[FRIEND].size(); j++) {
			if(ready_fighters[i] == fighters[FRIEND][j]) {
				current_menu = i;
				end = true;
				break;
			}
		}
		if(end) break;
	}
	if(current_menu >= 0)
		ready_fighters[current_menu]->draw_menu(buffer, 5, 2*PC_RESOLUTION_Y/3+5, PC_RESOLUTION_X/3-3, PC_RESOLUTION_Y/3-10);
}

int Fight::update() {
	time++;

	if(comqueue.size() && !command_is_executed) {
		comqueue[0].execute();
		comqueue.pop_front();
	}
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++) {
			fighters[i][j]->update();
		}

	int current_menu = -1;
	for(int i = 0; i < ready_fighters.size(); i++) {
		bool end = false;
		for(int j = 0; j < fighters[FRIEND].size(); j++) {
			if(ready_fighters[i] == fighters[FRIEND][j]) {
				current_menu = i;
				end = true;
				break;
			}
		}
		if(end) break;
	}
	int ret = -1;
	if(current_menu >= 0)
		ret = ready_fighters[current_menu]->update_menu();

	if(ret == 0) {
		ready_fighters.erase(ready_fighters.begin()+current_menu);
	}

	return 1; //0 = Kampfende
};

void Fight::enqueue_command(Command c) {
	comqueue.push_back(c);
}

void Fight::enqueue_ready_fighter(Fighter *f) {
	ready_fighters.push_back(f);
}

int Fight::get_fighter_count(int side) {
	if(side >= 2) return -1;
	return fighters[side].size();
}

void Fight::add_fighter_target(Command &c, int fighter, int side) {
	c.add_target(fighters[side][fighter]);
}

void Fight::mark_fighter(int fighter, int side, bool mark) {
	marked_fighters[side][fighter] = mark;
}

int Fight::get_side(Fighter* f) {
	for(int i = 0; i < fighters[FRIEND].size(); i++)
		if(fighters[FRIEND][i] == f)
			return FRIEND;
	for(int i = 0; i < fighters[ENEMY].size(); i++)
		if(fighters[ENEMY][i] == f)
			return ENEMY;
	return -1;
}

