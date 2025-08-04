#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define BUFSIZE 100
#define ENLARGE_SIZE(alloclen) (sizeof(int)*alloclen)
#define UnitAlloc 1

void ungetch(int);
int getch(void);
int get_set(int **p);
int isDuplicate(int *ptr, int alloclen, int c);
void set_print(int alloclen, int *address);
int IsNotValid(int num);

