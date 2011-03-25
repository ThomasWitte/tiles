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
#include <iostream>
#include <sstream>

#include "fight.h"
#include "fighter.h"
#include "iohelper.h"
#include "guihelper.h"

int fight_area_proc(int msg, DIALOG *d, int c) {
	return ((Fight*)d->dp)->fightarea(msg, d, c);
}

int status_box_proc(int msg, DIALOG *d, int c) {
	return ((Fight*)d->dp)->statusbox(msg, d, c);
}

int fighter_menu_proc(int msg, DIALOG *d, int c) {
	return ((Fight*)d->dp)->fightermenu(msg, d, c);
}

int target_choose_proc(int msg, DIALOG *d, int c) {
	return ((Fight*)d->dp)->target_choose(msg, d, c);
}

int listwin_proc(int msg, DIALOG *d, int c) {
	return ((Fight*)d->dp)->listwin(msg, d, c);
}

Fight::Fight(string dateiname, Game *g) {
	parent = g;
	bg = NULL;
	command_is_executed = 0;
	ifstream datei;
	string input;
	state = FIGHT;
	cur_cmd = NULL;
	fightarea_message_timeout = 0;
	
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

	//DIALOG erzeugen
	dialog.push_back(create_dialog(MAIN_DLG));
	player.push_back(init_dialog(dialog[0], 1));

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

	ready_fighters.resize(0);

	if(cur_cmd)
		delete cur_cmd;
	cur_cmd = NULL;
}

//Liefert die Einträge für die Attackenliste
FighterBase::MenuEntry *menu = NULL;
const char* get_list_win_entries(int index, int *size) {
	if(!menu)
		return "<Error>";

	if(index < 0) {
		*size = menu->submenu.size();
		return NULL;
	}

	if(index < menu->submenu.size()) {
		return menu->submenu[index].text.c_str();
	}

	return "<Error>";
}

