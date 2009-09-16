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

#include "object.h"
#include <fstream>

BaseObject::BaseObject(int x, int y, bool s, Map *parent) {
	this->x = x;
	this->y = y;
	this->parent = parent;
	solid = s;
}

void BaseObject::get_position(int &x, int &y) {
	x = this->x;
	y = this->y;
}

void BaseObject::set_position(int x, int y) {
	this->x = x;
	this->y = y;
}

void BaseObject::draw(int xpos, int ypos, BITMAP *buffer) {
}

void BaseObject::update() {
}

Animation::~Animation() {
	for(int i = 0; i < frames.size(); i++)
		destroy_bitmap(frames[i]);
}

void Animation::load(string dateiname) {
	name = dateiname;
	for(int i = 0; i < frames.size(); i++)
		destroy_bitmap(frames[i]);
	frames.resize(0);

	ifstream anifile;
	string prefix = string("Objects/").append(dateiname).append("/");
	string s = prefix;
	s.append(dateiname);
	anifile.open(s.c_str(), ios_base::in);
	
	anifile >> s;
	if(s == "[Animation]") {
		anifile >> s;
		while(s != "[eof]") {
			dateiname = prefix;
			dateiname.append(s);
			frames.push_back(load_bitmap(dateiname.c_str(), NULL));
			anifile >> s;
		}
	}

	anifile.close();
}

BITMAP* Animation::get_frame(int frame) {
	frame = frame%frames.size();
	return frames[frame];
}

void Object::draw(int xpos, int ypos, BITMAP *buffer) {
	BITMAP *sprite = ani->get_frame(frame);
	draw_sprite(buffer, sprite, xpos-sprite->w/2, ypos-sprite->h/2);
}
