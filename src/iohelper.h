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

#ifndef IOHELPER_H
#define IOHELPER_H

//TODO Imageloader Cache

#include <deque>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

template <class T>
string to_string(T toBeConverted) {
	stringstream buffer;
	buffer << toBeConverted;
	return buffer.str();
}

char* tochar(string s);

class FileParser {
	public:
		FileParser();
		FileParser(string dateiname, string type);
		void laden(string dateiname, string type);
		void dump();
		~FileParser();
		string getstring(string section, string element, string def = "");
		double getvalue(string section, string element, double def = 0.0);
		deque<string> get(string section, string element);
		deque<deque<string> > getall(string section, string element);
		deque<deque<string> > getsection(string section);
	protected:
		deque<deque<string> > daten;
};


class ImageLoader {
	public:
		~ImageLoader();
		BITMAP* load(string name);
		BITMAP* create(int w, int h);
		void destroy(string name);
		void destroy(BITMAP *bmp);
		void destroy_all(string name);
		void clear();
	protected:
		class Image {
			public:
			BITMAP *bmp;
			int count;
		};
		map<string, Image> imgs;
};

extern ImageLoader imageloader;

#endif
