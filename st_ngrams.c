#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "st_ngrams.h"
#define VECTOR_SIZE 5000
#define HASH_NUMBER 10
#define thread_number 4
void compress_hash(hash_trie *ht)
{
    int i;
    i=0;
    for(i=0;i<ht->bucket_num;i++)
    {
        compress(ht->bucket[i],ht->bucket[i]->root,NULL);
    }
}
void compress(Index* indx,trie_node* parent,trie_node* current)
{
	trie_node **temp;
	int paidia;
	int i;
	if(current==NULL)   			//vriskomai sto root//
	{
		if(indx->root_num>=2)		//an to root exei panw apo 2 paidia//
		{
			for(i=0;i<indx->root_num;i++)
			{
				compress(indx,&parent[i],&parent[i]);
			}
			return;
		}
		else if (indx->root_num==1)	//an to root exei 1 paidi-->compress//
		{
			if(indx->root->child_num==0)	//an to root einai sketo xwris paidia den exei na kanei kati compress kai teleiwnei//
				return;
			if(indx->root->child_num>=2)
				for(i=0;i<indx->root->child_num;i++)
				{
					compress(indx,&parent[i],&parent[i]);
				}
			else
			{
				current=indx->root->children[0];
				temp=parent->children;
				paidia=parent->child_num;
				compress_nodes(parent,current);
				delete_static_node(&current,temp,paidia);
				current=parent;
				compress(indx,parent,current);
				return;
			}
		}
		else					//an to root den exei paidia//
		{
			return;
		}
	}
	else if(current->child_num>=2)
	{

		for(i=0;i<current->child_num;i++)
		{
			compress(indx,current->children[i],current->children[i]);
		}
		return;
	}
	else if(current->child_num==1)
	{
		current=current->children[0];
		temp=parent->children;
		paidia=parent->child_num;
		compress_nodes(parent,current);
		delete_static_node(&current,temp,paidia);
		current=parent;
		compress(indx,parent,current);
		return;
	}
	else
	{
		return;
	}
}
void compress_nodes(trie_node* parent,trie_node* current)
{
	if(parent->word_flag==1 && current->word_flag==1)
	{
		parent->word=realloc(parent->word,strlen(parent->word)+strlen(current->word)+2);
		strcat(parent->word,current->word);
		parent->word_flag=1;
	}
	else if(parent->word_flag==0 && current->word_flag==1)
	{
		if(strlen(parent->static_word)+strlen(current->word)<=50)
			strcat(parent->static_word,current->word);
		else
		{
			parent->word=malloc(strlen(parent->static_word)+strlen(current->word)+2);
			strcpy(parent->word,parent->static_word);
			strcat(parent->word,current->word);
			parent->word_flag=1;
		}
	}
	else if(parent->word_flag==1 && current->word_flag==0)
	{
		parent->word=realloc(parent->word,strlen(parent->word)+strlen(current->static_word)+2);
		strcat(parent->word,current->static_word);
	}
	else if(parent->word_flag==0 && current->word_flag==0)
	{
		if(strlen(parent->static_word)+strlen(current->static_word)<=50)
		{
			strcat(parent->static_word,current->static_word);
		}
		else
		{
			parent->word=malloc(strlen(parent->static_word)+strlen(current->static_word)+2);
			strcpy(parent->word,parent->static_word);
			strcat(parent->word,current->static_word);
			parent->word_flag=1;
		}
	}
	parent->array_size++;
	parent->array=realloc(parent->array,parent->array_size*sizeof(signed short));
	parent->array[parent->array_size-1]=current->array[0];
	parent->child_num=current->child_num;
	parent->children=current->children;
}
void delete_static_node(trie_node** temp,trie_node** parent,int paidia)
{
	int i;
	for(i=0;i<paidia;i++)
	{
		free(parent[i]->array);
		parent[i]->array=NULL;
		free(parent[i]->fores);
		parent[i]->fores=NULL;
		if(parent[i]->word_flag==1)
		{
			free(parent[i]->word);
			parent[i]->word=NULL;
		}
		free(parent[i]);
		parent[i]=NULL;
	}
	free(parent);
	parent=NULL;
}
void st_delete_hash(hash_trie **ht)
{
    int i;
    i=0;
    for(i=0;i<(*ht)->bucket_num;i++)
        st_delete_trie(&((*ht)->bucket[i]));
    free((*ht)->bucket);
    (*ht)->bucket=NULL;
    free((*ht));
    (*ht)=NULL;
}
void st_delete_trie(Index **indx)
{
    int i;
    for(i=0;i<(*indx)->root_num;i++)
    {
        st_delete_helper(&((*indx)->root[i]));
        free((*indx)->root[i].array);
        (*indx)->root[i].array=NULL;
        free((*indx)->root[i].fores);
        (*indx)->root[i].fores=NULL;
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
void st_delete_helper(trie_node *node)
{
    if(!node->child_num) return;
    int i;
    for(i=0;i<node->child_num;i++)
    {
        st_delete_helper(node->children[i]);
        free(node->children[i]->array);
        node->children[i]->array=NULL;
        free(node->children[i]->fores);
        node->children[i]->fores=NULL;
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
int st_binary_search_root(char *phrase,trie_node *node,int length)
{
	 int  first, last, middle;
	 if(phrase==NULL)return -1;
	 char temp[2000];
	 first = 0;
	 last = length - 1;
	 middle = (first+last)/2;
	 while (first <= last)
	 {
		 memset(temp, 0,2000 );
		 if(node[first].word_flag==1)
		 {
			 strncpy(temp,node[first].word,abs(node[first].array[0]));
		 }
		 else
		 {
			 strncpy(temp,node[first].static_word,abs(node[first].array[0]));
		 }
	 	 if (!strcmp(temp,phrase))
	 	 {
		 	 return first;
	 	 }
		memset(temp, 0, 2000);
		if(node[last].word_flag==1)
		{
			strncpy(temp,node[last].word,abs(node[last].array[0]));
		}
		else
		{
			strncpy(temp,node[last].static_word,abs(node[last].array[0]));
		}
	 	if (!strcmp(temp,phrase))
	 	{
		 	 return last;
	 	}
		memset(temp, 0, 2000);
		if(node[middle].word_flag==1)
		{
			strncpy(temp,node[middle].word,abs(node[middle].array[0]));
		}
		else
		{
			strncpy(temp,node[middle].static_word,abs(node[middle].array[0]));
		}
        if ((strcmp(temp ,phrase))<0 )
        {
            first = middle + 1;
        }
        else if ((strcmp(temp ,phrase))==0)
        {
            return middle;
        }
        else
        {
            last = middle - 1;
        }
	    middle = (first + last)/2;
	 }
	 if (first > last)
	 {
        return -1;
	 }
}
int st_binary_search(char* phrase,trie_node** node,int length)
{
	 int  first, last, middle;
	 if(phrase==NULL)return -1;
	 char temp[100];
	 first = 0;
	 last = length - 1;
	 middle = (first+last)/2;
	 while (first <= last)
	 {
		 memset(temp, 0, 100);
		 if(node[first]->word_flag==1)
		 {
			 strncpy(temp,node[first]->word,abs(node[first]->array[0]));
		 }
		 else
		 {
			 strncpy(temp,node[first]->static_word,abs(node[first]->array[0]));
		 }
	 	 if (strcmp(temp,phrase)==0)
	 		 return first;
		 memset(temp, 0, 100);
		 if(node[last]->word_flag==1)
			 strncpy(temp,node[last]->word,abs(node[last]->array[0]));
		 else
		 {
			 strncpy(temp,node[last]->static_word,abs(node[last]->array[0]));
		 }
	 	 if (strcmp(temp,phrase)==0)
	 		 return last;
		 memset(temp, 0, 100);
		 if(node[middle]->word_flag==1)
		 {
			 strncpy(temp,node[middle]->word,abs(node[middle]->array[0]));
		 }
		 else
		 {
			 strncpy(temp,node[middle]->static_word,abs(node[middle]->array[0]));
		 }
		 if ((strcmp(temp ,phrase))<0 )
		 {
            first = middle + 1;
         }
		 else if ((strcmp(temp,phrase))==0 )
		 {
			 return middle;
			 break;
		 }
		 else
		 {
            last = middle - 1;
		 }
	     middle = (first + last)/2;
	 }
	 if (first > last)
	 {
		 return -1;
	 }
}
char* static_search(hash_trie *ht,char* fp,hash_keeper* hk,int id)
{
	int i;
	int found=-1;
	char *subphrase=NULL;
	int glob=0;
	char* bloom_vector;
	Arguments* args;
	args=malloc(sizeof(Arguments));
	initialize_args(args,VECTOR_SIZE,HASH_NUMBER);
	bloom_vector=malloc(args->size*sizeof(char));
	initialize_bloom_filter(args,bloom_vector);
	char* array;
	char* result;
	Index *indx;
    char *array_progress;
    array=strtok_r(fp," ",&array_progress);
	while(array!=NULL)
    {
            found=-1;
            indx=ht->bucket[search_hash(ht,array)];
            found=st_binary_search_root(array,indx->root,indx->root_num);		//PSAXNOUME NA VROUME AN H EPILEGMENH LEKSH VRISKETAI STO ROOT//
            if(found>=0)
                if(indx->root[found].array_size>1)
                {
                    if(indx->root[found].fores[id]==indx->root[found].array_size)indx->root[found].fores[id]=1;
                    else
                        indx->root[found].fores[id]++;
                }
            if(found==-1)			//AN DEN UPARXEI STO ROOT SUNEXIZOUME STHN EPOMENH//
            {
                array=strtok_r(NULL," ",&array_progress);
                continue;
            }
            else				//AN UPARXEI STO ROOT H PRWTH LEKSH//
            {
                subphrase=malloc(strlen(array)+1);    //XRHSIMOPOIOUME MIA VOITHITIKH METAVLHTH POU KRATAME TO APOTELESMA KATHE LEKSHS POU PAIRNOUME APO TO QUESTION//
                strcpy(subphrase,array);
                if(indx->root[found].array[0]>0)
                {
                    int check=-1;
                    for(i=0;i<args->plithos_hash;i++)
                    {
                        if((check_bit(bloom_vector,fnv_32_str(subphrase,i),args->size))==0)
                        {
                            check=1;
                            break;
                        }
                    }
                    if(check==1)
                    {
                        for(i=0;i<args->plithos_hash;i++)
                            set_bit(bloom_vector,fnv_32_str(subphrase,i),args->size);
                        if(glob==0)
                        {
                            result=malloc(strlen(subphrase)+1);
                            strcpy(result,subphrase);
                        }
                        else
                        {
                            result=realloc(result,strlen(result)+strlen(subphrase)+2);
                            strcat(result,"|");
                            strcat(result,subphrase);
                        }
                        (glob)++;
                        insert_hash_topk(hk,subphrase);
                    }
                }
                char *restphrase=malloc(strlen(array_progress)+1);
                strcpy(restphrase,array_progress);
                static_search_node(&(indx->root[found]),restphrase,&subphrase,bloom_vector,args,&glob,hk,&result, id);			//AN UPARXEI STO ROOT TOTE ANADROMIKA PHGAINOUME STA PAIDIA//
                free(subphrase);
                free(restphrase);
                subphrase=NULL;
            }
            array=strtok_r(NULL," ",&array_progress);
    }
	if(glob==0)
	{
		free(args);
		free(bloom_vector);
		return "-1";
	}
	free(args);
	free(bloom_vector);
	bloom_vector=NULL;
	args=NULL;
	return result;
}
int check_node_children_for_new_word(char* string,char* new_word,signed short* arr,int fores,int array_size)
{																			//elenxw an ena node  perilamvanei sta paidia tou thn leksh pou hrthe(milame gia compressed paidia)//
	int i,cnt=0;
	char* temp;
	char temp1[1000];
	if(array_size==fores)return -1;
	for(i=0;i<fores;i++)
	{
		cnt+=abs(arr[i]);
	}
	temp=&string[strlen(string)-(strlen(string)-cnt)];
	strncpy(temp1,temp,abs(arr[fores]));
	temp1[abs(arr[fores])]='\0';
	if(!strcmp(temp1,new_word))
	{
		return 1;
	}
	return -1;
}
void static_search_node(trie_node* node,char* fp1,char** subphrase,char* bloom_vector,Arguments* args,int* glob,hash_keeper* hk,char** result,int id)	//VOITHIKH ANADROMIKH SUNARTHSH POU PSAXNEI MESA STA PAIDIA//
{
	int found=-1,i,flag=0;
	int check=-1;
	check=-1;
	int cnt=0;
	char array[10000];
	char* array1=NULL;
	char *array_progress;
	int first=0;
	while(1)
	{
		if(flag==0)								//an to flag=0 tote shmainei oti exoume hdh mia leksh gia thn opoia den exoume apofasisei an akolouthei thn upoloiph frash//
        {
            if(first==0)
            {
                first++;
                array1=strtok_r(fp1," ",&array_progress);
                if(array1==NULL)
                {
                    node->fores[id]=0;
                    break;
                }
                strcpy(array,array1);
            }
            else
            {
                array1=strtok_r(NULL," ",&array_progress);
                if(array1==NULL)
                {
                    node->fores[id]=0;
                    break;
                }
                strcpy(array,array1);
            }
        }
		if(node->array_size<=1 || flag==1)		//an o pinakas me tis lekseis exei mesa mia leksh //
		{
			if(flag!=1)							//an to flag=1 dhladh exoume vrei pou anhkei h leksh alla den exoume dei an einai final tote den ksanapsaxnoume//
			{
				found=st_binary_search(array,node->children,node->child_num);
			}
			flag=0;
			if(found==-1 )					//an den to vrike tote epistrefoume pisw kai sunexizoume apo ekei pou eixame meinei//
			{
				node->fores[id]=0;
				return ;

			}
			if(found!=-1)		//an vrethei h leskh sta paidia tou node pou vriskomaste tote sunexizoume//
			{
				if(node->children[found]->fores[id]==node->children[found]->array_size)node->children[found]->fores[id]=1;		//an ton exw elenksei olo ton ksanaelenxw apo thn arxh//
				else node->children[found]->fores[id]++;														//auksana ton metrhth tou komvou pou metra poses fores exw elenksei ton pinaka me tis lekeis//
				*subphrase=realloc(*subphrase,strlen(*subphrase)+strlen(array)+2); //2= " " +/0 //
				strcat(*subphrase," ");
				strcat(*subphrase,array);
				if(node->children[found]->is_final=='Y')
				{
					for(i=0;i<args->plithos_hash;i++)
					{
						if((check_bit(bloom_vector,fnv_32_str(*subphrase,i),args->size))==0)
						{
							check=1;
							break;
						}
					}
					if(check==1)
					{
						for(i=0;i<args->plithos_hash;i++)
							set_bit(bloom_vector,fnv_32_str(*subphrase,i),args->size);
						if(*glob==0)
						{
		                    *result=malloc(strlen(*subphrase)+1);
		                    strcpy(*result,*subphrase);
						}
						else
						{
		                    *result=realloc(*result,strlen(*result)+strlen(*subphrase)+2);
		                    strcat(*result,"|");
		                    strcat(*result,*subphrase);
						}
						(*glob)++;
	                    insert_hash_topk(hk,*subphrase);
					}
				}
				node->fores[id]=0;
				node=node->children[found];
				found=-1;
				check=-1;
			}
			continue;
		}
		if(node->array_size>1 || flag==2)
		{
			flag=0;
			if(node->word_flag==1)
				found=check_node_children_for_new_word(node->word,array,node->array,node->fores[id],node->array_size);
			else
				found=check_node_children_for_new_word(node->static_word,array,node->array,node->fores[id],node->array_size);
			if(found==1)
			{
				if(node->fores[id]==node->array_size)node->fores[id]=1;
				else node->fores[id]++;
				cnt++;
				*subphrase=realloc(*subphrase,strlen(*subphrase)+strlen(array)+2); //2= " " +/0 //
				strcat(*subphrase," ");
				strcat(*subphrase,array);
				if(node->array[node->fores[id]-1]>0)			//shmainei oti to string einai final//
				{
					int check=-1;
					for(i=0;i<args->plithos_hash;i++)
					{
						if((check_bit(bloom_vector,fnv_32_str(*subphrase,i),args->size))==0)
						{
							check=1;
							break;
						}
					}
					if(check==1)
					{
						for(i=0;i<args->plithos_hash;i++)
							set_bit(bloom_vector,fnv_32_str(*subphrase,i),args->size);
						if(*glob==0)
						{
		                    *result=malloc(strlen(*subphrase)+1);
		                    strcpy(*result,*subphrase);
						}
						else
						{
		                    *result=realloc(*result,strlen(*result)+strlen(*subphrase)+2);
		                    strcat(*result,"|");
		                    strcat(*result,*subphrase);
						}
						(*glob)++;
	                    insert_hash_topk(hk,*subphrase);
					}
				}
				continue;
			}
			else
			{
				if(node->fores[id]<node->array_size)
				{
					node->fores[id]=0;
					break;
				}
				found=st_binary_search(array,node->children,node->child_num);
				if(found!=-1)
				{
					if(node->children[found]->array_size<=1)flag=1;
					else flag=2;
					if(flag==2)
					{
						node->fores[id]=0;
						node=node->children[found];
					}
				}
				else
				{
					node->fores[id]=0;
					break;
				}
			}
		}
		continue;
	}
}
int st_insert_ngram_hash(hash_trie *ht,char *phrase)//idia san tin insert_ngram_hash diladi kanei linear hashing apla to insert sto bucket ginetai apo tin st_insert_ngram
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
    if(hash_val>=ht->pointer)
    {
        int overflow;
        overflow=st_insert_ngram(ht->bucket[hash_val],phrase);
        if(overflow>1)
        {
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
    }
    else if(hash_val<ht->pointer)
    {
        md*=2;
        hash_val=hash_val_temp%md;
        int overflow;
        overflow=st_insert_ngram(ht->bucket[hash_val],phrase);
        if(overflow>1)
        {
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
    }
    free(temp1);
    temp1=NULL;
    return 1;
}
int st_insert_ngram(Index *indx, char *phrase)
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

		indx->root[indx->root_num-1].array=malloc(sizeof(signed short));
		indx->root[indx->root_num-1].array_size=1;
		int i;
		indx->root[indx->root_num-1].fores=malloc(thread_number*sizeof(int));
		for(i=0;i<thread_number;i++)
			indx->root[indx->root_num-1].fores[i]=0;
		if(strlen(str)<=50)					//statikh desmeush
		{
			strcpy(indx->root[indx->root_num-1].static_word,str);
			indx->root[indx->root_num-1].word_flag=0;
		}
		else					//dunamikh desmeush
		{
			indx->root[indx->root_num-1].word=malloc(strlen(str)+1);
			strcpy(indx->root[indx->root_num - 1].word, str);
			indx->root[indx->root_num-1].word_flag=1;
		}
		if(indx->root[indx->root_num-1].word_flag==0)
			indx->root[indx->root_num-1].array[indx->root[indx->root_num-1].array_size-1]=strlen(indx->root[indx->root_num - 1].static_word)-2*strlen(indx->root[indx->root_num - 1].static_word);
		else
			indx->root[indx->root_num-1].array[indx->root[indx->root_num-1].array_size-1]=strlen(indx->root[indx->root_num - 1].word)-2*strlen(indx->root[indx->root_num - 1].word);
		char *str1 = strtok_r(NULL, "",&progress);
		if (str1 == NULL) //an exei mono mia leksh h psrash//
		{
			indx->root[indx->root_num - 1].is_final = 'Y';
			if(indx->root[indx->root_num-1].word_flag==0)
			{
				indx->root[indx->root_num-1].array[indx->root[indx->root_num-1].array_size-1]=strlen(indx->root[indx->root_num - 1].static_word);
			}
			else
			{
				indx->root[indx->root_num-1].array[indx->root[indx->root_num-1].array_size-1]=strlen(indx->root[indx->root_num - 1].word);
			}
			InsertionSort_Root(indx->root,indx->root_num);
			return of;
		}
        int found=-1;
		InsertionSort_Root(indx->root,indx->root_num);
        found=binary_search_root(str,indx->root,indx->root_num);
		st_insert_node(&(indx->root[found]),str1);
	}
	else //to phrase uparxei sto root//
	{
		char *str1 = strtok_r(NULL, "",&progress);
		if (str1 == NULL) //an exei mono mia leksh h psrash//
		{
			indx->root[found].is_final = 'Y';
			if(indx->root[found].word_flag==0)
			{
				indx->root[found].array[indx->root[found].array_size-1]=strlen(indx->root[found].static_word);
			}
			else
			{
				indx->root[found].array[indx->root[found].array_size-1]=strlen(indx->root[found].word);
			}
			return of;
		}
		st_insert_node(&(indx->root[found]),str1);
	}
	return of;
}
void st_insert_node(trie_node* node,char* phrase)
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
            node->children[node->child_num-1]->array=malloc(sizeof(signed short));
            node->children[node->child_num-1]->array_size=1;
            node->children[node->child_num-1]->fores=malloc(thread_number*sizeof(int));
            int i;
            for(i=0;i<thread_number;i++)
            	node->children[node->child_num-1]->fores[i]=0;

    		if(strlen(str)<=50)					//statikh desmeush
    		{
                strcpy(node->children[node->child_num-1]->static_word,str);
                node->children[node->child_num-1]->word_flag=0;
    		}
    		else					//dunamikh desmeush
    		{
                node->children[node->child_num-1]->word=malloc(strlen(str)+1);
                strcpy(node->children[node->child_num-1]->word,str);
                node->children[node->child_num-1]->word_flag=1;
    		}
            node->children[node->child_num-1]->array[ node->children[node->child_num-1]->array_size-1]=strlen(str)-2*strlen(str);
            char *str1;
            str1=malloc((strlen(str)+1)*sizeof(char));
            strcpy(str1,str);
            str=strtok_r(NULL," ",&progress);
            if(str==NULL)
            {
            	node->children[node->child_num-1]->is_final='Y';
            	if(node->children[node->child_num-1]->word_flag==0)
            	{
            		node->children[node->child_num-1]->array[ node->children[node->child_num-1]->array_size-1]=strlen(node->children[node->child_num-1]->static_word);
            	}
            	else
            	{
            		node->children[node->child_num-1]->array[ node->children[node->child_num-1]->array_size-1]=strlen(node->children[node->child_num-1]->word);
            	}
            	InsertionSort(node->children,node->child_num);
                free(str1);
                str1=NULL;
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
            	if(node->children[found]->word_flag==0)
            	{
            		node->children[found]->array[ node->children[found]->array_size-1]=strlen(node->children[found]->static_word);
            	}
            	else
            	{
            		node->children[found]->array[ node->children[found]->array_size-1]=strlen(node->children[found]->word);
            	}
                break;
            }
            node=node->children[found];
        }
    }
}
