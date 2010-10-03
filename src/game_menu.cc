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
	dialog.push_back(new DIALOG[10]);
	DIALOG menu[] =
	{
	   /* (proc)        (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                    (dp2) (dp3) */
	   { d_box_proc, 0, 0,  320, 240,  0,   makecol(128,128,128),   0,    0,      0,   0,   NULL,				     NULL, NULL },
	   { d_button_proc, 56, 132, 212, 28,  makecol(0,0,0),   makecol(255,255,255),   0,    D_EXIT, 0,   0,   (void*)"Spiel starten", NULL, NULL },
	   { d_button_proc, 56, 176, 212, 28,  makecol(0,0,0),   makecol(255,255,255),   0,    D_EXIT, 0,   0,   (void*)"Beenden",       NULL, NULL },
	   { NULL,          0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                   NULL, NULL }
	};
	memcpy(dialog[0], menu, 4*sizeof(DIALOG));
	player.push_back(init_dialog(dialog[0], -1));
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
	if(!update_game_menu(player.back())) {
		if(player.back()->obj == -1)
			return 0; //zurück zur map
		else {
			//neuen Dialog öffnen?!
		}
	}
	return 1;
}

int GameMenu::update_game_menu(DIALOG_PLAYER *player)
{
   int c, cascii, cscan, ccombo, r, ret, nowhere, z;
   ASSERT(player);

   /* need to give the input focus to someone? */
   if (player->res & D_WANTFOCUS) {
      player->res ^= D_WANTFOCUS;
      player->res |= offer_focus(player->dialog, player->obj, &player->focus_obj, FALSE);
   }

   /* deal with keyboard input */
   if ((cascii) || (cscan) || (keypressed())) {
      if ((!cascii) && (!cscan))
	 cascii = ureadkey(&cscan);

      ccombo = (cscan<<8) | ((cascii <= 255) ? cascii : '^');

      /* let object deal with the key */
      if (player->focus_obj >= 0) {
	 object_message(&dialog.back()[player->focus_obj], MSG_CHAR, ccombo);
	 if (player->res & (D_USED_CHAR | D_CLOSE))
	    goto getout;

	 object_message(&dialog.back()[player->focus_obj], MSG_UCHAR, cascii);
	 if (player->res & (D_USED_CHAR | D_CLOSE))
	    goto getout;
      }

      /* keyboard shortcut? */
      for (c=0; player->dialog[c].proc; c++) {
	 if ((((cascii > 0) && (cascii <= 255) &&
	       (utolower(player->dialog[c].key) == utolower((cascii)))) ||
	      ((!cascii) && (player->dialog[c].key == (cscan<<8)))) &&
	     (!(player->dialog[c].flags & (D_HIDDEN | D_DISABLED)))) {
	    object_message(&dialog.back()[c], MSG_KEY, ccombo);
	    goto getout;
	 }
      }

      /* broadcast in case any other objects want it */
      for (c=0; player->dialog[c].proc; c++) {
	 if (!(player->dialog[c].flags & (D_HIDDEN | D_DISABLED))) {
	    object_message(&dialog.back()[c], MSG_XCHAR, ccombo);
	    if (player->res & D_USED_CHAR)
	       goto getout;
	 }
      }

      /* pass <CR> or <SPACE> to selected object */
      if (key[ACTION_KEY] &&
	  (player->focus_obj >= 0)) {
	 object_message(&dialog.back()[player->focus_obj], MSG_KEY, ccombo);
	 goto getout;
      }

      /* ESC closes dialog */
      if (key[INGAME_MENU_KEY]) {
	 player->res |= D_CLOSE;
	 player->obj = -1;
	 goto getout;
      }

      /* move focus around the dialog */
      //player->res |= move_focus(player->dialog, cascii, cscan, &player->focus_obj);
   }

   /* send idle messages */
   player->res |= dialog_message(player->dialog, MSG_IDLE, 0, &player->obj);

   getout:

   ret = (!(player->res & D_CLOSE));
   player->res &= ~D_CLOSE;
   return ret;
}

