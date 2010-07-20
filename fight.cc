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
#include "fighter.h"
#include "iohelper.h"
#include <iostream>
#include <sstream>

Fight::Fight(string dateiname, Game *g) {
	parent = g;
	bg = NULL;
	command_is_executed = false;
	ifstream datei;
	string input;
	state = FIGHT;
	
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
		fighters[ENEMY].push_back(new Monster(this, c, ret[i][0], side, dir));
	}
	time = 0;

	menu_bg = imageloader.create(PC_RESOLUTION_X, PC_RESOLUTION_Y/3);
	for(int i = 0; i < menu_bg->h; i++) {
		line(menu_bg, 0, i, menu_bg->w, i, makecol(i, i, 255-i));
	}
	vline(menu_bg, menu_bg->w/3, 3, menu_bg->h-4, makecol(255, 255, 255));
	rect(menu_bg, 3, 3, menu_bg->w-4, menu_bg->h-4, makecol(255, 255, 255));

	//Party hinzufügen (noch nicht final) am ende sollte ein fighter.override_character(c) stehen…
	if(parent) {
		string chars = parent->get_var("CharactersInBattle");
		int pos = chars.find_first_of(";");
		while(pos != string::npos) {
			string curchar = chars.substr(0, pos);

			Character c = {"Enemy", false, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			for(int i = 0; i < 11; i++)
				c.elements[i] = Character::NORMAL;
			for(int i = 0; i < 25; i++)
				c.status[i] = Character::NORMAL;

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

			fighters[FRIEND].push_back(new Hero(this, c, curchar, side, dir));
			c = fighters[FRIEND].back()->get_character();

			if(parent->get_var(curchar + ".name") != "")
				c.name = parent->get_var(curchar + ".name");
			if(parent->get_var(curchar + ".defensive") == "true") c.defensive = true;
			else if(parent->get_var(curchar + ".defensive") == "false") c.defensive = false;
			if(parent->get_var(curchar + ".hp") != "")
				c.hp = atoi(parent->get_var(curchar + ".hp").c_str());
			if(parent->get_var(curchar + ".curhp") != "")
				c.curhp = atoi(parent->get_var(curchar + ".curhp").c_str());
			else
				c.curhp = c.hp;
			if(parent->get_var(curchar + ".mp") != "")
				c.mp = atoi(parent->get_var(curchar + ".mp").c_str());
			if(parent->get_var(curchar + ".curmp") != "")
				c.curmp = atoi(parent->get_var(curchar + ".curmp").c_str());
			else
				c.curmp = c.mp;
			if(parent->get_var(curchar + ".speed") != "")
				c.speed = atoi(parent->get_var(curchar + ".speed").c_str());
			if(parent->get_var(curchar + ".vigor") != "")
				c.vigor = atoi(parent->get_var(curchar + ".vigor").c_str());
			if(parent->get_var(curchar + ".stamina") != "")
				c.stamina = atoi(parent->get_var(curchar + ".stamina").c_str());
			if(parent->get_var(curchar + ".mpower") != "")
				c.mpower = atoi(parent->get_var(curchar + ".mpower").c_str());
			if(parent->get_var(curchar + ".apower") != "")
				c.apower = atoi(parent->get_var(curchar + ".apower").c_str());
			if(parent->get_var(curchar + ".mdefense") != "")
				c.mdefense = atoi(parent->get_var(curchar + ".mdefense").c_str());
			if(parent->get_var(curchar + ".adefense") != "")
				c.adefense = atoi(parent->get_var(curchar + ".adefense").c_str());
			if(parent->get_var(curchar + ".mblock") != "")
				c.mblock = atoi(parent->get_var(curchar + ".mblock").c_str());
			if(parent->get_var(curchar + ".ablock") != "")
				c.ablock = atoi(parent->get_var(curchar + ".ablock").c_str());
			if(parent->get_var(curchar + ".xp") != "")
				c.xp = atoi(parent->get_var(curchar + ".xp").c_str());
			if(parent->get_var(curchar + ".levelupxp") != "")
				c.levelupxp = atoi(parent->get_var(curchar + ".levelupxp").c_str());
			if(parent->get_var(curchar + ".level") != "")
				c.level = atoi(parent->get_var(curchar + ".level").c_str());
			if(parent->get_var(curchar + ".hitrate") != "")
				c.hitrate = atoi(parent->get_var(curchar + ".hitrate").c_str());

			for(int i = 0; i < 11; i++) {
				char s[50] = "";
				sprintf(s, "%s.element%i", curchar.c_str(), i);
				if(parent->get_var(s) == "normal") c.elements[i] = Character::NORMAL;
				else if(parent->get_var(s) == "weak") c.elements[i] = Character::WEAK;
				else if(parent->get_var(s) == "absorb") c.elements[i] = Character::ABSORB;
				else if(parent->get_var(s) == "immune") c.elements[i] = Character::IMMUNE;
				else if(parent->get_var(s) == "resist") c.elements[i] = Character::RESISTANT;
			}

			for(int i = 0; i < 25; i++) {
				char s[50] = "";
				sprintf(s, "%s.status%i", curchar.c_str(), i);
				if(parent->get_var(s) == "normal") c.status[i] = Character::NORMAL;
				else if(parent->get_var(s) == "immune") c.status[i] = Character::IMMUNE;
				else if(parent->get_var(s) == "suffering") c.status[i] = Character::SUFFERING;
			}

			fighters[FRIEND].back()->override_character(c);

			chars.erase(0, pos+1);
			pos = chars.find_first_of(";");
		} 
	}
}

