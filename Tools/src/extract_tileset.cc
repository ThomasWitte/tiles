#include <allegro.h>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void help() {}

int calc_hash(BITMAP *bmp) {
	int col, sum = 0;
	for(int i = 0; i < bmp->w; i++)
		for(int j = 0; j < bmp->h; j++) {
			col = _getpixel32(bmp, i, j);
			//col = makecol(getr(col)/10, getg(col)/10, getb(col)/10);
			col *= (i-j);
			sum += col;
		}
	return sum;
}

void extract_tileset(string name) {
	string dir = name;
	dir.append("_tileset");
	string com = "mkdir \"";
	com += dir;
	com += "\"";
	system(com.c_str());

	allegro_init();
	set_color_depth(32);
	PALETTE pal;
	get_palette(pal);
	BITMAP *map = load_bitmap(name.c_str(), NULL);
	if(!map) return;
	ofstream tsfile, levelfile;
	string tsfilename = dir;
	tsfilename += "/";
	tsfilename += dir;
	tsfile.open(tsfilename.c_str(), ios_base::out);
	levelfile.open(name.append("_level").c_str(), ios_base::out);

	tsfile << "< tiles 1.0 Tileset >" << endl << "[Tileset]" << endl;
	levelfile << "< tiles 1.0 Level >" << endl << "[Map]" << endl;
	levelfile << "Tileset " << dir << " ;;" << endl;
	levelfile << "Size " << map->w/16 << " x " << map->h/16 << " ;;" << endl;
	levelfile << "Data" << endl;
	int sum = 0;
	int count = 0;
	BITMAP *bmp = NULL;
	bmp = create_bitmap(16,16);
	vector<int> tiledb;
	for(int i = 0; i < map->h/16; i++) {
		for(int j = 0; j < map->w/16; j++) {
			blit(map, bmp, j*16, i*16, 0, 0, 16, 16);
			sum = calc_hash(bmp);
			bool exists = false;
			for(int k = 0; k < tiledb.size(); k++) {
				if(sum == tiledb[k]) {
					levelfile << k << " ";
					exists = true;
					break;
				}
			}
			if(!exists) {
				tiledb.push_back(sum);
				char out[10];
				sprintf(out, "%i.tga", count);
				char savepath[100];
				sprintf(savepath, "%s/%s", dir.c_str(), out);
				save_tga(savepath, bmp, pal);
				tsfile << count << " " << out << " 0 ;;" << endl;
				levelfile << count << " "; 
				count++;
			}
		}
		levelfile << endl;
	}
	tsfile << "[eof]" << endl;
	tsfile.close();

	levelfile << ";;" << endl << "[Object]" << endl;
	levelfile << "sprite 5 5 DefaultSprite player ;;" << endl << "[Event]" << endl << "[eof]" << endl;
	levelfile.close();
}

int main(int argc, char **argv) {
	if(argc > 1) {
		extract_tileset(argv[1]);
	} else {
		help();
	}
	return 0;
}
