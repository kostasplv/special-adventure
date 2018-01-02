#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloom_functions.h"
#include "dy_ngrams.h"
#include <time.h>
#include <stdint.h>
#include "fnv.h"
#include <inttypes.h>
#define VECTOR_SIZE 5000
#define HASH_NUMBER 10

int the_counter=0;
int for_counter=0;
unsigned long hash_fun(unsigned char *str) //djb2
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
hash_trie* init_hash_trie()//initialization of hash_trie
{
    hash_trie* temp;
    temp=malloc(sizeof(hash_trie));
    temp->bucket_num=10;
    temp->pointer=0;
    temp->round=10;
    temp->bucket=malloc(temp->bucket_num*sizeof(Index *));
    int i;
    i=0;
    for(i=0;i<temp->bucket_num;i++)
        temp->bucket[i]=init_trie();//initializion for every bucket
    return temp;
}
Index* init_trie()
{
	Index *trie;
	trie = malloc(sizeof(Index));
	trie->root_size = 100;
	trie->root = malloc(trie->root_size * sizeof(trie_node));
	trie->root_num = 0;
	return trie;

}
trie_node* create_trie_node() {
	trie_node* node;
	node = malloc(sizeof(trie_node));
	node->word = NULL;
	node->node_size = 20;
	node->children = malloc(node->node_size * sizeof(trie_node *));
	int i = 0;
	for (i = 0; i < node->node_size; i++) {
		node->children[i] = NULL;
	}
	node->is_final = 'N';				//  N=no , Y=yes  //
	node->child_num = 0;
	return node;
}
int binary_search(char* phrase,trie_node** node,int length)
{
	 int  first, last, middle;
	 if(phrase==NULL)return -1;
	 first = 0;
	 last = length - 1;
	 middle = (first+last)/2;
	 while (first <= last)
	 {
		 if(node[first]->word_flag==1)
		 {
			 if ((strcmp(node[first]->word ,phrase))==0)
				 	 return first;
		 }
		 else
		 {
			 if ((strcmp(node[first]->static_word ,phrase))==0)
				 	 return first;
		 }
		 if(node[last]->word_flag==1)
		 {
			 if ((strcmp(node[last]->word ,phrase))==0)
				 	 return last;
		 }
		 else
		 {
			 if ((strcmp(node[last]->static_word ,phrase))==0)
				 	 return last;
		 }
		 if(node[middle]->word_flag==1)
		 {
			   if ((strcmp(node[middle]->word ,phrase))<0)
			         first = middle + 1;
			      else if ((strcmp(node[middle]->word ,phrase))==0)
			      {
			    	  return middle;
			         break;
			      }
			      else
			         last = middle - 1;
		 }
		 else
		 {
			   if ((strcmp(node[middle]->static_word ,phrase))<0)
			         first = middle + 1;
			      else if ((strcmp(node[middle]->static_word ,phrase))==0)
			      {
			    	  return middle;
			         break;
			      }
			      else
			         last = middle - 1;
		 }

	      middle = (first + last)/2;
	 }
	 if (first > last)
		 return -1;
}
int binary_search_root(char* phrase,trie_node* node,int length)
{
	 int  first, last, middle;
	 if(phrase==NULL)return -1;
	 first = 0;
	 last = length - 1;
	 middle = (first+last)/2;
	 while (first <= last)
	 {
		 if(node[first].word_flag==1)
		 {
			 if ((strcmp(node[first].word ,phrase))==0)
			 	 return first;
		 }
		else
		{
			 if ((strcmp(node[first].static_word ,phrase))==0)
			 	 return first;
		}
		if(node[last].word_flag==1)
		{
			 if ((strcmp(node[last].word ,phrase))==0)
			 	 return last;
		}
		else
		{
			 if ((strcmp(node[last].static_word ,phrase))==0)
			 	 return last;
		}
		if(node[middle].word_flag==1)
		{
			if ((strcmp(node[middle].word ,phrase))<0)
		         first = middle + 1;
		     else if ((strcmp(node[middle].word ,phrase))==0)
		     {
		    	 return middle;
		     }
		     else
		    	 last = middle - 1;
		}
		else
		{
			if ((strcmp(node[middle].static_word ,phrase))<0)
		         first = middle + 1;
		     else if ((strcmp(node[middle].static_word ,phrase))==0)
		     {
		    	 return middle;
		     }
		     else
		    	 last = middle - 1;
		}
	    middle = (first + last)/2;
	 }
	 if (first > last)
		 return -1;
}
int insert_ngram_hash(hash_trie *ht,char *phrase,int current_version)//we insert the first word in a bucket then we do linear hashing
{
    char *temp;
    temp=malloc(strlen(phrase)+1);
    strcpy(temp,phrase);
    char *str;
    char *progress;
    str=strtok_r(temp," ",&progress);
    char *temp1;
    temp1=malloc(strlen(str)+1);
    strcpy(temp1,str);
    free(temp);
    temp=NULL;
    unsigned long md;
    md=ht->round;
    unsigned long hash_val;
    unsigned long hash_val_temp;
    hash_val=0;
    hash_val_temp=0;
    hash_val_temp=hash_fun(temp1);
    hash_val=hash_val_temp%md;
    if(hash_val>=ht->pointer)//an to bucket ths prwths leksis einai to bucket tou pointer i meta ton pointer
    {
        int overflow=0;
        overflow=insert_ngram(ht->bucket[hash_val],phrase,current_version);//elegxoume an egine overflow kata tin eisogwgh sto bucket
        if(overflow>1)//an egine
        {
            md*=2;
            trie_node *temp_array;//apothikevoume proswrina ta periexomena tou bucket se enan pinaka
            int temp_counter;
            temp_counter=ht->bucket[ht->pointer]->root_num;
            temp_array=malloc(temp_counter*sizeof(trie_node));
            int j;
            j=0;
            for(j=0;j<temp_counter;j++)
            {
                temp_array[j]=ht->bucket[ht->pointer]->root[j];
            }
            ht->bucket[ht->pointer]->root_num=0;
            ht->bucket_num=ht->bucket_num+1;
            ht->bucket=realloc(ht->bucket,ht->bucket_num*sizeof(Index *));
            ht->bucket[ht->bucket_num-1]=init_trie();
            int cur_pointer=0;
            int cur_round_pointer=0;
            cur_pointer=ht->pointer;
            cur_round_pointer=ht->pointer+ht->round;
            ht->pointer=(ht->pointer+1)%ht->round;
            if(ht->pointer==0)
                ht->round=ht->bucket_num;
            for(j=0;j<temp_counter;j++)//kai meta analoga me to neo hashnum pou irthe ta vazoume i sto bucket tou pointer i sto bucket tou pointer+round
            {
                int rare_occasion_trigger=0;
                unsigned long newhashnum;
                if(temp_array[j].word_flag==1)
                	newhashnum=hash_fun(temp_array[j].word);
                else
                	newhashnum=hash_fun(temp_array[j].static_word);
                newhashnum=newhashnum%md;
                if(ht->bucket[newhashnum]->root_num == ht->bucket[newhashnum]->root_size)
                {
                    ht->bucket[newhashnum]->root_size*=2;
                    ht->bucket[newhashnum]->root=realloc(ht->bucket[newhashnum]->root,ht->bucket[newhashnum]->root_size*sizeof(trie_node));
                    rare_occasion_trigger++;
                }
                ht->bucket[newhashnum]->root_num++;
                ht->bucket[newhashnum]->root[ht->bucket[newhashnum]->root_num-1]=temp_array[j];
                if(rare_occasion_trigger)//spania periptosi apou ola tha mpoun sto bucket tou pointer+round tote tha ksanaginei spasimo apo to linear hashing
                    rare_occasion(ht);
            }
            InsertionSort_Root(ht->bucket[cur_pointer]->root,ht->bucket[cur_pointer]->root_num);//epanataksinomoume to bucket tou pointer
            InsertionSort_Root(ht->bucket[cur_round_pointer]->root,ht->bucket[cur_round_pointer]->root_num);//epanataksinomoume to bucket tou pointer+round
            free(temp_array);
            temp_array=NULL;
        }
    }
    else if(hash_val<ht->pointer)//an to bucket tis protis leksis einai prin ton pointer
    {
        md*=2;
        hash_val=hash_val_temp%md;
        int overflow=0;
        overflow=insert_ngram(ht->bucket[hash_val],phrase,current_version);//elegxoume an egine overflow kata tin eisogwgh sto bucket
        if(overflow>1)
        {
            trie_node *temp_array;//apothikevoume proswrina ta periexomena tou bucket se enan pinaka
            int temp_counter;
            temp_counter=ht->bucket[ht->pointer]->root_num;
            temp_array=malloc(temp_counter*sizeof(trie_node));
            int j;
            j=0;
            for(j=0;j<temp_counter;j++)
            {
                temp_array[j]=ht->bucket[ht->pointer]->root[j];
            }
            ht->bucket[ht->pointer]->root_num=0;
            ht->bucket_num=ht->bucket_num+1;
            ht->bucket=realloc(ht->bucket,ht->bucket_num*sizeof(Index *));
            ht->bucket[ht->bucket_num-1]=init_trie();
            int cur_pointer=0;
            int cur_round_pointer=0;
            cur_pointer=ht->pointer;
            cur_round_pointer=ht->pointer+ht->round;
            ht->pointer=(ht->pointer+1)%ht->round;
            if(ht->pointer==0)
                ht->round=ht->bucket_num;
            for(j=0;j<temp_counter;j++)//kai meta analoga me to neo hashnum pou irthe ta vazoume i sto bucket tou pointer i sto bucket tou pointer+round
            {
                int rare_occasion_trigger=0;
                unsigned long newhashnum;
                if(temp_array[j].word_flag==1)
                	newhashnum=hash_fun(temp_array[j].word);
                else
                	newhashnum=hash_fun(temp_array[j].static_word);
                newhashnum=newhashnum%md;
                if(ht->bucket[newhashnum]->root_num == ht->bucket[newhashnum]->root_size)
                {
                    ht->bucket[newhashnum]->root_size*=2;
                    ht->bucket[newhashnum]->root=realloc(ht->bucket[newhashnum]->root,ht->bucket[newhashnum]->root_size*sizeof(trie_node));
                    rare_occasion_trigger++;
                }
                ht->bucket[newhashnum]->root_num++;
                ht->bucket[newhashnum]->root[ht->bucket[newhashnum]->root_num-1]=temp_array[j];
                if(rare_occasion_trigger)//spania periptosi apou ola tha mpoun sto bucket tou pointer+round tote tha ksanaginei spasimo apo to linear hashing
                    rare_occasion(ht);
            }
            InsertionSort_Root(ht->bucket[cur_pointer]->root,ht->bucket[cur_pointer]->root_num);//epanataksinomoume to bucket tou pointer
            InsertionSort_Root(ht->bucket[cur_round_pointer]->root,ht->bucket[cur_round_pointer]->root_num);//epanataksinomoume to bucket tou pointer+round
            free(temp_array);
            temp_array=NULL;
        }
    }
    free(temp1);
    temp1=NULL;
    return 1;
}
void rare_occasion(hash_trie *ht)//spania periptosi opou ola ta stoixeia kata tin eisagogi mpikan ston bucket tou pointer+round kai egine overflow tote tha ksanaginei linear hashing
{
    int md;
    md=ht->round;
    md*=2;
    trie_node *temp_array;
    int temp_counter;
    temp_counter=ht->bucket[ht->pointer]->root_num;
    temp_array=malloc(temp_counter*sizeof(trie_node));
    int j;
    j=0;
    for(j=0;j<temp_counter;j++)
    {
        temp_array[j]=ht->bucket[ht->pointer]->root[j];
    }
    ht->bucket[ht->pointer]->root_num=0;
    ht->bucket_num=ht->bucket_num+1;
    ht->bucket=realloc(ht->bucket,ht->bucket_num*sizeof(Index *));
    ht->bucket[ht->bucket_num-1]=init_trie();
    int cur_pointer=0;
    int cur_round_pointer=0;
    cur_pointer=ht->pointer;
    cur_round_pointer=ht->pointer+ht->round;
    ht->pointer=(ht->pointer+1)%ht->round;
    if(ht->pointer==0)
        ht->round=ht->bucket_num;
    for(j=0;j<temp_counter;j++)
    {
        int rare_occasion_trigger=0;
        unsigned long newhashnum;
        if(temp_array[j].word_flag==1)
        	newhashnum=hash_fun(temp_array[j].word);
        else
        	newhashnum=hash_fun(temp_array[j].static_word);
        newhashnum=newhashnum%md;
        if(ht->bucket[newhashnum]->root_num == ht->bucket[newhashnum]->root_size)
        {
            ht->bucket[newhashnum]->root_size*=2;
            ht->bucket[newhashnum]->root=realloc(ht->bucket[newhashnum]->root,ht->bucket[newhashnum]->root_size*sizeof(trie_node));
            rare_occasion_trigger++;
        }
        ht->bucket[newhashnum]->root_num++;
        ht->bucket[newhashnum]->root[ht->bucket[newhashnum]->root_num-1]=temp_array[j];
        if(rare_occasion_trigger)
            rare_occasion(ht);
    }
    InsertionSort_Root(ht->bucket[cur_pointer]->root,ht->bucket[cur_pointer]->root_num);
    InsertionSort_Root(ht->bucket[cur_round_pointer]->root,ht->bucket[cur_round_pointer]->root_num);
    free(temp_array);
    temp_array=NULL;
}
int insert_ngram(Index *indx, char *phrase,int current_version)
{
    int of;
    of=1;
	char *str;
	char *progress;
	str= strtok_r(phrase," ",&progress);
	int found = -1;
	found=binary_search_root(str,indx->root,indx->root_num);
	if (found == -1)	//den vrethike i leksh sto root//
	{
		if (indx->root_size == indx->root_num) //an o pinakas root einai gematos kanoume realloc(diplasiazoume)//
		{
			indx->root_size *= 2;
			indx->root = realloc(indx->root,indx->root_size * sizeof(trie_node));
			of++;
		}
		indx->root_num++;
		indx->root[indx->root_num-1].node_size=20;
		indx->root[indx->root_num-1].children=malloc(indx->root[indx->root_num-1].node_size*sizeof(trie_node));
		indx->root[indx->root_num-1].is_final='N';
        indx->root[indx->root_num-1].child_num=0;
        indx->root[indx->root_num-1].a_version=current_version;
        indx->root[indx->root_num-1].d_version=0;
        if(strlen(str)<=50)
        {
    		strcpy(indx->root[indx->root_num - 1].static_word, str);
    		indx->root[indx->root_num - 1].word_flag=0;
        }
        else
        {
    		indx->root[indx->root_num - 1].word=malloc(strlen(str)+1);
    		strcpy(indx->root[indx->root_num - 1].word, str);
    		indx->root[indx->root_num - 1].word_flag=1;
        }
		char *str1 = strtok_r(NULL, "",&progress);
		if (str1 == NULL) //an exei mono mia leksh h psrash//
		{
			indx->root[indx->root_num - 1].is_final = 'Y';
            indx->root[indx->root_num - 1].a_version=current_version;
			InsertionSort_Root(indx->root,indx->root_num);
			return of;
		}
        int found=-1;
        InsertionSort_Root(indx->root,indx->root_num);
        found=binary_search_root(str,indx->root,indx->root_num);
		insert_node(&(indx->root[found]),str1,current_version);
	}
	else //to phrase uparxei sto root//
	{
		char *str1 = strtok_r(NULL, "",&progress);
		if (str1 == NULL) //an exei mono mia leksh h psrash//
		{
			indx->root[found].is_final = 'Y';
			indx->root[found].a_version=current_version;
			return of;
		}
		insert_node(&(indx->root[found]),str1,current_version);
	}
	return of;
}
void insert_node(trie_node* node,char* phrase,int current_version)
{
    char *progress;
	char *str = strtok_r(phrase, " ",&progress);
    while(1)
    {
        if(str==NULL)
            break;
        int found=-1;
        found=binary_search(str,node->children,node->child_num);
        if(found==-1)
        {
            if(node->child_num==node->node_size)
            {
                node->node_size*=2;
                node->children=realloc(node->children,node->node_size*sizeof(trie_node *));
            }
            node->child_num++;
            node->children[node->child_num-1]=create_trie_node();
            node->children[node->child_num-1]->a_version=current_version;
            node->children[node->child_num-1]->d_version=0;
            if(strlen(str)<=50)
            {
            	strcpy(node->children[node->child_num-1]->static_word,str);
            	 node->children[node->child_num-1]->word_flag=0;
            }
            else
            {
                node->children[node->child_num-1]->word=malloc(strlen(str)+1);
                strcpy(node->children[node->child_num-1]->word,str);
                node->children[node->child_num-1]->word_flag=1;
            }
            char *str1;
            str1=malloc((strlen(str)+1)*sizeof(char));
            strcpy(str1,str);
            str=strtok_r(NULL," ",&progress);
            if(str==NULL)
            {
                free(str1);
                str1=NULL;
            	node->children[node->child_num-1]->is_final='Y';
            	node->children[node->child_num-1]->a_version=current_version;
            	InsertionSort(node->children,node->child_num);
                break;
            }
            int found1=-1;
            InsertionSort(node->children,node->child_num);
            found1=binary_search(str1,node->children,node->child_num);
            free(str1);
            str1=NULL;
            node=node->children[found1];
        }
        else
        {
            str=strtok_r(NULL," ",&progress);
            if(str==NULL)
            {
                node->children[found]->is_final='Y';
                node->children[found]->a_version=current_version;
                break;
            }
            node=node->children[found];
        }
    }
}
int delete_ngram_hash(hash_trie *ht,char* phrase,int current_version)//vriskoume to to bucket tis protis leksis tou ngram pou theloume na diagrapsoume kai gi auto to bucket kaloume tin delete_ngram
{
    char *temp;
    temp=malloc(strlen(phrase)+1);
    strcpy(temp,phrase);
    char *str;
    char *progress;
    str=strtok_r(temp," ",&progress);
    char *temp1;
    temp1=malloc(strlen(str)+1);
    strcpy(temp1,str);
    free(temp);
    temp=NULL;
    unsigned long md;
    md=ht->round;
    unsigned long hash_val;
    unsigned long hash_val_temp;
    hash_val=0;
    hash_val_temp=0;
    hash_val_temp=hash_fun(temp1);
    hash_val=hash_val_temp%md;
    if(hash_val>=ht->pointer)//elegxoume an to hash_val pou vrikame einai o arithmos tou pointer i meta ton pointer
    {
        free(temp1);
        temp1=NULL;
        return delete_ngram(ht->bucket[hash_val],phrase,current_version);
    }
    else//an einai prin tote prepei na diplasiasoume ton arithmo pou kanoume mod
    {
        md*=2;
        hash_val=hash_val_temp%md;
        free(temp1);
        temp1=NULL;
        return delete_ngram(ht->bucket[hash_val],phrase,current_version);
    }
}
int delete_ngram(Index* indx,char *phrase,int current_version)// DAIGRADFEI ENA N-GRAM APO TO TRIE
{
    char *progress;
    char *str=strtok_r(phrase," ",&progress);//PAIRNEI THN PRWTH LEKSH TOY N-GRAM
    char *str1=strtok_r(NULL,"",&progress);//EINAI TO YPOLOIPO N-GRAM XORIS THN PRWTH LEKSH
	int found = -1;
	found=binary_search_root(str,indx->root,indx->root_num);//PSAXNOYME THN PRWTH LEKSH STO ROOT AN DEN YPARXEI EPISTREFEI MHDEN
	if(found==-1)
	{
        return 0;
	}
	else
	{
        if(str1==NULL)//AN TO N-GRAM EINAI MONO MIA LEKSH
        {
            if(indx->root[found].is_final=='Y')//KOITAME AN EINAI TELIKOS KOMVOS
            {
                if(indx->root[found].child_num==0)//AN DEN EXEI PAIDIA TOTE APLA TON DIAGRAFEI
                {
                    indx->root[found].d_version=current_version;
                	/*if(indx->root[found].word_flag==1)
                	{
                        free(indx->root[found].word);
                        indx->root[found].word=NULL;
                	}
					free(indx->root[found].children);
					indx->root[found].children=NULL;
                    memmove(indx->root + found,indx->root+found+1,(indx->root_num-found-1)*sizeof(trie_node));
                    indx->root_num=indx->root_num-1;*/
                    return 1;
                }
                else//AN EXEI PAIDIA APLA TON KANEI TO TELIKO KOMVO MH TELIKO
                {
                    indx->root[found].is_final='N';
                    indx->root[found].d_version=current_version;
                    return 1;
                }
            }
            else//AN DEN EINAI TELIKOS KOMVOS TOTE DEN YPARXEI TO N-GRAM KAI EPISTREFEI MHDEN
            {
                return 0;
            }
        }
        else//AN TO N-GRAM DEN HTAN MONO MIA LEKSI TOTE KALOYME GIA TO YPOLOIPO STRING THN VOHTHITIKH SYNARTHSH delete_node
        {
            int chc;
            chc=delete_node(&(indx->root[found]),str1,current_version);
            if(chc==2)//AN AYTI EPISTREPSEI 2 SHMAINEI OTI DIEGRAPSE TOYS KOMVOYS TWN ALLWN LEKSEWN ARA THA KOITAKSOYME NA DOYME AN MPOROYME NA DIAGRAPSOYME TON KOMVO THS PRWTHS LEKSHS
            {
                if(indx->root[found].is_final=='N')//AN EINAI MH TELIKOS O KOMVOS THS PRWTHS LEKSHS
                {
                    if(indx->root[found].child_num==0)//AN DEN EXEI PAIDIA TOTE TON DIAGRAFOYME
                    {
                        indx->root[found].d_version=current_version;
                    }
                    if(indx->root[found].child_num==0)//AN DEN EXEI PAIDIA TOTE TON DIAGRAFOYME
                    {
						indx->root[found].d_version=current_version;
                    	/*if(indx->root[found].word_flag==1)
                    	{
                            free(indx->root[found].word);
                            indx->root[found].word=NULL;
                    	}
						free(indx->root[found].children);
						indx->root[found].children=NULL;
                        memmove(indx->root + found,indx->root+found+1,(indx->root_num-found-1)*sizeof(trie_node));
                        indx->root_num=indx->root_num-1;*/
                        return 1;
                    }
                    else//AN APISTREPSEI 1 SHMAINEI OTI DEN MPORESE NA DIAGRAPSEI OLOYS TOY KOMVOYS TWN ALLWN LEKSEWN ARA APLA EPISTREFEI POS EGINE EPITYXWN H DIAGRAFH
                    {
                        return 1;
                    }
                }
                else
                {
                    return 1;
                }
            }
            else if(chc==1)//AN EPISTREPSEI 1 SHMAINEI PWS DEN MPORESE NA DIAGRAPSEI TOYS KOMVOYS TWN EPOMENWN LEKSEWN ARA EPISTREFEI PWS EGINE EPITYXWS H DIAGRAFH
            {
                return 1;
            }
            else//AN EPISTREPSEI MHDEN TOTE SHMAINEI OTI DEB VRIKE TO N-GRAM ARA EPISTREFEI LATHOS
            {
                return 0;
            }
        }
	}
	return 1;
}
int delete_node(trie_node *node,char *phrase,int current_version)//VOHTHITIKH SYNARTHSH THS delete_ngram
{
    int counter=0;
    int size=20;
    trie_node **temp;
    int *where;
    temp=malloc(size*sizeof(trie_node *));//SE AYTON TON PINAKA KRATAME TIS DIEYTHYNSEIS TWN trie_nodes POY EXOYN WS PAIDIA TIS LEKSEIS POY PSAXNOYME
    where=malloc(size*sizeof(int));//KAI SE AYTON KRATAME THN THESH MESA STON PINAKA children TWN trie_nodes OPOY EINAI OI LEKSEIS
    int found=-1;
    char *progress;
    char *str=strtok_r(phrase," ",&progress);
    int flag=3;
    while(1)//EDV APLA GEMIZOYME TOYS PARAPANW PINAKES
    {
        if(str==NULL) break;
        found=binary_search(str,node->children,node->child_num);
        if(found==-1)
        {
            free(temp);
            temp=NULL;
            free(where);
            where=NULL;
            return 0;
        }
        counter++;
        if(counter>size)
        {
            size*=2;
            temp=realloc(temp,size*sizeof(trie_node *));
            where=realloc(where,size*sizeof(int));
        }
        temp[counter-1]=node;
        where[counter-1]=found;
        str=strtok_r(NULL," ",&progress);
        node=node->children[found];
    }
    int i;
    for(i=counter-1;i>=0;i--)//KAI EDV APO THN TELEYTAIA LEKSH TOY N-GRAM KAI PROS TA PISW KSEKINAME THN DIADIKASIA THS DIAGRAFIS
    {
        if(flag==3)//H PRWTH LEKSH POY THA DIAGRAPSOYME
        {
            if(temp[i]->children[where[i]]->is_final=='Y')//AN EINAI TELIKOS KOMVOS
            {
                if(temp[i]->children[where[i]]->child_num==0)//AN DEN EXEI PAIDIA TON DIAGRAFOYME
                {
                    temp[i]->children[where[i]]->d_version=current_version;
                	/*if(temp[i]->children[where[i]]->word_flag==1)
                	{
                        free(temp[i]->children[where[i]]->word);
                        temp[i]->children[where[i]]->word=NULL;
                	}
					free(temp[i]->children[where[i]]->children);
					temp[i]->children[where[i]]->children=NULL;
                    free(temp[i]->children[where[i]]);
                    temp[i]->children[where[i]]=NULL;
                    memmove(temp[i]->children + where[i],temp[i]->children+where[i]+1,(temp[i]->child_num-where[i]-1)*sizeof(trie_node *));
                    temp[i]->child_num=temp[i]->child_num-1;*/
                }
                else//AN EXEI APLA TON KANOYME MH TELIKO
                {
                    temp[i]->children[where[i]]->is_final='N';
                    temp[i]->children[where[i]]->d_version=current_version;
                    flag--;
                }
            }
            else//AN DEN EINAI TELIKOS LATHOS
            {
                free(temp);
                temp=NULL;
                free(where);
                where=NULL;
                return 0;
            }
            flag--;
        }
        else if(flag==2)//AN DIAGRAPSAME TON KOMVO THS PROHGOYMENHS LEKSHS KOITAME THS EPOMENHS KAI KOITAME NA DOYME AN MPROYME NA TON DIAGRAPSOYME
        {
            if(temp[i]->children[where[i]]->is_final=='N')//AN DEN EINAI TELIKOS KOMVOS
            {
                if(temp[i]->children[where[i]]->child_num==0)//AN DEN EXEI PAIDIA TON DIAGRAFOYME
                {
                    temp[i]->children[where[i]]->d_version=current_version;
                	/*if(temp[i]->children[where[i]]->word_flag==1)
                	{
                        free(temp[i]->children[where[i]]->word);
                        temp[i]->children[where[i]]->word=NULL;
                	}
					free(temp[i]->children[where[i]]->children);
					temp[i]->children[where[i]]->children=NULL;
                    free(temp[i]->children[where[i]]);
                    temp[i]->children[where[i]]=NULL;
                    memmove(temp[i]->children + where[i],temp[i]->children+where[i]+1,(temp[i]->child_num-where[i]-1)*sizeof(trie_node *));
                    temp[i]->child_num=temp[i]->child_num-1;*/
                }
                else//AN EXEI DEN KANOYME TIPOTA
                {
                    flag--;
                }
            }
            else
            {
                flag--;
            }
        }
        else
        {
            break;
        }
    }
    if(flag==1)
    {
        free(temp);
        temp=NULL;
        free(where);
        where=NULL;
        return 1;
    }
    else
    {
        free(temp);
        temp=NULL;
        free(where);
        where=NULL;
        return 2;
    }
}
int search_hash(hash_trie* ht,char *phrase)//vriskei ton arithmo tou bicket pou vrisketai i leksi pou psaxnoume
{
    char *temp;
    temp=malloc(strlen(phrase)+1);
    strcpy(temp,phrase);
    unsigned long md;
    md=ht->round;
    unsigned long hash_val;
    unsigned long hash_val_temp;
    hash_val=0;
    hash_val_temp=0;
    hash_val_temp=hash_fun(temp);
    hash_val=hash_val_temp%md;
    if(hash_val>=ht->pointer)
    {
        free(temp);
        temp=NULL;
        return hash_val;
    }
    else
    {
        md*=2;
        hash_val=hash_val_temp%md;
        free(temp);
        temp=NULL;
        return hash_val;
    }
}
char* new_search(hash_trie* ht,char *parseddata,hash_keeper *hk,int current_version)//search function
{
	char* result=NULL;
    char *phr;
	char* bloom_vector;
	Arguments* args;
	args=malloc(sizeof(Arguments));
	initialize_args(args,VECTOR_SIZE,HASH_NUMBER);
	bloom_vector=malloc(args->size*sizeof(char));
	initialize_bloom_filter(args,bloom_vector);
	Index *indx;
    int file_stamp=0;
    int first_word=0;
    int check=-1;
    int i;
    i=0;
    //char olda[255];
    char *parseddataprogress;
    parseddataprogress=NULL;
    char *parseddataprogress1;
    parseddataprogress1=NULL;
    char *temp;
    temp=NULL;//first word
    char *temp1;
    temp1=NULL;
    int first;
    first=0;
    int first1;
    first1=0;
    char *temp3;//rest phrase
    temp3==NULL;
    char *temp4;
    temp4=NULL;
    int first2;
    first2=0;
    int temp3len;
    temp3len=0;
    while(1)
    {
        if(first)
        {
            temp=strtok_r(NULL," ",&parseddataprogress);
            if(temp==NULL)
            {
                if(temp3!=NULL)
                {
                    free(temp3);
                    temp3=NULL;
                }
                break;
            }
            if(parseddataprogress=='\0')
            {
                if(temp3!=NULL)
                {
                    free(temp3);
                    temp3=NULL;
                }
            	temp3=NULL;
            }
            else
            {
            	strcpy(temp3,parseddataprogress);
            }
            /*char *temp2;
            char *progress;
            if(first2)
            {
            	temp2=strtok_r(NULL," ",&progress);
				strcpy(temp3,progress);
            }
            else if(first2==0)
            {
            	temp2=strtok_r(temp4," ",&progress);
            	strcpy(temp3,progress);
            	first2=first2+1;
            }*/
            //temp2=strtok_r(NULL,"",&progress);
            //printf("progress %s\n",temp2);
            //getchar();
            //memmove(temp3, progress, strlen(progress));
            //temp3=(char *)&parseddataprogress;
            //temp3=realloc(temp3,strlen(parseddataprogress)+1);
            //strcpy(temp3,parseddataprogress);
        }
        else if(first==0)
        {
            temp=strtok_r(parseddata," ",&parseddataprogress);
            if(temp==NULL) break;
            //temp3=(char *)&parseddataprogress;
            if(parseddataprogress=='\0')
            {
                if(temp3!=NULL)
                {
                    free(temp3);
                    temp3=NULL;
                }
            	temp3=NULL;
            }
            else
            {
	            temp3=malloc(strlen(parseddataprogress)+1);
	            //temp3len=strlen(parseddataprogress)+1;
	            strcpy(temp3,parseddataprogress);
	            //memcpy(temp3,parseddataprogress,strlen(parseddataprogress)+1);
            }
            //temp4=malloc(strlen(parseddataprogress)+1);
            //strcpy(temp4,parseddataprogress);
            //printf("temp3 is %p\n",temp3);
            //getchar();
            first=first+1;
        }
        if(temp==NULL)
        {
            if(temp3!=NULL)
            {
                free(temp3);
                temp3=NULL;
            }
            break;
        }
        //printf("temp is %s\n",temp);
        //getchar();
        //sprintf(parseddataprogress,"%p",olda);
        //parseddataprogress1=parseddataprogress;
        phr=malloc(strlen(temp)+1);
        strcpy(phr,temp);
        int found;
        found=-1;
        indx=ht->bucket[search_hash(ht,phr)];
        found=binary_search_root(phr,indx->root,indx->root_num);//elegxoume an uparxei i leksi sto bucket to opoio pirame apo tin hash function
        if(found==-1)
        {
            free(phr);
            phr=NULL;
            continue;
        }
        int flg=0;
        if(indx->root[found].a_version>current_version || (indx->root[found].d_version<=current_version && indx->root[found].d_version!=0))
        {
        	//printf("1hs while\n");
            //free(phr);
           // phr=NULL;
            flg=1;
        	//continue;
        }
        if((indx->root[found].is_final=='Y' || indx->root[found].d_version>current_version) && flg==0)//an einai telikos komvos
        {
            check=-1;
            for(i=0;i<args->plithos_hash;i++)//elegxoume apo to bloom filter an tin exoume ksanaemfanisei
            {
                if((check_bit(bloom_vector,fnv_32_str(phr,i),args->size))==0)
                {
                    check=1;
                    break;
                }
            }
            if(check==1)
            {
                for(i=0;i<args->plithos_hash;i++)
                    set_bit(bloom_vector,fnv_32_str(phr,i),args->size);
                if(first_word==0)
                {
                    result=malloc(strlen(phr)+1);
                    strcpy(result,phr);
                }
                else
                {
                    result=realloc(result,strlen(result)+strlen(phr)+2);
                    strcat(result,"|");
                    strcat(result,phr);
                }

                first_word++;
                insert_hash_topk(hk,phr);
            }
        }
        trie_node *node;
        node=&(indx->root[found]);
        first1=0;
        temp1=strtok_r(temp3," ",&parseddataprogress1);
        while(temp1!=NULL)
        {
            found=-1;
            int counter1=0;
            char *phr1;
            //temp1=strtok_r(NULL," ",&parseddataprogress1);
            /*if(first1)
            {
                temp1=strtok_r(NULL," ",&parseddataprogress1);
            }
            else if(first1==0)
            {
                temp1=strtok_r(temp3," ",&parseddataprogress1);
                first1=first1+1;
            }*/
            //printf("%pp\n",parseddataprogress1);
            //if(temp1==NULL) break;
            //printf("temp1 is %s\n",temp1);
            //getchar();
            phr1=malloc(strlen(temp1)+1);
            strcpy(phr1,temp1);
            found=binary_search(phr1,node->children,node->child_num);//psaxnoume na doume an uparxei sto trie_node
            if(found==-1)
            {
                free(phr1);
                phr1=NULL;
                break;
            }
            int len_phr;
            int len_phr1;
            len_phr=strlen(phr);
            len_phr1=strlen(phr1);
            phr=realloc(phr,len_phr+1+len_phr1+1);
            strcat(phr," ");
            strcat(phr,phr1);
            //printf("phr is %s\n",phr);
            //getchar();
            if(node->children[found]->a_version>current_version || (node->children[found]->d_version<=current_version && node->children[found]->d_version!=0))
            {
            	//printf("2h while\n");
                node=node->children[found];
                free(phr1);
                phr1=NULL;
                temp1=strtok_r(NULL," ",&parseddataprogress1);
            	continue;
            }
            if(node->children[found]->is_final=='Y' || node->children[found]->d_version>current_version)//an einai telikos komvos
            {
                check=-1;
                for(i=0;i<args->plithos_hash;i++)//psaxnoume na doume an ton exoume ksanaepiskeftei me to bloom filter
                {
                    if((check_bit(bloom_vector,fnv_32_str(phr,i),args->size))==0)
                    {
                        check=1;
                        break;
                    }
                }
                if(check==1)
                {
                    for(i=0;i<args->plithos_hash;i++)
                        set_bit(bloom_vector,fnv_32_str(phr,i),args->size);
                    if(first_word==0)
                    {
                        result=malloc(strlen(phr)+1);
                        strcpy(result,phr);
                    }
                    else
                    {
                        result=realloc(result,strlen(result)+strlen(phr)+2);
                        strcat(result,"|");
                        strcat(result,phr);
                    }
                    first_word++;
                    insert_hash_topk(hk,phr);
                }
            }
            node=node->children[found];
            free(phr1);
            phr1=NULL;
            temp1=strtok_r(NULL," ",&parseddataprogress1);
        }
        //first1=0;
        free(phr);
        phr=NULL;
        //printf("%pp\n",parseddataprogress);
    }
    if(phr!=NULL)
    {
        free(phr);
        phr=NULL;
    }
    if(!first_word)
    {
        if(result!=NULL)
        {
            free(result);
            result=NULL;
        }
    	free(args);
    	free(bloom_vector);
    	return "-1";
    }
    //free(parseddataprogress);
    free(args);
    free(bloom_vector);
    return result;
}
void delete_trie_hash(hash_trie **ht)//diagrafei tou hash_trie
{
    int j;
    for(j=0;j<(*ht)->bucket_num;j++)
    {
        delete_trie(&((*ht)->bucket[j]));//gia kathe bucket kalei tin sunartisi delete_trie
    }
    free((*ht)->bucket);
    (*ht)->bucket=NULL;
    free((*ht));
    (*ht)=NULL;
}
void delete_trie(Index **indx)
{
    int i;
    for(i=0;i<(*indx)->root_num;i++)
    {
        delete_helper(&((*indx)->root[i]));
        if((*indx)->root[i].word_flag==1)
        {
            free((*indx)->root[i].word);
            (*indx)->root[i].word=NULL;
        }
        free((*indx)->root[i].children);
        (*indx)->root[i].children=NULL;
    }
    free((*indx)->root);
    (*indx)->root=NULL;
    free((*indx));
    (*indx)=NULL;
}
void delete_helper(trie_node *node)
{
    if(!node->child_num) return;
    int i;
    for(i=0;i<node->child_num;i++)
    {
        delete_helper(node->children[i]);
        if(node->children[i]->word_flag==1)
        {
            free(node->children[i]->word);
            node->children[i]->word=NULL;
        }
        free(node->children[i]->children);
        node->children[i]->children=NULL;
        free(node->children[i]);
        node->children[i]=NULL;
    }
}
/*void InsertionSort (trie_node** a, int n)
{
    int i, j = 0;
    trie_node* key;

    for (i = 1; i < n; j = i, i++)
    {
        key = a[i];
        if(a[j]->word_flag==0 && key->word_flag==0)
        {
        	while (j >= 0 && strcmp(a[j]->static_word ,key->static_word)>0)
        		j--;
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node*) * ((i - 1) - j));
            a[j + 1] = key;
        }
        else if(a[j]->word_flag==0 && key->word_flag==1)
        {
        	while (j >= 0 && strcmp(a[j]->static_word ,key->word)>0)
        		j--;
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node*) * ((i - 1) - j));
            a[j + 1] = key;
        }
        else if(a[j]->word_flag==1 && key->word_flag==0)
        {
        	while (j >= 0 && strcmp(a[j]->word ,key->static_word)>0)
        		j--;
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node*) * ((i - 1) - j));
            a[j + 1] = key;
        }
        else if(a[j]->word_flag==1 && key->word_flag==1)
        {
        	while (j >= 0 && strcmp(a[j]->word ,key->word)>0)
        		j--;
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node*) * ((i - 1) - j));
            a[j + 1] = key;
        }
    }
}*/
void InsertionSort (trie_node** a, int n)
{
    int i, j = 0;
    trie_node* key;

    for (i = 1; i < n; j = i, i++) {
        key = a[i];
        while(j>=0)
        {
            if(a[j]->word_flag==0 && key->word_flag==0)
            {
                if(strcmp(a[j]->static_word ,key->static_word)<=0)
                {
                    break;
                }
            }
            else if(a[j]->word_flag==1 && key->word_flag==0)
            {
                if(strcmp(a[j]->word ,key->static_word)<=0)
                {
                    break;
                }
            }
            else if(a[j]->word_flag==0 && key->word_flag==1)
            {
                if(strcmp(a[j]->static_word ,key->word)<=0)
                {
                    break;
                }
            }
            else if(a[j]->word_flag==1 && key->word_flag==1)
            {
                if(strcmp(a[j]->word ,key->word)<=0)
                {
                    break;
                }
            }
            j--;
        }
        memmove (a + (j + 2), a + (j + 1), sizeof (trie_node *) * ((i - 1) - j));
        a[j + 1] = key;
    }
}
/*void InsertionSort_Root (trie_node* a, int n)
{
    int i, j = 0;
    trie_node key;

    for (i = 1; i < n; j = i, i++) {
        key = a[i];
        if(a[j].word_flag==0 && key.word_flag==0)
        {
            while (j >= 0 && strcmp(a[j].static_word ,key.static_word)>0)
                j--;
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node) * ((i - 1) - j));
            a[j + 1] = key;
        }
        else if(a[j].word_flag==1 && key.word_flag==0)
        {
            printf("i am here7\n");
            //while (j >= 0 && strcmp(a[j].word ,key.static_word)>0)
            //    j--;
            while (j >= 0 && strcmp(a[j].word ,key.static_word)>0)
            {
                printf("---->%d %s %s %s\n",j,a[j].word,a[j].static_word,key.static_word);
                j--;
                printf("---->%d %s %s %s\n",j,a[j].word,a[j].static_word,key.static_word);
            }
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node) * ((i - 1) - j));
            a[j + 1] = key;
        }
        else if(a[j].word_flag==0 && key.word_flag==1)
        {
            while (j >= 0 && strcmp(a[j].static_word ,key.word)>0)
                j--;
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node) * ((i - 1) - j));
            a[j + 1] = key;
        }
        else if(a[j].word_flag==1 && key.word_flag==1)
        {
            while (j >= 0 && strcmp(a[j].word ,key.word)>0)
                j--;
            memmove (a + (j + 2), a + (j + 1), sizeof (trie_node) * ((i - 1) - j));
            a[j + 1] = key;
        }
    }
}*/
void InsertionSort_Root (trie_node* a, int n)
{
    int i, j = 0;
    trie_node key;

    for (i = 1; i < n; j = i, i++) {
        key = a[i];
        while(j>=0)
        {
            if(a[j].word_flag==0 && key.word_flag==0)
            {
                if(strcmp(a[j].static_word ,key.static_word)<=0)
                {
                    break;
                }
            }
            else if(a[j].word_flag==1 && key.word_flag==0)
            {
                if(strcmp(a[j].word ,key.static_word)<=0)
                {
                    break;
                }
            }
            else if(a[j].word_flag==0 && key.word_flag==1)
            {
                if(strcmp(a[j].static_word ,key.word)<=0)
                {
                    break;
                }
            }
            else if(a[j].word_flag==1 && key.word_flag==1)
            {
                if(strcmp(a[j].word ,key.word)<=0)
                {
                    break;
                }
            }
            j--;
        }
        memmove (a + (j + 2), a + (j + 1), sizeof (trie_node) * ((i - 1) - j));
        a[j + 1] = key;
    }
}
void top_hash_rem(hash_keeper **hk)//diagrafi tis domis gia ta top k
{
    int i;
    i=0;
    int j;
    j=0;
    for(i=0;i<(*hk)->bucket_num;i++)
    {
        for(j=0;j<(*hk)->bucket[i]->cnt;j++)
        {
            free((*hk)->bucket[i]->elems[j].name);
            (*hk)->bucket[i]->elems[j].name=NULL;
        }
        free((*hk)->bucket[i]->elems);
        (*hk)->bucket[i]->elems=NULL;
        free((*hk)->bucket[i]);
        (*hk)->bucket[i]=NULL;
    }
    free((*hk)->bucket);
    (*hk)->bucket=NULL;
    free((*hk));
    (*hk)=NULL;
}
void findk(hash_keeper *hk,char *str)//kaleite otan paroume apo to arxeio F
{
    int i;
    i=0;
    int ln;
    if(str==NULL)//an den mas exoun dosei arithmo tote apla adeiazoume ta bucket tis domis
    {
        int j;
        for(i=0;i<hk->bucket_num;i++)
        {
            for(j=0;j<hk->bucket[i]->cnt;j++)
            {
                free(hk->bucket[i]->elems[j].name);
                hk->bucket[i]->elems[j].name=NULL;
                hk->bucket[i]->elems[j].checked=0;
            }
            hk->bucket[i]->cnt=0;
        }
        return;
    }
    ln=strlen(str);
    for(i=0;i<ln;i++)
    {
        if(str[i]>=48 && str[i]<=57)
        {

        }
        else
        {
            return;
        }
    }
    int j;
    j=0;
    int maxnum;
    bck_elem *maxbckelem;
    int first;
    first=0;
    int num;
    num=atoi(str);
    int k;
    k=0;
    int first_string=0;
    for(k=0;k<num;k++)//psaxnoume na vroume ta top k apotelesmata
    {
        for(i=0;i<hk->bucket_num;i++)
        {
            for(j=0;j<hk->bucket[i]->cnt;j++)
            {
                if(first==0)
                {
                    if(hk->bucket[i]->elems[j].checked>0)
                    {
                        first=first+1;
                        maxnum=hk->bucket[i]->elems[j].checked;
                        maxbckelem=&(hk->bucket[i]->elems[j]);
                    }
                }
                else if(first!=0)
                {
                    if(hk->bucket[i]->elems[j].checked>0)
                    {
                        if(hk->bucket[i]->elems[j].checked>maxnum)
                        {
                            maxnum=hk->bucket[i]->elems[j].checked;
                            maxbckelem=&(hk->bucket[i]->elems[j]);
                        }
                        else if(hk->bucket[i]->elems[j].checked==maxnum)
                        {
                            if(strcmp(hk->bucket[i]->elems[j].name,maxbckelem->name)<0)
                            {
                                maxbckelem=&(hk->bucket[i]->elems[j]);
                            }
                        }
                    }
                }
            }
        }
        if(first==0) break; //an den vrike apla stamataei
        first=0;
        if(first_string==0)
        {
            first_string=first_string+1;
            printf("Top: %s",maxbckelem->name);
            maxbckelem->checked=-1;
        }
        else
        {
            printf("|%s",maxbckelem->name);
            maxbckelem->checked=-1;
        }
    }
    if(first_string)
        printf("\n");
    for(i=0;i<hk->bucket_num;i++)//adeiazoume ta bucket
    {
        for(j=0;j<hk->bucket[i]->cnt;j++)
        {
            free(hk->bucket[i]->elems[j].name);
            hk->bucket[i]->elems[j].name=NULL;
            hk->bucket[i]->elems[j].checked=0;
        }
        hk->bucket[i]->cnt=0;
    }
}

