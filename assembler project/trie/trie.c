#include "trie.h"
#include <stdlib.h>

#define TRIE_BASE_CHAR ' '
struct trie_node {
    void * end_of_str_ctx;
    struct trie_node * next[95];
};

struct trie {
    struct trie_node * next[95];
};

/**
 * @brief checking if a word is exists in our trie.
 * 
 * @param node_i - a trie we want to check on.
 * @param string - what do we want to check.
 * @return struct trie_node* - the function returns 'null' if the word is not exists in the trie, and otherwise
 *                          it returns the address of the trie.
 */
static struct trie_node *internal_trie_exists(struct trie_node * node_i,const char * string) {
    while(node_i) {
        if(*string == '\0' && node_i->end_of_str_ctx != NULL) {
            return node_i;
        }
        node_i = node_i->next[(*string) - TRIE_BASE_CHAR];
        string++;
    }
    return NULL;
}

/**
 * @brief creating a place for trie in the memory.
 * 
 * @return Trie - a place in memory that ready to use.
 */
Trie trie() {
    return calloc(1,sizeof(struct trie));
}

/**
 * @brief Putting the correct words or strings for the project.
 * 
 * @param trie - a trie we would like to add an object to it.
 * @param string - the word we want to add to our trie
 * @param end_of_str_ctx - the address that the trie ends.
 * @return const char* - the word we added.
 */
const char *trie_insert(Trie trie,const char *string,void * end_of_str_ctx) {
    struct trie_node ** iterator = &trie->next[(*string) - TRIE_BASE_CHAR];
    while(1) {
        if(*iterator == NULL) {
            (*iterator) = calloc(1,sizeof(struct trie_node));
            if(*iterator == NULL)
                return NULL;
        }
        string++;
        if(*string !='\0')
            iterator = &(*iterator)->next[(*string) - TRIE_BASE_CHAR];
        else
            break;
    }
    (*iterator)->end_of_str_ctx = end_of_str_ctx;
    return string;
}

/**
 * @brief Checking if word exists in our "dictionary"
 * 
 * @param trie - the trie we're checking in.
 * @param string - a word we're looking for in our trie.
 * @return void* - 'null' if it's no exists or returns the object itself if it found in our trie.
 */
void * trie_exists(Trie trie,const char *string) {
    struct trie_node * find_node;
    if(string == NULL)
        return NULL;
    find_node = internal_trie_exists(trie->next[(*string) - TRIE_BASE_CHAR],string+1);
    return find_node == NULL ? NULL : find_node->end_of_str_ctx;
}

/**
 * @brief a recursive function the deleting the data of the trie we give to it's main function.
 * 
 * @param node_i - the inside trie we check and want to make empty.
 */
static void trie_destroy_sub(struct trie_node * node_i) {
    int i;
    for(i=0;i<95;i++) {
        if(node_i->next[i] != NULL) {
            trie_destroy_sub(node_i->next[i]);
            node_i->next[i] = NULL;
        }
    }
    free(node_i);
}

/**
 * @brief Removing, freeing our memory at the end of the projectץ
 * 
 * @param trie - the trie we want to make empty and finish the work with it.
 */
void trie_destroy(Trie * trie) {
    int i;
    if(*trie != NULL) {
        Trie t = *trie;
        for(i=0;i<95;i++) {
            if(t->next[i] != NULL) 
                trie_destroy_sub(t->next[i]);
        }
        free(*trie);
        (*trie) = NULL;
    }
}