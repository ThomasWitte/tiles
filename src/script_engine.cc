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

#include "iohelper.h"
#include "script_engine.h"

//ScriptEngine ==================================================================

lua_State *ScriptEngine::L = NULL;

ScriptEngine::ScriptEngine() {
	if(!L) {
		L = luaL_newstate();
		luaL_openlibs(L);
	}
}

ScriptEngine::~ScriptEngine() {
	if(L) {
		lua_close(L);
		L = NULL;
	}
}

bool ScriptEngine::do_string(std::string s) {
	if(luaL_dostring(L, s.c_str()) == 0)
		return true;
	else {
		MSG(Log::WARN, "ScriptEngine", "Fehler beim Ausführen von \"" + s + "\"\n" + lua_tostring(L, -1));
		return false;
	}
}

bool ScriptEngine::do_file(std::string filename) {
	if(luaL_dofile(L, filename.c_str()) == 0)
		return true;
	else {
		MSG(Log::WARN, "ScriptEngine", "Fehler beim Ausführen von \"" + filename + "\"\n" + lua_tostring(L, -1));
		return false;
	}
}

bool ScriptEngine::compile_string(std::string s, std::string name) {
	int res = luaL_loadstring(L, s.c_str());
	if(res == 0) {
		lua_setglobal(L, name.c_str());
		return true;
	} else {
		switch(res) {
		case LUA_ERRMEM:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRMEM erhalten beim Laden von \"" + s + "\"");
		break;	

		case LUA_ERRSYNTAX:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRSYNTAX erhalten beim Laden von \"" + s + "\"");
		break;
		}
		return false;
	}
}

bool ScriptEngine::compile_file(std::string filename, std::string name) {
	int res = luaL_loadfile(L, filename.c_str());
	if(res == 0) {
		lua_setglobal(L, name.c_str());
		return true;
	} else {
		switch(res) {
		case LUA_ERRMEM:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRMEM erhalten beim Laden von \"" + filename + "\"");
		break;	

		case LUA_ERRSYNTAX:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRSYNTAX erhalten beim Laden von \"" + filename + "\"");
		break;

		case LUA_ERRFILE:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRFILE erhalten beim Laden von \"" + filename + "\"");
		break;
		}
		return false;
	}
}

bool ScriptEngine::run_chunk(std::string name) {
	switch(lua_pcall(L, 0, 0, 0)) {
		case LUA_ERRRUN:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRRUN erhalten beim Ausführen von \"" + name + "\"\n" + lua_tostring(L, -1));
		return false;

		case LUA_ERRMEM:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRMEM erhalten beim Ausführen von \"" + name + "\"\n" + lua_tostring(L, -1));
		return false;

		case LUA_ERRERR:
			MSG(Log::WARN, "ScriptEngine", "LUA_ERRERR erhalten beim Ausführen von \"" + name + "\"\n" + lua_tostring(L, -1));
		return false;
	}
	return true;
}

bool ScriptEngine::delete_chunk(std::string name) {
	lua_pushnil(L);
	lua_setglobal(L, name.c_str());
	return true;
}

void ScriptEngine::push_pointer(std::string name, void *p) {
	size_t pos = name.find('.');
	int index = LUA_GLOBALSINDEX;
	while(pos != std::string::npos) {
		if(index != LUA_GLOBALSINDEX)
			lua_remove(L, -2);
		std::string pref = name.substr(0, pos);
		name = name.substr(pos+1);

		lua_getfield(L, index, pref.c_str());
		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);
			lua_setfield(L, -2, pref.c_str());
			lua_getfield(L, index, pref.c_str());
		}

		index = -1;
		pos = name.find('.');
	}

	lua_pushlightuserdata(L, p);
	lua_setfield(L, index-1, name.c_str());
	lua_pop(L, 1);
}

void ScriptEngine::set_string(std::string key, std::string value) {
	int b = lua_gettop(L);
	key = "game.data." + key;
	size_t pos = key.find('.');
	int index = LUA_GLOBALSINDEX;
	while(pos != std::string::npos) {
		if(index != LUA_GLOBALSINDEX)
			lua_remove(L, -2);
		std::string pref = key.substr(0, pos);
		key = key.substr(pos+1);

		lua_getfield(L, index, pref.c_str());
		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);
			lua_setfield(L, -2, pref.c_str());
			lua_getfield(L, index, pref.c_str());
		}

		index = -1;
		pos = key.find('.');
	}

	lua_pushstring(L, value.c_str());
	lua_setfield(L, index-1, key.c_str());
	lua_pop(L, 1);
	MSG(Log::DEBUG, "ScriptEngine", "stack: " + to_string(lua_gettop(L)-b));
}

void ScriptEngine::set_number(std::string key, double value) {
	key = "game.data." + key;
	size_t pos = key.find('.');
	int index = LUA_GLOBALSINDEX;
	while(pos != std::string::npos) {
		if(index != LUA_GLOBALSINDEX)
			lua_remove(L, -2);
		std::string pref = key.substr(0, pos);
		key = key.substr(pos+1);

		lua_getfield(L, index, pref.c_str());
		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);
			lua_setfield(L, -2, pref.c_str());
			lua_getfield(L, index, pref.c_str());
		}

		index = -1;
		pos = key.find('.');
	}

	lua_pushnumber(L, value);
	lua_setfield(L, index-1, key.c_str());	
	lua_pop(L, 1);
}

std::string ScriptEngine::get_string(std::string key, std::string def) {
	key = "game.data." + key;
	size_t pos = key.find('.');
	int index = LUA_GLOBALSINDEX;
	while(pos != std::string::npos) {
		if(index != LUA_GLOBALSINDEX)
			lua_remove(L, -2);
		std::string pref = key.substr(0, pos);
		key = key.substr(pos+1);

		lua_getfield(L, index, pref.c_str());
		if(lua_isnil(L, -1)) {
			lua_pop(L, 2);
			return def;
		}

		index = -1;
		pos = key.find('.');
	}

	lua_getfield(L, index, key.c_str());
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2);
		return def;
	}
	std::string ret = lua_tostring(L, -1);
	lua_pop(L, 2);
	return ret;
}

double ScriptEngine::get_number(std::string key, double def) {
	key = "game.data." + key;
	size_t pos = key.find('.');
	int index = LUA_GLOBALSINDEX;
	while(pos != std::string::npos) {
		if(index != LUA_GLOBALSINDEX)
			lua_remove(L, -2);
		std::string pref = key.substr(0, pos);
		key = key.substr(pos+1);

		lua_getfield(L, index, pref.c_str());
		if(lua_isnil(L, -1)) {
			lua_pop(L, 2);
			return def;
		}

		index = -1;
		pos = key.find('.');
	}

	lua_getfield(L, index, key.c_str());
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2);
		return def;
	}
	double ret = lua_tonumber(L, -1);
	lua_pop(L, 2);
	return ret;
}

void ScriptEngine::connect(std::string name, lua_CFunction f) {
	size_t pos = name.find('.');
	int index = LUA_GLOBALSINDEX;
	while(pos != std::string::npos) {
		if(index != LUA_GLOBALSINDEX)
			lua_remove(L, -2);
		std::string pref = name.substr(0, pos);
		name = name.substr(pos+1);

		lua_getfield(L, index, pref.c_str());
		if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);
			lua_setfield(L, -2, pref.c_str());
			lua_getfield(L, index, pref.c_str());
		}

		index = -1;
		pos = name.find('.');
	}

	lua_pushcfunction(L, f);
	lua_setfield(L, index-1, name.c_str());
	lua_pop(L, 1);
}