int binary_search_top(char* phrase,bck_elem* node,int length)//binary search gia tin domi mas
{
	 int  first, last, middle;
	 if(phrase==NULL)return -1;
	 first = 0;
	 last = length - 1;
	 middle = (first+last)/2;
	 while (first <= last)
	 {
		 if ((strcmp(node[first].name ,phrase))==0)
			 	 return first;
		 if ((strcmp(node[last].name ,phrase))==0)
			 	 return last;
	   if ((strcmp(node[middle].name ,phrase))<0)
	         first = middle + 1;
	      else if ((strcmp(node[middle].name ,phrase))==0)
	      {
	    	  return middle;
	         break;
	      }
	      else
	         last = middle - 1;

	      middle = (first + last)/2;
	 }
	 if (first > last)
		 return -1;
}

void initialize_hash_topk(hash_keeper **hk)//arxikopoiisi tis domis mas i opoia einai ena hash_table me linear hashing tin opoia epileksame afou exei eisaigogi O(1)
{
    (*hk)=malloc(sizeof(hash_keeper));
    (*hk)->bucket_num=10;
    (*hk)->pointer=0;
    (*hk)->round=10;
    (*hk)->bucket=malloc((*hk)->bucket_num*sizeof(bck *));
    int i;
    i=0;
    for(i=0;i<(*hk)->bucket_num;i++)//arxikopoiisi ton bck
    {
        (*hk)->bucket[i]=malloc(sizeof(bck));
        (*hk)->bucket[i]->cnt=0;
        (*hk)->bucket[i]->bck_size=50;
        (*hk)->bucket[i]->elems=malloc((*hk)->bucket[i]->bck_size*sizeof(bck_elem));
    }
}

