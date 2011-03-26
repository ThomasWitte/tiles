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
#include "fighter.h"
#include "iohelper.h"
#include <iostream>
#include <sstream>

//Tabellen für Levelup
int xptable[] = {
	0,		0,		32,		96,		208,	400,	672,	1056,	1552,	2184,
	2976,	3936,	5080,	6432,	7992,	9784,	11840,	14152,	16736,	19616,
	22832,	26360,	30232,	34456,	39056,	44072,	49464,	55288,	61568,	68304,
	75496,	83184,	91384,	100088,	109344,	119136,	129504,	140464,	152008,	164184,
	176976,	190416,	204520,	219320,	234808,	251000,	267936,	285600,	304040,	323248,
	343248,	364064,	385696,	408160,	431488,	455680,	480776,	506760,	533680,	561528,
	590320,	620096,	650840,	682600,	715368,	749160,	784016,	819920,	856920,	895016,
	934208,	974536,	1016000,1058640,1102456,1147456,1193648,1241080,1289744,1339672,
	1390872,1443368,1497160,1552264,1608712,1666512,1725688,1786240,1848184,1911552,
	1976352,2042608,2110320,2179504,2250192,2322392,2396128,2471400,2548224,2637112,
	2637112
};

int hptable[] = {
	0,		11,		12,		14,		17,		20,		22,		24,		26,		27,
	28,		30,		35,		39,		44,		50,		54,		57,		61,		65,
	67,		69,		72,		76,		79,		82,		86,		90,		95,		99,
	100,	101,	102,	102,	103,	104,	106,	107,	108,	110,
	111,	113,	114,	116,	117,	119,	120,	122,	125,	128,
	130,	131,	133,	134,	136,	137,	139,	142,	144,	145,
	147,	148,	150,	152,	153,	155,	156,	158,	160,	162,
	160,	155,	151,	145,	140,	136,	132,	126,	120,	117,
	113,	110,	108,	105,	102,	100,	98,		95,		92,		90,
	88,		87,		85,		83,		82,		80,		83,		86,		88,		0
};

int mptable[] = {
	0,		0,		0,		5,		5,		6,		6,		7,		8,		8,
	9,		9,		10,		10,		10,		10,		10,		11,		11,		11,
	11,		11,		12,		12,		12,		12,		12,		13,		13,		13,
	13,		13,		14,		14,		14,		14,		14,		15,		15,		15,
	15,		15,		16,		16,		16,		16,		16,		17,		17,		17,
	16,		15,		14,		13,		12,		11,		10,		9,		8,		7,
	6,		5,		5,		6,		6,		7,		7,		7,		8,		8,
	8,		8,		8,		7,		7,		7,		6,		6,		6,		6,
	5,		5,		5,		5,		5,		5,		5,		6,		6,		6,
	6,		6,		7,		8,		9,		10,		11,		12,		13,		0
};

//ende

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
	current_animation = NORMAL;

	spritename = name;
	laden(name);

	menu.set_parent(this);
}

Fighter::~Fighter() {
	for(unsigned int i = 0; i < ts.normal.size(); i++)
		imageloader.destroy(ts.normal[i]);
}

