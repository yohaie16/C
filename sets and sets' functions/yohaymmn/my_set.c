#include "header.h"

/* this function gets **p which is  a pointer to pointer and returns the length of the dynamic allocation to the main function
   the funtion builds the set by allocating a memory in which every new memory will be after the old one and hold the new number 
   the function doesnt duplicates numbers as needs to be on a set
*/
int get_set(int **p)
{
    int number;
    int *q = NULL;
    int alloclen = 0;
    while ((scanf("%d",&number) != EOF))
    {
        if (alloclen == 0)
        {
            *p = (malloc ENLARGE_SIZE(UnitAlloc));
            **p = number;
            alloclen++;
        }
        else if (isDuplicate(*p, alloclen, number))
        {
            ungetch(number);
        }
        else
        {
            q = realloc(*p, ENLARGE_SIZE(alloclen+1));
            if (q == NULL)
            {
                free(*p);
                return 0;
            }
            *p = q;
            *(*p+alloclen) = number;
            alloclen++;
        }
    }
    return alloclen;
}

/*
this function gets the length of the alocation and the address of the first number in the set and prints the set 
*/
void set_print(int alloclen, int *address)
{
    int i;
    printf("{ ");
    for (i = 0; i < alloclen - 1; i++)
    {
        printf("%d, ", *(address + i));
    }
    printf("%d", *(address + i));
    printf(" }\n");
}

/*
this main function asks for numbers from the user for the set and in return it makes the set and prints it by calling get_set and set_print 
int the end it frees the memory
*/
int main()
{
    int alloclen = 0;
    int *result = NULL; 
    printf("please enter the numbers you want to make a set out of:\n");
    alloclen = get_set(&result);
    if((int*)result == NULL && alloclen == 0)
	{	
		printf("the set is empty\n");
		return(0);	 
	}
    set_print(alloclen, result);
    free(result);
    return 0;
}
