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

#ifndef GUIHELPER_H
#define GUIHELPER_H

#include <allegro.h>

#define D_OPEN 128 //neuen Dialog öffnen, ID im d2 feld der sendenden DIALOG-Struktur
int menu_bg_proc(int msg, DIALOG *d, int c);		//Farbverlauf als Menühintergrund
int gvar_update(int msg, DIALOG *d, int c);		//angezeigter text wird ständig aktualisiert
int v_ch_proc(int msg, DIALOG *d, int c);			//Container mit vertikaler Auswahl
int r_box_proc(int msg, DIALOG *d, int c);			//abgerundete Box

//aus Allegro Quellcode:

int move_focus(DIALOG *d, int ascii, int scan, int *focus_object);

#endif
