CC =      g++
CXXFLAGS = -O2 -march=native -pipe
#LDFLAGS	= `allegro-config --libs`
LDFLAGS	= -L/usr/lib64 -Wl,--export-dynamic -lalld-4.2.2 -lalleg_unsharable

OBJ =     main.o game.o map.o menu.o object.o sprite.o
BIN =     tiles

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.cc
	$(CC) $(CXXFLAGS) -c $<

clean:
	rm -rf $(BIN) $(OBJ)