DIALOG *Fight::create_dialog(int id) {
	DIALOG *ret = NULL;
	switch(id) {
		case MAIN_DLG:
		{
			ret = new DIALOG[8];
			DIALOG menu[] = {
				/* (proc)			(x)					(y)						(w)  					(h)					(fg)       (bg) (key) (flags) (d1)  (d2) 				(dp)              (dp2) (dp3) */
				{ menu_bg_proc,		-8,					2*PC_RESOLUTION_Y/3-8,	PC_RESOLUTION_X+16,		PC_RESOLUTION_Y/3+16,COL_WHITE,-1,  0,    0,      0,    0,   				NULL,             NULL, NULL },
				{ fight_area_proc,	0,					0,						PC_RESOLUTION_X,		2*PC_RESOLUTION_Y/3,0,         0,   0,    D_EXIT, 0,    0,   				this,             NULL, NULL },
				{ r_box_proc,		4,					4+2*PC_RESOLUTION_Y/3,	PC_RESOLUTION_X/3-4,	PC_RESOLUTION_Y/3-8,COL_WHITE, -1,  0,    0,      0,    0,   				NULL,             NULL, NULL },
				{ r_box_proc,		PC_RESOLUTION_X/3,	4+2*PC_RESOLUTION_Y/3,	2*PC_RESOLUTION_X/3-4,	PC_RESOLUTION_Y/3-8,COL_WHITE, -1,  0,    0,      0,    0,   				NULL,             NULL, NULL },
				{ status_box_proc,	PC_RESOLUTION_X/3+4,8+2*PC_RESOLUTION_Y/3,	2*PC_RESOLUTION_X/3-12,	PC_RESOLUTION_Y/3-16,COL_WHITE, -1,  0,   0,      0,    0,   				this,             NULL, NULL },
				{ fighter_menu_proc,8,					13+2*PC_RESOLUTION_Y/3,	PC_RESOLUTION_X/3-12,	PC_RESOLUTION_Y/3-16,COL_WHITE, -1,  0,   0,      0,    0,   				this,             NULL, NULL },
				{ NULL,				0,  				0,						0,   					0,					0,         0,   0,    0,      0,    0,   				NULL,             NULL, NULL }
			};
			memcpy(ret, menu, 8*sizeof(DIALOG));
			cout << "Fight created" << endl;
		}
		break;

		case LIST_WIN:
		{
			ret = new DIALOG[5];
			DIALOG test[] = {
				/* (proc)			(x)					(y)						(w)  					(h)					(fg)       (bg) (key) (flags) (d1)  (d2) 				(dp)              (dp2) (dp3) */
				{menu_bg_proc,		0,					2*PC_RESOLUTION_Y/3,	PC_RESOLUTION_X,		PC_RESOLUTION_Y/3,	COL_WHITE, -1,	0,	  0, 0, 0, NULL, NULL, NULL},
				{ff6_list,			10,					2*PC_RESOLUTION_Y/3+8,	2*PC_RESOLUTION_X/3,	PC_RESOLUTION_Y/3-12,COL_WHITE, -1,	0,    D_EXIT, 0, 0, (void*)get_list_win_entries, NULL, NULL},
				{ fight_area_proc,	0,					0,						PC_RESOLUTION_X,		2*PC_RESOLUTION_Y/3,0,         0,   0,    D_EXIT, 0,    0,   				this,             NULL, NULL },
				{listwin_proc,		0,					0,						0,						0,					0,		   0,	0,	  D_EXIT, BACK_KEY, 0, this, NULL, NULL},
				{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
			};
			memcpy(ret, test, 5*sizeof(DIALOG));
		}
		break;

		case TARGET_CHOOSER:
		{
			ret = new DIALOG[3];
			DIALOG test[] = {
				/* (proc)			(x)					(y)						(w)  					(h)					(fg)       (bg) (key) (flags) (d1)  (d2) 				(dp)              (dp2) (dp3) */
				{fight_area_proc,	0,					0,						PC_RESOLUTION_X,		2*PC_RESOLUTION_Y/3,0,         0,   0,    D_EXIT, 0,    0,   				this,             NULL, NULL },
				{target_choose_proc,0,					0,						PC_RESOLUTION_X,		2*PC_RESOLUTION_Y/3,0,			0,	0,		D_EXIT,0,	0,					this,				NULL,NULL},
				{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
			};
			memcpy(ret, test, 3*sizeof(DIALOG));
		}
		break;

	}
	return ret;
}

int Fight::listwin(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_START:
			d->d2 = 0;
		break;

		case MSG_IDLE: //MSG_XCHAR funktioniert nicht.
			if(key[d->d1] && d->flags & D_EXIT) {
				//Markieren, dass Menü ohne Auswahl geschlossen wurde
				d->d2 = 1;
				return D_CLOSE;
			}
		break;

		case MSG_END:
		{
			int current_menu = get_active_menu_fighter(0);

			if(d->d2 == 0) {
				//DIALOG wird durch Auswahl geschlossen
				//Target_chooser spawnen
				if(ready_fighters.size() > current_menu && ready_fighters[current_menu]) {
					if(cur_cmd) delete cur_cmd;
					cur_cmd = new Command(ready_fighters[current_menu]);
					int close_obj = dialog.back()[1].d1; //gewählter index aus der Liste
					cur_cmd->set_attack(get_list_win_entries(close_obj, NULL));

					//sich selber löschen
					player.pop_back();
					delete [] dialog.back();
					dialog.pop_back();

					//neuer Dialog
					dialog.push_back(create_dialog(TARGET_CHOOSER));
					player.push_back(init_dialog(dialog.back(), 1));

					//dies wird von delete_last_dialog wieder gelöscht
					player.push_back(NULL);
					dialog.push_back(create_dialog(TARGET_CHOOSER));

					ready_fighters.erase(ready_fighters.begin()+current_menu);
				}
			}
		}
		break;
	}
	return D_O_K;
}

int Fight::target_choose(int msg, DIALOG *d, int c) {
	//d->d1: cur_target
	//d->d2: multitarget
	//d->bg: (PlayerSide)target_side
	//d->dp2: fighter
	AttackLib::Attack a = AttackLib::get_attack(cur_cmd->get_attack());

	switch(msg) {
		case MSG_START:
		{
			//aktives Menü bestimmen
			int current_menu = get_active_menu_fighter(0);

			//Starttargets werden anhand der gewählten Attacke ausgesucht…
			d->dp2 = (void*)ready_fighters[current_menu];
			d->d2 = AttackLib::MULTI;
			if(a.possible_targets & AttackLib::SINGLE) {
				d->d2 = AttackLib::SINGLE;
			}

			//bestmögliches Ziel auswählen
			if(a.element == AttackLib::HEAL && (a.possible_targets & AttackLib::FRIEND)) {
				d->bg = ((Fighter*)d->dp2)->get_side();
				d->d1 = 0;
			} else if(a.possible_targets & AttackLib::ENEMY) {
				d->bg = ((Fighter*)d->dp2)->get_side();
				d->d1 = 0;
				for(int i = LEFT; i <= RIGHT; i++) {
					if((((Fighter*)d->dp2)->get_side() != i) && get_fighter_count((PlayerSide)i)) {
						d->bg = i;
						d->d1 = 0;
					}
				}
			} else if(a.possible_targets & AttackLib::FRIEND) {
				d->bg = ((Fighter*)d->dp2)->get_side();
				d->d1 = 0;
			} else { //SELF einziges mögliches ziel
				d->bg = ((Fighter*)d->dp2)->get_side();
				d->d1 = get_index_of_fighter(((Fighter*)d->dp2), (PlayerSide)d->bg);
			}
			
		}
		break;

		//Fokus akzeptieren
		case MSG_WANTFOCUS:
		return D_WANTFOCUS;

		case MSG_CHAR:
		{
			int fc = get_fighter_count((PlayerSide)d->bg);
			switch(c >> 8) {
				case DIR_UP:
					if((a.possible_targets & AttackLib::FRIEND) || (a.possible_targets & AttackLib::ENEMY)) {
						d->d1--;
						if(d->d1 < 0) d->d1 = fc-1;
					}
				return D_USED_CHAR;

				case DIR_DOWN:
					if((a.possible_targets & AttackLib::FRIEND) || (a.possible_targets & AttackLib::ENEMY)) {
						d->d1++;
						if(d->d1 >= fc) d->d1 = 0;
					}
				return D_USED_CHAR;

				case DIR_LEFT:
				{
					int i = d->bg;
					do {
						i--;
						if(i < 0) i = RIGHT;
						d->d1 = 0;
						d->bg = i;
					} while(get_fighter_count((PlayerSide)d->bg) == 0 ||
							((get_team(d->d1, (PlayerSide)d->bg) == Fight::FRIEND) && !(a.possible_targets & AttackLib::FRIEND)) || 
							((get_team(d->d1, (PlayerSide)d->bg) == Fight::ENEMY) && !(a.possible_targets & AttackLib::ENEMY))
							);
				}
				return D_USED_CHAR;

				case DIR_RIGHT:
				{
					int i = d->bg;
					do {
						i++;
						d->d1 = 0;
						d->bg = i%3;
					} while(get_fighter_count((PlayerSide)d->bg) == 0 ||
							((get_team(d->d1, (PlayerSide)d->bg) == Fight::FRIEND) && !(a.possible_targets & AttackLib::FRIEND)) || 
							((get_team(d->d1, (PlayerSide)d->bg) == Fight::ENEMY) && !(a.possible_targets & AttackLib::ENEMY))
							);
				}
				return D_USED_CHAR;

				case SWITCH_KEY:
				{
					if(d->d2 == AttackLib::SINGLE) {
						if(a.possible_targets & AttackLib::MULTI) {
							d->d2 = AttackLib::MULTI;
						}
					} else if (d->d2 == AttackLib::MULTI) {
						if(a.possible_targets & AttackLib::SINGLE) {
							d->d2 = AttackLib::SINGLE;
						}
					}
				}
				return D_USED_CHAR;

				case ACTION_KEY:
					if(d->d2 == AttackLib::SINGLE) {
						add_fighter_target(*cur_cmd, d->d1, (PlayerSide)d->bg);
					} else {
						for(int i = 0; i < get_fighter_count((PlayerSide)d->bg); i++)
							add_fighter_target(*cur_cmd, i, (PlayerSide)d->bg);
					}

					//Fighter in richtige Richtung drehen
					if(((Fighter*)d->dp2)->get_side() < d->bg)
						((Fighter*)d->dp2)->set_dir(1);
					else if(((Fighter*)d->dp2)->get_side() > d->bg)
						((Fighter*)d->dp2)->set_dir(0);

					//Befehl in Comqueue aufnehemen
					if(cur_cmd != NULL) {
						enqueue_command(*cur_cmd);
					}
				return D_CLOSE;
			}
		}
		break;

		case MSG_IDLE:
			//Alle Markierungen entfernen
			for(int h = 0; h < 2; h++)
				for(int i = 0; i < get_fighter_count(h); i++)
					mark_fighter(i, h, false);

			//aktuelle Markierungen hinzufügen
			if(d->d2 == AttackLib::SINGLE) {
				mark_fighter(d->d1, (PlayerSide)d->bg, true);
			} else {
				for(int i = 0; i < get_fighter_count((PlayerSide)d->bg); i++)
					mark_fighter(i, (PlayerSide)d->bg, true);
			}
		break;

		case MSG_END:
			for(int h = 0; h < 2; h++)
				for(int i = 0; i < get_fighter_count(h); i++)
					mark_fighter(i, h, false);

			if(cur_cmd) {
				delete cur_cmd;
				cur_cmd = NULL;
			}
		break;
	}
	return D_O_K;
}

int Fight::fightarea(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_DRAW:
			draw_fightarea(gui_get_screen(), d);
		break;

		case MSG_IDLE:
			if(update_fightarea() == 0 && d->flags & D_EXIT) return D_CLOSE;
		break;
	}
	return D_O_K;
}

