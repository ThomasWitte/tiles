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

#define MAX_TILES_PER_TILESET 1024

//Spielname
#define TILES_NAME "tiles"
#define TILES_VERSION 1.0

//Logische Framerate
#define GAME_TIMER_BPS 60
#define MAX_FRAMESKIP 4

//Plattform
//#define GP2X
//#define WINDOWS
//#define ALLEGRO5

//interne Auflösung
#define PC_RESOLUTION_X 320
#define PC_RESOLUTION_Y 240

//Faktor, um den die Fensterausgabe gestreckt wird
#define PC_STRETCH_FACTOR 2

//Anzahl der Frames bis sich die Spriteanimation ändert
#define SPRITE_ANIMATION_SPEED 3

//fps anzeigen?
//#define ENABLE_FRAME_COUNTER

//Spieleinstellungen
//Während Dialogen bewegung verbieten?
#define ENABLE_DIALOG_MOVE_LOCK
#define MAX_DAMAGE 9999
#define XP_FACTOR 9.15 //werte zwischen 9.05 und 9.25 sollten funktionieren; niedriger = schneller levelup;

//Tastenbelegungen
#define DIR_LEFT KEY_LEFT
#define DIR_RIGHT KEY_RIGHT
#define DIR_UP KEY_UP
#define DIR_DOWN KEY_DOWN
#define INGAME_MENU_KEY KEY_ESC
#define MENU_KEY KEY_F12
#define ACTION_KEY KEY_SPACE

//Farben
#define COL_WHITE makecol(255,255,255)
#define COL_YELLOW makecol(255,255,0)
#define COL_GREEN makecol(0,255,0)
#define COL_BLACK makecol(0,0,0)
#define COL_LIGHT_BLUE makecol(128,128,255)
#define COL_RED makecol(255,0,0)

extern volatile int timecounter;

#endif
