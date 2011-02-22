#include <allegro.h>
#include <string>
#include <fstream>
#include <vector>
#include "../../src/iohelper.h"
#include "../../src/tileset.h"

void help() {
	std::cout << "Usage:" << std::endl;
	std::cout << "    mapedit [LEVELNAME]" << std::endl;
	std::cout << "    LEVELNAME: Name of the Level e.g. defaultLevel" << std::endl;
}

int mapedit(std::string level);

int main(int argc, char **argv) {
	if(argc > 1) {
		allegro_init();
		set_color_depth(32);
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
		install_keyboard();
		install_timer();

		return mapedit(argv[1]);
	} else {
		help();
	}

	return 0;
}

int **get_tilemap(std::string level, Tileset &ts, int &tilesx, int &tilesy) {
	FileParser parser("Levels/" + level, "Level");

	ts.load(parser.getstring("Map", "Tileset"));

	deque<std::string> size;
	size = parser.get("Map", "Size"); //Size hat die Form "tilesx x tilesy"
	tilesx = atoi(size[0].c_str());
	tilesy = atoi(size[2].c_str());

	int **tilemap = new (int*[tilesx]); //Array für Map reservieren
	for(int i = 0; i < tilesx; i++)
		tilemap[i] = new int[tilesy];

	deque<std::string> ret = parser.get("Map", "Data"); //Map einlesen
	int index = 0;
	for(int cury = 0; cury < tilesy; cury++) {
		for(int curx = 0; curx < tilesx; curx++) {
			tilemap[curx][cury] = atoi(ret[index].c_str());
			index++;
		}
	}

	return tilemap;
}

void save_tilemap(int **tilemap, int sizex, int sizey, std::string name) {
	std::ofstream levelfile;
	levelfile.open(name.c_str(), ios_base::out);

	levelfile << "< tiles 1.0 Level >" << std::endl << "[Map]" << std::endl;
	levelfile << "Tileset " << name << " ;;" << std::endl;
	levelfile << "Size " << sizex << " x " << sizey << " ;;" << std::endl;
	levelfile << "Data" << std::endl;

	for(int i = 0; i < sizey; i++) {
		for(int j = 0; j < sizex; j++) {
			levelfile << tilemap[j][i] << " ";
		}
		levelfile << std::endl;
	}

	levelfile << ";;" << std::endl << "[Object]" << std::endl;
	levelfile << "sprite 5 5 DefaultSprite player ;;" << std::endl << "[Event]" << std::endl << "[eof]" << std::endl;
	levelfile.close();
}

void save_tileset(Tileset *ts, std::string name) {
	ofstream tsfile;
	tsfile.open(name.c_str(), ios_base::out);

	tsfile << "< tiles 1.0 Tileset >" << std::endl << "[Tileset]" << std::endl;

	for(int i = 0; i < MAX_TILES_PER_TILESET; i++) {
		if(ts->get_tile(i) == NULL)
			break;
		tsfile << i << " " << to_string(i)+".tga " << (ts->is_walkable(i) ? "1" : "0") << " ;;" << std::endl;
	}

	tsfile << "[eof]" << std::endl;
	tsfile.close();
}

char postxt[50] = "";

