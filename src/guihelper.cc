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

#include "guihelper.h"
#include "game.h"
#include "iohelper.h"
#include "config.h"

int menu_bg_proc(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_DRAW:
			for(int i = d->y, x = 0; i < d->h+d->y; i+=d->h/80, x++) {
				rectfill(gui_get_screen(), d->x, i, d->x+d->w, i+d->h/80, makecol(x, x, 255-x));
			}
			rect(gui_get_screen(), d->x+3, d->y+3, d->x+d->w-4, d->y+d->h-4, makecol(255, 255, 255));
		break;
	}
	return D_O_K;
}

int gvar_update(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_START:
			d->dp3 = (void*)new char[50];
			sprintf((char*)d->dp3, "%s", ((Game*)d->dp)->get_var((char*)d->dp2).c_str());
		break;
		case MSG_END:
			delete [] (char*)d->dp3;
		break;
		case MSG_IDLE:
			sprintf((char*)d->dp3, "%s", ((Game*)d->dp)->get_var((char*)d->dp2).c_str());
		break;
		case MSG_DRAW:
			gui_textout_ex(gui_get_screen(), (char*)d->dp3, d->x, d->y, d->fg, d->bg, FALSE);
		break;
	}
	return D_O_K;
}

int char_select(int msg, DIALOG *d, int c) {
	BITMAP *scr = gui_get_screen();
	int offset = 0;
	string *name = NULL;
	Game *g = (Game*)d->dp;
	string chars;
	switch(msg) {
		case MSG_START:
			name = new string();
			//eigenen namen herausfinden
			chars = g->get_var("CharactersInBattle");
			for(int i = 0; i <= d->d2; i++) { //d->d2 = 0…3 im Menü
				int pos = chars.find_first_of(";");
				if(pos == string::npos) {
					//Position bleibt leer
					d->dp2 = NULL;
					d->flags |= D_DISABLED;
					break;
				}
				*name = chars.substr(0, pos);
				chars.erase(0, pos+1);
			}
			//portrait laden…
			d->dp2 = (void*)new BITMAP*[2];
			((BITMAP**)d->dp2)[0] = imageloader.load("Fights/Fighters/" + *name + "/face.tga");
			//finger
			((BITMAP**)d->dp2)[1] = imageloader.load("Images/auswahl.tga");
			d->dp3 = (void*)name;

			if(!(d->flags & D_DISABLED)) {
				if(g->get_var((*name) + ".defensive") == "true") {
					d->flags |= D_SELECTED;
				} else {
					d->flags &= ~D_SELECTED;
				}
			}
		break;
		case MSG_DRAW:
			if(!(d->flags & D_DISABLED)) { //sonst leere position…
				name = (string*)d->dp3;
				int def = 0;
				if(d->flags & D_SELECTED)
					def = 10;
				//portrait
				masked_blit(((BITMAP**)d->dp2)[0], scr, 0, 0, d->x + 3 + def, d->y,((BITMAP**)d->dp2)[0]->w, ((BITMAP**)d->dp2)[0]->h);
				//name
				gui_textout_ex(scr, g->get_var((*name) + ".name").c_str(), d->x + 60, d->y, d->fg, d->bg, FALSE);
				//level
				gui_textout_ex(scr, "LV", d->x + 65, d->y + 12, d->fg, d->bg, FALSE);
				gui_textout_ex(scr, g->get_var((*name) + ".level").c_str(), d->x + 97, d->y + 12, d->fg, d->bg, FALSE);
				//HP
				if(g->get_var((*name) + ".status9") == "suffering") { //NearFatal
					gui_textout_ex(scr, "HP      /", d->x + 65, d->y + 24, COL_YELLOW, d->bg, FALSE);
					gui_textout_ex(scr, g->get_var((*name) + ".curhp").c_str(), d->x + 97, d->y + 24, COL_YELLOW, d->bg, FALSE);
					gui_textout_ex(scr, g->get_var((*name) + ".hp").c_str(), d->x + 137, d->y + 24, COL_YELLOW, d->bg, FALSE);
				} else if(g->get_var((*name) + ".status7") == "suffering") { //Wound
					gui_textout_ex(scr, "HP      /", d->x + 65, d->y + 24, COL_RED, d->bg, FALSE);
					gui_textout_ex(scr, g->get_var((*name) + ".curhp").c_str(), d->x + 97, d->y + 24, COL_RED, d->bg, FALSE);
					gui_textout_ex(scr, g->get_var((*name) + ".hp").c_str(), d->x + 137, d->y + 24, COL_RED, d->bg, FALSE);
				} else {
					gui_textout_ex(scr, "HP      /", d->x + 65, d->y + 24, d->fg, d->bg, FALSE);
					gui_textout_ex(scr, g->get_var((*name) + ".curhp").c_str(), d->x + 97, d->y + 24, d->fg, d->bg, FALSE);
					gui_textout_ex(scr, g->get_var((*name) + ".hp").c_str(), d->x + 137, d->y + 24, d->fg, d->bg, FALSE);
				}
				//MP
				gui_textout_ex(scr, "MP      /", d->x + 65, d->y + 36, d->fg, d->bg, FALSE);
				gui_textout_ex(scr, g->get_var((*name) + ".curmp").c_str(), d->x + 97, d->y + 36, d->fg, d->bg, FALSE);
				gui_textout_ex(scr, g->get_var((*name) + ".mp").c_str(), d->x + 137, d->y + 36, d->fg, d->bg, FALSE);

				if(d->flags & D_GOTFOCUS) {
					offset = 30*d->d1/GAME_TIMER_BPS;
					if(offset < 0) offset *= -1;
					masked_blit(((BITMAP**)d->dp2)[1], scr, 0, 0, d->x-((BITMAP**)d->dp2)[1]->w-offset+10, d->y+10, ((BITMAP**)d->dp2)[1]->w, ((BITMAP**)d->dp2)[1]->h);
				}
			}
		break;
		case MSG_IDLE:
			//reihenfolge verändert?
			name = new string("empty");
			chars = g->get_var("CharactersInBattle");
			for(int i = 0; i <= d->d2; i++) { //d->d2 = 0…3 im Menü
				int pos = chars.find_first_of(";");
				if(pos == string::npos) {
					break;
				}
				*name = chars.substr(0, pos);
				chars.erase(0, pos+1);
			}
			if(*name != *(string*)d->dp3) {
				char_select(MSG_END, d, c);
				char_select(MSG_START, d, c);
			}
			delete name;

			//beweglicher finger:
			d->d1--;
			if(d->d1 < -GAME_TIMER_BPS/6)
				d->d1 = GAME_TIMER_BPS/6;
		break;
		case MSG_END:
			if(d->flags & D_SELECTED) {
				g->set_var(*((string*)d->dp3) + ".defensive", "true");
			} else {
				g->set_var(*((string*)d->dp3) + ".defensive", "false");
			}
			imageloader.destroy(((BITMAP**)d->dp2)[0]);
			imageloader.destroy(((BITMAP**)d->dp2)[1]);
			delete [] (BITMAP*)d->dp2;
			delete (string*)d->dp3;
		break;
		default:
			return d_button_proc(msg, d, c);
	}
	return D_O_K;
}

