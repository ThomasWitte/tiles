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

Command::Command(long time) {
	exec_time = time;
}

long Command::get_time() {
	return exec_time;
}

void Command::execute() {
}

Fighter::Fighter(Fight *f, Character c, string name, PlayerSide side, int dir) {
	parent = f;
	this->c = c;
	atb = 0;
	step = 0;
	this->side = side;
	direction = dir;
	
	laden(name);
	//Items in Character oder durch abgeleitete Klassen?
	string items[4] = {"Angriff","Item","Magie",""};
	menu.set_items(items);
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

Fighter::FighterMenu::FighterMenu() {
	pointer = imageloader.load("Images/auswahl.tga");
	if(!pointer)
		cout << "Images/auswahl.tga konnte nicht geladen werden" << endl;
	pointer_position = 0;
	pointer_delta = 1;
	auswahl = 0;
	pause = 0;
}

Fighter::FighterMenu::~FighterMenu() {
	imageloader.destroy(pointer);
}

void Fighter::FighterMenu::set_items(string items[4]) {
	for(int i = 0; i < 4; i++) {
		menu_items[i] = items[i];
	}
}

int Fighter::FighterMenu::update() {
	if(pause < 0) {
		if(key[KEY_UP]) {
			auswahl--;
			if(auswahl < 0)
				auswahl = 3;
			for(int i = 0; menu_items[auswahl] == "" && i < 10; i++) {
				auswahl--;
				if(auswahl < 0)
					auswahl = 3;
			}
			pause = GAME_TIMER_BPS/10;
		} else if(key[KEY_DOWN]) {
			auswahl++;
			if(auswahl > 3)
				auswahl = 0;
			for(int i = 0; menu_items[auswahl] == "" && i < 10; i++) {
				auswahl++;
				if(auswahl > 3)
					auswahl = 0;
			}
			pause = GAME_TIMER_BPS/10;
		}
	} else pause--;
	pointer_position += pointer_delta;
	if(pointer_position >= 5 || pointer_position <= -5)
		pointer_delta *= -1; 
	return 1; //0 = schließen
}

void Fighter::FighterMenu::draw(BITMAP *buffer, int x, int y, int w, int h) {
	for(int i = 0; i < 4; i++) {
		textout_ex(buffer, font, menu_items[i].c_str(), x+26, y+(i*h/4)+5, makecol(255,255,255), -1);
	}
	masked_blit(pointer, buffer, 0, 0, x+5+pointer_position, y+auswahl*h/4+5, pointer->w, pointer->h);
}

Fight::Fight(string dateiname) {
	bg = NULL;
	ifstream datei;
	string input;
	
	FileParser parser(string("Fights/") + dateiname, "Fight");

	bg = imageloader.load(string("Fights/Images/") + parser.getstring("Fight", "Background"));

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

	if(comqueue.size())
		if(comqueue[0].get_time() <= time) {
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

void Fight::enqueue_ready_fighter(Fighter *f) {
	ready_fighters.push_back(f);
}
