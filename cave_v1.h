#ifndef __CAVE_V1__
#define __CAVE_V1__

#include <iostream>
#include <stdio.h>
#include <time.h>

#include "consts.h"
//#include "queue.h"
//#include "stack.h"

#include "My_Stack.h"
#include "maze_stack.h"

#define PROPABLY_ALLIVE 4 // from 10
#define LIVE_STEPS 5
#define LIVE_LIMIT 3
#define DEATH_LIMIT 4

#define LIVE -1
#define DEATH 0

void maze_copy(struct Maze * maze_in, struct Maze * maze_out) {
	for (int iy = 0; iy < maze_in->sy; ++iy) {
		for (int ix = 0; ix < maze_in->sx; ++ix) {
			maze_in->map[iy][ix] = maze_out->map[iy][ix];
		}
	}
}
char * name_txt_add(char * name) {
	int lenght = strlen(name);
	char * nametxt;
	nametxt = (char *)malloc(sizeof(char) * lenght + 4);
	for (int i = 0; i < lenght; ++i) {
		nametxt[i] = name[i];
	}
	nametxt[lenght + 0] = '.';
	nametxt[lenght + 1] = 't';
	nametxt[lenght + 2] = 'x';
	nametxt[lenght + 3] = 't';
	nametxt[lenght + 4] = 0;

	return nametxt;
}

void pre_generation_cv(struct Maze * maze) {
	int p;
	for (int iy = 0; iy < maze->sy; ++iy) {
		for (int ix = 0; ix < maze->sx; ++ix) {
			p = (rand() % 10);
			if (p < PROPABLY_ALLIVE) {
				maze->map[iy][ix] = -1;
			}
			else {
				maze->map[iy][ix] = 0;
			}
		}
	}
}

int live_search(struct Maze * maze, int x, int y) {
	int live_inside = 0;
	if (x > 0) live_inside = live_inside - maze->map[y][x - 1];
	if (x < (maze->sx - 1)) live_inside = live_inside - maze->map[y][x + 1];
	if (y > 0) live_inside = live_inside - maze->map[y - 1][x];
	if (y < (maze->sy - 1)) live_inside = live_inside - maze->map[y + 1][x];
	if ((x > 0) && (y > 0)) live_inside = live_inside - maze->map[y - 1][x - 1];
	if ((x < (maze->sx - 1)) && (y > 0)) live_inside = live_inside - maze->map[y - 1][x + 1];
	if ((x > 0) && (y < (maze->sy - 1))) live_inside = live_inside - maze->map[y + 1][x - 1];
	if ((x < (maze->sx - 1)) && (y < (maze->sy - 1))) live_inside = live_inside - maze->map[y + 1][x + 1];

	return live_inside;
}
void change_status_sp(struct Maze * maze, struct Maze * maze_system, int x, int y) {
	int live_insight = live_search(maze, x, y);
	
	if (maze->map[y][x] == LIVE) {
		if (live_insight < DEATH_LIMIT) {
			maze_system->map[y][x] = DEATH;
		}
		else {
			maze_system->map[y][x] = LIVE;
		}
	}
	else {
		if (live_insight > LIVE_LIMIT) {
			maze_system->map[y][x] = LIVE;
		}
		else {
			maze_system->map[y][x] = DEATH;
		}
	}

}
void cave_live_step(struct Maze * maze, struct Maze * maze_system) {

	for (int iy = 0; iy < maze->sy; ++iy) {
		for (int ix = 0; ix < maze->sx; ++ix) {
				change_status_sp(maze, maze_system, ix, iy);
		}
	}
	maze_copy(maze, maze_system);
}



struct Maze * generate_cave_v_1(int x, int y) {
	struct Maze * maze;
	struct Maze * maze_system;
	srand(time(NULL));
	maze = make_map(x, y);
	maze_system = make_map(x, y);
	pre_generation_cv(maze);
	for (int i = 0; i < LIVE_STEPS; ++i) {
		cave_live_step(maze, maze_system);
	}




	return maze;
 }


 #endif //__CAVE_V1__