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

#include "menu.h"
#include "config.h"
#include "iohelper.h"
#include "guihelper.h"
#include <dirent.h>
#include <algorithm>
#include <vector>

void Menu::resize_menu(int x, int y, DIALOG *menu) {
	int wmax = 0;
	int hmax = 0;

	for(int i = 0; menu[i].proc; i++) {
		if(menu[i].x + menu[i].w > wmax)
			wmax = menu[i].x + menu[i].w;
		if(menu[i].y + menu[i].h > hmax)
			hmax = menu[i].y + menu[i].h;
	}

	float stretchx = (float)x/(float)wmax;
	float stretchy = (float)y/(float)hmax;

	for(int i = 0; menu[i].proc; i++) {
		menu[i].x *= stretchx;
		menu[i].y *= stretchy;
		menu[i].w *= stretchx;
		menu[i].h *= stretchy;
	}
}

int Menu::main_menu() {
	DIALOG menu[] =
	{
	   /* (proc)        (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                    (dp2) (dp3) */
	   { menu_bg_proc,  0,  0,   320, 240, 0,   0,   0,    0,      0,   0,   NULL,                   NULL, NULL },
	   { ff6_button,	56, 132, 212, 28,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Spiel starten", NULL, NULL },
	   { ff6_button,	56, 176, 212, 28,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Beenden",       NULL, NULL },
	   { d_bitmap_proc, 56, 36,  212, 80,  0,   0,   0,    0,      0,   0,   NULL,				     NULL, NULL },
	   { NULL,          0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                   NULL, NULL }
	};

	for(int i = 0; menu[i].proc; i++) {
		menu[i].fg = makecol(255,255,255);
		menu[i].bg = makecol(0,0,255);
	} 

	resize_menu(PC_RESOLUTION_X*PC_STRETCH_FACTOR, PC_RESOLUTION_Y*PC_STRETCH_FACTOR, menu);

	BITMAP *logo = imageloader.load("Images/title.tga");
	BITMAP *st_logo = imageloader.create(menu[3].w, menu[3].h);
	stretch_blit(logo, st_logo, 0, 0, logo->w, logo->h, 0, 0, st_logo->w, st_logo->h);
	menu[3].dp = (void*) st_logo;

	int ret = animated_dialog(menu, 2);

	imageloader.destroy(logo);
	imageloader.destroy(st_logo);

	if(ret == 1) ret = GAME;
	else if(ret == 2) ret = EXIT;

	return ret;
}

vector<string> files;

char* Menu::filelist_getter(int index, int* list_size) {
	if(index >= 0) {
		char *ret = new char[30];
		strcpy(ret, files[index].c_str());
		return ret;
	} else {
		*list_size = files.size();
		return NULL;
	}
}

string Menu::load_menu() {
	files.resize(0);

	DIR *dp;
    dirent *ep;
     
    dp = opendir ("./Saves/");
    if (dp != NULL) {
    	while (ep = readdir (dp))
            files.push_back(ep->d_name);
        closedir(dp);
    }

	sort(files.begin(), files.end());
	files.erase(files.begin(), files.begin()+2);

	DIALOG menu[] =
	{
	   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                      (dp2) (dp3) */
	   { menu_bg_proc,    0,   0,   320, 240, 0,   0,   0,    0,      0,   0,   NULL,                     NULL, NULL },
	   { d_list_proc,   32,  44,  256, 148, 0,   0,   0,    D_EXIT, 0,   0,   (void*)filelist_getter,   NULL, NULL },
	   { d_button_proc, 192, 200, 96,  20,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Laden",           NULL, NULL },
	   { d_ctext_proc,  32,  16,  256, 12,  0,   0,   0,    0,      0,   0,   (void*)"Spiel auswählen", NULL, NULL },
	   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                     NULL, NULL }
	};

	for(int i = 0; menu[i].proc; i++) {
		menu[i].fg = makecol(255,255,255);
		menu[i].bg = makecol(0,0,255);
	}

	resize_menu(SCREEN_W, SCREEN_H, menu);

	do_dialog(menu, 1);
	string s = filelist_getter(menu[1].d1, NULL);

	return s;
}

string Menu::save_menu() {
	files.resize(0);

	DIR *dp;
    dirent *ep;
     
    dp = opendir ("./Saves/");
    if (dp != NULL) {
    	while (ep = readdir (dp))
            files.push_back(ep->d_name);
        closedir(dp);
    }

	sort(files.begin(), files.end());
	files.erase(files.begin(), files.begin()+2);

	char text[101] = "";

	DIALOG menu[] =
	{
	   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                      (dp2) (dp3) */
	   { menu_bg_proc,    0,   0,   320, 240, 0,   0,   0,    0,      0,   0,   NULL,                     NULL, NULL },
	   { d_box_proc,    32,  200, 140, 20, 	0,   0,   0,    0,      0,   0,   NULL,                     NULL, NULL },
	   { d_list_proc,   32,  44,  256, 148, 0,   0,   0,    D_EXIT, 0,   0,   (void*)filelist_getter,   NULL, NULL },
	   { d_edit_proc,   34,  202, 136, 16,  0,   0,   0,    0,      100, 0,   (void*)text,				NULL, NULL },
	   { d_button_proc, 192, 200, 96,  20,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Speichern",       NULL, NULL },
	   { d_ctext_proc,  32,  16,  256, 12,  0,   0,   0,    0,      0,   0,   (void*)"Spiel auswählen", NULL, NULL },
	   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                     NULL, NULL }
	};

	for(int i = 0; menu[i].proc; i++) {
		menu[i].fg = makecol(255,255,255);
		menu[i].bg = makecol(0,0,255);
	}

	resize_menu(SCREEN_W, SCREEN_H, menu);

	do_dialog(menu, 2);

	string s;
	if(!strcmp(text, ""))
		s = filelist_getter(menu[2].d1, NULL);
	else
		s = text;

	return s;
}

int Menu::pause_menu() {
	DIALOG menu[] =
	{
	   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                     (dp2) (dp3) */
	   { d_text_proc,   0,   0,   320, 240, -1, -1,   0,    0,      0,   0,   (void*)"",               NULL, NULL },
	   { menu_bg_proc,    32,  44,  252, 152, 0,   0,   0,    0,      0,   0,   NULL,                    NULL, NULL },
	   { d_ctext_proc,  44,  52,  228, 12,  0,   0,   0,    0,      0,   0,   (void*)"PAUSE",          NULL, NULL },
	   { d_button_proc, 48,  76,  132, 20,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"zurück", 		   NULL, NULL },
	   { d_button_proc, 48,  101, 132, 20,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Speichern",      NULL, NULL },
	   { d_button_proc, 48,  126, 132, 20,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Hauptmenü",      NULL, NULL },
	   { d_button_proc, 48,  151, 132, 20,  0,   0,   0,    D_EXIT, 0,   0,   (void*)"Beenden",        NULL, NULL },
	   { d_text_proc,   188, 76,  84,  95,  0,   0,   0,    0,      0,   0,   (void*)"image: zahnrad", NULL, NULL },
	   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                    NULL, NULL }
	};

	for(int i = 2; menu[i].proc; i++) {
		menu[i].fg = makecol(255,255,255);
		menu[i].bg = makecol(0,0,255);
	}

	resize_menu(SCREEN_W, SCREEN_H, menu);

	clear_keybuf();
	int ret = do_dialog(menu, 3);

	int ret2 = CANCEL;

	if(ret == 4) ret2 = SAVE;
	else if(ret == 5) ret2 = ENDE;
	else if(ret == 6) ret2 = EXIT;
	else if(ret == 3) ret2 = CANCEL;

	return ret2;
}