void Fighter::laden(string name) {
	string path;
	path = string("Fights/Fighters/") + name + string("/");

	FileParser parser(path + name, "Fighter");

	deque< deque<string> > ret = parser.getsection("normal");
	for(unsigned int i = 0; i < ret.size(); i++)
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
	c.xp = parser.getvalue("Fighter", "xp", c.xp);
	c.levelupxp = parser.getvalue("Fighter", "levelupxp", c.levelupxp);

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

	if(c.status[Character::WOUND] != Character::SUFFERING &&
		c.status[Character::STOP] != Character::SUFFERING &&
		c.status[Character::SLEEP] != Character::SUFFERING &&
		c.status[Character::PETRIFY] != Character::SUFFERING)

		if(atb < 65536) {
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

	if(itc > 255 && c.status[Character::WOUND] != Character::SUFFERING) {
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
}

void Fighter::animate() {
	step++;
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

void Fighter::get_ready() {
	atb = 0;
}

bool Fighter::is_friend() {
	if(parent->get_team(this) == Fight::FRIEND) return true;
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
			for(int j = 0; j < 25; j++) {
				c.status[j] = o.status[j];
			}
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

FighterBase::MenuEntry* Fighter::FighterMenu::get_menu_entry(string name, MenuEntry *e) {
	if(!e) e = &menu;
	if(e->text == name)
		return e;

	for(unsigned int i = 0; i < e->submenu.size(); i++) {
		MenuEntry *ret = NULL;
		ret = get_menu_entry(name, &e->submenu[i]);
		if(ret)
			return ret;
	}

	return NULL;
}

Fighter::FighterMenu::FighterMenu() {
	fighter = NULL;
}

Fight *get_parent(Fighter& f) {
	return f.parent;
}

void Fighter::FighterMenu::set_parent(Fighter *fighter) {
	this->fighter = fighter;
}

Fighter::FighterMenu::~FighterMenu() {
}

void Fighter::FighterMenu::set_items(deque< deque<string> > items) {
	//Das Speicherformat gefällt mir überhaupt nicht (nur einfach verschachtelt)…
	//bei gelegenheit ändern
	MenuEntry e, e2;
	menu.text = "Menu";
	for(unsigned int i = 0; i < items.size(); i++) {
		e.submenu.resize(0);
		e.text = items[i][0];
		for(unsigned int j = 1; j < items[i].size(); j++) {
			e2.text = items[i][j];
			e.submenu.push_back(e2);
		}
		menu.submenu.push_back(e);
	}
}

Hero::Hero(Fight *f, Character c, string name, PlayerSide side, int dir)
	: Fighter(f, c, name, side, dir) {
}

int Hero::get_xp(int xp) {
	c.xp += xp;
	c.levelupxp -= xp;
	while(c.levelupxp <= 0 && c.level < 100) {
		c.level++;
		c.levelupxp += xptable[c.level]-xptable[c.level-1];

		//HP etc steigern…
		c.hp += hptable[c.level-1];
		if(c.hp > MAX_HP) c.hp = MAX_HP;
		c.mp += mptable[c.level-1];
		if(c.mp > MAX_MP) c.mp = MAX_MP;
		return 1;
	}
	return 0;
}

void Hero::draw(BITMAP *buffer, int x, int y) {
	switch(current_animation) {
		case ATTACK:
			x -= (get_side()-1)*(step*3*PC_RESOLUTION_X/8)/GAME_TIMER_BPS;
		break;
		case RETURN:
			x -= (get_side()-1)*((GAME_TIMER_BPS/3-step)*3*PC_RESOLUTION_X/8)/GAME_TIMER_BPS;
		break;
		default:
		break;
	}

	Fighter::draw(buffer, x, y);
}

Monster::Monster(Fight *f, Character c, string name, PlayerSide side, int dir)
	: Fighter(f, c, name, side, dir) {
	laden(name);
}

void Monster::update() {
	Fighter::update();
	if(c.status[Character::WOUND] == Character::SUFFERING) {
		if(current_animation != DIE) {
			set_animation(DIE);
		} else if(step > GAME_TIMER_BPS/3) {
			parent->defeated_fighter(this);
		}
	}
}

void Monster::draw(BITMAP *buffer, int x, int y) {
	switch(current_animation) {
		case HURT:
			x += step%3-1;
		break;
	
		case DIE:
		{
			set_trans_blender(200, 128, 255, 0);
			int index = (step/SPRITE_ANIMATION_SPEED)%ts.normal.size();

			//spiegeln, wenn direction = 0(links)
			if(direction == 0) {
				BITMAP *temp = create_bitmap(ts.normal[index]->w, ts.normal[index]->h);
				draw_sprite_h_flip(temp, ts.normal[index], 0, 0);
				draw_lit_sprite(buffer, ts.normal[index], x-ts.normal[index]->w/2, y-ts.normal[index]->h/2, 100+step*300/GAME_TIMER_BPS);
				destroy_bitmap(temp);
			} else {
				draw_lit_sprite(buffer, ts.normal[index], x-ts.normal[index]->w/2, y-ts.normal[index]->h/2, 100+step*300/GAME_TIMER_BPS);
			}
			if(textremframes) {
				textremframes--;
				textout_ex(buffer, font, texttoshow.c_str(), x-10, y-25+textremframes/2, textcol, -1);
			}
		}
		return;

		default:
		break;
	}

	Fighter::draw(buffer, x, y);
}

Command Monster::get_command() {
	Command c(this);
	if(com_script.size() == 0) {
		c.set_attack("Fight");
		return c;
	}

	int index = 0;

	while(true) {
		if(com_script[comcounter][index] == "if") {
			//Stimmt zwar nicht, aber das Skriptsystem muss sowieso überarbeitet werden
			if(com_script[comcounter][index+1] == "var") {
				index++;
			}
			if(com_script[comcounter][index+3] == "var") {
				index++;
			}
			index += 5;
			for(unsigned int i = 0; i < com_script[comcounter].size(); i++) {
				if(com_script[comcounter][i] == "else")
					com_script[comcounter].erase(com_script[comcounter].begin()+i);
			}

		} if(com_script[comcounter][index] == "rand") {
			c.set_attack(com_script[comcounter][index + 1 + random()%(com_script[comcounter].size()-index-1)]);
			break;
		} else {
			c.set_attack(com_script[comcounter][index]);
			break;
		}
	}

	comcounter++;
	if(comcounter >= com_script.size()) comcounter = 0;
	return c;
}

void Monster::laden(string name) {
	string path;
	path = string("Fights/Fighters/") + name + string("/");
	FileParser parser(path + name, "Fighter");

	t.xp = parser.getvalue("Treasure", "xp", 0);
	t.gp = parser.getvalue("Treasure", "gp", 0);
	t.dropped_items = parser.get("Treasure", "Dropped");
	t.stolen_items = parser.get("Treasure", "Stolen");
	t.morph_items = parser.get("Treasure", "MorphItems");
	t.morph = parser.getvalue("Treasure", "Morph", 0);

	com_script = parser.getsection("Script");
	comcounter = 0;
}

Monster::Treasure Monster::treasure() {
	return t;
}