int Fight::statusbox(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_DRAW:
			for(int i = 0; i < fighters[FRIEND].size(); i++) {
				fighters[FRIEND][i]->draw_status(gui_get_screen(), d->x, d->y+i*((d->h)/fighters[FRIEND].size()), d->w, (d->h)/fighters[FRIEND].size());
			}
		break;
	}
	return D_O_K;
}

int Fight::nestedmenu(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_START:
			if(d->dp2)
				d->dp3 = (void*)init_dialog((DIALOG*)d->dp2, 0);
		break;

		case MSG_END:
			if(d->dp2) {
				shutdown_dialog((DIALOG_PLAYER*)d->dp3);
				delete [] (DIALOG*)d->dp2;
				d->dp2 = NULL;
			}
		break;

		case MSG_CHAR:
			d->bg = c; //letzter tastendruck wird in d->bg gespeichert bis zur idle message
		return D_USED_CHAR;

		case MSG_DRAW:
			dialog_message((DIALOG*)d->dp2, MSG_DRAW, 0, NULL);
		break;

		case MSG_IDLE:
			//keyboard-input wieder in buffer schreiben	
			if(d->bg >= 0) {
				simulate_ukeypress(d->bg%256, d->bg/256);
				d->bg = -1;
			}

			//nested menu updaten
			if(!update_game_menu(true, (DIALOG_PLAYER*)d->dp3)) {
				//D_CLOSE erhalten
				nestedmenu(MSG_END, d, c);
			}
		break;
	}
	return D_O_K;
}

