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

#ifndef MENU_BASE_H
#define MENU_BASE_H

#include <allegro.h>
#include <deque>
#include "config.h"

using namespace std;

class MenuBase {
	public:
		~MenuBase();
		void draw(BITMAP *buffer);
		virtual int update(); //liefert 0, wenn Menü geschlossen wurde
	protected:
		//DIALOG_PLAYER und DIALOG als Stack…
		deque<DIALOG_PLAYER*> player;
		deque<DIALOG*> dialog;

		virtual DIALOG *create_dialog(int id) = 0;
		int update_game_menu(bool esc_possible, DIALOG_PLAYER *player);

		void delete_last_dialog();
};

#endif
