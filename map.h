#ifndef _MAP_H
#define MAP_H

#define MAX_TILES_PER_TILESET 256

#include <allegro.h>
#include <vector>
#include <string>

using namespace std;

class BaseObject {
	protected:
		int x;
		int y;
		class Map *parent;
	public:
		BaseObject(int x, int y, Map *parent = NULL);
		virtual void get_position(int &x, int &y);
		virtual void set_position(int x, int y);
		virtual void draw(int xpos, int ypos, BITMAP *buffer);
		virtual void update();
};

class Tileset {
		BITMAP *tiles[MAX_TILES_PER_TILESET];
		int walk[MAX_TILES_PER_TILESET];
	public:
		Tileset();
		~Tileset();
		void load(string dateiname);
		int is_walkable(int index);
		int get_tilesize();
		BITMAP* get_tile(int index);
};

class Map {
	private:
		vector<BaseObject*> objects;
		int **tilemap;
		int **walkable;
		int tilesx;
		int tilesy;
		int focus;
		BITMAP *buffer;
		Tileset current_tileset;
	public:
		Map(string dateiname);
		~Map();
		bool is_walkable(int x, int y);
		void centre(int index = 0);
		void update();
		void draw();
};

#endif /* MAP_H */
