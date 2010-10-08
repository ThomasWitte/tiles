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

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <allegro.h>
#include <string>
#include <deque>
#include "config.h"
#include "game.h"

using namespace std;

class GameMenu {
	public:
		GameMenu(Game *parent);
		~GameMenu();
		void draw(BITMAP *buffer);
		int update(); //liefert 0, wenn Menü geschlossen wurde
	protected:
		enum DIALOG_ID {MAIN_DIALOG, ITEM_DIALOG, SKILL_DIALOG, EQUIP_DIALOG, RELIC_DIALOG, STATUS_DIALOG};
		Game *parent;
		int update_game_menu();
		DIALOG *create_dialog(DIALOG_ID id);
		//DIALOG_PLAYER und DIALOG als Stack…
		deque<DIALOG_PLAYER*> player;
		deque<DIALOG*> dialog;
};

#endif
