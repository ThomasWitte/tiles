/*  Copyright 2009-2011 Thomas Witte

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

#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include <string>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#define SE_CONNECTABLE_METHOD(X, NAME) \
	inline int NAME(lua_State *L) {		\
		return X(L);				\
	}

class ScriptEngine {
	public:
		static ScriptEngine& get_engine() {
			static ScriptEngine engine;
			return engine;
		}

		static void connect_all();

		//skript laden (liefert false bei fehlern)
		bool do_string(std::string s);
		bool do_file(std::string file);
		bool compile_string(std::string s, std::string name);
		bool compile_file(std::string filename, std::string name);
		bool run_chunk(std::string name);
		bool delete_chunk(std::string name);

		void push_pointer(std::string name, void *p);
		void set_string(std::string key, std::string value);
		void set_number(std::string key, double value);
		std::string get_string(std::string key, std::string def = "");
		double get_number(std::string key, double def = 0.0);
	private:
		ScriptEngine();
		~ScriptEngine();
		void connect(std::string name, lua_CFunction f);

		static lua_State *L;
};

#endif
