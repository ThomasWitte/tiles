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

#include <fstream>
#include <iostream>
#include <string>
#include "game.h"

Game::Game() : m("defaultLevel") {
}

Game::~Game() {
}

Game::Game(string spielstand) : m("defaultLevel") {
	laden(spielstand);
}

void Game::speichern(string spielstand) {
	ofstream file;
	spielstand.insert(0, "Saves/");
	file.open(spielstand.c_str(), ios_base::out);

	file << "[level]" << endl << m.get_level_name() << endl;
	file << "[userdata]" << endl;

	for(map<string, string>::iterator i = vars.begin(); i != vars.end(); i++) {
		file << "var " << i->first << " " << i->second << endl; 
	}

	file << "[eof]" << endl;

	file.close();
}

void Game::laden(string spielstand) {
	ifstream savefile;
	spielstand.insert(0, "Saves/");
	savefile.open(spielstand.c_str(), ios_base::in);

	vars.clear();

	string input, input2;
	int state = 0;
	savefile >> input;

	while(input != "[eof]") {
		if(input == "[level]") {state = 1; savefile >> input; }
		if(input == "[userdata]") {state = 2; savefile >> input; }

		switch(state) {
			case 1:
				m = Map(input);
				state = 0;
			break;

			case 2:
				if(input == "var") {
					savefile >> input >> input2;
					vars[input] = input2;
				}
			break;
		}

		savefile >> input;
	}
	
	savefile.close();	
}

void Game::update() {
	m.update();
}

void Game::draw() {
	m.draw();
}

