#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "dy_ngrams.h"
#include "st_ngrams.h"
#include <stdlib.h>
#include "fnv.h"
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
   return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
   return 0;
}
void test_compress(void)
{
    hash_trie *Trie;
    Trie=init_hash_trie();
    hash_keeper *hk;
    initialize_hash_topk(&hk);
	fpos_t position1;

    char phrase[1000],phrase1[1000];
    strcpy(phrase1,"this is a cat and a dog and this is not a balloon");
    strcpy(phrase,"this");
    st_insert_ngram_hash(Trie,phrase);
    char* result;

    strcpy(phrase,"this is");
    st_insert_ngram_hash(Trie,phrase);

    strcpy(phrase,"this is a");
    st_insert_ngram_hash(Trie,phrase);

    strcpy(phrase,"this is not");
    st_insert_ngram_hash(Trie,phrase);

    strcpy(phrase,"this is not an");
    st_insert_ngram_hash(Trie,phrase);

    strcpy(phrase,"this is not a");
    st_insert_ngram_hash(Trie,phrase);

    strcpy(phrase,"this is not a balloon");
    st_insert_ngram_hash(Trie,phrase);

    compress_hash(Trie);
    result=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0!=strcmp(result,"this"));
	CU_ASSERT(0!=strcmp(result,"this|this is"));
	CU_ASSERT(0!=strcmp(result,"this|this is|this is a"));
	CU_ASSERT(0!=strcmp(result,"this|this is|this is a|this is not"));
	CU_ASSERT(0!=strcmp(result,"this|this is|this is a|this is not|this is not a"));
	CU_ASSERT(0==strcmp(result,"this|this is|this is a|this is not|this is not a|this is not a balloon"));
	free(result);

    st_delete_hash(&Trie);
    top_hash_rem(&hk);
}
FILE* clear_file_and_write(FILE *fp1,char* phrase)
{
	fp1=fopen("unit_testing/test_Static_Search.txt","w+");
	fprintf(fp1,"%s",phrase);
	fclose(fp1);
	return fopen("unit_testing/test_Static_Search.txt","r");
}
void test_Static_Search(void)
{
	FILE* fp;
    hash_trie *Trie;
    Trie=init_hash_trie();
    hash_keeper *hk;
    initialize_hash_topk(&hk);
    char* temp;
    char phrase[1000],phrase1[1000];


	temp=static_search(Trie,"",hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"this");
	temp=static_search(Trie,phrase,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"this is");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase,"this");
    st_insert_ngram_hash(Trie,phrase);
	temp=static_search(Trie,"this",hk,0);
	CU_ASSERT(0==strcmp(temp,phrase));
	free(temp);

	strcpy(phrase1,"this");
	strcpy(phrase,"this is a ball");
	st_insert_ngram_hash(Trie,phrase);
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"is this");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"ball");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"this is");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"this is a ball");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a ball"));
	free(temp);

	strcpy(phrase1,"this");
	strcpy(phrase,"this is a dog");
	st_insert_ngram_hash(Trie,phrase);
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"this is");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"ball dog");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"is this");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"this is a dog");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a dog"));
	free(temp);

	strcpy(phrase1,"this is a ball and this is a dog");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a ball|this is a dog"));
	free(temp);

	strcpy(phrase,"is a dog and");
	st_insert_ngram_hash(Trie,phrase);
	temp=static_search(Trie,"this",hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	temp=static_search(Trie,"is",hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"is a dog");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"is a dog and");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"is a dog and"));
	free(temp);

	strcpy(phrase1,"this is a dog and");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a dog|is a dog and"));
	free(temp);

	strcpy(phrase,"is");
	st_insert_ngram_hash(Trie,phrase);
	temp=static_search(Trie,"is",hk,0);
	CU_ASSERT(0==strcmp(temp,"is"));
	free(temp);

	strcpy(phrase1,"is a");
	strcpy(phrase,"is a dog");
	st_insert_ngram_hash(Trie,phrase);
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"is"));
	free(temp);

	strcpy(phrase1,"this is a dog and this is a ball");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a dog|is|is a dog|is a dog and|this is a ball"));
	free(temp);

	strcpy(phrase,"a");
	strcpy(phrase1,"is a");
	st_insert_ngram_hash(Trie,phrase);
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"is|a"));
	free(temp);

	temp=static_search(Trie,"a",hk,0);
	CU_ASSERT(0==strcmp(temp,"a"));
	free(temp);

	strcpy(phrase1,"this is a this is a");
	temp=static_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|is|a"));
	free(temp);

	st_delete_hash(&Trie);
    top_hash_rem(&hk);
}
void test_Dynamic_Search(void)
{
	FILE* fp;
    hash_trie *Trie;
    Trie=init_hash_trie();
    hash_keeper *hk;
    initialize_hash_topk(&hk);
    char* temp;
    char phrase[1000],phrase1[1000];

	temp=new_search(Trie," ",hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));


	temp=new_search(Trie,"this",hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"this is");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));


	strcpy(phrase,"this");
    insert_ngram_hash(Trie,phrase,0);
	temp=new_search(Trie,"this",hk,0);
	CU_ASSERT(0==strcmp(temp,phrase));
	free(temp);

	strcpy(phrase,"this is a ball");
	insert_ngram_hash(Trie,phrase,0);
	temp=new_search(Trie,"this",hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"is this");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	temp=new_search(Trie,"ball",hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"this is");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"this is a ball");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a ball"));
	free(temp);

	strcpy(phrase,"this is a dog");
	insert_ngram_hash(Trie,phrase,0);
	temp=new_search(Trie,"this",hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"this is");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"ball dog");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"is this");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"this is a dog");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a dog"));
	free(temp);

	strcpy(phrase1,"this is a ball and this is a dog");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a ball|this is a dog"));
	free(temp);

	strcpy(phrase,"is a dog and");
	insert_ngram_hash(Trie,phrase,0);
	temp=new_search(Trie,"this",hk,0);
	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);


	temp=new_search(Trie,"is",hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"is a dog");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1,"is a dog and");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"is a dog and"));
	free(temp);

	strcpy(phrase1,"this is a dog and");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a dog|is a dog and"));
	free(temp);

	strcpy(phrase,"is");
	insert_ngram_hash(Trie,phrase,0);
	temp=new_search(Trie,"is",hk,0);
	CU_ASSERT(0==strcmp(temp,"is"));
	free(temp);

	strcpy(phrase1,"is a");
	strcpy(phrase,"is a dog");
	insert_ngram_hash(Trie,phrase,0);
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"is"));
	free(temp);

	strcpy(phrase1,"this is a dog and this is a ball");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|this is a dog|is|is a dog|is a dog and|this is a ball"));
	free(temp);

	strcpy(phrase1,"is a");
	strcpy(phrase,"a");
	insert_ngram_hash(Trie,phrase,0);
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"is|a"));
	free(temp);

	temp=new_search(Trie,"a",hk,0);
	CU_ASSERT(0==strcmp(temp,"a"));
	free(temp);

	strcpy(phrase1,"this is a this is a");
	temp=new_search(Trie,phrase1,hk,0);
	CU_ASSERT(0==strcmp(temp,"this|is|a"));
	free(temp);

	delete_trie_hash(&Trie);
    top_hash_rem(&hk);
}
void test_Static_Insert(void)
{
	FILE* fp;
    hash_trie *Trie;
    Trie=init_hash_trie();
    hash_keeper *hk;
    initialize_hash_topk(&hk);
    char phrase[1000],phrase1[1000];


		char* temp=static_search(Trie,"",hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		temp=static_search(Trie," ",hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		strcpy(phrase1,"this dsad ads");
		temp=static_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		strcpy(phrase,"this");
		st_insert_ngram_hash(Trie,phrase);
		temp=static_search(Trie,"this",hk,0);
		CU_ASSERT(0==strcmp(temp,"this"));
		free(temp);

		strcpy(phrase1,"this is");
		strcpy(phrase,"this is");
		st_insert_ngram_hash(Trie,phrase);
		temp=static_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is"));
		free(temp);

		strcpy(phrase1,"this is a dog");
		strcpy(phrase,"this is a dog");
		st_insert_ngram_hash(Trie,phrase);
		temp=static_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is|this is a dog"));
		free(temp);

		strcpy(phrase1,"this is a cat");
		temp=static_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is"));
		free(temp);

		strcpy(phrase1,"this is a cat");
		strcpy(phrase,"this is a cat");
		st_insert_ngram_hash(Trie,phrase);
		temp=static_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is|this is a cat"));
		free(temp);

		temp=static_search(Trie,"is",hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		strcpy(phrase,"is");
		st_insert_ngram_hash(Trie,phrase);
		temp=static_search(Trie,"is",hk,0);
		CU_ASSERT(0==strcmp(temp,"is"));
		free(temp);

		strcpy(phrase1,"is a cat");
		temp=static_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"is"));
		free(temp);

		strcpy(phrase1,"is a cat");
		strcpy(phrase,"is a cat");
		st_insert_ngram_hash(Trie,phrase);
		temp=static_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"is|is a cat"));
		free(temp);
	    top_hash_rem(&hk);
	st_delete_hash(&Trie);
}
void test_Dynamic_Insert(void)
{
	FILE* fp;
    hash_trie *Trie;
    Trie=init_hash_trie();
    hash_keeper *hk;
    initialize_hash_topk(&hk);
    char phrase[1000],phrase1[1000];

		char* temp=new_search(Trie,"",hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		temp=new_search(Trie," ",hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		strcpy(phrase1,"this dsad ads");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		strcpy(phrase,"this");
		insert_ngram_hash(Trie,phrase,0);
		temp=new_search(Trie,"this",hk,0);
		CU_ASSERT(0==strcmp(temp,"this"));
		free(temp);

		strcpy(phrase1,"this is");
		strcpy(phrase,"this is");
		insert_ngram_hash(Trie,phrase,0);
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is"));
		free(temp);

		strcpy(phrase1,"this is a dog");
		strcpy(phrase,"this is a dog");
		insert_ngram_hash(Trie,phrase,0);
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is|this is a dog"));
		free(temp);

		strcpy(phrase1,"this is a cat");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is"));
		free(temp);

		strcpy(phrase1,"this is a cat");
		strcpy(phrase,"this is a cat");
		insert_ngram_hash(Trie,phrase,0);
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"this|this is|this is a cat"));
		free(temp);

		temp=new_search(Trie,"is",hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

		strcpy(phrase,"is");
		insert_ngram_hash(Trie,phrase,0);
		temp=new_search(Trie,"is",hk,0);
		CU_ASSERT(0==strcmp(temp,"is"));
		free(temp);

		strcpy(phrase1,"is a cat");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"is"));
		free(temp);

		strcpy(phrase1,"is a cat");
		strcpy(phrase,"is a cat");
		insert_ngram_hash(Trie,phrase,0);
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"is|is a cat"));
		free(temp);

		delete_trie_hash(&Trie);
	    top_hash_rem(&hk);
}
void test_Dynamic_Delete(void)
{
	FILE* fp;
    hash_trie *Trie;
    Trie=init_hash_trie();
    hash_keeper *hk;
    initialize_hash_topk(&hk);
    char phrase[1000],phrase1[1000];
    char *testit=malloc(100*sizeof(char));
    char *temp;

	strcpy(phrase1,"");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));

	strcpy(phrase1," ");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));

    strcpy(phrase1,"this dsad ads");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));

    strcpy(phrase,"this is a cat and a dog and a mouse");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is a cat and a dog and a");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is a cat and a dog and");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is a cat and a dog");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is a cat and a");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is a cat and");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is a cat");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is a");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this is");
    insert_ngram_hash(Trie,phrase,0);
    strcpy(phrase,"this");
    insert_ngram_hash(Trie,phrase,0);

    strcpy(phrase1,"this");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this"));
	free(temp);

	strcpy(phrase1,"this is");
    temp=new_search(Trie,phrase1,hk,0);

    CU_ASSERT(0==strcmp(temp,"this|this is"));
	free(temp);

	strcpy(phrase1,"this is a");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a"));
	free(temp);

	strcpy(phrase1,"this is a cat");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a|this is a cat"));
	free(temp);

	strcpy(phrase1,"this is a cat and");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a|this is a cat|this is a cat and"));
	free(temp);

	strcpy(phrase1,"this is a cat and a");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a|this is a cat|this is a cat and|this is a cat and a"));
	free(temp);

	strcpy(phrase1,"this is a cat and a dog");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a|this is a cat|this is a cat and|this is a cat and a|this is a cat and a dog"));
	free(temp);

	strcpy(phrase1,"this is a cat and a dog and");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a|this is a cat|this is a cat and|this is a cat and a|this is a cat and a dog|this is a cat and a dog and"));
	free(temp);

	strcpy(phrase1,"this is a cat and a dog and a");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a|this is a cat|this is a cat and|this is a cat and a|this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a"));
	free(temp);

	strcpy(phrase1,"this is a cat and a dog and a mouse");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this|this is|this is a|this is a cat|this is a cat and|this is a cat and a|this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
	free(temp);

    strcpy(testit,"this");
    CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));

	strcpy(phrase1,"this is a cat and a dog and a mouse");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this is|this is a|this is a cat|this is a cat and|this is a cat and a|this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
	free(temp);
	CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));

	strcpy(phrase1,"this");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));

    strcpy(testit,"this is");
    CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is a cat and a dog and a mouse");
    temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this is a|this is a cat|this is a cat and|this is a cat and a|this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
	free(temp);

	CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));
    strcpy(testit,"this is a");
    CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is a cat and a dog and a mouse");
    temp=new_search(Trie,phrase1,hk,0);

    CU_ASSERT(0==strcmp(temp,"this is a cat|this is a cat and|this is a cat and a|this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
	free(temp);
	CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is a");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));
    strcpy(testit,"this is a cat");
    CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is a cat and a dog and a mouse");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this is a cat and|this is a cat and a|this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
	free(temp);
	CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is a cat");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));
    strcpy(testit,"this is a cat and");
    CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is a cat and a dog and a mouse");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"this is a cat and a|this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
	free(temp);
	CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
	strcpy(phrase1,"this is a cat and");
	temp=new_search(Trie,phrase1,hk,0);

	CU_ASSERT(0==strcmp(temp,"-1"));
        strcpy(testit,"this is a cat and a");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"this is a cat and a dog and a mouse");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0==strcmp(temp,"this is a cat and a dog|this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
		free(temp);
		CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
        strcpy(testit,"this is a cat and a dog");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"this is a cat and a dog and a mouse");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0==strcmp(temp,"this is a cat and a dog and|this is a cat and a dog and a|this is a cat and a dog and a mouse"));
		free(temp);
		CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
		strcpy(phrase1,"this is a cat and a dog");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0==strcmp(temp,"-1"));
        strcpy(testit,"this is a cat and a dog and");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"this is a cat and a dog and a mouse");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0==strcmp(temp,"this is a cat and a dog and a|this is a cat and a dog and a mouse"));
		free(temp);
		CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
		strcpy(phrase1,"this is a cat and a dog and");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0==strcmp(temp,"-1"));
        strcpy(testit,"this is a cat and a dog and a");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"this is a cat and a dog and a mouse");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0==strcmp(temp,"this is a cat and a dog and a mouse"));
		free(temp);
		CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
		strcpy(phrase1,"this is a cat and a dog and a");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

        strcpy(testit,"this is a cat and a dog and a mouse");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"this is a cat and a dog and a mouse");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0!=strcmp(temp,"-1"));

		CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
		strcpy(phrase1,"this is a cat and a dog and a mouse");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0!=strcmp(temp,"-1"));

        strcpy(phrase,"league of legends world championship");
        insert_ngram_hash(Trie,phrase,0);
        strcpy(phrase,"league of legends");
        insert_ngram_hash(Trie,phrase,0);
    	strcpy(phrase1,"league of legends world championship");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"league of legends|league of legends world championship"));
		free(temp);

        strcpy(testit,"league of legends world championship");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"league of legends world championship");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0!=strcmp(temp,"league of legends"));

		free(temp);
		CU_ASSERT(0==delete_ngram_hash(Trie,testit,0));
        strcpy(phrase,"overwatch pro league");
        insert_ngram_hash(Trie,phrase,0);
        strcpy(phrase,"overwatch");
        insert_ngram_hash(Trie,phrase,0);
    	strcpy(phrase1,"league of legends");
        temp=new_search(Trie,phrase1,hk,0);

        CU_ASSERT(0==strcmp(temp,"league of legends"));
        free(temp);
    	strcpy(phrase1,"overwatch");
        temp=new_search(Trie,phrase1,hk,0);

        CU_ASSERT(0==strcmp(temp,"overwatch"));
        free(temp);
    	strcpy(phrase1,"overwatch pro league");
        temp=new_search(Trie,phrase1,hk,0);
        CU_ASSERT(0==strcmp(temp,"overwatch|overwatch pro league"));

        free(temp);
        strcpy(testit,"overwatch pro league");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"overwatch pro league");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0!=strcmp(temp,"overwatch"));
		free(temp);
        strcpy(testit,"overwatch");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"overwatch");
		temp=new_search(Trie,phrase1,hk,0);

		CU_ASSERT(0==strcmp(temp,"-1"));
    	strcpy(phrase1,"league of legends");
        temp=new_search(Trie,phrase1,hk,0);
        CU_ASSERT(0==strcmp(temp,"league of legends"));

        free(temp);
        strcpy(testit,"league of legends");
        CU_ASSERT(1==delete_ngram_hash(Trie,testit,0));
    	strcpy(phrase1,"league of legends");
		temp=new_search(Trie,phrase1,hk,0);
		CU_ASSERT(0==strcmp(temp,"-1"));

    free(testit);
    delete_trie_hash(&Trie);
    top_hash_rem(&hk);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ((NULL == CU_add_test(pSuite, "test of Compress()", test_compress))
		   ||(NULL == CU_add_test(pSuite, "test of Static Search()", test_Static_Search))
		   ||(NULL == CU_add_test(pSuite, "test of Dynamic Search()", test_Dynamic_Search))
		   ||(NULL == CU_add_test(pSuite, "test of Static Insert()", test_Static_Insert))
		   ||(NULL == CU_add_test(pSuite, "test of Dynamic Insert()", test_Dynamic_Insert))
		   ||(NULL == CU_add_test(pSuite, "test of Dynamic Delete()",test_Dynamic_Delete)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
