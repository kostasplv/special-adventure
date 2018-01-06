#ifndef DY_NGRAMS_H_
#define DY_NGRAMS_H_
typedef struct Index Index;
typedef struct trie_node trie_node;
typedef struct hash_trie hash_trie;
typedef struct bck bck;
typedef struct hash_keeper hash_keeper;
typedef struct bck_elem bck_elem;


#include "bloom_functions.h"

struct hash_trie
{
    Index **bucket;
    int bucket_num;
    int pointer;
    int round;
};
struct trie_node
{
	char* word;
	trie_node** children;
	char is_final;
	int child_num;
	int node_size;
	signed short *array;
	int array_size;
	int *fores;
	char static_word[51];
	int word_flag;			//0=statiko   1=dynamiko
	int a_version;
	int d_version;
};
struct Index
{
	trie_node* root;
	int root_size;
	int root_num;
};
struct bck_elem
{
    int checked;
    char *name;
};
struct bck
{
    int cnt;
    int bck_size;
    bck_elem *elems;
};
struct hash_keeper
{
    bck **bucket;
    int bucket_num;
    int pointer;
    int round;
};

unsigned long hash_fun(unsigned char *);
hash_trie *init_hash_trie();
void InsertionSort (trie_node** a, int n);
void InsertionSort_Root (trie_node* a, int n);
Index* init_trie();
trie_node* create_trie_node();
int insert_ngram(Index*,char*,int);
int insert_ngram_hash(hash_trie*,char*,int);
int delete_ngram(Index*,char *,int);
int delete_ngram_hash(hash_trie*,char*,int);
int delete_node(trie_node *,char *,int);
void insert_node(trie_node* node,char* phrase,int);
int search_hash(hash_trie*,char*);
void delete_trie_hash(hash_trie **);
void delete_trie(Index **);
void delete_helper(trie_node *);
int binary_search(char* phrase,trie_node** node,int length);
int binary_search_root(char *phrase,trie_node *node,int length);
void findk(hash_keeper*,char*);
char* new_search(hash_trie*,char*,hash_keeper*,int);
int binary_search_top(char*,bck_elem*,int);
void initialize_hash_topk(hash_keeper**);
void insert_hash_topk(hash_keeper*,char*);
void top_hash_rem(hash_keeper **);
void top_hash_del(hash_keeper *);
void rare_occasion(hash_trie*);
void merge_hash_topk(hash_keeper*,char*,int);
void merge_everything(hash_keeper **,int);
void clean_up_helper(trie_node *,hash_trie *,int);
void clean_up(Index *,hash_trie *,int);
void clean_up_hash(hash_trie *,int);
#endif /* NGRAMS_H_ */
