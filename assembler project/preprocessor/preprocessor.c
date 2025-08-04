#include "preprocessor.h"
#include "../vector/vector.h"
#include "../trie/trie.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdbool.h>


#define SPACE(s) while(*s && isspace(*s)) s++;
#define UNSPACE(s,base) while(*s && isspace(*s) && base != s) s++;
#define as_end ".as"
#define am_end ".am"
#define Max_Mcro_Length 31
#define Max_Line_Length 81
#define SPACE_CHARS " \t\n\f\r\v"

struct macro {
    char name[Max_Mcro_Length + 1];
    Vector lines;
};

/**
 * @brief saving place in our memory for a vector.
 * 
 * @param copy - the variable we wan to give memory
 * @return void* - the variable with it's memory
 */
static void *line_copy(const void *copy) {
    const char *line = copy;
    return strcpy(malloc(strlen(line) + 1), line);
}

/**
 * @brief freeing memory
 * 
 * @param something - the memory section we need to set free.
 */
static void line_destroyer(void *something) {
    free(something);
}

/**
 * @brief freeing memory
 * 
 * @param item - the memory section we need to set free.
 */
static void macro_dtor(void *item) {
    struct macro *macro = item;
    vector_destroy(&macro->lines);
    free((void *) macro);
}

/**
 * @brief saving place in our memory for a vector.
 * 
 * @param copy - the variable we wan to give memory
 * @return void* - the variable with it's memory
 */
void *macro_copy(const void *copy) {
    const struct macro *copy1 = copy;
    struct macro *macro = malloc(sizeof(struct macro));
    strcpy(macro->name, copy1->name);
    macro->lines = new_vector(line_copy, line_destroyer);
    return macro;
}

/**
 * @brief desribing the line- we are looking for macros, end marcos to open macros.
 * 
 * @param line - the line we're checking it's type.
 * @param macro - a struct that holds the name of the macro and represent the macro.
 * @param macro_table - a dictionary with all of our macros so far.
 * @param macro_search - a macro that saving all macro's names and their details.
 * @return enum line_roll - returning the type of the line- macro, empty, comment or a normal command.
 */
enum line_roll line_meanings(char *line,struct macro **macro,const Trie macro_table, Vector macro_search){
    char *temp;
    char *ptr;
    /*char *copy_line = malloc(Max_Line_Length);*/
    struct macro new_macro = {0};
    struct macro * local;
    ptr = strchr(line,';');
    if(ptr)
        *ptr = '\0';
    SPACE(line);
    if(*line == '\0')
        return empty;
    ptr = strstr(line,"endmcro");
    if(ptr)
    {
        temp = ptr;
        UNSPACE(temp,line);
        if(temp != line)
        {
            return bad_macro_ending;
        }
        ptr=ptr+7;
        SPACE(ptr);
        if(*ptr != '\0')
        {
            return bad_macro_ending;
        }
        *macro = NULL; 
        return macro_end;
    }
    ptr = strstr(line,"mcro");
    if(ptr)
    {
        temp = ptr;
        UNSPACE(temp,line);
        if(temp != line)
        {
            return bad_definition;
        }
        ptr = ptr+4;
        SPACE(ptr);
        line = ptr;
        ptr = strpbrk(line,SPACE_CHARS);
        if(ptr){
            *ptr = '\0';
            ptr++;
            SPACE(ptr);
            if(*ptr != '\0')
                return bad_definition;
        }
    *macro = trie_exists(macro_table,line);
    if(*macro)
        return exists;
    strcpy(new_macro.name,line);
    *macro = vector_insert(macro_search, &new_macro);
    trie_insert(macro_table, line, (*macro));
    return macro_start;
    }
    ptr = strpbrk(line, SPACE_CHARS);
    if(ptr){
        *ptr = '\0';
    }
    local = trie_exists(macro_table, line);
    if(local == NULL){
        *ptr = ' ';
        return other;
    }
    if(ptr){
        ptr++;
        SPACE(ptr);
        if(*ptr != '\0'){
            return bad_macro_call;
        }
    }
    *macro = local;
    return macro_call;
    
}


/**
 * @brief the program will create an 'am' file. in short- the preprocessor meant to 
 * open macros, remove null lines and comments and the rest of the code will be copied do the am file.
 * 
 * @param file - the name of the 'as' file we want to convert to 'am' file.
 * @return const char* - the name of the 'am' file we created by the preprocessor.
 */
const char *preprocessor(const char *file) {
    size_t file_length;
    enum line_roll mpld;
    char line_buff[Max_Line_Length] = {0};
    char *as_file_name;
    char *am_file_name;
    struct macro *find_macro = NULL;
    Vector macro_vector = NULL;
    Trie macro_search = NULL;
    void *const *begin;
    void *const *end;
    FILE *am;
    FILE *as;
    int line_count = 1;
    bool inside_macro = false;

    file_length = strlen(file);
    as_file_name = malloc(file_length + strlen(as_end) + 1);
    am_file_name = malloc(file_length + strlen(am_end) + 1);
    as_file_name = strcat(strcpy(as_file_name, file), as_end);
    am_file_name = strcat(strcpy(am_file_name, file), am_end);

    as = fopen(as_file_name, "r");
    if (as == NULL) {
        free(as_file_name);
        free(am_file_name);
        return NULL;
    }
    am = fopen(am_file_name, "w");
    if (am == NULL) {
        free(as_file_name);
        free(am_file_name);
        return NULL;
    }

    macro_vector = new_vector(macro_copy, macro_dtor);
    macro_search = trie();

    while (fgets(line_buff, sizeof(line_buff), as)) {
        switch (mpld = line_meanings(line_buff, &find_macro, macro_search, macro_vector)) {
            case empty:
            break;
            case macro_start:
                inside_macro = true;
                break;
            case macro_end:
                inside_macro = false;
                break;
            case macro_call:
                VECTOR_FOR_EACH(begin, end, find_macro->lines) {
                    if (*begin) {
                        fputs((const char *) (*begin), am);
                    }
                }
                find_macro = NULL;
                break;
            case other:
                if (inside_macro && find_macro) {
                    vector_insert(find_macro->lines, &line_buff[0]);
                } else {
                    fputs(line_buff, am);
                }
                break;
            case exists:
                /*ERROR....*/
            break;
            case bad_macro_ending:
                /*ERROR....*/
            break;
            case bad_macro_call:
                /*ERROR....*/
            break;
            case bad_definition:
                /*ERROR....*/
            break;
        }
        line_count++;
    }
   vector_destroy(&macro_vector);
    trie_destroy(&macro_search);
    free(as_file_name);
    fclose(as);
    fclose(am);
    return am_file_name;
}