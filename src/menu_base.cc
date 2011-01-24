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

#include "menu_base.h"
#include "guihelper.h"

MenuBase::~MenuBase() {
	for(int i = 0; i < player.size(); i++) {
		shutdown_dialog(player[i]);
		delete [] dialog[i];
	}
}

void MenuBase::draw(BITMAP *buffer) {
	gui_set_screen(buffer);
	dialog_message(dialog.back(), MSG_DRAW, 0, NULL);
	gui_set_screen(NULL);
}

int MenuBase::update() {
	if(!update_game_menu(true)) { //D_CLOSE erhalten! true: esc schließt dialog
		if(player.size() <= 1) {
			return 0; //zurück zur map
		} else { //aktiven Dialog schließen
			delete_last_dialog();
		}
	} else if(player.back()->res & D_SPAWN) {
		player.back()->res &= ~D_SPAWN;
		int id = (player.back()->dialog + player.back()->obj)->d2;
		dialog.push_back(create_dialog(id));
		player.push_back(init_dialog(dialog.back(), 1));
	}
	return 1;
}

void MenuBase::delete_last_dialog() {
	shutdown_dialog(player.back());
	player.pop_back();
	delete [] dialog.back();
	dialog.pop_back();
}

#define MESSAGE(i, msg, c) {                       \
   r = object_message(player.back()->dialog+i, msg, c);   \
   if (r != D_O_K) {                               \
      player.back()->res |= r;                            \
      player.back()->obj = i;                             \
   }                                               \
}

int MenuBase::update_game_menu(bool esc_possible)
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
      if (key[INGAME_MENU_KEY] && esc_possible) {
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