int insertionSort_top(bck_elem* array, int length)//insertion sort gia ta bucket
{
   int i, j;
   bck_elem temp;
   for (i = 1 ; i < length ; i++)
   {
       j = i;

       while ( j > 0 && (strcmp(array[j].name,array[j-1].name))<0)
       {
    	   temp.name=array[j].name;
    	   temp.checked=array[j].checked;
    	   array[j].name=array[j-1].name;
    	   array[j].checked=array[j-1].checked;
    	   array[j-1].name=temp.name;
    	   array[j-1].checked=temp.checked;
    	   j--;
       }
    }
   return j;
}

void insert_hash_topk(hash_keeper *hk,char *phrase)//eisagogi enos stoixeio stin domi(Linear Hashing)
{
    //printf("i am here\n");
    //getchar();
    unsigned long md;
    md=hk->round;
    unsigned long hash_val;
    unsigned long hash_val_temp;
    hash_val=0;
    hash_val_temp=0;
    hash_val_temp=hash_fun(phrase);
    hash_val=hash_val_temp%md;
    if(hash_val>=hk->pointer)//an to hash number tou stoixeio einai o arithmos tou pointer i megalutero
    {
        bck *curbck;
        curbck=hk->bucket[hash_val];
        int found;
        found=-1;;
        found=binary_search_top(phrase,curbck->elems,curbck->cnt);
        if(found>=0)//an uparxei apla afksise to checked
        {
            curbck->elems[found].checked++;
        }
        else if(found==-1)//and den uparxei
        {
            int overflow;
            overflow=0;

            if(curbck->cnt==curbck->bck_size)//elegxoume an einai gemato to bucket
            {
                overflow=overflow+1;
                curbck->bck_size*=2;
                curbck->elems=realloc(curbck->elems,curbck->bck_size*sizeof(bck_elem));
            }
            curbck->cnt=curbck->cnt+1;//eisagogi tis neas leksis
            curbck->elems[curbck->cnt-1].name=malloc(strlen(phrase)+1);
            strcpy(curbck->elems[curbck->cnt-1].name,phrase);
            curbck->elems[curbck->cnt-1].checked=0;
            curbck->elems[curbck->cnt-1].checked=curbck->elems[curbck->cnt-1].checked+1;
            insertionSort_top(curbck->elems,curbck->cnt);
            if(overflow>0)//an epathe overflow tote ginetai linear hashing
            {
                md*=2;
                bck_elem *temp_array;//apothikeuoume prosorina ta stoixeia tou bucket tou pointer se ena pinaka
                int temp_counter;
                temp_counter=hk->bucket[hk->pointer]->cnt;
                temp_array=malloc(temp_counter*sizeof(bck_elem));
                int j;
                j=0;
                for(j=0;j<temp_counter;j++)
                {
                    temp_array[j].name=hk->bucket[hk->pointer]->elems[j].name;
                    temp_array[j].checked=hk->bucket[hk->pointer]->elems[j].checked;
                }
                hk->bucket[hk->pointer]->cnt=0;
                hk->bucket_num=hk->bucket_num+1;
                hk->bucket=realloc(hk->bucket,hk->bucket_num*sizeof(bck *));
                hk->bucket[hk->bucket_num-1]=malloc(sizeof(bck));
                hk->bucket[hk->bucket_num-1]->cnt=0;
                hk->bucket[hk->bucket_num-1]->bck_size=50;
                hk->bucket[hk->bucket_num-1]->elems=malloc(hk->bucket[hk->bucket_num-1]->bck_size*sizeof(bck_elem));
                for(j=0;j<temp_counter;j++)//kai meta ta moirazoume anamesa sto bucket tou pointer kai tou neou bucket
                {
                    unsigned long newhashnum1;
                    newhashnum1=hash_fun(temp_array[j].name);
                    int newhashnum=newhashnum1%md;
                    if(hk->bucket[newhashnum]->cnt == hk->bucket[newhashnum]->bck_size)
                    {
                        hk->bucket[newhashnum]->bck_size*=2;
                        hk->bucket[newhashnum]->elems=realloc(hk->bucket[newhashnum]->elems,hk->bucket[newhashnum]->bck_size*sizeof(bck_elem));
                    }
                    hk->bucket[newhashnum]->cnt=hk->bucket[newhashnum]->cnt+1;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].name=temp_array[j].name;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].checked=temp_array[j].checked;
                }
                insertionSort_top(hk->bucket[hk->pointer]->elems,hk->bucket[hk->pointer]->cnt);
                insertionSort_top(hk->bucket[hk->pointer+hk->round]->elems,hk->bucket[hk->pointer+hk->round]->cnt);
                hk->pointer=(hk->pointer+1)%hk->round;
                if(hk->pointer==0)
                    hk->round=hk->bucket_num;
                free(temp_array);
                temp_array=NULL;
            }
        }
    }
    else if(hash_val<hk->pointer)//an to hash number einai mikrotero tou arithmou tou bucket tou pointer
    {
        md*=2;
        hash_val=hash_val_temp%md;
        bck *curbck;
        curbck=hk->bucket[hash_val];
        int found;
        found=-1;
        found=binary_search_top(phrase,curbck->elems,curbck->cnt);
        int k;
        k=0;
        if(found>=0)
        {
            curbck->elems[found].checked=curbck->elems[found].checked+1;
        }
        else if(found==-1)
        {
            int overflow;
            overflow=0;

            if(curbck->cnt==curbck->bck_size)
            {
                overflow=overflow+1;
                curbck->bck_size*=2;
                curbck->elems=realloc(curbck->elems,curbck->bck_size*sizeof(bck_elem));
            }
            curbck->cnt=curbck->cnt+1;
            curbck->elems[curbck->cnt-1].name=malloc(strlen(phrase)+1);
            strcpy(curbck->elems[curbck->cnt-1].name,phrase);
            curbck->elems[curbck->cnt-1].checked=0;
            curbck->elems[curbck->cnt-1].checked=curbck->elems[curbck->cnt-1].checked+1;
            insertionSort_top(curbck->elems,curbck->cnt);
            if(overflow>0)
            {
                //md*=2;
                bck_elem *temp_array;
                int temp_counter;
                temp_counter=hk->bucket[hk->pointer]->cnt;
                temp_array=malloc(temp_counter*sizeof(bck_elem));
                int j;
                j=0;
                for(j=0;j<temp_counter;j++)
                {
                    temp_array[j].name=hk->bucket[hk->pointer]->elems[j].name;
                    temp_array[j].checked=hk->bucket[hk->pointer]->elems[j].checked;
                }
                hk->bucket[hk->pointer]->cnt=0;
                hk->bucket_num=hk->bucket_num+1;
                hk->bucket=realloc(hk->bucket,hk->bucket_num*sizeof(bck *));
                hk->bucket[hk->bucket_num-1]=malloc(sizeof(bck));
                hk->bucket[hk->bucket_num-1]->cnt=0;
                hk->bucket[hk->bucket_num-1]->bck_size=50;
                hk->bucket[hk->bucket_num-1]->elems=malloc(hk->bucket[hk->bucket_num-1]->bck_size*sizeof(bck_elem));
                for(j=0;j<temp_counter;j++)
                {
                    unsigned long newhashnum1;
                    newhashnum1=hash_fun(temp_array[j].name);
                    int newhashnum=newhashnum1%md;
                    if(hk->bucket[newhashnum]->cnt == hk->bucket[newhashnum]->bck_size)
                    {
                        hk->bucket[newhashnum]->bck_size*=2;
                        hk->bucket[newhashnum]->elems=realloc(hk->bucket[newhashnum]->elems,hk->bucket[newhashnum]->bck_size*sizeof(bck_elem));
                    }
                    hk->bucket[newhashnum]->cnt=hk->bucket[newhashnum]->cnt+1;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].name=temp_array[j].name;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].checked=temp_array[j].checked;
                }
                insertionSort_top(hk->bucket[hk->pointer]->elems,hk->bucket[hk->pointer]->cnt);
                insertionSort_top(hk->bucket[hk->pointer+hk->round]->elems,hk->bucket[hk->pointer+hk->round]->cnt);
                hk->pointer=(hk->pointer+1)%hk->round;
                if(hk->pointer==0)
                    hk->round=hk->bucket_num;
                free(temp_array);
                temp_array=NULL;
            }
        }
    }
}

