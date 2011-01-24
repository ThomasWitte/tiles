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

#include "game_menu.h"
#include "guihelper.h"

GameMenu::GameMenu(Game *parent) {
	this->parent = parent;

	dialog.push_back(create_dialog(MAIN_DIALOG));
	player.push_back(init_dialog(dialog[0], 10));
}

DIALOG *GameMenu::create_dialog(int id) {
	DIALOG *ret = NULL;
	switch(id) {
		case ITEM_DIALOG:
			ret = create_item_dialog();
		break;
		case ITEM_SP_DIALOG:
			ret = create_item_sp_dialog();
		break;
		case SKILL_CH_DIALOG:
			ret = create_ch_chooser(SKILL_DIALOG);
		break;
		case SKILL_DIALOG:
			ret = create_skill_dialog();
		break;
		case EQUIP_CH_DIALOG:
			ret = create_ch_chooser(EQUIP_DIALOG);
		break;
		case EQUIP_DIALOG:
			ret = create_equip_dialog();
		break;
		case RELIC_CH_DIALOG:
			ret = create_ch_chooser(RELIC_DIALOG);
		break;
		case RELIC_DIALOG:
			ret = create_relic_dialog();
		break;
		case STATUS_DIALOG:
			ret = create_status_dialog();
		break;
		case STATUS_CH_DIALOG:
			ret = create_ch_chooser(STATUS_DIALOG);
		break;
		default:
		case MAIN_DIALOG:
			ret = create_main_dialog();
		break;
	}
	return ret;
}