Fight::~Fight() {
	for(int j = 0; j < 2; j++)
	for(int i = 0; i < fighters[j].size(); i++)
	if(!fighters[j][i]->is_monster()) {
		Character c = fighters[j][i]->get_character();
		string curchar = fighters[j][i]->get_spritename();

		parent->set_var(curchar + ".name", c.name);
		if(c.defensive == true) parent->set_var(curchar + ".defensive", "true");
		else parent->set_var(curchar + ".defensive", "false");
		parent->set_var(curchar + ".hp", c.hp);
		parent->set_var(curchar + ".curhp", c.curhp);
		parent->set_var(curchar + ".mp", c.mp);
		parent->set_var(curchar + ".curmp", c.curmp);
		parent->set_var(curchar + ".speed", c.speed);
		parent->set_var(curchar + ".vigor", c.vigor);
		parent->set_var(curchar + ".stamina", c.stamina);
		parent->set_var(curchar + ".mpower", c.mpower);
		parent->set_var(curchar + ".apower", c.apower);
		parent->set_var(curchar + ".mdefense", c.mdefense);
		parent->set_var(curchar + ".adefense", c.adefense);
		parent->set_var(curchar + ".mblock", c.mblock);
		parent->set_var(curchar + ".ablock", c.ablock);
		parent->set_var(curchar + ".xp", c.xp);
		parent->set_var(curchar + ".levelupxp", c.levelupxp);
		parent->set_var(curchar + ".level", c.level);
		parent->set_var(curchar + ".hitrate", c.hitrate);

		for(int i = 0; i < 11; i++) {
			char s[50] = "";
			sprintf(s, "%s.element%i", curchar.c_str(), i);
			if(c.elements[i] == Character::NORMAL) parent->set_var(s, "normal");
			else if(c.elements[i] == Character::WEAK) parent->set_var(s, "weak");
			else if(c.elements[i] == Character::ABSORB) parent->set_var(s, "absorb");
			else if(c.elements[i] == Character::IMMUNE) parent->set_var(s, "immune");
			else if(c.elements[i] == Character::RESISTANT) parent->set_var(s, "resist");
		}

		for(int i = 0; i < 25; i++) {
			char s[50] = "";
			sprintf(s, "%s.status%i", curchar.c_str(), i);
			if(c.status[i] == Character::NORMAL) parent->set_var(s, "normal");
			else if(c.status[i] == Character::IMMUNE) parent->set_var(s, "immune");
			else if(c.status[i] == Character::SUFFERING) parent->set_var(s, "suffering");
		}
	}

	imageloader.destroy(bg);
	imageloader.destroy(menu_bg);
	imageloader.destroy(auswahl);
	for(int i = 0; i < 2; i++) 
		for(int j = 0; j < fighters[i].size(); j++) {
			delete fighters[i][j];
		}
	for(int i = 0; i < defeated_fighters.size(); i++)
		delete defeated_fighters[i];
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
	switch(state) {
	case FIGHT:
		{
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

		if(fighters[ENEMY].size() == 0) {
			state = MENU;
		}
		}
	break;
	case MENU:
		{
		int xp = 0;
		int gp = 0;
		for(int i = 0; i < defeated_fighters.size(); i++) {
			Monster::Treasure t = ((Monster*)defeated_fighters[i])->treasure();
			xp += t.xp;
			gp += t.gp;
		}
		int living_heroes = 0;
		for(int i = 0; i < fighters[FRIEND].size(); i++) {
			if(fighters[FRIEND][i]->get_status(Character::WOUND) != Character::SUFFERING && !(fighters[FRIEND][i]->is_monster()))
				living_heroes++;
		}
		xp /= living_heroes;
		//enqueue_menu(); //xp
		for(int i = 0; i < fighters[FRIEND].size(); i++) {
			if(fighters[FRIEND][i]->get_status(Character::WOUND) != Character::SUFFERING && !(fighters[FRIEND][i]->is_monster()))
				if(((Hero*)fighters[FRIEND][i])->get_xp(xp)) {
					//levelup…
					//enqueue_menu(); //levelup?
				}
		}

		parent->set_var("gp", gp + atoi(parent->get_var("gp").c_str()));
		//enqueue_menu(); //gp
		//enqueue_menu(); //items
		
		}
		return 0;
	break;
	}
	return 1; //0 = Kampfende
};

