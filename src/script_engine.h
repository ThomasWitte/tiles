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
#include <deque>
#include <map>

using namespace std;

class Scriptable {
	public:
		Scriptable();
		~Scriptable();

		//skript laden (liefert false bei fehlern)
		virtual bool set_script(string s);

		//run liefert false, wenn Skript endet
		virtual bool run();

	protected:

		class Argument {
			public:
				virtual string value() = 0;
				virtual string debug() = 0;
		};

		class StringArgument : public Argument {
			public:
				StringArgument(string value) {
					val = value;
				}

				string value() {
					return val;
				}

				string debug() {
					return "(String)" + val;
				}
			private:
				string val;
		};

		class VarArgument : public Argument {
			public:
				VarArgument(Scriptable *p, string value) {
					parent = p;
					var = value;
				}

				string value() {
					return parent->vars[var];
				}

				string debug() {
					return "(Var:" + var + ")" + value();
				}
			private:
				Scriptable *parent;
				string var;
		};

		class ScriptNode {
			public:
				ScriptNode(Scriptable *p = NULL);
				virtual ~ScriptNode() {};
				virtual ScriptNode *exec() = 0;
				virtual void set_next(ScriptNode *n) = 0;
				bool delete_mark;
			protected:
				Scriptable *parent;
		};

		class ScriptSwitch : public ScriptNode {
			public:
				ScriptSwitch(char op, Argument *arg1, Argument *arg2, Scriptable *p);
				~ScriptSwitch();
				ScriptNode *exec();
				void set_next(ScriptNode *n);
			protected:
				ScriptNode *next[2];
				Argument *arg[2];
				char op;
		};

		class ScriptInstruction : public ScriptNode {
			public:
				ScriptInstruction(
					string (Scriptable::*instruction)(deque<Argument*> &args),
					deque<Argument*> arguments,
					string rv,
					Scriptable *p);
				~ScriptInstruction();
				void set_next(ScriptNode *n);
				ScriptNode *exec();
			protected:
				ScriptNode *next;
				string return_var;
				string (Scriptable::*instruction)(deque<Argument*> &args);
				deque<Argument*> arguments;
		};

		class NoOp : public ScriptNode {
			public:
				NoOp() {
					next = NULL;
				}
				~NoOp() {
					delete_mark = true;
					if(next && !next->delete_mark)
						delete next;
				}
				ScriptNode *exec() {
					return next;
				}
				void set_next(ScriptNode *n) {
					next = n;
				}
			protected:
				ScriptNode *next;
		};

		struct Funktion {
			string(Scriptable::*func)(deque<Argument*>&);
			int argc;
		};

		//verbindet string mit Methodenaufruf
		virtual void connect(string name, string (Scriptable::*func)(deque<Argument*> &args), int argc);

		map<string,Funktion> assoc_list;
		map<string,string> vars;

		ScriptNode *start, *position;
		bool script_is_running;

		//Vordefinierte Funktionen
		//wait unterbricht Skript (wird beim nächsten run() fortgesetzt)
		string wait_func(deque<Argument*> &args);
		string return_func(deque<Argument*> &args);
		string set_func(deque<Argument*> &args);

	private:
		void remove_ws(string &s);
		ScriptNode *create_instruction(string current);
		ScriptNode *process_chunk(string s, ScriptNode *start);
};

#endif
