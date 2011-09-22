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

#include "script_engine.h"
#include "game.h"

SE_CONNECTABLE_METHOD(Game::luaf_load_legacy_map, luacf_load_legacy_map)
SE_CONNECTABLE_METHOD(Game::luaf_game_run, luacf_game_run)
SE_CONNECTABLE_METHOD(Game::luaf_game_cleanup, luacf_game_cleanup)
SE_CONNECTABLE_METHOD(Game::luaf_game_exec_events, luacf_game_exec_events)
SE_CONNECTABLE_METHOD(Game::luaf_game_main_menu_dlg, luacf_game_main_menu_dlg)
SE_CONNECTABLE_METHOD(Game::luaf_game_choose_savefile_dlg, luacf_game_choose_savefile_dlg)

void ScriptEngine::connect_all() {
	ScriptEngine::get_engine().connect("game._map_class.load_legacy_map", luacf_load_legacy_map);
	ScriptEngine::get_engine().connect("game.run", luacf_game_run);
	ScriptEngine::get_engine().connect("game.cleanup", luacf_game_cleanup);
	ScriptEngine::get_engine().connect("game.exec_events", luacf_game_exec_events);
	ScriptEngine::get_engine().connect("game.main_menu_dlg", luacf_game_main_menu_dlg);
	ScriptEngine::get_engine().connect("game.choose_savefile_dlg", luacf_game_choose_savefile_dlg);
}
