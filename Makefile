CC =      g++
CXXFLAGS = -O2 -march=native -pipe
LDFLAGS	= `allegro-config --libs`

OBJ =     main.o game.o map.o menu.o object.o sprite.o
BIN =     tiles

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.cc
	$(CC) $(CXXFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ)
