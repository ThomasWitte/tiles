CC =      g++
LDFLAGS	= `allegro-config --libs`

OBJ =     main.o game.o map.o menu.o
BIN =     tiles

all: $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.cc
	$(CC) -c $<

clean:
	rm -rf $(BIN) $(OBJ)