#define MSG_REBUILD_MENU MSG_USER+1

int Fight::fightermenu(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_START:
			{
			char** strings = new char*[5];
			strings[4] = NULL;
			d->dp2 = (void*) new DIALOG[6];
			DIALOG menu[] = {
				/* (proc)			(x)			(y)				(w)  		(h)			(fg)		(bg) (key) (flags) (d1)  (d2) 	(dp)    				(dp2)  (dp3) */
				{ ff6_button,		d->x+16,	d->y,			d->w-8,		8,			COL_WHITE,	-1,  0,    0,      0,    0,		(void*)(strings[0] = new char[25]),   NULL,  NULL },
				{ ff6_button,		d->x+16,	d->y+1*d->h/4,	d->w-8,		8,			COL_WHITE,	-1,  0,    0,      0,    0,		(void*)(strings[1] = new char[25]),   NULL,  NULL },
				{ ff6_button,		d->x+16,	d->y+2*d->h/4,	d->w-8,		8,			COL_WHITE,	-1,  0,    0,      0,    0,		(void*)(strings[2] = new char[25]),   NULL,  NULL },
				{ ff6_button,		d->x+16,	d->y+3*d->h/4,	d->w-8,		8,			COL_WHITE,	-1,  0,    0,      0,    0,		(void*)(strings[3] = new char[25]),   NULL,  NULL },
				{ dialog_cleanup,	0,  		0,  			0,			0,			0,	        0,   0,    0,      0,    0,     (void*)strings,     	NULL,                  NULL },
				{ NULL,				0,  		0,				0,   		0,			0,			0,   0,    0,      0,    0,   	NULL,   NULL,  NULL }
			};
			memcpy((DIALOG*)d->dp2, menu, 6*sizeof(DIALOG));

			d->dp3 = (void*)init_dialog((DIALOG*)d->dp2, 0);
			d->d1 = -1;
			fightermenu(MSG_REBUILD_MENU, d, c);
			}
		return D_O_K;
		break;

		case MSG_REBUILD_MENU:
			if(d->d1 >= 0) {
				FighterBase::MenuEntry *menu_items = ready_fighters[d->d1]->get_menu_entry("Menu");
				if(!menu_items) break;

				for(int i = 0; i < 4; i++) {
					((DIALOG*)d->dp2)[i].flags &= ~D_OPEN;
					((DIALOG*)d->dp2)[i].flags &= ~D_EXIT;
						
					if(menu_items->submenu.size() > i) {
						strcpy((char*)((DIALOG*)d->dp2)[i].dp, menu_items->submenu[i].text.c_str());
						((DIALOG*)d->dp2)[i].flags &= ~D_HIDDEN;
						if(menu_items->submenu[i].submenu.size() > 0) //der aktuelle Eintrag besitzt ein Submenü
							((DIALOG*)d->dp2)[i].flags |= D_OPEN; //schickt beim aktivieren ein D_SPAWN
						else
							((DIALOG*)d->dp2)[i].flags |= D_EXIT; //Eintrag ist schon ein Command -> Menü schließen etc…
					} else {
						((DIALOG*)d->dp2)[i].flags |= D_HIDDEN;
					}
				}
			} else {
				for(int i = 0; i < 4; i++) {
					((DIALOG*)d->dp2)[i].flags |= D_HIDDEN;
					((DIALOG*)d->dp2)[i].flags &= ~D_OPEN;
					((DIALOG*)d->dp2)[i].flags &= ~D_EXIT;
				}
			}
		return D_O_K;
		break;

		case MSG_IDLE:
		{
			//Fighter updaten
			for(int i = 0; i < 2; i++)
				for(int j = 0; j < fighters[i].size(); j++) {
					fighters[i][j]->update();
				}

			//Aktuell aktives Menü neu bestimmen
			int current_menu = get_active_menu_fighter(-1);

			//neues Menü?, dann Strings updaten
			if(current_menu != d->d1) {
				d->d1 = current_menu;
				fightermenu(MSG_REBUILD_MENU, d, c);
			}

			//keyboard-input wieder in buffer schreiben	
			if(d->bg >= 0) {
				simulate_ukeypress(d->bg%256, d->bg/256);
				d->bg = -1;
			}

			//nested menu updaten und bei bedarf dialog spawnen
			if(!update_game_menu(true, (DIALOG_PLAYER*)d->dp3) && d->d1 >= 0) {
				//wurde Dialog durch auswahl eines Eintrags geschlossen?
				int close_obj = ((DIALOG_PLAYER*)d->dp3)->obj;
				if(close_obj >= 0) {
					//TargetChoose Dialog spawnen
					if(cur_cmd) delete cur_cmd;
					cur_cmd = new Command(ready_fighters[current_menu]);
					FighterBase::MenuEntry *menu_items = ready_fighters[d->d1]->get_menu_entry("Menu");
					cur_cmd->set_attack(menu_items->submenu[close_obj].text.c_str());

					dialog.push_back(create_dialog(TARGET_CHOOSER));
					player.push_back(init_dialog(dialog.back(), 1));

					ready_fighters.erase(ready_fighters.begin()+current_menu);
				} else {
					ready_fighters.push_back(ready_fighters[current_menu]);
					ready_fighters.erase(ready_fighters.begin()+current_menu);
				}
			} else if(((DIALOG_PLAYER*)d->dp3)->res & D_SPAWN && d->d1 >= 0) {
				((DIALOG_PLAYER*)d->dp3)->res &= ~D_SPAWN;

				int c = ((DIALOG_PLAYER*)d->dp3)->obj;
				menu = ready_fighters[d->d1]->get_menu_entry((char*)((DIALOG*)d->dp2)[c].dp); //c enthält das auslösende widget
				dialog.push_back(create_dialog(LIST_WIN));
				player.push_back(init_dialog(dialog.back(), 1));
			}

		}
		return D_O_K;
		break;

		case MSG_DRAW:
		case MSG_CHAR:
		case MSG_END:
		return nestedmenu(msg, d, c);

	}
	return d_button_proc(msg,d,c);
}