int r_box_proc(int msg, DIALOG *d, int c) {
	BITMAP *scr = gui_get_screen();
	switch(msg) {
		case MSG_DRAW:
			if(d->bg != -1) { //nicht transparent
				circlefill(scr, d->x+4, d->y+4, 4, d->bg);
				circlefill(scr, d->x+d->w-4, d->y+4, 4, d->bg);
				circlefill(scr, d->x+d->w-4, d->y+d->h-4, 4, d->bg);
				circlefill(scr, d->x+4, d->y+d->h-4, 4, d->bg);
				rectfill(scr, d->x+4, d->y, d->x+d->w-4, d->y+d->h, d->bg);
				rectfill(scr, d->x, d->y+4, d->x+d->w, d->y+d->h-4, d->bg);
			}
			if(d->fg != -1) {
				vline(scr, d->x, d->y+4, d->y+d->h-4, d->fg);
				vline(scr, d->x+d->w, d->y+4, d->y+d->h-4, d->fg);
				hline(scr, d->x+4, d->y, d->x+d->w-4, d->fg);
				hline(scr, d->x+4, d->y+d->h, d->x+d->w-4, d->fg);
				arc(scr, d->x+4, d->y+4, itofix(64), itofix(128), 4, d->fg);
				arc(scr, d->x+d->w-4, d->y+4, itofix(0), itofix(64), 4, d->fg);
				arc(scr, d->x+d->w-4, d->y+d->h-4, itofix(-64), itofix(0), 4, d->fg);
				arc(scr, d->x+4, d->y+d->h-4, itofix(128), itofix(192), 4, d->fg);
			}
		break;
		default:
			return d_box_proc(msg, d, c);
	}
	return D_O_K;
}

