#  Copyright 2009 Thomas Witte
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.


CC =      g++
CXXFLAGS = -O2 -march=native -pipe
LDFLAGS	= `allegro-config --libs`

OBJ =     main.o game.o map.o menu.o object.o sprite.o fight.o
BIN =     tiles

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.cc
	$(CC) $(CXXFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ)
