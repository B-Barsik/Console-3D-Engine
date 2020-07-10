#ifndef __CONSTS_H__
#define __CONSTS_H__

enum objT {
	EMPTY =  0,
	WALL  = -1,
	IN_    = -2,
	OUT_   = -3,
	HERO  = -4
};

enum dirT {
	STOP  =  0,
	UP    =  1,
	DOWN  = -1,
	LEFT  = -2,
	RIGHT =  2
};


struct Point {
	int x, y;
};

struct Maze {
	int sx, sy;
	int** map;
};

struct Step {
	int x, y;
	dirT dir;
};

Point get_point (struct Maze*, objT);
Point get_input (struct Maze*);
Point get_output(struct Maze*);

struct Maze* read_maze(const char*);
void write_maze(const char*, struct Maze*);
void print_maze(struct Maze*);

void draw_maze (struct Maze*);

/*
map = (int**)malloc(sizeof(int*) * sy);	
map[0] = (int*)malloc(sizeof(int) * sx * sy);
for(i = 1; i < sy; ++i)
	map[i] = map[i - 1] + sx;
*/


#endif //__CONSTS_H__
