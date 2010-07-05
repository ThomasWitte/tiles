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

int mpause = 0;

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
	caster->get_ready();
}

Fighter::Fighter(Fight *f, Character c, string name, PlayerSide side, int dir) {
	parent = f;
	this->c = c;
	atb = 0;
	step = 0;
	this->side = side;
	direction = dir;
	
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

	c.hp = parser.getvalue("Fighter", "hp", c.hp);
	c.speed = parser.getvalue("Fighter", "speed", c.speed);
	c.name = parser.getstring("Fighter", "name", c.name);
	deque< deque<string> > menu_items = parser.getsection("Menu");
	menu.set_items(menu_items);
}

void Fighter::update() {
	if(atb < 65536)
		atb += 3*c.speed+20;
	if(atb > 65536) {
		atb = 65536;
		parent->enqueue_ready_fighter(this);
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
}

void Fighter::draw_status(BITMAP *buffer, int x, int y, int w, int h) {
	textout_ex(buffer, font, c.name.c_str(), x+5, y+h/2-text_height(font)/2, makecol(255,255,255), -1);
	char text[10];
	sprintf(text, "%i", c.hp);
	textout_right_ex(buffer, font, text, x+w*2/3, y+h/2-text_height(font)/2, makecol(255,255,255), -1);
	rect(buffer, x+w*2/3+2, y+h/2-4, x+w-3, y+h/2+3, makecol(255,255,255));

	int color;
	if(atb < 65536)
		color = makecol(255,255,255);
	else
		color = makecol(255, 255, 0);

	rectfill(buffer, x+w*2/3+4, y+h/2-2, x+w*2/3+4+(atb*(w/3-8))/65536, y+h/2+1, color);
}

void Fighter::draw_menu(BITMAP *buffer, int x, int y, int w, int h) {
	menu.draw(buffer, x, y, w, h);
}

void Fighter::get_ready() {
	atb = 0;
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
	Character c = {"Enemy", 0, 0};
	PlayerSide side;
	int dir;

	for(int i = 0; i < ret.size(); i++) {
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
		fighters[ENEMY].push_back(new Fighter(this, c, ret[i][0], side, dir));
	}
	time = 0;

	menu_bg = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y/3);
	for(int i = 0; i < menu_bg->h; i++) {
		line(menu_bg, 0, i, menu_bg->w, i, makecol(i, i, 255-i));
	}
	vline(menu_bg, menu_bg->w/3, 3, menu_bg->h-4, makecol(255, 255, 255));
	rect(menu_bg, 3, 3, menu_bg->w-4, menu_bg->h-4, makecol(255, 255, 255));

	//Party hinzufügen (noch nicht final)
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
