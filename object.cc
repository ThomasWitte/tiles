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

BaseObject::BaseObject(int x, int y, Map *parent) {
	this->x = x;
	this->y = y;
	this->parent = parent;
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
