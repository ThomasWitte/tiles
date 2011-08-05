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

#include <allegro.h>
#include <deque>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "config.h"

#define IMGLOADER ImageLoader::get_instance()
#define MSG(LVL, SENDER, STR) Log::get_log().msg<LVL>(SENDER, STR)

template <class T>
std::string to_string(T toBeConverted) {
	std::stringstream buffer;
	buffer << toBeConverted;
	return buffer.str();
}

template <class T>
T from_string(std::string Converted)
{
	std::stringstream buffer;
	T ret;
	buffer << Converted;
	buffer >> ret;
	return ret;
}

char* tochar(std::string s);

class FileParser {
	public:
		FileParser();
		FileParser(std::string dateiname, std::string type);
		void laden(std::string dateiname, std::string type);
		void dump();
		~FileParser();
		std::string getstring(std::string section, std::string element, std::string def = "");
		double getvalue(std::string section, std::string element, double def = 0.0);
		std::deque<std::string> get(std::string section, std::string element);
		std::deque<std::deque<std::string> > getall(std::string section, std::string element);
		std::deque<std::deque<std::string> > getsection(std::string section);
		std::string getsection_raw(std::string section);
	protected:
		std::deque<std::deque<std::string> > daten;
};


class ImageLoader {
	public:
		~ImageLoader();
		BITMAP* load(std::string name);
		BITMAP* create(int w, int h);
		void destroy(std::string name);
		void destroy(BITMAP *bmp);
		void destroy_all(std::string name);
		void clear();
		BITMAP* copy(BITMAP *bmp);
		static ImageLoader& get_instance() {
			static ImageLoader loader;
			return loader;
		}
	protected:
		struct Image {
			std::string name;
			BITMAP *bmp;
			int count;
		};
		std::deque<Image> imgs;
		float data_size;
		void cleanup(float size);
	private:
		ImageLoader();
};

class Log {
	public:
		static Log& get_log() {
			if(l) return *l;
			else {
				l = new Log();
				return *l;
			}
		}

		static void destroy() {
			delete l;
			l = NULL;
		}

		void set_output(std::ostream *o) {
			if(o)
				out = o;
		}

		enum {DEBUG, INFO, WARN, ERROR};
		template <int L>
		inline void msg(std::string sender, std::string message) {
			char strings[][15] = {"Debug", "Information", "Warnung", "Fehler"};
			*out << sender << ": [" << strings[L] << "] " << message << std::endl;
		}

	private:
		static Log *l;
		std::ostream *out;
		Log() {
			out = &std::cout;
		}

		~Log() {}
};

#endif
