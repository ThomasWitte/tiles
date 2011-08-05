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
#include <iostream>
#include <cstdlib>

//Scriptable ==================================================================

Scriptable::Scriptable() {
	start = NULL;
	position = NULL;

	connect("wait", boost::bind(&Scriptable::wait_func, this, _1), 0);
	connect("return", boost::bind(&Scriptable::return_func, this, _1), 0);
	connect("random", boost::bind(&Scriptable::random_func, this, _1), -1);
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
			MSG(Log::ERROR, "Scriptable", "Fehlerhafte Zuweisung");
			return NULL;
		}
		args.push_back(new StringArgument(pl[1].substr(1, pl[1].length()-2)));
		return new ScriptInstruction(boost::bind(&Scriptable::set_func, this, _1), args, pl[0], this);

	} else if(aufb == "=();") {
		if(assoc_list.count(pl[1]) == 0) {
			MSG(Log::ERROR, "Scriptable", "Unbekannte Funktion");
			return NULL;
		}
		if(pl[3] != "") {
			MSG(Log::ERROR, "Scriptable", "Fehlerhafter Funktionsaufruf");
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
				MSG(Log::ERROR, "Scriptable", "Fehlerhafter Parameter");
				return NULL;
			}

			pl[2].erase(0, trenner+1);
			trenner = pl[2].find(',');
		}
		int argc = assoc_list[pl[1]].argc;
		if(argc != -1 && (int)args.size() != argc) {
			MSG(Log::ERROR, "Scriptable", "Falsche Parameterzahl");
			return NULL;
		}

		return new ScriptInstruction(assoc_list[pl[1]].func, args, pl[0], this);
	} else if(aufb == "();") {
		if(assoc_list.count(pl[0]) == 0) {
			MSG(Log::ERROR, "Scriptable", "Unbekannte Funktion");
			return false;
		}
		if(pl[2] != "") {
			MSG(Log::ERROR, "Scriptable", "Fehlerhafter Funktionsaufruf");
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
				MSG(Log::ERROR, "Scriptable", "Fehlerhafter Parameter");
				return false;
			}

			pl[1].erase(0, trenner+1);
			trenner = pl[1].find(',');
		}
		int argc = assoc_list[pl[0]].argc;
		if(argc != -1 && (int)args.size() != argc) {
			MSG(Log::ERROR, "Scriptable", "Falsche Parameterzahl");
			return false;
		}

		return new ScriptInstruction(assoc_list[pl[0]].func, args, "void", this);
	}

	MSG(Log::ERROR, "Scriptable", "Missgestaltete Anweisung");
	return NULL;
}

Scriptable::ScriptNode *Scriptable::create_switch(string bed) {
	size_t trenner = bed.find_first_of("=!><");

	if(trenner == string::npos) {
		MSG(Log::ERROR, "Scriptable", "Fehlerhafte Bedingung");
		return NULL;
	}

	Argument *a[] = {NULL, NULL};
	string as[] = {bed.substr(0,trenner), bed.substr(trenner+1)};

	for(int i = 0; i < 2; i++) {
		if(as[i][0] == '"' && as[i][as[i].length()-1] == '"') {
			a[i] = new StringArgument(as[i].substr(1, as[i].length()-2));
		} else if(as[i].find('"') == string::npos) {
			a[i] = new VarArgument(this, as[i]);
		} else {
			MSG(Log::ERROR, "Scriptable", "Fehlerhaftes Argument in Bedingung");
			return false;
		}
	}
	return new ScriptSwitch(bed[trenner], a[0], a[1], this);
}

Scriptable::ScriptNode *Scriptable::process_chunk(string s, Scriptable::ScriptNode *start) {
	if(s.empty())
		return start;

	if(s[s.length()-1] != ';') {
		MSG(Log::ERROR, "Scriptable", "Unerwartetes Blockende");
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

Scriptable::ScriptNode *Scriptable::process_script(string &s, ScriptNode *start) {
	size_t posif = s.find("if");
	size_t poswhile = s.find("while");
	size_t chend = s.find("}");

	if(chend <= posif && chend <= poswhile) {
		start = process_chunk(s.substr(0, chend), start);
		s.erase(0, (chend == string::npos ? chend : chend+1));
		return start;
	}

	//Alles bis zur Bedingung bearbeiten
	start = process_chunk(s.substr(0, (posif<poswhile ? posif : poswhile)), start);
	if(!start) return NULL;
	s.erase(0, (posif<poswhile ? posif : poswhile));
	//Es folgt eine If/While-Anweisung
	size_t condend = s.find("{");
	if(s[(posif < poswhile ? 2 : 5)] != '(' || s[condend-1] != ')') {
		MSG(Log::ERROR, "Scriptable", "Fehlerhaftes " + to_string(posif<poswhile ? "If" : "While"));
		return NULL;
	}

	if(posif < poswhile) {
		string bed = s.substr(3, condend-4);
		ScriptNode *temp = create_switch(bed);
		start->set_next(temp);
		start = temp;
		s.erase(0, condend+1);
		//true block
		ScriptNode* end_true = process_script(s, start);
		if(!end_true) return NULL;
		temp = new NoOp();
		end_true->set_next(temp);
		start->set_next(temp);
		start = temp;
	} else if(poswhile < posif) {
		string bed = s.substr(6, condend-7);
		ScriptNode *temp = create_switch(bed);
		start->set_next(temp);
		start = temp;
		s.erase(0, condend+1);
		//schleifenblock
		ScriptNode* end_true = process_script(s, start);
		if(!end_true) return NULL;
		end_true->set_next(temp);
		start = temp;
	}
	return process_script(s, start);
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
	position = process_script(s, start);
	if(!position)
		return false;
	if(!s.empty()) {
		MSG(Log::ERROR, "Scriptable", "Fehlerhafte Blockstruktur");
		return false;
	}

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
	boost::function< string (deque<Argument*>&) > func,
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

string Scriptable::random_func(deque<Argument*> &args) {
	if(args.size() == 0) {
		return to_string(random()%256);
	}
	return args[random()%args.size()]->value();
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

	if(next[0] && !next[0]->delete_mark)
		delete next[0];
	else if(next[1] && !next[1]->delete_mark)
		delete next[1];

	for(int i = 0; i < 2; i++) {
		delete arg[i];
	}
}

Scriptable::ScriptNode *Scriptable::ScriptSwitch::exec() {
	switch(op) {
		case '=':
			if(arg[0]->value() == arg[1]->value())
				return next[0];
		break;

		case '!': //!=
			if(arg[0]->value() != arg[1]->value())
				return next[0];
		break;

		case '>':
			if(atof(arg[0]->value().c_str()) > atof(arg[1]->value().c_str()))
				return next[0];
		break;

		case '<':
			if(atof(arg[0]->value().c_str()) < atof(arg[1]->value().c_str()))
				return next[0];
		break;
		default:
			MSG(Log::ERROR, "Scriptable", "Unbekannter Operator " + to_string(op) + " in\n" +
							">   " + arg[0]->debug() + to_string(op) + arg[1]->debug());
	}
	return next[1];
}

void Scriptable::ScriptSwitch::set_next(Scriptable::ScriptNode *n) {
	if(next[0])
		next[1] = n;
	else
		next[0] = n;
}

//ScriptInstruction ===========================================================

Scriptable::ScriptInstruction::ScriptInstruction(
	boost::function< string (deque<Argument*>&) > instruction,
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
	parent->vars[return_var] = instruction(arguments);
	return next;
}