int ch_button(int msg, DIALOG *d, int c) {
	BITMAP *scr = gui_get_screen();
	Game *g = (Game*)d->dp;
	string chars;
	int offset;
	BITMAP *ausw = NULL, *bg = NULL;
	switch(msg) {
		case MSG_START:
			//ist an der position überhaupt ein character?
			chars = g->get_var("CharactersInBattle");
			for(int i = 0; i <= d->bg; i++) { //d->bg = 0…3 im Menü
				int pos = chars.find_first_of(";");
				if(pos == string::npos) {
					//Position bleibt leer
					d->flags |= D_DISABLED;
					break;
				}
				chars.erase(0, pos+1);
			}

			d->dp2 = (void*)new BITMAP*[2];
			((BITMAP**)d->dp2)[0] = ausw = imageloader.load("Images/auswahl.tga");
			((BITMAP**)d->dp2)[1] = bg = NULL;
			return D_O_K;
		break;
		case MSG_END:
			imageloader.destroy(((BITMAP**)d->dp2)[0]);
			imageloader.destroy(((BITMAP**)d->dp2)[1]);
			delete [] (BITMAP*)d->dp2;
			return D_O_K;
		break;
		case MSG_DRAW:
			if(!((BITMAP**)d->dp2)[1]) {
				((BITMAP**)d->dp2)[1] = bg = imageloader.create(((BITMAP**)d->dp2)[0]->w+10, ((BITMAP**)d->dp2)[0]->h);
				blit(scr, bg, d->x-((BITMAP**)d->dp2)[0]->w+5, d->y+10, 0, 0, bg->w, bg->h);
			}

			if(!(d->flags & D_DISABLED)) {
				ausw = ((BITMAP**)d->dp2)[0];
				bg = ((BITMAP**)d->dp2)[1];
				blit(bg, scr, 0, 0, d->x-ausw->w+5, d->y+10, bg->w, bg->h);
				if(d->flags & D_GOTFOCUS) {
					offset = 30*d->d1/GAME_TIMER_BPS;
					if(offset < 0) offset *= -1;
					masked_blit(ausw, scr, 0, 0, d->x-ausw->w-offset+10, d->y+10, ausw->w, ausw->h);
				}
			}
			return D_O_K;
		break;
		case MSG_KEY:
			if(d->flags & D_OPEN) {
				g->set_var((char*)d->dp3, to_string(d->bg));
			}
			return ff6_button(msg,d,c);
		break;
	}
	return ff6_button(msg,d,c);
}

