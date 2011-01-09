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

GameMenu::~GameMenu() {
	for(int i = 0; i < player.size(); i++) {
		shutdown_dialog(player[i]);
		delete [] dialog[i];
	}
}

void GameMenu::draw(BITMAP *buffer) {
	gui_set_screen(buffer);
	dialog_message(dialog.back(), MSG_DRAW, 0, NULL);
	gui_set_screen(NULL);
}

int GameMenu::update() {
	if(!update_game_menu()) { //D_CLOSE erhalten!
		if(player.size() <= 1) {
			return 0; //zurück zur map
		} else { //aktiven Dialog schließen
			shutdown_dialog(player.back());
			player.pop_back();
			delete [] dialog.back();
			dialog.pop_back();
		}
	} else if(player.back()->res & D_SPAWN) {
		player.back()->res &= ~D_SPAWN;
		DIALOG_ID id = (DIALOG_ID)(player.back()->dialog + player.back()->obj)->d2;
		dialog.push_back(create_dialog(id));
		player.push_back(init_dialog(dialog.back(), 1));
	}
	return 1;
}

DIALOG *GameMenu::create_dialog(DIALOG_ID id) {
	DIALOG *ret = NULL;
	switch(id) {
		default:
		case ITEM_DIALOG:
		case SKILL_DIALOG:
		case EQUIP_DIALOG:
		case RELIC_DIALOG:
		break;
		case STATUS_DIALOG:
			ret = create_status_dialog();
		break;
		case STATUS_CH_DIALOG:
			ret = create_status_chooser();
		break;
		case MAIN_DIALOG:
			ret = create_main_dialog();
		break;
	}
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
	   { ff6_button,   240, 32,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   SKILL_DIALOG,  (void*)"Skills",    NULL,                   NULL },
	   { ff6_button,   240, 48,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   EQUIP_DIALOG,  (void*)"Equip",     NULL,                   NULL },
	   { ff6_button,   240, 64,  64,  16,  COL_WHITE, -1,  0,    D_OPEN,     0,   RELIC_DIALOG,  (void*)"Relic",     NULL,                   NULL },
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

DIALOG *GameMenu::create_status_chooser() {
	DIALOG *ret = new DIALOG[5];
	DIALOG menu[] =
	{
	   /* (proc)       (x)  (y)  (w)  (h)  (fg)       (bg) (key) (flags)     (d1) (d2)           (dp)                (dp2)                  (dp3) */
	   { ch_button,    8,   8,   216, 56,  COL_WHITE, 0,   0,    D_OPEN,     0,   STATUS_DIALOG, (void*)parent,      NULL,					(void*)"Internal.dlgID" },
	   { ch_button,    8,   64,  216, 56,  COL_WHITE, 1,   0,    D_OPEN,     0,   STATUS_DIALOG, (void*)parent,	     NULL,					(void*)"Internal.dlgID" },
	   { ch_button,    8,   120, 216, 56,  COL_WHITE, 2,   0,    D_OPEN,     0,   STATUS_DIALOG, (void*)parent,	     NULL,					(void*)"Internal.dlgID" },
	   { ch_button,    8,   176, 216, 56,  COL_WHITE, 3,   0,    D_OPEN,     0,   STATUS_DIALOG, (void*)parent,	     NULL,					(void*)"Internal.dlgID" },
	   { NULL,         0,   0,   0,   0,   0,         0,   0,    0,          0,   0,             NULL,               NULL,                  NULL }
	};
	memcpy(ret, menu, 5*sizeof(DIALOG));
	cout << "status_chooser created" << endl;
	return ret;
}

DIALOG *GameMenu::create_status_dialog() {
	shutdown_dialog(player.back()); //status chooser entfernen
	player.pop_back();
	delete [] dialog.back();
	dialog.pop_back();

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

#define MESSAGE(i, msg, c) {                       \
   r = object_message(player.back()->dialog+i, msg, c);   \
   if (r != D_O_K) {                               \
      player.back()->res |= r;                            \
      player.back()->obj = i;                             \
   }                                               \
}

int GameMenu::update_game_menu()
{
   int c, cascii, cscan, ccombo, r, ret, nowhere, z;
   ASSERT(player.back());

   /* need to give the input focus to someone? */
   if (player.back()->res & D_WANTFOCUS) {
      player.back()->res ^= D_WANTFOCUS;
      player.back()->res |= offer_focus(player.back()->dialog, player.back()->obj, &player.back()->focus_obj, FALSE);
   }

   /* deal with keyboard input */
   if ((cascii) || (cscan) || (keypressed())) {
      if ((!cascii) && (!cscan))
	 cascii = ureadkey(&cscan);

      ccombo = (cscan<<8) | ((cascii <= 255) ? cascii : '^');

      /* let object deal with the key */
      if (player.back()->focus_obj >= 0) {
	 MESSAGE(player.back()->focus_obj, MSG_CHAR, ccombo);
	 if (player.back()->res & (D_USED_CHAR | D_CLOSE))
	    goto getout;

	 MESSAGE(player.back()->focus_obj, MSG_UCHAR, cascii);
	 if (player.back()->res & (D_USED_CHAR | D_CLOSE))
	    goto getout;
      }

      /* keyboard shortcut? */
      for (c=0; player.back()->dialog[c].proc; c++) {
	 if ((((cascii > 0) && (cascii <= 255) &&
	       (utolower(player.back()->dialog[c].key) == utolower((cascii)))) ||
	      ((!cascii) && (player.back()->dialog[c].key == (cscan<<8)))) &&
	     (!(player.back()->dialog[c].flags & (D_HIDDEN | D_DISABLED)))) {
	    MESSAGE(c, MSG_KEY, ccombo);
	    goto getout;
	 }
      }

      /* broadcast in case any other objects want it */
      for (c=0; player.back()->dialog[c].proc; c++) {
	 if (!(player.back()->dialog[c].flags & (D_HIDDEN | D_DISABLED))) {
	    MESSAGE(c, MSG_XCHAR, ccombo);
	    if (player.back()->res & D_USED_CHAR)
	       goto getout;
	 }
      }

      /* pass <CR> or <SPACE> to selected object */
      if (key[ACTION_KEY] && (player.back()->focus_obj >= 0)) {
		 MESSAGE(player.back()->focus_obj, MSG_KEY, ccombo);
		 goto getout;
      }

      /* ESC closes dialog */
      if (key[INGAME_MENU_KEY]) {
	 player.back()->res |= D_CLOSE;
	 player.back()->obj = -1;
	 goto getout;
      }

      /* move focus around the dialog */
      player.back()->res |= move_focus(player.back()->dialog, cascii, cscan, &player.back()->focus_obj);
   }

   /* send idle messages */
   player.back()->res |= dialog_message(player.back()->dialog, MSG_IDLE, 0, &player.back()->obj);

   getout:

   ret = (!(player.back()->res & D_CLOSE));
   player.back()->res &= ~D_CLOSE;
   return ret;
}