#undef MSG_REBUILD_MENU

void Fight::draw_fightarea(BITMAP *buffer, DIALOG *dlg) {
	//Hintergrund
	stretch_blit(bg, buffer, 0, 0, bg->w, bg->h, dlg->x, dlg->y, dlg->w, dlg->h);

	//Fighter pro Seite zählen
	int sz[3]; for(int i = 0; i < 3; i++) sz[i] = 0;
	int szd[3]; for(int i = 0; i < 3; i++) szd[i] = 0;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++) {
			sz[fighters[i][j]->get_side()]++;
		}

	int x, y;
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < fighters[i].size(); j++) {

			//Fighter platzieren
			x = dlg->w/8 + dlg->w/8 * 3 * fighters[i][j]->get_side();
			y = dlg->h / (sz[fighters[i][j]->get_side()]+1) * (szd[fighters[i][j]->get_side()]+1);
			szd[fighters[i][j]->get_side()]++;

			//Fighter zeichnen
			fighters[i][j]->draw(buffer, x, y);
			if(marked_fighters[i][j])
				masked_blit(auswahl, buffer, 0, 0, x, y-25, auswahl->w, auswahl->h);

		}

	if(fightarea_message_timeout > 0) {
		DIALOG pos = { NULL, dlg->x, dlg->y, dlg->w, 24, COL_WHITE, -1, 0, 0, 0, 0, NULL, NULL, NULL };
		menu_bg_proc(MSG_DRAW, &pos, 0);
		gui_textout_ex(buffer, fightarea_message.c_str(), dlg->w/2, 8, COL_WHITE, -1, TRUE);
	}
}

