/*  Copyright 2009-2011 Thomas Witte

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

#ifndef ATTACK_ANIMATIONS_H
#define ATTACK_ANIMATIONS_H

#include <allegro.h>
#include <vector>
#include "config.h"
#include "iohelper.h"

struct Position {
	int x, y;
};

struct AnimationData {
	enum {INITIALIZE = -1, DESTROY = -2};
	std::vector<Position> targets;
	Position caster;
	std::vector<int> d;
	std::vector<void*> dp;
};

/***********************************************************
Es ist nur sichergestellt, dass die Animationsfunktion einmal
mit step = INITIALIZE und einmal mit step = DESTROY aufgerufen
wird, dazwischen können Schritte übersprungen oder verdoppelt
werden.
Beim INITIALIZE und DESTROY-Aufruf wird als buffer NULL über-
geben.
***********************************************************/

int fight_ani(int step, AnimationData *data, BITMAP *buffer);

#endif

