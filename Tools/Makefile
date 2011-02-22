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


CC = g++
CXXFLAGS = -O2 -march=native -pipe
LDFLAGS	= `allegro-config --libs`

BIN = extract_tileset mapedit

all: $(BIN)

extract_tileset: extract_tileset.o
	$(CC) $(CXXFLAGS) -o extract_tileset extract_tileset.o $(LDFLAGS)

mapedit: mapedit.o
	$(CC) $(CXXFLAGS) -c ../src/iohelper.cc
	$(CC) $(CXXFLAGS) -c ../src/tileset.cc
	$(CC) $(CXXFLAGS) -o mapedit mapedit.o iohelper.o tileset.o $(LDFLAGS)

%.o: src/%.cc
	$(CC) $(CXXFLAGS) -c $<

clean:
	rm -rf $(BIN) *.o *~
