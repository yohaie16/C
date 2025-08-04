#include "share.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 * @brief saving place in our memory for a vector.
 * 
 * @param copy - the variable we wan to give memory
 * @return void* - the variable with it's memory
 */
void * ctor_memory_word(const void * copy){
    return memcpy(malloc(sizeof(unsigned int)), copy,sizeof(unsigned int));
}

void dtor_memory_word(void * deleter){
    free(deleter);
}

/**
 * @brief saving place in our memory for a vector.
 * 
 * @param copy - the variable we wan to give memory
 * @return void* - the variable with it's memory
 */
void * ctor_symbol(const void * copy){
    return memcpy(malloc(sizeof(unsigned int)), copy, sizeof(unsigned int));
}

/**
 * @brief freeing memory
 * 
 * @param deleter - the memory section we need to set free.
 */
void dtor_symbol(void * deleter){
    free(deleter);
}

/**
 * @brief saving place in our memory for a vector.
 * 
 * @param copy - the variable we wan to give memory
 * @return void* - the variable with it's memory
 */
void * ctor_extern_call(const void * copy){
    const struct extern_call * ec_copy = copy;
    struct extern_call * new_extern = malloc(sizeof(struct missing_symbol));
    strcpy(new_extern->name, ec_copy->name);
    new_extern->call_address = new_vector(ctor_memory_word, dtor_memory_word);
    return new_extern;
}

/**
 * @brief freeing memory
 * 
 * @param deleter - the memory section we need to set free.
 */
void dtor_extern_call(void * item){
    struct extern_call * ec_copy = item;
    vector_destroy(&ec_copy->call_address);
    free(ec_copy);
}

/**
 * @brief saving place in our memory for a vector.
 * 
 * @param copy - the variable we wan to give memory
 * @return void* - the variable with it's memory
 */
void * ctor_missing_symbol(const void * copy){
    return memcpy(malloc(sizeof(unsigned int)), copy, sizeof(unsigned int));
}

/**
 * @brief freeing memory
 * 
 * @param deleter - the memory section we need to set free.
 */
void dtor_missing_symbol(void * item){
    free(item);
}

/**
 * @brief after a label declared as an extern, we save it with this function in a vector.
 * of course after checking that the label is not already saved.
 * 
 * @param extern_calls - a vector that holds the extern labels, and what address did we call them.
 * @param name - the name of the extern that used, and we're looking for it in the extern table.
 * @param address - the address we called the extern label.
 */
void add_extern(Vector extern_calls, const char * name, const unsigned int address){
    void * const * begin_ex;
    void * const * end_ex;
    struct extern_call new_extern = {0};
    VECTOR_FOR_EACH(begin_ex, end_ex, extern_calls);
    if(*begin_ex){
        if(strcmp(name, ((const struct extern_call *)(*begin_ex))->name) == 0){
            vector_insert(((const struct extern_call *)(*begin_ex))->call_address, &address);
            return;
        }
    }
    strcpy(new_extern.name,name);
    new_extern.call_address = new_vector(ctor_memory_word,dtor_memory_word);
    vector_insert(new_extern.call_address,&address);
    vector_insert(extern_calls,&new_extern);
}

/*create_new_object: opening an object file*/
/**
 * @brief opening an object file*
 * 
 * @return struct object_file - a data structure ready with all vectors and trie ready to be used.
 */
struct object_file create_new_object(){
    struct object_file ret = {0};
    ret.code_section = new_vector(ctor_memory_word,dtor_memory_word);
    ret.data_section = new_vector(ctor_memory_word,dtor_memory_word);
    ret.symbol_table = new_vector(ctor_symbol, dtor_symbol);
    ret.extern_calls = new_vector(ctor_extern_call, dtor_extern_call);
    ret.symbol_table_lookup = trie();
    return ret;
}

/**
 * @brief closing the object file.
 * 
 * @param obj - the object that we want to close all of his vectors and tries.
 */
void destroy_object(struct object_file * obj){
    vector_destroy(&obj->code_section);
    vector_destroy(&obj->data_section);
    vector_destroy(&obj->extern_calls);
    vector_destroy(&obj->symbol_table);
    trie_destroy(&obj->symbol_table_lookup);
}
