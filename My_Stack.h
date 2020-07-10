#ifndef __MY_STACK__
#define __MY_STACK__

#include "stdio.h"
#include "iostream"
#include <stdlib.h>
#include "consts.h"

typedef struct Point Data;

struct Stack {
	int n;
	int size;
	Data * a;
};

struct Stack * My_stack_create(int size) {

	struct Stack *a;
	a = (struct Stack*)malloc(sizeof(struct Stack));

	a->n = 0;
	a->size = size;
	a->a = (Data*)malloc(size * (sizeof(Data)));

	return a;
}
void My_stack_push(struct Stack * s, Data x) {
	Data *p;
	if (s->n == s->size) {
		p = (Data*)realloc(s->a, (s->size + 100) * sizeof(Data));
		if (p != NULL) {
			s->a = p;
			s->size = s->size + 100;

		}
	}
	s->a[s->n] = x;
	++s->n;
}
Data My_stack_pop(struct Stack * s) {
	return s->a[(s->n--) - 1];
}
Data My_stack_get(struct Stack * s) {
	return  s->a[s->n - 1];
}
void My_stack_print(struct Stack * s) {
	if (s->n == 0) {
		printf("Empty stack");
	}
	else {
		for (int i = 0; i < s->n; ++i) {
			printf("%d ", s->a[i]);
		}
	}
	printf("\n");
}
int My_stack_size(struct Stack * s) {
	return s->n;
}
void My_stack_clear(struct Stack * s) {
	s->n = 0;
}
int My_stack_is_empty(struct Stack * s) {
	return !s->n;
}
void My_stack_destroy(struct Stack * s) {
	free(s->a);
	free(s);
}

#endif //__MY_STACK__