#include "header_my_string.h"
#include <stdio.h>

#include <string.h>

/*
the function gets 2 string and if there is a diffefent letters by the lexicographical order if ct is bigger it will return -1 if cs is bigger
it will return 1 if one strinf is bigger than the other and both have same letters it will returns -1 or 1 depends on the bigger one 
*/
int my_strcmp(const char* cs, const char* ct)
{
    int min_length, i, cs_length, ct_length;
    cs_length = strlen(cs);
    ct_length = strlen(ct);
    min_length = (cs_length < ct_length) ? cs_length : ct_length;
    for (i = 0; i < min_length; i++)
    {
        if (cs[i] < ct[i])
            return CT_IS_BIGGER;
        else if (cs[i] > ct[i])
            return CS_IS_BIGGER;
    }
    if (cs_length == ct_length)
        return CS_EQUAL_TO_CT;
    else
        return (cs_length < ct_length) ? CT_IS_BIGGER : CS_IS_BIGGER;

}

/*
This function takes 2 strings and an integer and return if in the first n letters there is a letter in cs that is before the letter in ct 
or the opposite
-1 if cs has a letter which is before ct in lexicographical order in one of the first n characters 
0 if cs and ct equal  the first n characters and n is smaller or equal to the smallest string or 1 if cs is after ct in lexicographical order the first n characters
if n is bigger than both string there are 2 options they are equal if the loop ends with out going into one of the loop ifs or they are only equal in first n characters therfore checks the lengths
*/
int my_strncmp(const char* cs, const char* ct, int n)
{
    int min_length, i, cs_length, ct_length, min_min_length;
    cs_length = strlen(cs);
    ct_length = strlen(ct);
    min_length = (cs_length < ct_length) ? cs_length : ct_length;
    min_min_length = (min_length <= n) ? min_length : n;

    for (i = 0; i < min_min_length; i++)
    {
        if (cs[i] < ct[i])
            return CT_IS_BIGGER;
        else if (cs[i] > ct[i])
            return CS_IS_BIGGER;
    }
    if (n > min_length)
        if (cs_length != ct_length)
            return (cs_length > ct_length) ? CS_IS_BIGGER : CT_IS_BIGGER;
    return CS_EQUAL_TO_CT;
}

/*
This function takes a string and a character as input, and returns the index
of the first appearence of the character c in cs.
if c isnt found in cs, the function returns -1
*/
int my_strchr(const char* cs, char c)
{
    int i, length;
    length = strlen(cs);
    for (i = 0; i < length; i++)
        if (cs[i] == c)
            return i;
    return CHAR_IS_NOT_FOUND;
}


/*
when program run the main will ask to input a number between 1 to 3
1 for my_strcmp
2 for my_strncmp
3 for my_strchr
after choosing 
*/
int main()
{	
	int UserAnswer;    
	printf("Please enter a number from 1 to 3, each number triggers different function:\n");
	printf("my_strcmp: type 1\nmy_strncmp: type 2\nmy_strchr: type 3\n"); 
	scanf("%d", &UserAnswer);
    
switch(UserAnswer)
{

	 case 1:
            {
                int result;
                char cs[MAX_LENGTH], ct[MAX_LENGTH];
                printf("Please type the parameters you want to use with strcmp(2 strings):\n");
                scanf("%s %s", cs, ct);
                result = my_strcmp(cs, ct); 
                printf("Function called: my_strcmp\n");
                printf("Parameters are: %s, %s\n", cs, ct);
                printf("The answer is: %d\n", result);
            }
		break;
          
        case 2:
            {
                int n, result;
                char cs[MAX_LENGTH], ct[MAX_LENGTH];
                printf("Please type the parameters you want to use with strncmp(2 strings and an int):\n");
                scanf("%s %s %d", cs, ct, &n);
                result = my_strncmp(cs, ct, n);
                printf("Function called: my_strncmp\n");
                printf("Parameters are: %s, %s, %d\n", cs, ct, n);
                printf("The answer is: %d\n", result);
            }
		break;
           
        case 3:
            {
                char c, cs[MAX_LENGTH];
                int result;
                printf("Please type the parameters you want to use with strchr(string and char):\n");
                scanf("%s %c", cs, &c);
                result = my_strchr(cs, c);
                printf("Function called: my_strchr\n");
                printf("Parameters are: %s, %c\n", cs, c);
                printf("The answer is: %d\n", result);
            }
            
		break;
	printf("Error: NOT A VALID NUMBER\n");
}
return 0;
}