void Fight::enqueue_command(Command c) {
	comqueue.push_back(c);
}

void Fight::enqueue_ready_fighter(FighterBase *f) {
	ready_fighters.push_back(f);
}

int Fight::get_fighter_count(int side) {
	if(side >= 2) return -1;
	return fighters[side].size();
}

int Fight::get_fighter_count(PlayerSide side) {
	int fc = 0;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++)
			if(fighters[i][j]->get_side() == side)
				fc++;
	return fc;
}

void Fight::add_fighter_target(Command &c, int fighter, int side) {
	c.add_target(fighters[side][fighter]);
}

void Fight::add_fighter_target(Command &c, int fighter, PlayerSide side) {
	int fc = 0;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++)
			if(fighters[i][j]->get_side() == side)
				if(fc == fighter) {
					c.add_target(fighters[i][j]);
					return;
				} else {
					fc++;
				}
}

void Fight::mark_fighter(int fighter, int side, bool mark) {
	marked_fighters[side][fighter] = mark;
}

void Fight::mark_fighter(int fighter, PlayerSide side, bool mark) {
	int fc = 0;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++)
			if(fighters[i][j]->get_side() == side)
				if(fc == fighter) {
					marked_fighters[i][j] = mark;
					return;
				} else {
					fc++;
				}
}

int Fight::get_index_of_fighter(FighterBase* f, PlayerSide s) {
	int fc = 0;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++)
			if(fighters[i][j]->get_side() == s)
				if(f == fighters[i][j]) {
					return fc;
				} else {
					fc++;
				}
}

int Fight::get_team(FighterBase* f) {
	for(int i = 0; i < fighters[FRIEND].size(); i++)
		if(fighters[FRIEND][i] == f)
			return FRIEND;
	for(int i = 0; i < fighters[ENEMY].size(); i++)
		if(fighters[ENEMY][i] == f)
			return ENEMY;
	return -1;
}

int Fight::get_team(int fighter, PlayerSide side) {
	int fc = 0;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++)
			if(fighters[i][j]->get_side() == side)
				if(fighter == fc) {
					return i;
				} else {
					fc++;
				}
}

PlayerSide Fight::get_PlayerSide(FighterBase *f) {
	return f->get_side();
}

void Fight::defeated_fighter(FighterBase *f) {
	for(int i = 0; i < ready_fighters.size(); i++) {
		if(f == ready_fighters[i])
			ready_fighters.erase(ready_fighters.begin()+i);
	}
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++) {
			if(fighters[i][j] == f) {
				fighters[i].erase(fighters[i].begin()+j);
				defeated_fighters.push_back(f);
			}
		}
}
