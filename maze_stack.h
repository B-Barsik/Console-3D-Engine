#ifndef __MAZE_STACK__
#define __MAZE_STACK__

#include <iostream>
#include <stdio.h>
#include <ctime>

#include "consts.h"
//#include "queue.h"
//#include "stack.h"

#include "My_Stack.h"

#define HOLE 50 // 1 per HOLE
#define BORDER 12 // from side

int mat(int z) {
	return (z - 1) / 2;
}

struct Maze * make_map(int x, int y) {
	struct Maze * maze;
	maze = (struct Maze *)malloc(sizeof(struct Maze));
	maze->sx = x;
	maze->sy = y;

	maze->map = (int**)malloc(sizeof(int*) * maze->sy);
	maze->map[0] = (int*)malloc(sizeof(int) * maze->sx * maze->sy);
	for (int i = 1; i < maze->sy; ++i) {
		maze->map[i] = maze->map[i - 1] + maze->sx;
	}
	return maze;
}
struct Maze * create_matrix(struct Maze * maze) {
	struct Maze * matrix;
	matrix = (struct Maze *)malloc(sizeof(struct Maze));
	matrix->sx = (maze->sx - 1) / 2;
	matrix->sy = (maze->sy - 1) / 2;

	matrix->map = (int**)malloc(sizeof(int*) * matrix->sy);
	matrix->map[0] = (int*)malloc(sizeof(int) * matrix->sx * matrix->sy);
	for (int i = 1; i < matrix->sy; ++i) {
		matrix->map[i] = matrix->map[i - 1] + matrix->sx;
	}
	for (int i = 0; i < matrix->sy; ++i) {
		for (int ii = 0; ii < matrix->sx; ++ii) {
			matrix->map[i][ii] = 0;
		}
	}
	return matrix;
}
void pre_generation_sm(struct Maze * maze) {
	int x, y;
	if ((maze->sx % 2) == 1) {
		x = maze->sx;
	}
	else {
		x = maze->sx - 1;
		for (int i = 0; i < maze->sy; ++i) {
			maze->map[i][x] = -1;
		}
	}

	if ((maze->sy % 2) == 1) {
		y = maze->sy;
	}
	else {
		y = maze->sy - 1;
		for (int i = 0; i < maze->sx; ++i) {
			maze->map[y][i] = -1;
		}
	}

	for (int i = 0; i < y; ++i) {
		for (int ii = 0; ii < x; ++ii) {
			if (i % 2 != 1) {
				maze->map[i][ii] = -1;
			}
			else {
				if (ii % 2 != 1) {
					maze->map[i][ii] = -1;
				}
				else {
					maze->map[i][ii] = 0;
				}
			}
		}
	}
}
void print_maze(struct Maze * maze){
	for (int i = 0; i < maze->sy; ++i) {
		for (int ii = 0; ii < maze->sx; ++ii) {
			if (maze->map[i][ii] == 0) {
				printf("  ");
			}
			else if (maze->map[i][ii] == -2) {
				printf("II");
			}
			else if (maze->map[i][ii] == -3) {
				printf("OO");
			}
			else {
				printf("##");
			}
		}
		printf("\n");
	}
}
void save_maze(struct Maze * maze) {
	//char * s;
	//scanf("%s", s);
	
}

