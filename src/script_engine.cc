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
#include <iostream>
#include <cstdlib>

//Scriptable ==================================================================

Scriptable::Scriptable() {
	start = NULL;
	position = NULL;

	connect("wait", &Scriptable::wait_func, 0);
	connect("return", &Scriptable::return_func, 0);
}

Scriptable::~Scriptable() {
	if(start) {
		delete start;
	}
}

void Scriptable::remove_ws(string &s) {
	bool quoted = false;
	for(unsigned int i = 0; i < s.length(); i++) {
		switch(s[i]) {
			case ' ':
			case '\t':
			if(!quoted)
			case '\n':
				s.erase(i--,1);
			break;
			case '"':
			if(!quoted)
				quoted = true;
			else {
				if(s[i-1] == '\\')
					s.erase(--i,1);
				else
					quoted = false;
			}
			break;
		}
	}
}

Scriptable::ScriptNode *Scriptable::create_instruction(string current) {
	string aufb;
	deque<string> pl;

	size_t p = current.find_first_of("=();");
	while(p != string::npos) { //Anweisungen zerlegen
		pl.push_back(current.substr(0,p));
		aufb += current[p];

		current.erase(0, p+1);
		p = current.find_first_of("=();");
	}

	deque<Argument*> args;

	if(aufb == "=;") {
		if(pl[1][0] != '"' || pl[1][pl[1].length()-1] != '"') {
			cout << "Scriptable: [Fehler] Fehlerhafte Zuweisung" << endl;
			return NULL;
		}
		args.push_back(new StringArgument(pl[1].substr(1, pl[1].length()-2)));
		return new ScriptInstruction(&Scriptable::set_func, args, pl[0], this);

	} else if(aufb == "=();") {
		if(assoc_list.count(pl[1]) == 0) {
			cout << "Scriptable: [Fehler] Unbekannte Funktion" << endl;
			return NULL;
		}
		if(pl[3] != "") {
			cout << "Scriptable: [Fehler] Fehlerhafter Funktionsaufruf" << endl;
			return NULL;
		}

		if(!pl[2].empty())
			pl[2] += ",";
		size_t trenner = pl[2].find(',');
		while(trenner != string::npos) {
			string a = pl[2].substr(0, trenner);
			if(a[0] == '"' && a[a.length()-1] == '"') {
				args.push_back(new StringArgument(a.substr(1, a.length()-2)));
			} else if(a.find('"') == string::npos) {
				args.push_back(new VarArgument(this, a));
			} else {
				cout << "Scriptable: [Fehler] Fehlerhafter Parameter" << endl;
				return NULL;
			}

			pl[2].erase(0, trenner+1);
			trenner = pl[2].find(',');
		}
		int argc = assoc_list[pl[1]].argc;
		if(argc != -1 && (int)args.size() != argc) {
			cout << "Scriptable: [Fehler] Falsche Parameterzahl" << endl;
			return NULL;
		}

		return new ScriptInstruction(assoc_list[pl[1]].func, args, pl[0], this);
	} else if(aufb == "();") {
		if(assoc_list.count(pl[0]) == 0) {
			cout << "Scriptable: [Fehler] Unbekannte Funktion" << endl;
			return false;
		}
		if(pl[2] != "") {
			cout << "Scriptable: [Fehler] Fehlerhafter Funktionsaufruf" << endl;
			return false;
		}

		if(!pl[1].empty())
			pl[1] += ",";
		size_t trenner = pl[1].find(',');
		while(trenner != string::npos) {
			string a = pl[1].substr(0, trenner);
			if(a[0] == '"' && a[a.length()-1] == '"') {
				args.push_back(new StringArgument(a.substr(1, a.length()-2)));
			} else if(a.find('"') == string::npos) {
				args.push_back(new VarArgument(this, a));
			} else {
				cout << "Scriptable: [Fehler] Fehlerhafter Parameter" << endl;
				return false;
			}

			pl[1].erase(0, trenner+1);
			trenner = pl[1].find(',');
		}
		int argc = assoc_list[pl[0]].argc;
		if(argc != -1 && (int)args.size() != argc) {
			cout << "Scriptable: [Fehler] Falsche Parameterzahl" << endl;
			return false;
		}

		return new ScriptInstruction(assoc_list[pl[0]].func, args, "void", this);
	}

	cout << "Scriptable: [Fehler] Missgestaltete Anweisung" << endl;
	return NULL;
}

