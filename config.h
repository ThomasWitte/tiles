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

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_TILES_PER_TILESET 256

//Logische Framerate
#define GAME_TIMER_BPS 60
#define MAX_FRAMESKIP 4

//Plattform
//#define GP2X
//#define WINDOWS

//interne Auflösung
#define PC_RESOLUTION_X 320
#define PC_RESOLUTION_Y 240

//Faktor, um den die Fensterausgabe gestreckt wird
#define PC_STRETCH_FACTOR 2

//Anzahl der Frames bis sich die Spriteanimation ändert
#define SPRITE_ANIMATION_SPEED 3

//fps anzeigen?
//#define ENABLE_FRAME_COUNTER

#endif
