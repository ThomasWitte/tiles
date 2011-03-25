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

#include "allegro.h"
#include "iohelper.h"
#include "config.h"
#include <cstdlib>

char* tochar(string s) {
	char* ret = new char[s.length()+1];
	sprintf(ret, "%s", s.c_str());
	return ret;
}

FileParser::FileParser() {
}

FileParser::FileParser(string dateiname, string type) {
	laden(dateiname, type);
}

void FileParser::laden(string dateiname, string type) {
	daten.resize(0);
	ifstream file;
	string s, cursection = "";
	file.open(dateiname.c_str(), ios_base::in);
	if(!file) {
		cerr << dateiname << ": [Fehler] Datei konnte nicht geöffnet werden." << endl;
		return;
	}

	file >> s; // <

	if(!(file >> s)) {
		cerr << dateiname << ": [Fehler] Unerwartetes Dateiende" << endl;
		return;
	}
	if(s != TILES_NAME) {
		cerr << dateiname << ": [Fehler] Falsche Anwendung" << endl;
		return;
	}
	if(!(file >> s)) {
		cerr << dateiname << ": [Fehler] Unerwartetes Dateiende" << endl;
		return;
	}
	if(atof(s.c_str()) > TILES_VERSION) {
		cerr << dateiname << ": [Fehler] Datei stammt aus einer neueren Spielversion" << endl;
		return;
	} else if(atof(s.c_str()) < TILES_VERSION) {
		cerr << dateiname << ": [Warnung] Datei stammt aus einer älteren Spielversion" << endl;
	}
	if(!(file >> s)) {
		cerr << dateiname << ": [Fehler] Unerwartetes Dateiende" << endl;
		return;
	}
	if(s != type) {
		cerr << dateiname << ": [Fehler] Datei ist kein " << type << endl;
		return;
	}
	if(!(file >> s)) {
		cerr << dateiname << ": [Fehler] Unerwartetes Dateiende" << endl;
		return;
	}
	if(s != ">") {
		cerr << dateiname << ": [Fehler] Beschädigter Dateiheader" << endl;
		return;
	}


	while(file >> s) {
		if(s.at(0) == '[' && s.at(s.length()-1) == ']') {
			cursection = s.substr(1, s.length()-2);
			if(cursection == "eof")
				break;
		} else if(s == "#") {
			while(s != ";;") {
				if(!(file >> s)) {
					cerr << dateiname << ": [Warnung] Unerwartetes Dateiende" << endl;
					break;
				}
			}
		} else {
			deque<string> temp;
			temp.push_back(cursection);

			if(s == ";;")
				cerr << dateiname << ": [Warnung] Leere Anweisung oder beschädigte Datei" << endl;

			while(s != ";;") {
				temp.push_back(s);

				if(s.at(0) == '[' && s.at(s.length()-1) == ']') {
					cerr << dateiname << ": [Warnung] Die Datei scheint beschädigt zu sein." << endl;
				}

				if(!(file >> s)) {
					cerr << dateiname << ": [Warnung] Unerwartetes Dateiende" << endl;
					break;
				}
			}
			daten.push_back(temp);
		}
	}
	file.close();
}

void FileParser::dump() {
	for(int i = 0; i < daten.size(); i++) {
		for(int j = 0; j < daten[i].size(); j++)
			cout << daten[i][j] << " ";
		cout << endl;
	}
}

FileParser::~FileParser() {
}

string FileParser::getstring(string section, string element, string def) {
	for(int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element)
			return daten[i][2];
	return def;
}

double FileParser::getvalue(string section, string element, double def) {
	for(int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element)
			return atof(daten[i][2].c_str());
	return def;
}

deque<string> FileParser::get(string section, string element) {
	deque<string> temp;
	for(int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element) {
			temp = daten[i];
			temp.pop_front(); //section und element entfernen
			temp.pop_front();
			return temp;
		}
	return temp;
}

deque<deque<string> > FileParser::getall(string section, string element) {
	deque<deque<string> > temp;
	for(int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element) {
			temp.push_back(daten[i]);
			temp[temp.size()-1].pop_front();
			temp[temp.size()-1].pop_front();
		}
	return temp;
}

deque<deque<string> > FileParser::getsection(string section) {
	deque<deque<string> > temp;
	for(int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section) {
			temp.push_back(daten[i]);
			temp[temp.size()-1].pop_front();
		}
	return temp;
}

ImageLoader::~ImageLoader() {
	for(map<string, Image>::iterator i = imgs.begin(); i != imgs.end(); i++) {
		if(i->second.count) {
			cout << "Imageloader [Warnung]: " << i->first << " geladen, aber nicht freigegeben." << i->second.count << endl; 
		}
	}
}

BITMAP* ImageLoader::load(string name) {
	if(imgs.find(name) == imgs.end()) {
		imgs[name].bmp = load_bitmap(name.c_str(), NULL);
		imgs[name].count = 1;
	} else {
		imgs[name].count++;
	}
	return imgs[name].bmp;
}

BITMAP* ImageLoader::create(int w, int h) {
	char name[25];
	do {
		sprintf(name, "%i", random());
	} while(imgs.find(name) != imgs.end());
	imgs[name].bmp = create_bitmap(w,h);
	imgs[name].count = 1;
	return imgs[name].bmp;
}

BITMAP* ImageLoader::copy(BITMAP *bmp) {
	if(!bmp) return NULL;

	BITMAP *ret = create(bmp->w, bmp->h);
	blit(bmp, ret, 0, 0, 0, 0, bmp->w, bmp->h);

	return ret;
}

void ImageLoader::destroy(string name) {
	if(imgs.find(name) != imgs.end()) {
		imgs[name].count--;
		if(imgs[name].count < 1) {
			if(imgs[name].bmp) destroy_bitmap(imgs[name].bmp);
			imgs.erase(imgs.find(name));
		}
	}
}

void ImageLoader::destroy(BITMAP *b) {
	for(map<string, Image>::iterator i = imgs.begin(); i != imgs.end(); i++) {
		if(i->second.bmp == b) {
			i->second.count--;

			if(i->second.count < 1) {
				if(b) destroy_bitmap(b);
				imgs.erase(i);
				b = NULL;
			}
			return;
		} 
	}
}

void ImageLoader::destroy_all(string name) {
	if(imgs.find(name) != imgs.end())
		if(imgs[name].count) destroy_bitmap(imgs[name].bmp);
	imgs.erase(imgs.find(name));
}

void ImageLoader::clear() {
	for(map<string, Image>::iterator i = imgs.begin(); i != imgs.end(); i++) {
		if(i->second.count) destroy_bitmap(i->second.bmp); 
	}
	imgs.clear();
}

ImageLoader imageloader;