Scriptable::ScriptNode *Scriptable::process_chunk(string s, Scriptable::ScriptNode *start) {

	if(s[s.length()-1] != ';') {
		cout << "Scriptable: [Fehler] Unerwartetes Blockende" << endl;
		return NULL;
	}

	size_t pos = s.find(';');
	while(pos != string::npos) { //String in Anweisungen zerlegen
		string current = s.substr(0, pos+1);

		//Instruction erzeugen
		ScriptNode *temp = create_instruction(current);
		if(!temp)
			return NULL;

		//Position verschieben
		if(start) {
			start->set_next(temp);
		}
		start = temp;

		s.erase(0, pos+1);
		pos = s.find(';');
	}
	return start;
}

bool Scriptable::set_script(string s) {
	//Aufräumen
	if(start)
		delete start;
	position = NULL;
	start = new NoOp();

	//Whitespaces entfernen
	remove_ws(s);

	//Block bearbeiten
	position = process_chunk(s, start);
	if(!position)
		return false;

	position = start;
	return true;
}

bool Scriptable::run() {
	script_is_running = true;
	while(script_is_running) {
		if(!position) {
			position = start;
			script_is_running = false;
			return false;
		}

		ScriptNode *opos = position;
		ScriptNode *npos = position->exec();

		//wenn die Position nicht durch eine Funktion manipuliert wurde
		if(position == opos)
			position = npos;
	}
	return true;
}

void Scriptable::connect(
	string name,
	string (Scriptable::*func)(deque<Argument*> &args),
	int argc) {

	assoc_list[name].func = func;
	assoc_list[name].argc = argc;
}

string Scriptable::wait_func(deque<Argument*> &args) {
	script_is_running = false;
	return "nil";
}

string Scriptable::return_func(deque<Argument*> &args) {
	script_is_running = false;
	position = start;
	return "nil";
}

string Scriptable::set_func(deque<Argument*> &args) {
	return args[0]->value();
}

//ScriptNode ==================================================================

Scriptable::ScriptNode::ScriptNode(Scriptable *p) {
	parent = p;
	delete_mark = false;
}

//ScriptSwitch ================================================================

Scriptable::ScriptSwitch::ScriptSwitch(
	char op,
	Argument *arg1,
	Argument *arg2,
	Scriptable *p) : 
	Scriptable::ScriptNode(p) {

	next[0] = NULL;
	next[1] = NULL;
	arg[0] = arg1;
	arg[1] = arg2;
	this->op = op;
}

Scriptable::ScriptSwitch::~ScriptSwitch() {
	delete_mark = true;
	for(int i = 0; i < 2; i++) {
		if(!next[i]->delete_mark)
			delete next[i];
		delete arg[i];
	}
}

Scriptable::ScriptNode *Scriptable::ScriptSwitch::exec() {
	switch(op) {
		case '=':
			if(arg[0]->value() == arg[1]->value())
				return next[1];
		break;

		case '!': //!=
			if(arg[0]->value() != arg[1]->value())
				return next[1];
		break;

		case '>':
			if(atof(arg[0]->value().c_str()) > atof(arg[1]->value().c_str()))
				return next[1];
		break;

		case '<':
			if(atof(arg[0]->value().c_str()) < atof(arg[1]->value().c_str()))
				return next[1];
		break;
		default:
			cout << "Scriptable: [Fehler] Unbekannter Operator " << op << " in" << endl;
			cout << "    " << arg[0]->debug() << op << arg[1]->debug() << endl;
	}
	return next[0];
}

void Scriptable::ScriptSwitch::set_next(Scriptable::ScriptNode *n) {
	if(next[0])
		next[1] = n;
	else
		next[0] = n;
}

//ScriptInstruction ===========================================================

Scriptable::ScriptInstruction::ScriptInstruction(
	string (Scriptable::*instruction)(deque<Argument*> &args),
	deque<Argument*> arguments,
	string rv,
	Scriptable *p) : 
	Scriptable::ScriptNode(p) {

	this->instruction = instruction;
	this->arguments = arguments;
	return_var = rv;
	next = NULL;
}

Scriptable::ScriptInstruction::~ScriptInstruction() {
	delete_mark = true;
	if(next && !next->delete_mark)
		delete next;
	for(unsigned i = 0; i < arguments.size(); i++)
		delete arguments[i];
}

void Scriptable::ScriptInstruction::set_next(Scriptable::ScriptNode *n) {
	next = n;
}

Scriptable::ScriptNode *Scriptable::ScriptInstruction::exec() {
	parent->vars[return_var] = (parent->*instruction)(arguments);
	return next;
}
