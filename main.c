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
        thread_param *temp;//domi gia na perasoume orismata sta threads

        Job_Scheduler* sch=initialize_scheduler(thread_number,Trie,hk,0,&temp);//arxikopoioume ton job_scheduler kai ta threads
        int question_counter=0;
        int counter1=0,current_version=0,prev_current_version=-1;  		//arxikopoioume to current version se 0 kai to prev_current_version se -1//
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
                prev_current_version=0;						//h metavlhth auth eksasfalizei oti exei erthei toulaxiston ena question etsi wste na arxise na auksanetai to current vesrion//
            	char *phrase1_progress;
            	char *phrase1;
            	phrase1=strtok_r(phrase," ",&phrase1_progress);
            	phrase1=strtok_r(NULL,"",&phrase1_progress);
            	counter1++;
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
                submit_job(sch,job,question_counter,current_version);//vazoume tin erotisi ston job_scheduler
                question_counter=question_counter+1;
            }
            else if(phrase[0]=='F')
            {
            	char *phrase1_progress;
            	char *phrase1;
            	phrase1=strtok_r(phrase," ",&phrase1_progress);
            	phrase1=strtok_r(NULL,"",&phrase1_progress);
                char **buffer;
                buffer=malloc(question_counter*sizeof(char *));
				int j;
                for(j=0;j<thread_number;j++)
                {
                    temp[j].buffer=buffer;
                    temp[j].flagslen=question_counter;
                }
                if(question_counter!=0)//an den exoun erotithei Q min mpeis sta threads
                {
                    execute_all_jobs(sch,temp,buffer);//ektelese tis douleies sto buffer
                    counter1=0;
                    wait_all_tasks_finish(sch);//perimene mexri na ektelestoun oi douleies sotn buffer
                }
                reset_queue(sch->q);//arxikopoiei ton buffer gia tin epomeni epanalipsi
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
            	merge_everything(hk,thread_number);//ennonei ola ta topk apo ola ta thread
                findk(hk[0],phrase1);
                question_counter=0;
                //clean_up_hash(Trie,current_version); //cleanup
                //delete_threads(sch);
            }
            else if(phrase[0]=='D')
            {
             	if(prev_current_version==0)				//h sunthiki auth elenxei oti prin aukshsoume to current version prepei prwta na exei mpei toulaxiston 1 question//
             	{
             		current_version++;
             	}
             	prev_current_version=1;
            	char *phrase1_progress;
            	char *phrase1;
            	phrase1=strtok_r(phrase," ",&phrase1_progress);
            	phrase1=strtok_r(NULL,"",&phrase1_progress);
                delete_ngram_hash(Trie,phrase1,current_version);
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
                insert_ngram_hash(Trie,phrase1,current_version);
            }
            free(phrase);
            phrase=NULL;
            if(c==EOF) break;
            free(job);
        }
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
        delete_threads(&sch,&temp);
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
            st_insert_ngram_hash(Trie,phrase);
            free(phrase);
            phrase=NULL;
            if(c==EOF) break;
        }

        compress_hash(Trie);
        char c[1000000];
        thread_param *temp;//domi gia na perasoume ta orismata sta threads
        Job_Scheduler* sch=initialize_scheduler(thread_number,Trie,hk,0,&temp);//arxikopoiei ton job_scheduler kai ta threads
        int question_counter=0,counter=0;
        while(1)
        {
            Job *job=malloc(sizeof(Job));
            job->job_work=NULL;
        	memset(c,0,1000000);
        	if(fscanf(query_f,"%[^\n]",c)!=1)
        	{
        		free(job);
        		break;
        	}
        	if(c[0]=='Q')
        	{
        		sprintf(job->job_name,"QS");
            	memmove(c, c+2, strlen(c));
        		if(job->job_work==NULL)
        		{
        			job->job_work=malloc(strlen(c)+1);
        			strcpy(job->job_work,c);
        		}
        		submit_job(sch,job,question_counter,0);//prosthetei tin erotisei ston buffer
        		counter++;
                question_counter++;
        	}
            if(c[0]=='F')
            {
                char **buffer;
				buffer=malloc(question_counter*sizeof(char *));
                int j;
                for(j=0;j<thread_number;j++)
                {
                    temp[j].buffer=buffer;
                    temp[j].flagslen=question_counter;
                }
                if(question_counter!=0)//an den exoun erotithei Q tote min mpeis sta threads
                {
                    execute_all_jobs(sch,temp,buffer);//ektelese tin douleies pou uparxoun ston buffer
                    counter=0;
                    wait_all_tasks_finish(sch);//perimene mexri o buffer na teleiosei tis douleies
                }
		        reset_queue(sch->q);//arxikopoiise ton Q gia tin epomeni ripi
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
            	merge_everything(hk,thread_number);//enose ta topk ton threads
                findk(hk[0],c);
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
        delete_threads(&sch,&temp);
    }
}
