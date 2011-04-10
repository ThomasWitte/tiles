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
	for(unsigned int i = 0; i < daten.size(); i++) {
		for(unsigned int j = 0; j < daten[i].size(); j++)
			cout << daten[i][j] << " ";
		cout << endl;
	}
}

FileParser::~FileParser() {
}

string FileParser::getstring(string section, string element, string def) {
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element)
			return daten[i][2];
	return def;
}

double FileParser::getvalue(string section, string element, double def) {
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element)
			return atof(daten[i][2].c_str());
	return def;
}

deque<string> FileParser::get(string section, string element) {
	deque<string> temp;
	for(unsigned int i = 0; i < daten.size(); i++)
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
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element) {
			temp.push_back(daten[i]);
			temp[temp.size()-1].pop_front();
			temp[temp.size()-1].pop_front();
		}
	return temp;
}

deque<deque<string> > FileParser::getsection(string section) {
	deque<deque<string> > temp;
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section) {
			temp.push_back(daten[i]);
			temp[temp.size()-1].pop_front();
		}
	return temp;
}

//Nicht schön, aber funktioniert erstmal
//Durch die Leerzeichen versuchen ich den Originalstring so gut wie mäglich wieder aufzubauen
string FileParser::getsection_raw(string section) {
	deque<deque<string> > temp = getsection(section);
	string ret;
	size_t lines = temp.size();
	for(unsigned int i = 0; i < lines; i++) {
		size_t words = temp[i].size();
		for(unsigned int j = 0; j < words; j++) {
			if(temp[i][j] != ";;") {
				ret += temp[i][j];
				ret += " ";
			}
		}
	}
	return ret;
}

ImageLoader::ImageLoader() {
	data_size = 0;
}

ImageLoader::~ImageLoader() {
	for(unsigned int i = 0; i < imgs.size(); i++) {
		if(imgs[i].count > 0) {
			cout << "Imageloader [Warnung]: " << imgs[i].name << " geladen, aber nicht freigegeben." << imgs[i].count << endl; 
		}
	}
}

BITMAP* ImageLoader::load(string name) {
	for(unsigned int i = 0; i < imgs.size(); i++) {
		if(imgs[i].name == name) {
			imgs[i].count++;
			return imgs[i].bmp;
		}
	}

	Image img;
	img.bmp = load_bitmap(name.c_str(), NULL);
	img.count = 1;
	img.name = name;

	#ifdef ENABLE_IMAGELOADER_CACHE
	//Ich hab keine Ahnung, ob die Berechnung annährend richtig ist
	data_size += float(img.bmp->w * img.bmp->h * COLOR_DEPTH)/8000.0;
	if(data_size > IMAGELOADER_CACHESIZE) {
		cleanup(0.1*IMAGELOADER_CACHESIZE + (data_size-IMAGELOADER_CACHESIZE));
	}
	#endif

	imgs.push_back(img);
	return img.bmp;
}

BITMAP* ImageLoader::create(int w, int h) {
	Image img;

	img.name = to_string(random());
	img.bmp = create_bitmap(w,h);
	img.count = 1;

	#ifdef ENABLE_IMAGELOADER_CACHE
	data_size += float(img.bmp->w * img.bmp->h * COLOR_DEPTH)/8000.0;
	if(data_size > IMAGELOADER_CACHESIZE) {
		cleanup(0.1*IMAGELOADER_CACHESIZE + (data_size-IMAGELOADER_CACHESIZE));
	}
	#endif

	imgs.push_back(img);
	return img.bmp;
}

BITMAP* ImageLoader::copy(BITMAP *bmp) {
	if(!bmp) return NULL;

	BITMAP *ret = create(bmp->w, bmp->h);
	blit(bmp, ret, 0, 0, 0, 0, bmp->w, bmp->h);

	return ret;
}

void ImageLoader::destroy(string name) {
	for(unsigned int i = 0; i < imgs.size(); i++)
		if(imgs[i].name == name) {
			imgs[i].count--;

			#ifndef ENABLE_IMAGELOADER_CACHE
			if(imgs[i].count < 1) {
				if(imgs[i].bmp) {
					destroy_bitmap(imgs[i].bmp);
					imgs[i].bmp = NULL;
				}
				imgs.erase(imgs.begin()+i);
			}
			#endif
			return;
		}
}

void ImageLoader::destroy(BITMAP *b) {
	for(unsigned int i = 0; i < imgs.size(); i++) {
		if(imgs[i].bmp == b) {
			imgs[i].count--;

			#ifndef ENABLE_IMAGELOADER_CACHE
			if(imgs[i].count < 1) {
				if(b) destroy_bitmap(b);
				imgs[i].bmp = NULL;
				imgs.erase(imgs.begin()+i);
			}
			#endif

			b = NULL;
			return;
		}
	}
}

void ImageLoader::destroy_all(string name) {
	for(unsigned int i = 0; i < imgs.size(); i++)
		if(imgs[i].name == name) {
			if(imgs[i].count) destroy_bitmap(imgs[i].bmp);
			imgs.erase(imgs.begin()+i);
			return;
		}
}

void ImageLoader::clear() {
	for(unsigned int i = 0; i < imgs.size(); i++) {
		if(imgs[i].bmp) destroy_bitmap(imgs[i].bmp); 
	}
	imgs.resize(0);
}

void ImageLoader::cleanup(float size) {
	cout << "ImageLoader [Information]: cleaning up " << size << "/" << data_size << "KB." << endl;
	float deleted = 0.0;
	data_size = 0.0;
	for(unsigned int i = 0; i < imgs.size() ; i++) {
		if(imgs[i].count < 1 && deleted < size) {
			if(imgs[i].bmp) {
				deleted += float(imgs[i].bmp->w * imgs[i].bmp->h * COLOR_DEPTH)/8000.0;
				destroy_bitmap(imgs[i].bmp);
				imgs[i].bmp = NULL;
			}
			imgs.erase(imgs.begin() + i);
		} else {
			data_size += float(imgs[i].bmp->w * imgs[i].bmp->h * COLOR_DEPTH)/8000.0;
		}
	}
	cout << "ImageLoader [Information]: finished freeing " << deleted << "KB." << endl;
}

ImageLoader imageloader;
