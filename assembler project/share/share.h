#ifndef __SHARED_H_
#define __SHARED_H_

#include "../trie/trie.h"
#include "../vector/vector.h"

#define MAX_LEN 81

struct extern_call{
    char name[30];
    Vector call_address;
};

struct symbol{
    enum{
        symbol_extern, /*Cannot be defined in this file.*/
        symbol_entry, /*Has to be be defined in this file.*/
        symbol_code,
        symbol_data,
        symbol_entry_code,
        symbol_entry_data
    }sym_type;
    unsigned int address;
    unsigned int decleratoin;
    char name[31 + 1];
};

struct object_file{
    Vector code_section;
    Vector data_section;
    Vector symbol_table;
    Vector extern_calls;
    Trie symbol_table_lookup;
    int entry_counter;
};

struct missing_symbol{
    char name[MAX_LEN];
    unsigned int call_line;
    unsigned int * machine_word;
    unsigned int call_address;
};

void * ctor_memory_word(const void * copy);
void dtor_memory_word(void * deleter);
void * ctor_symbol(const void * copy);
void dtor_symbol(void * deleter);
void * ctor_extern_call(const void * copy);
void dtor_extern_call(void * item);
void * ctor_missing_symbol(const void * copy);
void dtor_missing_symbol(void * item);
void add_extern(Vector extern_calls, const char * name, const unsigned int address);
struct object_file create_new_object();
void destroy_object(struct object_file * obj);

#endif
