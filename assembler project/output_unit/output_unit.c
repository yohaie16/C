#include "output_unit.h"
#include "../share/share.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#define EXT_EXTENSION ".ext"
#define ENT_EXTENSION ".ent"
#define OB_EXTENSION ".OB"
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

/**
 * @brief printing into an 'ent' file what labels defined as 'entries' 
 * and what line did we use them.
 * 
 * @param ent_file_name - the name of the 'ent' file we want to write to.
 * @param symbol_table - a vector that bring me the all entries and what line did I call them.
 */
static void ent_file(const char *ent_file_name,Vector symbol_table)
{
    FILE *ent_file;
    void *const * begin;
    void *const * end;
    ent_file = fopen(ent_file_name,"w");
    if(ent_file)
    {
        VECTOR_FOR_EACH(begin,end,symbol_table){
            if(*begin)
            {
                if(((struct symbol*)(*begin))->sym_type >= symbol_entry_code){
                    fprintf(ent_file,"%s\t%u\n",((struct symbol*)(*begin))->name,((struct symbol*)(*begin))->address);
                }
            
            }
        }
        fclose(ent_file);

    }

}

/*mmn14_output_ext_file: printing into an 'ext' file what labels defined
as 'externs' labels, and what lines in the code did we use them.*/
/**
 * @brief printing into an 'ext' file what labels defined
 * as 'externs' labels, and what lines in the code did we use them.
 * 
 * @param ext_file_name - the name of the 'ext' file we want to write to.
 * @param extern_calls - a vector that bring me the all entries and what line did I call them.
 */
static void mmn14_output_ext_file(const char *ext_file_name,Vector extern_calls){
FILE *ext_file;
void* const *begin_ext;
void* const *end_ext;
void* const *begin_addr;
void* const *end_addr;
ext_file = fopen(ext_file_name,"w");
if(ext_file)
{
    VECTOR_FOR_EACH(begin_ext,end_ext,extern_calls)
    {
        if(*begin_ext){
            VECTOR_FOR_EACH(begin_addr,end_addr,((const struct extern_call*)(*begin_ext))->call_address)
            fprintf(ext_file,"%s\t%u\n",((const struct extern_call *)(*begin_ext))->name,*((unsigned int*)(*begin_addr)));
        }
    }
}
fclose(ext_file);

}

/**
 * @brief printing the way every line saved in the memory. we've been
 * told we have 12 bits, and the best way to represent it its by Base 64- that using 6 bits, so every
 * code and data in our code can be represented by 2 chars in base 64.
 * 
 * @param obj_file - the name of the 'ob' file we want to write to.
 * @param memory_section - writing there all the machine words after transfering it to base 64.
 */
static void mmn14_output_memory_section(FILE *obj_file,const Vector memory_section){
    void* const* begin;
    void* const* end;
    const char *const b64_chars = BASE64;
    unsigned int msb;
    unsigned int lsb;
    VECTOR_FOR_EACH(begin,end,memory_section){
        if(*begin){
            msb = (*(unsigned int *) (*begin) >> 6)& 0x3F;  /*the first 6 bits*/
            lsb = (*(unsigned int*)(*begin)& 0x3F);         /*the other 6 bits*/
            fprintf(obj_file,"%c%c",b64_chars[msb],b64_chars[lsb]); /*printing the result*/
            fprintf(obj_file,"\n");
        }
    }
}

/**
 * @brief getting a name of am file, opening it and creating from the object-file that he recieves
 * 'ob' file, 'ent' file and 'ext' file. 
 * 
 * @param base_name - the name of the 'as' file we about to work with.
 * @param obj - a data structure that holds in every vector the externs, entries and the missing symbols.
 * @param out_dir - a start of the output files, if we want.
 */
void mmn14_output(char * base_name,const struct object_file *obj,const char *out_dir)
 {
    char *ent_file_name;
    char *ext_file_name;
    char *obj_file_name;
    FILE * obj_file;
    size_t str_len;
    base_name = out_dir != NULL ? basename(base_name) : base_name;
    base_name = strcat(strcpy(malloc(strlen(out_dir) + strlen(base_name)+1),out_dir),base_name);
    str_len = strlen(base_name);
    if(obj -> entry_counter >= 1){
        /*creating 'ent' file*/
        ent_file_name = strcat(strcpy(malloc(str_len+strlen(ENT_EXTENSION)+1),base_name),ENT_EXTENSION);
        ent_file(ent_file_name,obj -> symbol_table);
        free(ent_file_name);
    }
    if(vector_get_item_count(obj->extern_calls)>=1){
        /*creating 'ext' file*/
        ext_file_name = strcat(strcpy(malloc(str_len+strlen(EXT_EXTENSION)+1),base_name),EXT_EXTENSION);
        mmn14_output_ext_file(ext_file_name,obj->extern_calls);
        free(ext_file_name);
    }
    obj_file_name = strcat(strcpy(malloc(str_len+strlen(OB_EXTENSION)+1),base_name),OB_EXTENSION);
    obj_file = fopen(obj_file_name,"w");
    if(obj_file)
    {
        /*creating 'ob' file*/
        fprintf(obj_file,"%lu %lu\n",vector_get_item_count(obj->code_section),vector_get_item_count(obj->data_section));
        mmn14_output_memory_section(obj_file,obj->code_section);
        mmn14_output_memory_section(obj_file,obj->data_section);
        fclose(obj_file);
    }

    free(obj_file_name);
    if(out_dir)
    {
        free(base_name);

    }

 }