int ff6_button(int msg, DIALOG *d, int c) {
	BITMAP *scr = gui_get_screen();
	int offset;
	switch(msg) {
		case MSG_START:
			d->dp2 = (void*)imageloader.load("Images/auswahl.tga");
		break;
		case MSG_END:
			imageloader.destroy((BITMAP*)d->dp2);
		break;
		case MSG_DRAW:
			if(d->flags & D_DISABLED) {
				gui_textout_ex(scr, (char*)d->dp, d->x, d->y, makecol(128,128,128), d->bg, FALSE);
			} else {
				gui_textout_ex(scr, (char*)d->dp, d->x, d->y, d->fg, d->bg, FALSE);
				if(d->flags & D_GOTFOCUS) {
					offset = 30*d->d1/GAME_TIMER_BPS;
					if(offset < 0) offset *= -1;
					masked_blit((BITMAP*)d->dp2, scr, 0, 0, d->x-((BITMAP*)d->dp2)->w-offset, d->y, ((BITMAP*)d->dp2)->w, ((BITMAP*)d->dp2)->h);
				}
			}
		break;
		case MSG_KEY:
			if(d->flags & D_OPEN)
				return D_SPAWN;
			else
				return d_button_proc(msg,d,c);
		break;
		case MSG_IDLE:
			d->d1--;
			if(d->d1 < -GAME_TIMER_BPS/6)
				d->d1 = GAME_TIMER_BPS/6;
		break;
		default:
			return d_button_proc(msg, d, c);
	}
	return D_O_K;
}

//Aus Allegro Quellcode kopiert:

#define MAX_OBJECTS     512

typedef struct OBJ_LIST
{
   int index;
   int diff;
} OBJ_LIST;

/* Weight ratio between the orthogonal direction and the main direction
   when calculating the distance for the focus algorithm. */
#define DISTANCE_RATIO  8

/* Maximum size (in bytes) of a dialog array. */
#define MAX_SIZE  0x10000  /* 64 kb */

enum axis { X_AXIS, Y_AXIS };

/* obj_list_cmp:
 *  Callback function for qsort().
 */
static int obj_list_cmp(AL_CONST void *e1, AL_CONST void *e2)
{
   return (((OBJ_LIST *)e1)->diff - ((OBJ_LIST *)e2)->diff);
}

/* cmp_tab:
 *  Comparison function for tab key movement.
 */
static int cmp_tab(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (int)((AL_CONST unsigned long)d2 - (AL_CONST unsigned long)d1);

   /* Wrap around if d2 is before d1 in the dialog array. */
   if (ret < 0)
      ret += MAX_SIZE;

   return ret;
}



/* cmp_shift_tab:
 *  Comparison function for shift+tab key movement.
 */
static int cmp_shift_tab(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (int)((AL_CONST unsigned long)d1 - (AL_CONST unsigned long)d2);

   /* Wrap around if d2 is after d1 in the dialog array. */
   if (ret < 0)
      ret += MAX_SIZE;

   return ret;
}

/* min_dist:
 *  Returns the minimum distance between dialogs 'd1' and 'd2'. 'main_axis'
 *  is taken account to give different weights to the axes in the distance
 *  formula, as well as to shift the actual position of 'd2' along the axis
 *  by the amount specified by 'bias'.
 */
static int min_dist(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2, enum axis main_axis, int bias)
{
   int x_left = d1->x - d2->x - d2->w + 1;
   int x_right = d2->x - d1->x - d1->w + 1;
   int y_top = d1->y - d2->y - d2->h + 1;
   int y_bottom = d2->y - d1->y - d1->h + 1;

   if (main_axis == X_AXIS) {
      x_left -= bias;
      x_right += bias;
      y_top *= DISTANCE_RATIO;
      y_bottom *= DISTANCE_RATIO;
   }
   else {
      x_left *= DISTANCE_RATIO;
      x_right *= DISTANCE_RATIO;
      y_top -= bias;
      y_bottom += bias;
   }

   if (x_left > 0) { /* d2 is left of d1 */
      if (y_top > 0)  /* d2 is above d1 */
         return x_left + y_top;
      else if (y_bottom > 0)  /* d2 is below d1 */
         return x_left + y_bottom;
      else  /* vertically overlapping */
         return x_left;
   }
   else if (x_right > 0) { /* d2 is right of d1 */
      if (y_top > 0)  /* d2 is above d1 */
         return x_right + y_top;
      else if (y_bottom > 0)  /* d2 is below d1 */
         return x_right + y_bottom;
      else  /* vertically overlapping */
         return x_right;
   }
   /* horizontally overlapping */
   else if (y_top > 0)  /* d2 is above d1 */
      return y_top;
   else if (y_bottom > 0)  /* d2 is below d1 */
      return y_bottom;
   else  /* overlapping */
      return 0;
}


