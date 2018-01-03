#include <stdio.h>
#include "st_ngrams.h"
#include "dy_ngrams.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Job_Scheduler.h"
#define thread_number 4
int main(int argc, char *argv[])
{
	int i;
	char *phrase;
	FILE* init_f,*query_f;
	int icheck;
	int qcheck;
	icheck=0;
	qcheck=0;
    if(argc!=5)
    {
        printf("wrong number of arguements\n");
        return -1;
    }
    else
    {
        for(i=1;i<argc;i=i+2)/*gets the arguements from command line*/
        {
            if(!strcmp(argv[i],"-i"))
            {
                init_f=fopen(argv[i+1],"r");
                if(init_f==NULL)
                {
                    printf("error opening initialisation file\n");
                    return -1;
                }
                icheck=1;
            }
            else if(!strcmp(argv[i],"-q"))
            {
                query_f=fopen(argv[i+1],"r");
                if(query_f==NULL)
                {
                    printf("error opening query file\n");
                    return -1;
                }
                qcheck=1;
            }
        }
    }
    if(icheck==0 || qcheck==0)
    {
        printf("wrong arguements\n");
        return -1;
    }
    //Job *job=malloc(sizeof(Job));
    //job->job_work=NULL;
    char *choice;
    choice = malloc(20);
    char cc;
    int phs;
    phs=20;
    int counter1;
    counter1=0;
    cc=fgetc(init_f);
    while(cc!=EOF && cc!='\n')
    {
        if(counter1==phs)
        {
            phs*=2;
            choice=realloc(choice,phs*sizeof(char));
        }
        choice[counter1]=cc;
        counter1++;
        cc=fgetc(init_f);
    }
    choice[counter1]='\0';
    if(!strcmp("DYNAMIC",choice))
    {
        hash_trie *Trie;
        Trie=init_hash_trie();
        hash_keeper **hk;
        hk=malloc(thread_number*sizeof(Job *));
        int kk;
        kk=0;
        for(kk=0;kk<thread_number;kk++)
        {
            initialize_hash_topk(&hk[kk]);
        }
        while(1)
        {
            int phrase_size=20;
            phrase=malloc(phrase_size*sizeof(char));
            int counter=0;
            char c;
            c=fgetc(init_f);
            if(c==EOF)
                {
                free(phrase);
                break;
                }
            while(c!=EOF && c!='\n')
            {
                if(counter==phrase_size)
                {
                    phrase_size*=2;
                    phrase=realloc(phrase,phrase_size*sizeof(char));
                }
                phrase[counter]=c;
                counter++;
                c=fgetc(init_f);
            }
            if(counter==phrase_size)
            {
                phrase_size*=2;
                phrase=realloc(phrase,phrase_size*sizeof(char));
            }
            phrase[counter]='\0';
            insert_ngram_hash(Trie,phrase,0);
            free(phrase);
            phrase=NULL;
            if(c==EOF) break;
        }
        char *qphrase;
        char c[1000000];
        char *progress;
        char *command;
        char *parseddata;
        Job_Scheduler* sch=initialize_scheduler(thread_number);
        int question_counter=0,top_counter=0;
        int counter1=0,current_version=0,prev_current_version=-1;
        while(1)
        {
            int phrase_size=20;
            phrase=malloc(phrase_size*sizeof(char));
            int counter=0;
            char c;
            c=fgetc(query_f);
            if(c==EOF)
                {
                free(phrase);
                break;
                }
            while(c!=EOF && c!='\n')
            {
                if(counter==phrase_size)
                {
                    phrase_size*=2;
                    phrase=realloc(phrase,phrase_size*sizeof(char));
                }
                phrase[counter]=c;
                counter++;
                c=fgetc(query_f);
            }
            if(counter==phrase_size)
            {
                phrase_size*=2;
                phrase=realloc(phrase,phrase_size*sizeof(char));
            }
            phrase[counter]='\0';
            Job* job=malloc(sizeof(Job));
            job->job_work=NULL;
            if(phrase[0]=='Q')
            {
                prev_current_version=0;
            	char *phrase1_progress;
            	char *phrase1;
            	phrase1=strtok_r(phrase," ",&phrase1_progress);
            	phrase1=strtok_r(NULL,"",&phrase1_progress);
            	int phrase1len;
            	/*if(phrase1==NULL)
            	{
                    printf("its true\n");
                    getchar();
            	}*/
            	//phrase1len=strlen(phrase1);
            	counter1++;
            	//memmove(phrase, phrase+2, strlen(phrase));
                //printf("i am here\n");
            	/*char* result;
                result=new_search(Trie,phrase1,hk);
                printf("%s",result);
                printf("\n");
                if(strcmp(result,"-1"))
                {
                	free(result);
                    result=NULL;
                }*/
                //printf("i am here\n");
                sprintf(job->job_name,"QD");
                if(job->job_work==NULL)
                {
                    if(phrase1==NULL)
                    {
                        job->job_work=NULL;
                    }
                    else
                    {
                        job->job_work=malloc(strlen(phrase1)+1);
                        strcpy(job->job_work,phrase1);
                    }
                }
                //printf("past\n");
                submit_job(sch,job,question_counter,current_version);
                question_counter=question_counter+1;
            }
            else if(phrase[0]=='F')
            {
            	char *phrase1_progress;
            	char *phrase1;
            	phrase1=strtok_r(phrase," ",&phrase1_progress);
            	phrase1=strtok_r(NULL,"",&phrase1_progress);
            	//memmove(phrase, phrase+2, strlen(phrase));
                //findk(hk,phrase1);
                char **buffer;
                buffer=malloc(question_counter*sizeof(char *));
                thread_param *temp;
                temp=execute_all_jobs(sch,Trie,hk,counter,buffer);
                counter1=0;
                wait_all_tasks_finish(sch);
                reset_queue(sch->q);
            	int k;
            	k=0;
            	for(k=0;k<question_counter;k++)
            	{
                    printf("%s\n",buffer[k]);
            	}
            	for(k=0;k<question_counter;k++)
            	{
                    free(buffer[k]);
            	}
                free(buffer);
            	merge_everything(hk,thread_number);
                findk(hk[0],phrase1);
                question_counter=0;
                free(temp);
                //delete_threads(sch);
            }
            else if(phrase[0]=='D')
            {
             	if(prev_current_version==0)
             	{
             		current_version++;
             	}
             	prev_current_version=1;
            	char *phrase1_progress;
            	char *phrase1;
            	phrase1=strtok_r(phrase," ",&phrase1_progress);
            	phrase1=strtok_r(NULL,"",&phrase1_progress);
            	//memmove(phrase, phrase+2, strlen(phrase));
                /*if(delete_ngram_hash(Trie,phrase1)!=1)
                {
                    printf("Error in delete\n");
                }*/
                if(delete_ngram_hash(Trie,phrase1,current_version)!=1)
                {
                    //printf("Error in delete\n");
                }
            }
            else if(phrase[0]=='A')
            {
             	if(prev_current_version==0)
             	{
             		current_version++;
             	}
             	prev_current_version=1;
            	char *phrase1_progress;
            	char *phrase1;
            	phrase1=strtok_r(phrase," ",&phrase1_progress);
            	phrase1=strtok_r(NULL,"",&phrase1_progress);
            	//memmove(phrase, phrase+2, strlen(phrase));
                //insert_ngram_hash(Trie,phrase1);
                insert_ngram_hash(Trie,phrase1,current_version);
            }
            //insert_ngram_hash(Trie,phrase);
            free(phrase);
            phrase=NULL;
            if(c==EOF) break;
            free(job);
        }
        /*while(1)
        {
            memset(c,0,1000000);
            //printf("i am here1\n");
            int what;
            if((what=fscanf(query_f,"%[^\n]",c))!=1)
            {
                //printf("%d iiiiii\n",what);
                break;
            }
            //command=strtok_r(c," ",&progress);
            //parseddata=strtok_r(NULL,"",&progress);
            if(c[0]=='Q')
            {
            	memmove(c, c+2, strlen(c));
                //printf("i am here\n");
            	char* result;
                result=new_search(Trie,c,hk);
                printf("%s",result);
                printf("\n");
                if(strcmp(result,"-1"))
                {
                	free(result);
                    result=NULL;
                }
            }
            else if(c[0]=='F')
            {
            	memmove(c, c+2, strlen(c));
                findk(hk,c);
            }
            else if(c[0]=='D')
            {
            	memmove(c, c+2, strlen(c));
                if(delete_ngram_hash(Trie,c)!=1)
                {
                    printf("Error in delete\n");
                }
            }
            else if(c[0]=='A')
            {
            	memmove(c, c+2, strlen(c));
                insert_ngram_hash(Trie,c);
            }
            fgetc(query_f);
        }*/
        free(choice);
        choice=NULL;
        fclose(init_f);
        fclose(query_f);
        delete_trie_hash(&Trie);
        kk=0;
        for(kk=0;kk<thread_number;kk++)
        {
            top_hash_rem(&hk[kk]);
        }
        free(hk);
        delete_threads(&sch);
    }
    else if(!strcmp("STATIC",choice))
    {
        hash_trie *Trie;
        Trie=init_hash_trie();
        hash_keeper **hk;
        hk=malloc(thread_number*sizeof(Job *));
        int kk;
        kk=0;
        for(kk=0;kk<thread_number;kk++)
        {
            initialize_hash_topk(&hk[kk]);
        }
        while(1)
        {
            int phrase_size=20;
            phrase=malloc(phrase_size*sizeof(char));
            int counter=0;
            char c;
            c=fgetc(init_f);
            if(c==EOF)
                {
                free(phrase);
                break;
                }
            while(c!=EOF && c!='\n')
            {
                if(counter==phrase_size)
                {
                    phrase_size*=2;
                    phrase=realloc(phrase,phrase_size*sizeof(char));
                }
                phrase[counter]=c;
                counter++;
                c=fgetc(init_f);
            }
            if(counter==phrase_size)
            {
                phrase_size*=2;
                phrase=realloc(phrase,phrase_size*sizeof(char));
            }
            phrase[counter]='\0';
            //printf("i am here1\n");
            //printf("%s first letter %c\n",phrase,phrase[0]);
            st_insert_ngram_hash(Trie,phrase);
            //printf("i am here2\n");
            free(phrase);
            phrase=NULL;
            if(c==EOF) break;
        }

        compress_hash(Trie);
        char c[1000000];
        Job_Scheduler* sch=initialize_scheduler(thread_number);
        // return 1;
        int question_counter=0,top_counter=0,counter=0;
        //execute_all_jobs(sch,Trie,hk);

        while(1)
        {
            //printf("i am here\n");
            Job *job=malloc(sizeof(Job));
            job->job_work=NULL;
        	memset(c,0,1000000);
        	if(fscanf(query_f,"%[^\n]",c)!=1)
        	{
        		//printf("axi allo");
        		free(job);
        		break;
        	}

        	//printf("c=%s\n",c);
        	//printf("c=%c\n",c[0]);
        	//getchar();
        	if(c[0]=='Q')
        	{
                //printf("%s\n",c);
        		//printf("mpika\n");
        		sprintf(job->job_name,"QS");
            	memmove(c, c+2, strlen(c));
        		if(job->job_work==NULL)
        		{
        			job->job_work=malloc(strlen(c)+1);
        			strcpy(job->job_work,c);
        		}
        		//printf("%s\n",c);
                //job->job_fun_st=static_search;
                //job->job_fun_dy=NULL;
        		//printf("sumbit %d\n",question_counter);
        		submit_job(sch,job,question_counter,0);
        		counter++;
               /* char* result;
                result=static_search(Trie,job->job_work,hk);
                printf("%s",result);
                printf("\n");
                if(strcmp(result,"-1"))
                {
                	free(result);
                }*/
                //free(job->job_work);
                //job->job_work=NULL;

            	//printf("job=%s\nstr=%s\n",job->job_name,job->job_work);
                question_counter++;
        	}
            if(c[0]=='F')
            {
                //printf("i am here4\n");
				//printf("c=%s\n",c);
                //sch->buf_counter=0;
                char **buffer;
				buffer=malloc(question_counter*sizeof(char *));
				thread_param *temp;
		        temp=execute_all_jobs(sch,Trie,hk,counter,buffer);
		        counter=0;
		        wait_all_tasks_finish(sch);
		        reset_queue(sch->q);
            	int k;
            	k=0;
            	for(k=0;k<question_counter;k++)
            	{
                    printf("%s\n",buffer[k]);
            	}
            	for(k=0;k<question_counter;k++)
            	{
                    free(buffer[k]);
            	}
            	question_counter=0;
            	free(buffer);
            	memmove(c, c+2, strlen(c));
            	merge_everything(hk,thread_number);
                findk(hk[0],c);
                free(temp);
                //delete_threads(sch);
            }
            fgetc(query_f);
            free(job);
        }
        free(choice);
        fclose(init_f);
        fclose(query_f);
        st_delete_hash(&Trie);
        kk=0;
        for(kk=0;kk<thread_number;kk++)
        {
            top_hash_rem(&hk[kk]);
        }
        free(hk);
        delete_threads(&sch);
    }
}
