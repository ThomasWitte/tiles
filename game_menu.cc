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

#include "game_menu.h"

GameMenu::GameMenu(Game *parent) {
	this->parent = parent;

	//Hier muss ein echter Dialog her…
	dialog = new DIALOG[10];
	DIALOG menu[] =
	{
	   /* (proc)        (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                    (dp2) (dp3) */
	   { d_button_proc, 56, 132, 212, 28,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Spiel starten", NULL, NULL },
	   { d_button_proc, 56, 176, 212, 28,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Beenden",       NULL, NULL },
	   { d_bitmap_proc, 56, 36,  212, 80,  0,   0,   0,    0,      0,   0,   NULL,				     NULL, NULL },
	   { NULL,          0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                   NULL, NULL }
	};
	memcpy(dialog, menu, 4*sizeof(DIALOG));
	player = init_dialog(dialog, -1);
}

GameMenu::~GameMenu() {
	shutdown_dialog(player);
	delete [] dialog;
}

void GameMenu::draw(BITMAP *buffer) {
	gui_set_screen(buffer);
	dialog_message(dialog, MSG_DRAW, 0, NULL);
	gui_set_screen(NULL);
}

int GameMenu::update() {
	update_dialog(player); //Ich brauche einen Ersatz, der nichts zeichnet…
	return 0; //0 = zurück zur map
}