int Fight::update_fightarea() {
	if(fightarea_message_timeout > 0) {
		fightarea_message_timeout--;
	}

	switch(state) {
	case FIGHT:
	{

		if(command_is_executed > 0) {
			if(command_is_executed > 3000) {

				for(int i = 0; i < 2; i++)
				for(int j = 0; j < fighters[i].size(); j++) {
					fighters[i][j]->set_animation(Fighter::NORMAL);
				}

				comqueue[0].execute();
				if(comqueue[0].get_attack() != "Fight")
					set_fightarea_message(GAME_TIMER_BPS, comqueue[0].get_attack());
				comqueue.pop_front();
				command_is_executed = -1;

			} else if(command_is_executed == 1) {
				//falls eigener Kämpfer -> laufanimation
				for(int i = 0; i < fighters[FRIEND].size(); i++) {
					if(comqueue[0].is_caster(fighters[FRIEND][i]))
						fighters[FRIEND][i]->set_animation(Fighter::ATTACK);
				}

			} else if(command_is_executed == 2000) {
				for(int i = 0; i < 2; i++)
				for(int j = 0; j < fighters[i].size(); j++) {
					//Treffer und zurück bewegen
					if(comqueue[0].is_target(fighters[i][j])) {
						fighters[i][j]->set_animation(Fighter::HURT);
					}
					if(comqueue[0].is_caster(fighters[i][j])) {
						fighters[i][j]->set_animation(Fighter::RETURN);
					}
				}

			} else if(command_is_executed == GAME_TIMER_BPS/3) {
				command_is_executed = 1999; //Direkt zur hit animation springen

			} else if(command_is_executed == 2000+GAME_TIMER_BPS/3) {
				command_is_executed = 3000; //zum execute()
			}
			command_is_executed++;
		}

		if(comqueue.size() > 0 && command_is_executed == 0) {
			command_is_executed = 1;
		}

		//Fighter animieren
		for(int i = 0; i < 2; i++)
			for(int j = 0; j < fighters[i].size(); j++) {
				fighters[i][j]->animate();
			}

		//Kampf beendet?
		if(fighters[ENEMY].size() == 0 && fightarea_message_timeout <= 0) {
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

int Fight::update() {
	if(!update_game_menu(false, player.back())) { //D_CLOSE erhalten! false: esc beendet Kampf nicht
		if(player.size() <= 1) {
			return 0; //zurück zur map
		} else { //aktiven Dialog schließen
			delete_last_dialog();
		}
	} else if(player.back()->res & D_SPAWN) {
		player.back()->res &= ~D_SPAWN;
		int id = (player.back()->dialog + player.back()->obj)->d2;
		dialog.push_back(create_dialog(id));
		player.push_back(init_dialog(dialog.back(), 1));
	}
	return 1;
}

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

int Fight::get_active_menu_fighter(int defval) {
	int current_menu = defval;
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
	return current_menu;
}

void Fight::set_fightarea_message(int timeout, string text) {
	fightarea_message_timeout = timeout;
	fightarea_message = text;
}
