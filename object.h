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

#ifndef OBJECT_H
#define OBJECT_H

#include <allegro.h>
#include "config.h"
#include "map.h"

class BaseObject {
	protected:
		int x;
		int y;
		class Map *parent;
	public:
		BaseObject(int x, int y, class Map *parent = NULL);
		virtual void get_position(int &x, int &y);
		virtual void set_position(int x, int y);
		virtual void draw(int xpos, int ypos, BITMAP *buffer);
		virtual void update();
};

#include "sprite.h"

#endif
