
#include <stdio.h>
#include <stdlib.h>

struct fibonachi {
    unsigned long val;
    struct fibonachi* next;
};

void fibonachiprint(const struct fibonachi* first, const char *print,int n) {
    const struct fibonachi* i;
    FILE* fibo;
    fibo = fopen(print, "w");
	if(fibo == NULL)
	{
		printf("Failed to open the file for writing.\n");
        return;
    }
    i = first;
    fprintf(fibo, "this program gets a number(n) which define the amount of numbers n+1 and print the n+1 numbers in fibonachi sequence from highest to the first\nyou chose n = %d\n",n);
    do {
        printf("%lu\n", i->val);
        fprintf(fibo, "%lu\n", i->val);
        i = i->next;
    } while (i != first);
    fclose(fibo);
}

struct fibonachi* fibfib(int n) {
    unsigned long f1 = 0;
    unsigned long f2 = 1;
    unsigned long f3 = 1;
    struct fibonachi* first;
    struct fibonachi* cur;
    struct fibonachi* prev = NULL;
    
    first = malloc(sizeof(struct fibonachi));
	if (first == NULL)
	{
		printf("Memory allocation failed.\n");
        return 0;
	}
    cur = first;

    while (n > 0) {
        cur->val = f3;
        cur->next = prev;
        prev = cur;
        f3 = f2 + f1;
        f1 = f2;
        f2 = f3;
        cur = malloc(sizeof(struct fibonachi));
	if (cur == NULL)
	{
		printf("Memory allocation failed.\n");
        return 0;
	}
        n--;
    }

    cur->val = f3;
    cur->next = prev;
    first->next = cur;
    return cur;
}

void freef(struct fibonachi* fib) {
    struct fibonachi* n;
    struct fibonachi* i = fib;
    do {
        n = i->next;
        free(i);
        i = n;
    } while (i != fib);
}

int main() {
	struct fibonachi* test;
	int result;
	int n;
	char print[50];
	printf("Please enter a number for the Fibonacci function: ");    
	result =scanf("%d", &n);
	if (result != 1) {
        	printf("Invalid input! Please enter only one integer.\n");
		return (0);
	}
    printf("Please enter the filename to write the Fibonacci sequence: ");
    scanf("%s", print);

    test = fibfib(n);
    fibonachiprint(test, print,n);
    freef(test); 

    return 0;
}