/* cmp_right:
 *  Comparison function for right arrow key movement.
 */
static int cmp_right(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int bias;

   /* Wrap around if d2 is not fully contained in the half-plan
      delimited by d1's right edge and not containing it. */
   if (d2->x < d1->x + d1->w)
      bias = +SCREEN_W;
   else
      bias = 0;

   return min_dist(d1, d2, X_AXIS, bias);
}



/* cmp_left:
 *  Comparison function for left arrow key movement.
 */
static int cmp_left(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int bias;

   /* Wrap around if d2 is not fully contained in the half-plan
      delimited by d1's left edge and not containing it. */
   if (d2->x + d2->w > d1->x)
      bias = -SCREEN_W;
   else
      bias = 0;

   return min_dist(d1, d2, X_AXIS, bias);
}



/* cmp_down:
 *  Comparison function for down arrow key movement.
 */
static int cmp_down(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int bias;

   /* Wrap around if d2 is not fully contained in the half-plan
      delimited by d1's bottom edge and not containing it. */
   if (d2->y < d1->y + d1->h)
      bias = +SCREEN_H;
   else
      bias = 0;

   return min_dist(d1, d2, Y_AXIS, bias);
}



/* cmp_up:
 *  Comparison function for up arrow key movement.
 */
static int cmp_up(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int bias;

   /* Wrap around if d2 is not fully contained in the half-plan
      delimited by d1's top edge and not containing it. */
   if (d2->y + d2->h > d1->y)
      bias = -SCREEN_H;
   else
      bias = 0;

   return min_dist(d1, d2, Y_AXIS, bias);
}

/* move_focus:
 *  Handles arrow key and tab movement through a dialog, deciding which
 *  object should be given the input focus.
 */
int move_focus(DIALOG *d, int ascii, int scan, int *focus_obj)
{
   int (*cmp)(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2);
   OBJ_LIST obj[MAX_OBJECTS];
   int obj_count = 0;
   int fobj, c;
   int res = D_O_K;

   /* choose a comparison function */
   switch (scan) {
      case KEY_TAB:   cmp = (ascii == '\t') ? cmp_tab : cmp_shift_tab; break;
      case KEY_RIGHT: cmp = cmp_right; break;
      case KEY_LEFT:  cmp = cmp_left;  break;
      case KEY_DOWN:  cmp = cmp_down;  break;
      case KEY_UP:    cmp = cmp_up;    break;
      default:        return D_O_K;
   }

   /* fill temporary table */
   for (c=0; d[c].proc; c++) {
      if (((*focus_obj < 0) || (c != *focus_obj))
	  && !(d[c].flags & (D_DISABLED | D_HIDDEN))) {
	 obj[obj_count].index = c;
	 if (*focus_obj >= 0)
	    obj[obj_count].diff = cmp(d+*focus_obj, d+c);
	 else
	    obj[obj_count].diff = c;
	 obj_count++;
	 if (obj_count >= MAX_OBJECTS)
	    break;
      }
   }

   /* sort table */
   qsort(obj, obj_count, sizeof(OBJ_LIST), obj_list_cmp);

   /* find an object to give the focus to */
   fobj = *focus_obj;
   for (c=0; c<obj_count; c++) {
      res |= offer_focus(d, obj[c].index, focus_obj, FALSE);
      if (fobj != *focus_obj)
	 break;
   }

   return res;
}
