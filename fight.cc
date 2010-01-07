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
#include <fstream>

Command::Command(long time) {
	exec_time = time;
}

long Command::get_time() {
	return exec_time;
}

void Command::execute() {
}

Fighter::Fighter(Fight *f) {
	parent = f;
}

Fighter::~Fighter() {
}

void Fighter::update() {
}

void Fighter::draw(BITMAP *buffer, int x, int y) {
	rectfill(buffer, x-10, y-10, x+10, y+10, makecol(255,0,0));
}

Fight::Fight(string dateiname) {
	bg = NULL;
	ifstream datei;
	string input;

	dateiname = ((string)("Fights/")).append(dateiname);
	datei.open(dateiname.c_str(), ios_base::in);

	datei >> input;
	while(input != "[eof]") {
		if(input == "Background") {
			datei >> input;
			input = ((string)("Fights/Images/")).append(input);
			bg = load_bitmap(input.c_str(), NULL);
		}
		datei >> input;
	}

	datei.close();
	time = 0;

	menu_bg = create_bitmap(PC_RESOLUTION_X, PC_RESOLUTION_Y/3);
	for(int i = 0; i < menu_bg->h; i++) {
		line(menu_bg, 0, i, menu_bg->w, i, makecol(i, i, 255-i));
	}
	vline(menu_bg, menu_bg->w/3, 3, menu_bg->h-4, makecol(255, 255, 255));
	rect(menu_bg, 3, 3, menu_bg->w-4, menu_bg->h-4, makecol(255, 255, 255));

	//Positionstest
	for(int i = 0; i < 3; i++)
		fighters[LEFT].push_back(new Fighter(this));
	for(int i = 0; i < 2; i++)
		fighters[MIDDLE].push_back(new Fighter(this));
	for(int i = 0; i < 4; i++)
		fighters[RIGHT].push_back(new Fighter(this));

}

Fight::~Fight() {
	if(bg)
		destroy_bitmap(bg);
	destroy_bitmap(menu_bg);
}

void Fight::draw(BITMAP *buffer) {
	int x, y;
	stretch_blit(bg, buffer, 0, 0, bg->w, bg->h, 0, 0, buffer->w, buffer->h);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < fighters[i].size(); j++) {
			x = PC_RESOLUTION_X/8 + PC_RESOLUTION_X/8 * 3 * i; // + (i-1) * j-fighters[i].size()/2 * PC_RESOLUTION_X/16;
			y = (2*PC_RESOLUTION_Y/3) / (fighters[i].size()+1) * (j+1);
			fighters[i][j]->draw(buffer, x, y);
		}
	blit(menu_bg, buffer, 0, 0, 0, 2*PC_RESOLUTION_Y/3, PC_RESOLUTION_X, PC_RESOLUTION_Y);
}

int Fight::update() {
	time++;
	if(comqueue.size())
		if(comqueue[0].get_time() <= time) {
			comqueue[0].execute();
			comqueue.pop_front();
		}
	return 1; //0 = Kampfende
};
