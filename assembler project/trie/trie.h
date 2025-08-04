#ifndef __TRIE_C_
#define __TRIE_C_

typedef struct trie * Trie;

Trie trie();

const char * trie_insert(Trie trie,const char *string, void* end_of_str_context);

void *trie_exists(Trie trie,const char *string);

void trie_destroy(Trie * trie);

#endif
