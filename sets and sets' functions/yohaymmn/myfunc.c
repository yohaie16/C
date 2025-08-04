#include "header.h"
 char buf[BUFSIZE];
 int bufp = 0;
/*
this function gets a integer which is the last input from the user and undo it 
*/
void ungetch(int c)
{
	if(bufp>= BUFSIZE)
	{
		printf("ungetch: too many characters\n");
	}
	else
	{
		buf[bufp++] = c;
	}
}
/* this function gets a pointer which define the address of the starting point of the allocation and the length of it and a number and checks if there is a dupplication of the number if there is returns 1 which is true otherwise returns 0 which is false
*/
int isDuplicate(int *ptr, int alloclen, int c)
{
    int *i = NULL;
    for (i = ptr; i < ptr + alloclen; i++)
    {
        if (*i == c)
        {
            return 1;
        }
    }
    return 0;
}








