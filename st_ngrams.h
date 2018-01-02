#ifndef ST_NGRAMS_H_
#define ST_NGRAMS_H_
#include "dy_ngrams.h"
void compress();
int st_binary_search(char* phrase,trie_node** node,int length);
char* static_search(hash_trie*,char*,hash_keeper*,int id);
int st_binary_search_root(char *phrase,trie_node *node,int length);
void static_search_node(trie_node* node,char*,char** subphrase,char* bloom_vector,Arguments* args,int* glob,hash_keeper*,char**,int id);
int st_insert_ngram(Index *indx, char *phrase);
void st_insert_node(trie_node* node,char* phrase);
void delete_static_node(trie_node** temp,trie_node **parent,int paidia);
int check_node_children_for_new_word(char* string,char* new_word,signed short* arr,int fores,int array_size);
void st_delete_helper(trie_node *node);
void st_delete_trie(Index **indx);
void compress_nodes(trie_node* parent,trie_node* current);
void compress_hash(hash_trie *);
void st_delete_hash(hash_trie **);
int st_insert_ngram_hash(hash_trie*,char*);
#endif /* NGRAMS_H_ */
