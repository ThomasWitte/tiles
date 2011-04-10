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
#include "iohelper.h"

BaseObject::BaseObject(int x, int y, bool s, Map *parent) {
	this->x = x;
	this->y = y;
	this->parent = parent;
	solid = s;
	action = false;
#ifdef ENABLE_DIALOG_MOVE_LOCK
	locked = false;
#endif
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
		imageloader.destroy(frames[i]);
}

void Animation::load(string dateiname) {
	name = dateiname;
	for(int i = 0; i < frames.size(); i++)
		imageloader.destroy(frames[i]);
	frames.resize(0);

	string prefix = string("Objects/") + dateiname + string("/");
	string s = prefix + dateiname;
	
	FileParser parser(s, "Object");
	deque< deque<string> > ret = parser.getsection("Animation");

	for(int i = 0; i< ret.size(); i++) {
		dateiname = prefix + ret[i][0];
		frames.push_back(imageloader.load(dateiname));
	}
}

BITMAP* Animation::get_frame(int frame) {
	frame = frame%frames.size();
	return frames[frame];
}

void Object::draw(int xpos, int ypos, BITMAP *buffer) {
	BITMAP *sprite = ani->get_frame(frame);
	if(parent)
		draw_sprite(buffer, sprite, xpos-parent->get_tilesize()/2, ypos-sprite->h+parent->get_tilesize()/2);
}