int mapedit_proc(int msg, DIALOG *d, int c) {
	switch(msg) {
		case MSG_START:
			d->fg = 0; //posx
			d->bg = 0; //posy

			d->flags = 0; //kopierter Tileindex
		break;

		case MSG_END:
		break;

		case MSG_CHAR:
			switch(c >> 8) {
				case KEY_UP:
					if(--d->bg < 0) d->bg = 0;
				break;

				case KEY_DOWN:
					if(++d->bg >= d->d1) d->bg = d->d1-1;
				break;

				case KEY_LEFT:
					if(--d->fg < 0) d->fg = 0;
				break;

				case KEY_RIGHT:
					if(++d->fg >= d->d2) d->fg = d->d2-1;
				break;

				case KEY_C:
				{
					Tileset *ts = (Tileset*)d->dp2;
					int **tilemap = (int**)d->dp;
					d->flags = tilemap[d->fg][d->bg]*D_USER;
				}
				break;

				case KEY_V:
				{
					Tileset *ts = (Tileset*)d->dp2;
					int **tilemap = (int**)d->dp;
					tilemap[d->fg][d->bg] = d->flags/D_USER;
				}
				break;

				case KEY_W:
				{
					Tileset *ts = (Tileset*)d->dp2;
					int **tilemap = (int**)d->dp;
					ts->toggle_walkable(tilemap[d->fg][d->bg]);
				}
				break;

				case KEY_S:
				{
					Tileset *ts = (Tileset*)d->dp2;
					int **tilemap = (int**)d->dp;
					save_tileset(ts, *(string*)d->dp3 + "_ts");
					save_tilemap(tilemap, d->d1, d->d2, *(string*)d->dp3 + "_map");
				}
				break;

				case KEY_Q:
				return D_EXIT;

				default:
				return D_O_K;
			}
			sprintf(postxt, "[c: %3i][x: %3i     y: %3i]", d->flags, d->fg, d->bg);
		return D_USED_CHAR | D_REDRAW;

		case MSG_WANTFOCUS:
		return D_WANTFOCUS;

		case MSG_DRAW:
		{
			Tileset *ts = (Tileset*)d->dp2;
			int tsz = ts->get_tilesize();
			BITMAP *scr = gui_get_screen();
			int **tilemap = (int**)d->dp;
			int tileindex;

			for(int x = 0; x < d->w/(2*tsz); x++) {
				for(int y = 0; y < d->h/(2*tsz); y++) {
					if(d->d1 > x+d->fg-d->w/(4*tsz) && d->d2 > y+d->bg-d->h/(4*tsz) && y+d->bg >= d->h/(4*tsz) && x+d->fg >= d->w/(4*tsz)) {
						tileindex = tilemap[x+d->fg-d->w/(4*tsz)][y+d->bg-d->h/(4*tsz)];
						stretch_blit(ts->get_tile(tileindex), scr, 0, 0, tsz, tsz, x*tsz*2, y*tsz*2, 2*tsz, 2*tsz);
						if(!ts->is_walkable(tileindex)) {
							line(scr, x*tsz*2, y*tsz*2, (x+1)*tsz*2-1, (y+1)*tsz*2-1, makecol(0,255,0));
							line(scr, (x+1)*tsz*2-1, y*tsz*2, x*tsz*2, (y+1)*tsz*2-1, makecol(0,255,0));
						}
						gui_textout_ex(scr, to_string(tileindex).c_str(), x*tsz*2, y*tsz*2, 0, makecol(255,255,255), 0);
					} else {
						//leeres Feld zeichnen
						stretch_blit(ts->get_tile(0), scr, 0, 0, tsz, tsz, x*tsz*2, y*tsz*2, 2*tsz, 2*tsz);
					}
					if(x == d->w/(4*tsz) && y == d->h/(4*tsz))
						rect(scr, x*tsz*2, y*tsz*2, (x+1)*tsz*2-1, (y+1)*tsz*2-1, makecol(255,0,0));
				}
			}
		}
		break;
	}
	return D_O_K;
}

int mapedit(std::string level) {
	Tileset ts;
	int tilesx, tilesy, **tilemap = NULL;

	tilemap = get_tilemap(level, ts, tilesx, tilesy);

	DIALOG mapedit_dlg[] =
	{
	   /* (proc)       (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)  (dp2) (dp3) */
	   { mapedit_proc, 0,  0,   800, 590, 0,   0,   0,    D_EXIT, tilesx, tilesy, (void*)tilemap, (void*)&ts, (void*)&level },
	   { d_text_proc,  0,  591, 800, 10,  makecol(255,255,255),   0,   0,    0,      0,   0,   (void*)"[c]copy [v]paste [s]save [q]quit [w]toggle walkable", NULL, NULL },
	   { d_text_proc,  600,591, 200, 10,  makecol(255,255,255),   0,   0,    0,      0,   0,   (void*)postxt, NULL, NULL },
	   { NULL,         0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL, NULL, NULL }
	};

	do_dialog(mapedit_dlg, -1);

	if(tilemap)
		for(int i = 0; i < tilesx; i++)
			delete [] tilemap[i];
		delete [] tilemap;

	return 0;
}