DIALOG *GameMenu::create_item_dialog() {
	DIALOG *ret = new DIALOG[10];
	DIALOG menu[] =
	{
	   /* (proc)        (x)  (y) (w)  (h)  (fg)       (bg) (key) (flags) (d1) (d2) 				(dp)              (dp2) (dp3) */
	   { menu_bg_proc,  0,   0,  320, 240, COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { ff6_button,    208, 12, 88,  16,  COL_WHITE, -1,  0,    D_OPEN, 0,   ITEM_SP_DIALOG,   (void*)"Special", NULL, NULL },
	   { ff6_button,    120, 12, 88,  16,  COL_WHITE, -1,  0,    D_DISABLED,0, 0,   			(void*)"Sort",    NULL, NULL },
	   { r_box_proc,    8,   8,  96,  16,  COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { r_box_proc,    104, 8,  208, 16,  COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { r_box_proc,    8,   24, 304, 48,  COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { r_box_proc,    8,   72, 304, 160, COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { d_text_proc,   16,  12, 48,  8,   COL_WHITE, -1,  0,    0,      0,   0,   				(void*)"Item",    NULL, NULL },
	   { d_text_proc,   16,  28, 290, 40,  COL_WHITE, -1,  0,    0,      0,   0,   				(void*)"",        NULL, NULL },
	   { NULL,          0,   0,  0,   0,   0,         0,   0,    0,      0,   0,   				NULL,             NULL, NULL }
	};
	memcpy(ret, menu, 10*sizeof(DIALOG));
	cout << "item_dialog created" << endl;
	return ret;
}

DIALOG *GameMenu::create_item_sp_dialog() {
	DIALOG *ret = new DIALOG[10];
	DIALOG menu[] =
	{
	   /* (proc)        (x)  (y) (w)  (h)  (fg)       (bg) (key) (flags) (d1) (d2) 				(dp)              (dp2) (dp3) */
	   { menu_bg_proc,  0,   0,  320, 240, COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { ff6_button,    208, 12, 88,  16,  COL_WHITE, -1,  0,    D_EXIT, 0,   0,   				(void*)"Useable", NULL, NULL },
	   { ff6_button,    120, 12, 88,  16,  COL_WHITE, -1,  0,    D_DISABLED,0, 0,   			(void*)"Sort",    NULL, NULL },
	   { r_box_proc,    8,   8,  96,  16,  COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { r_box_proc,    104, 8,  208, 16,  COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { r_box_proc,    8,   24, 304, 48,  COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { r_box_proc,    8,   72, 304, 160, COL_WHITE, -1,  0,    0,      0,   0,   				NULL,             NULL, NULL },
	   { d_text_proc,   16,  12, 48,  8,   COL_WHITE, -1,  0,    0,      0,   0,   				(void*)"Item",    NULL, NULL },
	   { d_text_proc,   16,  28, 290, 40,  COL_WHITE, -1,  0,    0,      0,   0,   				(void*)"",        NULL, NULL },
	   { NULL,          0,   0,  0,   0,   0,         0,   0,    0,      0,   0,   				NULL,             NULL, NULL }
	};
	memcpy(ret, menu, 10*sizeof(DIALOG));
	cout << "item_sp_dialog created" << endl;
	return ret;
}

DIALOG *GameMenu::create_main_dialog() {
	DIALOG *ret = new DIALOG[22];
	DIALOG menu[] =
	{
	   /* (proc)       (x)  (y)  (w)  (h)  (fg)       (bg) (key) (flags)     (d1) (d2)           (dp)                (dp2)                   (dp3) */
	   { menu_bg_proc, 0,   0,   320, 240, 0,         0,   0,    0,          0,   0,             NULL,               NULL,                   NULL },
	   { r_box_proc,   232, 8,   80,  136, COL_WHITE, -1,  0,    0,          0,   0,             NULL,               NULL,                   NULL },
	   { r_box_proc,   232, 144, 80,  32,  COL_WHITE, -1,  0,    0,          0,   0,             NULL,               NULL,                   NULL },
	   { r_box_proc,   232, 176, 80,  56,  COL_WHITE, -1,  0,    0,          0,   0,             NULL,               NULL,                   NULL },
	   { d_text_proc,  240, 184, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Time",      NULL,                   NULL },
	   { d_text_proc,  240, 208, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Steps",     NULL,                   NULL },
	   { d_text_proc,  240, 152, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Gil",       NULL,                   NULL },
	   { gvar_update,  248, 192, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)"Game.Playtime", NULL },
	   { gvar_update,  248, 216, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)"Game.Steps",    NULL },
	   { gvar_update,  248, 160, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)"gp",            NULL },
	   { ff6_button,   240, 16,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   ITEM_DIALOG,   (void*)"Item",      NULL,                   NULL },
	   { ff6_button,   240, 32,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   SKILL_CH_DIALOG,  (void*)"Skills",    NULL,                   NULL },
	   { ff6_button,   240, 48,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   EQUIP_CH_DIALOG,  (void*)"Equip",     NULL,                   NULL },
	   { ff6_button,   240, 64,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   RELIC_CH_DIALOG,  (void*)"Relic",     NULL,                   NULL },
	   { ff6_button,   240, 80,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   STATUS_CH_DIALOG, (void*)"Status",    NULL,                   NULL },
	   { ff6_button,   240, 96,  64,  16,  COL_WHITE, -1,  0,    D_DISABLED, 0,   0,             (void*)"Save/Exit", NULL,                   NULL },
	   { ff6_button,   240, 112, 64,  16,  COL_WHITE, -1,  0,    D_EXIT,     0,   0,             (void*)"Close",     NULL,                   NULL },
	   { char_select,  8,   8,   216, 56,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      NULL,                   NULL },
	   { char_select,  8,   64,  216, 56,  COL_WHITE, -1,  0,    0,          0,   1,             (void*)parent,      NULL,                   NULL },
	   { char_select,  8,   120, 216, 56,  COL_WHITE, -1,  0,    0,          0,   2,             (void*)parent,      NULL,                   NULL },
	   { char_select,  8,   176, 216, 56,  COL_WHITE, -1,  0,    0,          0,   3,             (void*)parent,      NULL,                   NULL },
	   { NULL,         0,   0,   0,   0,   0,         0,   0,    0,          0,   0,             NULL,               NULL,                   NULL }
	};
	memcpy(ret, menu, 22*sizeof(DIALOG));
	cout << "main_dialog created" << endl;
	return ret;
}

DIALOG *GameMenu::create_ch_chooser(DIALOG_ID id) {
	DIALOG *ret = new DIALOG[6];
	DIALOG menu[] =
	{
	   /* (proc)       (x)  (y)  (w)  (h)  (fg)       (bg) (key) (flags)     (d1) (d2)  (dp)                (dp2)                  	(dp3) */
	   { d_text_proc,  0,   0,   0,   0,   0,		  0,   0,    0,          0,   0,    (void*)"",      	NULL,                   NULL },
	   { ch_button,    8,   8,   216, 56,  COL_WHITE, 0,   0,    D_OPEN,     0,   id,	(void*)parent,      NULL,					(void*)"Internal.dlgID" },
	   { ch_button,    8,   64,  216, 56,  COL_WHITE, 1,   0,    D_OPEN,     0,   id, 	(void*)parent,	    NULL,					(void*)"Internal.dlgID" },
	   { ch_button,    8,   120, 216, 56,  COL_WHITE, 2,   0,    D_OPEN,     0,   id,	(void*)parent,	    NULL,					(void*)"Internal.dlgID" },
	   { ch_button,    8,   176, 216, 56,  COL_WHITE, 3,   0,    D_OPEN,     0,   id,	(void*)parent,	    NULL,					(void*)"Internal.dlgID" },
	   { NULL,         0,   0,   0,   0,   0,         0,   0,    0,          0,   0,    NULL,               NULL,                  	NULL }
	};
	memcpy(ret, menu, 6*sizeof(DIALOG));
	cout << "character_chooser created" << endl;
	return ret;
}

DIALOG *GameMenu::create_skill_dialog() {
	delete_last_dialog();
	string pstr = get_chosen_player();

	DIALOG *ret = new DIALOG[3];
	DIALOG menu[] =
	{
	   /* (proc)       (x)  (y)  (w)  (h)  (fg)       (bg) (key) (flags)     (d1) (d2) (dp)                     (dp2) (dp3) */
	   { menu_bg_proc, 0,   0,   320, 240, COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { d_text_proc,  16,  12,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"Skill",         NULL, NULL },
	   { NULL,         0,   0,   0,   0,   0,         0,   0,    0,          0,   0,   NULL,                    NULL, NULL }
	};
	
	memcpy(ret, menu, 3*sizeof(DIALOG));
	cout << "skill_dialog created" << endl;
	return ret;
}

DIALOG *GameMenu::create_equip_dialog() {
	delete_last_dialog();
	string pstr = get_chosen_player();

	char** strings = new char*[3];
	strings[2] = NULL;

	DIALOG *ret = new DIALOG[16];
	DIALOG menu[] =
	{
	   /* (proc)       (x)  (y)  (w)  (h)  (fg)       (bg) (key) (flags)     (d1) (d2) (dp)                     (dp2) (dp3) */
	   { menu_bg_proc, 0,   0,   320, 240, COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { ff6_button,   240, 12,  72,  16,  COL_WHITE, -1,  0,    D_DISABLED, 0,   0,   (void*)"Empty",          NULL, NULL },
	   { ff6_button,   160, 12,  72,  16,  COL_WHITE, -1,  0,    D_DISABLED, 0,   0,   (void*)"Remove",         NULL, NULL },
	   { ff6_button,   80,  12,  72,  16,  COL_WHITE, -1,  0,    D_DISABLED, 0,   0,   (void*)"Optimal",        NULL, NULL },
	   { r_box_proc,   8,   8,   304, 16,  COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { r_box_proc,   8,   24,  304, 72,  COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { r_box_proc,   8,   96,  304, 136, COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { d_text_proc,  16,  12,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"Equip.",         NULL, NULL },
	   { d_text_proc,  16,  32,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"R-Hand",         NULL, NULL },
	   { d_text_proc,  16,  48,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"L-Hand",         NULL, NULL },
	   { d_text_proc,  16,  64,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"Head",           NULL, NULL },
	   { d_text_proc,  16,  80,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"Body",           NULL, NULL },
	   { gvar_update,  232, 80,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)parent,      		(void*)(strings[0] = tochar(pstr+".name")), NULL },
	   { transp_bmp,   256, 32,  48,  48,  0,         0,   0,    0,          0,   0,   (void*)(strings[1] = tochar("Fights/Fighters/" + pstr + "/face.tga")), NULL, NULL },
	   { dialog_cleanup,0,  0,   0,   0,   0,         0,   0,    0,          0,   0,   (void*)strings,     		NULL,                  NULL },
	   { NULL,         0,   0,   0,   0,   0,         0,   0,    0,          0,   0,   NULL,                    NULL, NULL }
	};
	
	memcpy(ret, menu, 16*sizeof(DIALOG));
	cout << "equip_dialog created" << endl;
	return ret;
}

DIALOG *GameMenu::create_relic_dialog() {
	delete_last_dialog();
	string pstr = get_chosen_player();

	char** strings = new char*[3];
	strings[2] = NULL;

	DIALOG *ret = new DIALOG[12];
	DIALOG menu[] =
	{
	   /* (proc)       (x)  (y)  (w)  (h)  (fg)       (bg) (key) (flags)     (d1) (d2) (dp)                     (dp2) (dp3) */
	   { menu_bg_proc, 0,   0,   320, 240, COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { ff6_button,   160, 12,  72,  16,  COL_WHITE, -1,  0,    D_DISABLED, 0,   0,   (void*)"Remove",         NULL, NULL },
	   { r_box_proc,   8,   8,   304, 16,  COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { r_box_proc,   8,   24,  304, 72,  COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { r_box_proc,   8,   96,  304, 136, COL_WHITE, -1,  0,    0,          0,   0,   NULL,                    NULL, NULL },
	   { d_text_proc,  16,  12,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"Equip.",         NULL, NULL },
	   { d_text_proc,  16,  64,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"Relic",          NULL, NULL },
	   { d_text_proc,  16,  80,  48,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)"Relic",          NULL, NULL },
	   { gvar_update,  232, 80,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,   (void*)parent,      		(void*)(strings[0] = tochar(pstr+".name")), NULL },
	   { transp_bmp,   256, 32,  48,  48,  0,         0,   0,    0,          0,   0,   (void*)(strings[1] = tochar("Fights/Fighters/" + pstr + "/face.tga")), NULL, NULL },
	   { dialog_cleanup,0,  0,   0,   0,   0,         0,   0,    0,          0,   0,   (void*)strings,		    NULL,                  NULL },
	   { NULL,         0,   0,   0,   0,   0,         0,   0,    0,          0,   0,   NULL,                    NULL, NULL }
	};
	
	memcpy(ret, menu, 12*sizeof(DIALOG));
	cout << "relic_dialog created" << endl;
	return ret;
}


DIALOG *GameMenu::create_status_dialog() {
	delete_last_dialog(); //status chooser entfernen
	string pstr = get_chosen_player();

	char** strings = new char*[20];
	strings[19] = NULL;
	
	DIALOG *ret = new DIALOG[40];
	DIALOG menu[] =
	{
	   /* (proc)       (x)  (y)  (w)  (h)  (fg)       (bg) (key) (flags)     (d1) (d2)           (dp)                (dp2)                  (dp3) */
	   { menu_bg_proc, 0,   0,   320, 240, 0,         0,   0,    0,          0,   0,             NULL,               NULL,                  NULL },
	   { r_box_proc,   8,   8,   304, 16,  COL_WHITE, -1,  0,    0,          0,   0, 			 NULL,               NULL,					NULL },
	   { r_box_proc,   8,   24,  304, 208, COL_WHITE, -1,  0,    0,          0,   0, 			 NULL,               NULL,					NULL },
	   { r_box_proc,   160, 84,  140, 80,  COL_WHITE, -1,  0,    0,          0,   0,             NULL,               NULL,					NULL },
	   { d_text_proc,  16,  12,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Status",    NULL,                  NULL },
	   { d_text_proc,  70,  42,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"LV",        NULL,                  NULL },
	   { gvar_update,  102, 42,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[0] = tochar(pstr+".level")), NULL },
	   { d_text_proc,  70,  54,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"HP      /", NULL,                  NULL },
	   { gvar_update,  102, 54,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[1] = tochar(pstr+".curhp")), NULL },
	   { gvar_update,  142, 54,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[2] = tochar(pstr+".hp")), NULL },
	   { d_text_proc,  70,  66,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"MP      /", NULL,                  NULL },
	   { gvar_update,  102, 66,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[3] = tochar(pstr+".curmp")), NULL },
	   { gvar_update,  142, 66,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[4] = tochar(pstr+".mp")), NULL },
	   { d_text_proc,  16,  92,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Your Exp.", NULL,                  NULL },
	   { gvar_update,  70,  108, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[5] = tochar(pstr+".xp")), NULL },
	   { d_text_proc,  16,  124, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)"For level up",NULL,                NULL },
	   { gvar_update,  70,  140, 56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[6] = tochar(pstr+".levelupxp")), NULL },
	   { transp_bmp,   16,  40,  48,  48,  0,          0,  0,    0,          0,   0,             (void*)(strings[7] = tochar("Fights/Fighters/" + pstr + "/face.tga")), NULL, NULL },
	   { gvar_update,  16,  28,  56,  8,   COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[8] = tochar(pstr+".name")), NULL },
	   { d_text_proc,  16,  186,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Vigor",     NULL,                  NULL },
	   { gvar_update,  100, 186,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[9] = tochar(pstr+".vigor")), NULL },
	   { d_text_proc,  16,  198,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Speed",     NULL,                  NULL },
	   { gvar_update,  100, 198,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[10] = tochar(pstr+".speed")), NULL },
	   { d_text_proc,  16,  210,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Stamina",   NULL,                  NULL },
	   { gvar_update,  100, 210,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[11] = tochar(pstr+".stamina")), NULL },
	   { d_text_proc,  16,  222,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Mag.Pwr",   NULL,                  NULL },
	   { gvar_update,  100, 222,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[12] = tochar(pstr+".mpower")), NULL },
	   { d_text_proc,  160, 174,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Bat.Pwr",   NULL,                  NULL },
	   { gvar_update,  244, 174,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[13] = tochar(pstr+".apower")), NULL },
	   { d_text_proc,  160, 186,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Defense",   NULL,                  NULL },
	   { gvar_update,  244, 186,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[14] = tochar(pstr+".adefense")), NULL },
	   { d_text_proc,  160, 198,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Evade%",    NULL,                  NULL },
	   { gvar_update,  244, 198,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[15] = tochar(pstr+".ablock")), NULL },
	   { d_text_proc,  160, 210,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"Mag.Def",   NULL,                  NULL },
	   { gvar_update,  244, 210,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[16] = tochar(pstr+".mdefense")), NULL },
	   { d_text_proc,  160, 222,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)"MBlock%",   NULL,                  NULL },
	   { gvar_update,  244, 222,  56,  8,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)parent,      (void*)(strings[17] = tochar(pstr+".mblock")), NULL },
	   { menu_items,   172, 88,  140, 72,  COL_WHITE, -1,  0,    0,          0,   0,             (void*)(strings[18] = tochar(pstr)),NULL,					NULL },
	   { dialog_cleanup,0,  0,   0,   0,   0,         0,   0,    0,          0,   0,             (void*)strings,     NULL,                  NULL },
	   { NULL,         0,   0,   0,   0,   0,         0,   0,    0,          0,   0,             NULL,               NULL,                  NULL }
	};
	//ich brauche ein array mit den speicheradressen das an ein dialogelement übergeben wird: dieses dialog_cleanup element gibt den speicher frei…
	memcpy(ret, menu, 40*sizeof(DIALOG));
	cout << "status_dialog created" << endl;
	return ret;
}

string GameMenu::get_chosen_player() {
	string pstr;
	int player = atoi(parent->get_var("Internal.dlgID").c_str());
	string chars = parent->get_var("CharactersInBattle");
	for(int i = 0; i <= player; i++) {
		int pos = chars.find_first_of(";");
		if(pos == string::npos)
			break;
		pstr = chars.substr(0, pos);
		chars.erase(0, pos+1);
	}
	return pstr;
}