void merge_hash_topk(hash_keeper *hk,char *phrase,int number)
{
    unsigned long md;
    md=hk->round;
    unsigned long hash_val;
    unsigned long hash_val_temp;
    hash_val=0;
    hash_val_temp=0;
    hash_val_temp=hash_fun(phrase);
    hash_val=hash_val_temp%md;
    if(hash_val>=hk->pointer)//an to hash number tou stoixeio einai o arithmos tou pointer i megalutero
    {
        bck *curbck;
        curbck=hk->bucket[hash_val];
        int found;
        found=-1;;
        found=binary_search_top(phrase,curbck->elems,curbck->cnt);
        if(found>=0)//an uparxei apla afksise to checked
        {
            curbck->elems[found].checked=curbck->elems[found].checked+number;
        }
        else if(found==-1)//and den uparxei
        {
            int overflow;
            overflow=0;

            if(curbck->cnt==curbck->bck_size)//elegxoume an einai gemato to bucket
            {
                overflow=overflow+1;
                curbck->bck_size*=2;
                curbck->elems=realloc(curbck->elems,curbck->bck_size*sizeof(bck_elem));
            }
            curbck->cnt=curbck->cnt+1;//eisagogi tis neas leksis
            curbck->elems[curbck->cnt-1].name=malloc(strlen(phrase)+1);
            strcpy(curbck->elems[curbck->cnt-1].name,phrase);
            curbck->elems[curbck->cnt-1].checked=0;
            curbck->elems[curbck->cnt-1].checked=curbck->elems[curbck->cnt-1].checked+number;
            insertionSort_top(curbck->elems,curbck->cnt);
            if(overflow>0)//an epathe overflow tote ginetai linear hashing
            {
                md*=2;
                bck_elem *temp_array;//apothikeuoume prosorina ta stoixeia tou bucket tou pointer se ena pinaka
                int temp_counter;
                temp_counter=hk->bucket[hk->pointer]->cnt;
                temp_array=malloc(temp_counter*sizeof(bck_elem));
                int j;
                j=0;
                for(j=0;j<temp_counter;j++)
                {
                    temp_array[j].name=hk->bucket[hk->pointer]->elems[j].name;
                    temp_array[j].checked=hk->bucket[hk->pointer]->elems[j].checked;
                }
                hk->bucket[hk->pointer]->cnt=0;
                hk->bucket_num=hk->bucket_num+1;
                hk->bucket=realloc(hk->bucket,hk->bucket_num*sizeof(bck *));
                hk->bucket[hk->bucket_num-1]=malloc(sizeof(bck));
                hk->bucket[hk->bucket_num-1]->cnt=0;
                hk->bucket[hk->bucket_num-1]->bck_size=50;
                hk->bucket[hk->bucket_num-1]->elems=malloc(hk->bucket[hk->bucket_num-1]->bck_size*sizeof(bck_elem));
                for(j=0;j<temp_counter;j++)//kai meta ta moirazoume anamesa sto bucket tou pointer kai tou neou bucket
                {
                    unsigned long newhashnum1;
                    newhashnum1=hash_fun(temp_array[j].name);
                    int newhashnum=newhashnum1%md;
                    if(hk->bucket[newhashnum]->cnt == hk->bucket[newhashnum]->bck_size)
                    {
                        hk->bucket[newhashnum]->bck_size*=2;
                        hk->bucket[newhashnum]->elems=realloc(hk->bucket[newhashnum]->elems,hk->bucket[newhashnum]->bck_size*sizeof(bck_elem));
                    }
                    hk->bucket[newhashnum]->cnt=hk->bucket[newhashnum]->cnt+1;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].name=temp_array[j].name;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].checked=temp_array[j].checked;
                }
                insertionSort_top(hk->bucket[hk->pointer]->elems,hk->bucket[hk->pointer]->cnt);
                insertionSort_top(hk->bucket[hk->pointer+hk->round]->elems,hk->bucket[hk->pointer+hk->round]->cnt);
                hk->pointer=(hk->pointer+1)%hk->round;
                if(hk->pointer==0)
                    hk->round=hk->bucket_num;
                free(temp_array);
                temp_array=NULL;
            }
        }
    }
    else if(hash_val<hk->pointer)//an to hash number einai mikrotero tou arithmou tou bucket tou pointer
    {
        md*=2;
        hash_val=hash_val_temp%md;
        bck *curbck;
        curbck=hk->bucket[hash_val];
        int found;
        found=-1;
        found=binary_search_top(phrase,curbck->elems,curbck->cnt);
        int k;
        k=0;
        if(found>=0)
        {
            curbck->elems[found].checked=curbck->elems[found].checked+number;
        }
        else if(found==-1)
        {
            int overflow;
            overflow=0;

            if(curbck->cnt==curbck->bck_size)
            {
                overflow=overflow+1;
                curbck->bck_size*=2;
                curbck->elems=realloc(curbck->elems,curbck->bck_size*sizeof(bck_elem));
            }
            curbck->cnt=curbck->cnt+1;
            curbck->elems[curbck->cnt-1].name=malloc(strlen(phrase)+1);
            strcpy(curbck->elems[curbck->cnt-1].name,phrase);
            curbck->elems[curbck->cnt-1].checked=0;
            curbck->elems[curbck->cnt-1].checked=curbck->elems[curbck->cnt-1].checked+number;
            insertionSort_top(curbck->elems,curbck->cnt);
            if(overflow>0)
            {
                //md*=2;
                bck_elem *temp_array;
                int temp_counter;
                temp_counter=hk->bucket[hk->pointer]->cnt;
                temp_array=malloc(temp_counter*sizeof(bck_elem));
                int j;
                j=0;
                for(j=0;j<temp_counter;j++)
                {
                    temp_array[j].name=hk->bucket[hk->pointer]->elems[j].name;
                    temp_array[j].checked=hk->bucket[hk->pointer]->elems[j].checked;
                }
                hk->bucket[hk->pointer]->cnt=0;
                hk->bucket_num=hk->bucket_num+1;
                hk->bucket=realloc(hk->bucket,hk->bucket_num*sizeof(bck *));
                hk->bucket[hk->bucket_num-1]=malloc(sizeof(bck));
                hk->bucket[hk->bucket_num-1]->cnt=0;
                hk->bucket[hk->bucket_num-1]->bck_size=50;
                hk->bucket[hk->bucket_num-1]->elems=malloc(hk->bucket[hk->bucket_num-1]->bck_size*sizeof(bck_elem));
                for(j=0;j<temp_counter;j++)
                {
                    unsigned long newhashnum1;
                    newhashnum1=hash_fun(temp_array[j].name);
                    int newhashnum=newhashnum1%md;
                    if(hk->bucket[newhashnum]->cnt == hk->bucket[newhashnum]->bck_size)
                    {
                        hk->bucket[newhashnum]->bck_size*=2;
                        hk->bucket[newhashnum]->elems=realloc(hk->bucket[newhashnum]->elems,hk->bucket[newhashnum]->bck_size*sizeof(bck_elem));
                    }
                    hk->bucket[newhashnum]->cnt=hk->bucket[newhashnum]->cnt+1;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].name=temp_array[j].name;
                    hk->bucket[newhashnum]->elems[hk->bucket[newhashnum]->cnt-1].checked=temp_array[j].checked;
                }
                insertionSort_top(hk->bucket[hk->pointer]->elems,hk->bucket[hk->pointer]->cnt);
                insertionSort_top(hk->bucket[hk->pointer+hk->round]->elems,hk->bucket[hk->pointer+hk->round]->cnt);
                hk->pointer=(hk->pointer+1)%hk->round;
                if(hk->pointer==0)
                    hk->round=hk->bucket_num;
                free(temp_array);
                temp_array=NULL;
            }
        }
    }
}

void merge_everything(hash_keeper **hk,int tm)
{
    int i;
    int j;
    int k;
    i=0;
    j=0;
    k=0;
    for(i=1;i<tm;i++)
    {
        for(j=0;j<hk[i]->bucket_num;j++)
        {
            for(k=0;k<hk[i]->bucket[j]->cnt;k++)
            {
                merge_hash_topk(hk[0],hk[i]->bucket[j]->elems[k].name,hk[i]->bucket[j]->elems[k].checked);
            }
        }
        for(j=0;j<hk[i]->bucket_num;j++)//adeiazoume ta bucket
        {
            for(k=0;k<hk[i]->bucket[j]->cnt;k++)
            {
                free(hk[i]->bucket[j]->elems[k].name);
                hk[i]->bucket[j]->elems[k].name=NULL;
                hk[i]->bucket[j]->elems[k].checked=0;
            }
            hk[i]->bucket[j]->cnt=0;
        }
    }
}
