my_set: my_set.o myfunc.o
	gcc -g -Wall -ansi -pedantic my_set.o myfunc.o -o my_set
my_set.o: my_set.c header.h
	gcc -c -ansi -Wall -pedantic my_set.c -o my_set.o
myfunc.o: myfunc.c header.h
	gcc -c -ansi -Wall -pedantic myfunc.c -o myfunc.o
