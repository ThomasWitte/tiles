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

#ifndef MENU_H
#define MENU_H

#include <allegro.h>
#include <string>

using namespace std;

class Menu {
	private:

	static char* filelist_getter(int, int*);
	static void resize_menu(int, int, DIALOG*);

	public:
	
	enum {GAME, ENDE, EXIT, SAVE};

	static int main_menu();
	static string load_menu();
	static string save_menu();
	static int pause_menu();
};

#endif
