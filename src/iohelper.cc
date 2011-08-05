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

Log *Log::l = NULL;

#ifndef SHOW_DEBUG_MSG
template <>
inline void Log::msg<Log::DEBUG>(std::string sender, std::string message) {}
#endif

#ifndef SHOW_INFO_MSG
template <>
inline void Log::msg<Log::INFO>(std::string sender, std::string message) {}
#endif

#ifndef SHOW_WARN_MSG
template <>
inline void Log::msg<Log::WARN>(std::string sender, std::string message) {}
#endif

#ifndef SHOW_ERROR_MSG
template <>
inline void Log::msg<Log::ERROR>(std::string sender, std::string message) {}
#endif

char* tochar(std::string s) {
	char* ret = new char[s.length()+1];
	sprintf(ret, "%s", s.c_str());
	return ret;
}

FileParser::FileParser() {
}

FileParser::FileParser(std::string dateiname, std::string type) {
	laden(dateiname, type);
}

void FileParser::laden(std::string dateiname, std::string type) {
	MSG(Log::INFO, "FileParser", "\"" + dateiname + "\" wird gelesen.");
	daten.resize(0);
	std::ifstream file;
	std::string s, cursection = "";
	file.open(dateiname.c_str(), std::ios_base::in);
	if(!file) {
		MSG(Log::ERROR, "FileParser", "Datei \"" + dateiname + "\" konnte nicht geöffnet werden.");
		return;
	}

	file >> s; // <

	if(!(file >> s)) {
		MSG(Log::ERROR, "FileParser", "Unerwartetes Dateiende in \"" + dateiname + "\".");
		return;
	}
	if(s != TILES_NAME) {
		MSG(Log::ERROR, "FileParser", "Falsche Anwendung angegeben in \"" + dateiname + "\".");
		return;
	}
	if(!(file >> s)) {
		MSG(Log::ERROR, "FileParser::laden", "Unerwartetes Dateiende in \"" + dateiname + "\".");
		return;
	}
	if(atof(s.c_str()) > TILES_VERSION) {
		MSG(Log::ERROR, "FileParser", "Datei \"" + dateiname + "\" stammt aus einer neueren Version.");
		return;
	} else if(atof(s.c_str()) < TILES_VERSION) {
		MSG(Log::WARN, "FileParser", "Datei \"" + dateiname + "\" stammt aus einer neueren Version.");
	}
	if(!(file >> s)) {
		MSG(Log::ERROR, "FileParser", "Unerwartetes Dateiende in \"" + dateiname + "\".");
		return;
	}
	if(s != type) {
		MSG(Log::ERROR, "FileParser", "Datei \"" + dateiname + "\" ist kein " + type + ".");
		return;
	}
	if(!(file >> s)) {
		MSG(Log::ERROR, "FileParser", "Unerwartetes Dateiende in \"" + dateiname + "\".");
		return;
	}
	if(s != ">") {
		MSG(Log::ERROR, "FileParser", "Beschädigter Dateiheader in \"" + dateiname + "\".");
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
					MSG(Log::WARN, "FileParser", "Unerwartetes Dateiende in \"" + dateiname + "\".");
					break;
				}
			}
		} else {
			std::deque<std::string> temp;
			temp.push_back(cursection);

			if(s == ";;")
				MSG(Log::WARN, "FileParser", "Leere Anweisung oder beschädigte Datei \"" + dateiname + "\".");

			while(s != ";;") {
				temp.push_back(s);

				if(s.at(0) == '[' && s.at(s.length()-1) == ']') {
					MSG(Log::WARN, "FileParser", "Datei \"" + dateiname + "\" scheint beschädigt zu sein.");
				}

				if(!(file >> s)) {
					MSG(Log::WARN, "FileParser", "Unerwartetes Dateiende in \"" + dateiname + "\".");
					break;
				}
			}
			daten.push_back(temp);
		}
	}
	file.close();
}

void FileParser::dump() {
	MSG(Log::DEBUG, "FileParser", "Filedump:");
	for(unsigned int i = 0; i < daten.size(); i++) {
		std::string s;
		for(unsigned int j = 0; j < daten[i].size(); j++)
			s = s + daten[i][j] + " ";
		MSG(Log::DEBUG, ">", s);
	}
}

FileParser::~FileParser() {
}

std::string FileParser::getstring(std::string section, std::string element, std::string def) {
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element)
			return daten[i][2];

	MSG(Log::DEBUG, "FileParser", "Konnte String \"" + section + "/" + element + "\" nicht finden.");
	return def;
}

double FileParser::getvalue(std::string section, std::string element, double def) {
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element)
			return atof(daten[i][2].c_str());

	MSG(Log::DEBUG, "FileParser", "Konnte Double \"" + section + "/" + element + "\" nicht finden.");
	return def;
}

std::deque<std::string> FileParser::get(std::string section, std::string element) {
	std::deque<std::string> temp;
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element) {
			temp = daten[i];
			temp.pop_front(); //section und element entfernen
			temp.pop_front();
			return temp;
		}

	MSG(Log::DEBUG, "FileParser", "Konnte Wert \"" + section + "/" + element + "\" nicht finden.");
	return temp;
}

std::deque<std::deque<std::string> > FileParser::getall(std::string section, std::string element) {
	std::deque<std::deque<std::string> > temp;
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section && daten[i][1] == element) {
			temp.push_back(daten[i]);
			temp[temp.size()-1].pop_front();
			temp[temp.size()-1].pop_front();
		}
	return temp;
}

std::deque<std::deque<std::string> > FileParser::getsection(std::string section) {
	std::deque<std::deque<std::string> > temp;
	for(unsigned int i = 0; i < daten.size(); i++)
		if(daten[i][0] == section) {
			temp.push_back(daten[i]);
			temp[temp.size()-1].pop_front();
		}
	return temp;
}

//Nicht schön, aber funktioniert erstmal
//Durch die Leerzeichen versuchen ich den Originalstring so gut wie möglich wieder aufzubauen
std::string FileParser::getsection_raw(std::string section) {
	std::deque<std::deque<std::string> > temp = getsection(section);
	std::string ret;
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
			MSG(Log::WARN, "ImageLoader", "\"" + imgs[i].name + "\" geladen, aber nicht freigegeben [" + to_string(imgs[i].count) + "].");
		}
	}
}

BITMAP* ImageLoader::load(std::string name) {
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

void ImageLoader::destroy(std::string name) {
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

void ImageLoader::destroy_all(std::string name) {
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
	MSG(Log::DEBUG, "ImageLoader", "Entferne " + to_string(size) + " von " + to_string(data_size) + "kB aus dem Cache.");
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
	MSG(Log::DEBUG, "ImageLoader", to_string(deleted) + "kB entfernt.");
}