int is_matrix_empty(struct Maze * matrix) {
	for (int i = 0; i < matrix->sy; ++i) {
		for (int ii = 0; ii < matrix->sx; ++ii) {
			if (matrix->map[i][ii] == 0) {
				return 0;
			}
		}
	}
	return 1;
}
void create_input(struct Maze * matrix, struct Stack * stack, struct Maze * maze, int x, int y) {
	int x2 = (x - 1) / 2;
	int y2 = (y - 1) / 2;
	struct Point inp;
	if (x >=0 && y >= 0 ) {
		if (maze->sx > BORDER) {
			inp.x = ((maze->sx - 2 - (((2 * (rand() % x2)) + 1) / 4)) / 2) * 2 +1;
		}
		else {
			inp.x =(2 * (rand() % x2)) + 1;
		}
		inp.y = (rand() % y) + 1;
		maze->map[inp.y][inp.x] = 0;
		maze->map[inp.y - 1][inp.x] = -2;
		My_stack_push(stack, inp);
		matrix->map[(inp.y - 1)/2][(inp.x - 1)/2] = -2;
	}
}
int have_neighbour(struct Maze * matrix, struct Maze * maze, struct Point p, int i) {
	int x[] = {0,0,0,0,0};
	if ((p.x > 2) && (matrix->map[mat(p.y)][mat(p.x) - 1]) == 0) { //left
		x[0] = 1;
	}
	if ((p.x < maze->sx - 3) && (matrix->map[mat(p.y)][(mat(p.x) + 1)]) == 0) { //right
		x[1] = 1;
	}
	if ((p.y < maze->sy - 3) && (matrix->map[mat(p.y) + 1][mat(p.x)]) == 0) { //down
		x[2] = 1;

	}
	if ((p.y > 2) && (matrix->map[mat(p.y) - 1][mat(p.x)] == 0)) { //up
		x[3] = 1;
	}
	x[4] = x[1] + x[2] + x[3] + x[0];
	return x[i];
}
void next_point(struct Maze * matrix, struct Stack * stack, struct Maze * maze, struct Point p) {
	int x, i = 0, y = 0;
	//chose random neighbour
	x = have_neighbour(matrix, maze, My_stack_get(stack), 4);
	x = 1 + rand() % x;
	while (i != x) {
		if ((have_neighbour(matrix, maze, My_stack_get(stack), y)) == 1) {
			++i;
		}
		++y;
	} //1-left, 2-right, 3-down, 4-up
	 //delite wall and select point
	struct Point new_p;
	switch (y) {
	case 1:
		new_p.x = p.x - 2;
		new_p.y = p.y;
		maze->map[new_p.y][new_p.x + 1] = 0;
		My_stack_push(stack, new_p);
		matrix->map[(new_p.y -1) /2][(new_p.x - 1) /2] = 1;
		break;
	case 2:
		new_p.x = p.x + 2;
		new_p.y = p.y;
		maze->map[new_p.y][new_p.x - 1] = 0;
		My_stack_push(stack, new_p);
		matrix->map[(new_p.y - 1) / 2][(new_p.x - 1) / 2] = 1;
		break;
	case 3:
		new_p.x = p.x;
		new_p.y = p.y + 2;
		maze->map[new_p.y - 1][new_p.x] = 0;
		My_stack_push(stack, new_p);
		matrix->map[(new_p.y - 1) / 2][(new_p.x - 1) / 2] = 1;
		break;
	case 4:
		new_p.x = p.x;
		new_p.y = p.y - 2;
		maze->map[new_p.y + 1][new_p.x] = 0;
		My_stack_push(stack, new_p);
		matrix->map[(new_p.y - 1) / 2][(new_p.x - 1) / 2] = 1;
		break;
	}
//	printf("%d %d\n", new_p.x, new_p.y);
}
void back_point(struct Maze * maze, struct Stack * stack) {
	My_stack_pop(stack);
}
void create_output(struct Maze * maze) {
	int x = 0;
	int y = maze->sy;
	if (x >= 0 && y >= 0) {
		while (maze->map[((y - 1)/ 2) * 2 - 1][x] == -1) {
			if (maze->sx > BORDER) {
				x = rand() % (maze->sx / 4);
			}
			else {
				x = rand() % (maze->sx);
			}
		}
	}
	maze->map[((y - 1)/ 2) * 2][x] = -3;

}

void generation(struct Maze * maze) {
	struct Stack * stack;
	struct Maze * matrix;
	stack = My_stack_create(500);
	srand(time(NULL));
	matrix = create_matrix(maze);


	create_input(matrix, stack, maze, maze->sx, 1);
//	print_maze(matrix);
//	print_maze(maze);

	while ((is_matrix_empty(matrix) == 0)) {
		if (have_neighbour(matrix, maze, My_stack_get(stack), 4) != 0) {
			next_point(matrix, stack, maze, My_stack_get(stack));
//			print_maze(matrix);
//			print_maze(maze);
		}
		else if (My_stack_is_empty(stack) == 0) {
			back_point(maze, stack);
//			print_maze(matrix);
//			print_maze(maze);
		}
	};
	free(matrix);
	create_output(maze);
}
void maze_stack_hole(struct Maze * maze) {
	int hole_number = maze->sx * maze->sy / HOLE; 
	int hx, hy;
	for (int i = 0; i < hole_number; ++i) {
		hx = 1 + rand() % (maze->sx - 3);
		hy = 1 + rand() % (maze->sy - 3);
		maze->map[hy][hx] = 0;
	}
}

struct Maze * generate_stack_maze_v_1 (int x, int y) {
	struct Maze * maze;
	maze = make_map(x, y);
	pre_generation_sm(maze);
	generation(maze);

	return maze;
}

struct Maze * generate_stack_maze_v_2(int x, int y) {
	struct Maze * maze;
	maze = make_map(x, y);
	pre_generation_sm(maze);
	generation(maze);
	maze_stack_hole(maze);
	return maze;
}

#endif //__MAZE_STACK__