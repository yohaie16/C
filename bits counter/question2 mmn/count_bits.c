
#include <stdio.h>
#include <math.h>
/*
count bits gets a integer and counts the amount of turned bits(1) in the even places
*/
int countbits(unsigned long bit)
{
int counter = 0;
int place = 0;
/*
while the number is bigger than zero the loop will check if the standing on
place is 1 and if the place is even if it is the counter will sum into it
1 and go one step right else only go right in the end will return counter 
which is the amount of even place that the bit there is turned
*/
while(bit>0)
{
	if(((bit&1) && ((place%2) == 0)))
	{
		counter=counter+1;
	}
	bit>>=1;
	place = place+1;
}
return counter;
}
/*
the main is getting an input and send it to the function countbits so in the end it will print to the screen the returning value which is the sum of turned bits
*/
int main()
{
	int result;
	unsigned long input;
	printf("please enter the veraible to check his amount of turned bits\n ");
	scanf("%lu",&input);
	printf("input is: %lu\n",input);
	result = countbits(input);
	printf("the parameter is: %lu \n",input);
	printf("the amount of turned bits in the even places are %d",result);
return 0;
}
