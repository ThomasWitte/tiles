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

#include <iostream>
using namespace std;

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
	if(!update_game_menu(true, player.back())) { //D_CLOSE erhalten! true: esc schließt dialog
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
   r = object_message(p->dialog+i, msg, c);   \
   if (r != D_O_K) {                               \
      p->res |= r;                            \
      p->obj = i;                             \
   }                                               \
}

int MenuBase::update_game_menu(bool esc_possible, DIALOG_PLAYER *p) {
   int c, cascii = 0, cscan = 0, ccombo, r, ret, nowhere, z;
   ASSERT(p);

	p->res = 0;
   /* need to give the input focus to someone? */
   if (p->res & D_WANTFOCUS) {
      p->res ^= D_WANTFOCUS;
      p->res |= offer_focus(p->dialog, p->obj, &p->focus_obj, FALSE);
   }

   /* deal with keyboard input */
   if ((cascii) || (cscan) || (keypressed())) {
      if ((!cascii) && (!cscan))
	 cascii = ureadkey(&cscan);

      ccombo = (cscan<<8) | ((cascii <= 255) ? cascii : '^');

      /* let object deal with the key */
      if (p->focus_obj >= 0) {
	 MESSAGE(p->focus_obj, MSG_CHAR, ccombo);
	 if (p->res & (D_USED_CHAR | D_CLOSE)) {
	    goto getout;
	}

	 MESSAGE(p->focus_obj, MSG_UCHAR, cascii);
	 if (p->res & (D_USED_CHAR | D_CLOSE)) {
	    goto getout;
	}
      }

      /* keyboard shortcut? */
      for (c=0; p->dialog[c].proc; c++) {
	 if ((((cascii > 0) && (cascii <= 255) &&
	       (utolower(p->dialog[c].key) == utolower((cascii)))) ||
	      ((!cascii) && (p->dialog[c].key == (cscan<<8)))) &&
	     (!(p->dialog[c].flags & (D_HIDDEN | D_DISABLED)))) {
	    MESSAGE(c, MSG_KEY, ccombo);
	    goto getout;
	 }
      }

      /* broadcast in case any other objects want it */
      for (c=0; p->dialog[c].proc; c++) {
	 if (!(p->dialog[c].flags & (D_HIDDEN | D_DISABLED))) {
	    MESSAGE(c, MSG_XCHAR, ccombo);
	    if (p->res & D_USED_CHAR) {
	       goto getout;
		}
	 }
      }

      /* pass <CR> or <SPACE> to selected object */
      if (key[ACTION_KEY] && (p->focus_obj >= 0)) {
		 MESSAGE(p->focus_obj, MSG_KEY, ccombo);
		 goto getout;
      }

      /* ESC closes dialog */
      if (key[BACK_KEY] && esc_possible) {
	 p->res |= D_CLOSE;
	 p->obj = -1;
	 goto getout;
      }

      /* move focus around the dialog */
      p->res |= move_focus(p->dialog, cascii, cscan, &p->focus_obj);
   }

   getout:

   /* send idle messages */
   p->res |= dialog_message(p->dialog, MSG_IDLE, 0, &p->obj);

   ret = (!(p->res & D_CLOSE));
   p->res &= ~D_CLOSE;
   return ret;
}

