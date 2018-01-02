#include <pthread.h>
#include <stdio.h>
#include "Job_Scheduler.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "st_ngrams.h"

pthread_mutex_t start_mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t id_counter_mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t *read_mutex;//= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t *mutex;
pthread_cond_t  write_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t  *read_cond;//=PTHREAD_COND_INITIALIZER;
int id_counter=0;
int threads_exited=0;
pthread_mutex_t threads_exited_mutex=PTHREAD_MUTEX_INITIALIZER;
char **buffer=NULL;
void *Worker(void* j)
{
    int thread_mutex_place=0;
	thread_param* x=(thread_param*)j;
	int question_id=-1;
	while(1)
	{
		//if(x->sch->q->end==0)continue;
		pthread_mutex_lock( &start_mutex );
		//printf("i am the thread %d\n",x->id);
		int start=x->sch->q->start;
		thread_mutex_place=start;
		start=(start+1)%(x->sch->q->size);
		x->sch->q->start=start;
		//printf("i am the thread %d kai teleiwnw me start=%d\n",x->id,thread_mutex_place);
		pthread_mutex_unlock( &start_mutex );
	    //printf("i am the thread %d and i exit from start and i try %d position on mutex\n",x->id,thread_mutex_place);

		pthread_mutex_lock( &(mutex[thread_mutex_place]));
			//printf("eimai to thread %d kai h thesh mou einai %d\n",x->id,thread_mutex_place);
			if(!strcmp(x->sch->q->jobs[thread_mutex_place].job_name,"-1"))
			{
				//printjobs(x->sch);
				//pthread_cond_broadcast(&write_cond);
				//printf("eimai to thread %d kai teleiwnw\n",x->id);
				pthread_mutex_unlock( &(mutex[thread_mutex_place]));
				//pthread_mutex_lock( &threads_exited_mutex );
				//threads_exited++;
				//pthread_mutex_unlock( &threads_exited_mutex );

			//	printf("Threads Exited=%d\n",threads_exited+1);
				pthread_exit(&x->id);
				//printf("thread %d thread_mutex_place=%d---end=%d\n",x->id,thread_mutex_place,x->sch->q->end);
				//printf("eimai to %d thread kai koimamai end=%d\n",x->id,x->sch->q->end);
				//pthread_cond_wait( &(read_cond[thread_mutex_place]), &(mutex[thread_mutex_place]) );
			}
			//printf("AAAA current %d start %d\n",thread_mutex_place,start);
			//printf("job_name %s\n",x->sch->q->jobs[thread_mutex_place].job_name);
            execute_job(x,thread_mutex_place);
			//printf("eimai to %d thread kai ksipnisa\n",x->id);


			//printf("i am the thread %d\n",x->id);

			/*if(strncmp(x->sch->q->jobs[thread_mutex_place].job_name,"Question",8)==0)
			{
				//printf("i am the thread %d kai ksekinaw douleia\n",x->id);
				//printf("Job:%s\n",x->sch->q->jobs[thread_mutex_place].job_name);
				char* result;
				//printf("JOB IS %s\n",x->sch->q->jobs[thread_mutex_place].job_work);
				question_id=x->sch->q->jobs[thread_mutex_place].id;
				result=static_search(x->indx,x->sch->q->jobs[thread_mutex_place].job_work,x->hash,x->id);
				//if(strcmp(result,"-1"))
				{
					//free(result);
				}
				//buffer[question_id]=malloc(strlen(result)+1);
				////printf("question id=%d \nthesh=%d\n",question_id,(question_id)%(x->sch->q->size));
			//	printf("result=%s",result);
				//printf("\n");
				//strcpy(buffer[(question_id)],result);
				//printf("%s\n",buffer[question_id]);

				int i=0;


				pthread_mutex_lock( &id_counter_mutex );
				//printf("thread %d question_id=%d---id_counter=%d\n",x->id,question_id,id_counter);
				while(question_id!=id_counter)
				{
					//printf("eimai to %d thread kai perimenw me \n",x->id);;
					pthread_cond_wait( &write_cond, &id_counter_mutex );
				}

				//printf("%s",result);
                //printf("\n");
				if(strcmp(result,"-1"))
				{
					//free(result);
					x->buffer[id_counter]=result;
				}
				else if(!strcmp(result,"-1"))
                {
                    x->buffer[id_counter]=malloc(3);
                    strcpy(x->buffer[id_counter],result);
                }
 				id_counter++;
				pthread_cond_broadcast(&write_cond);
				pthread_mutex_unlock( &id_counter_mutex );
                //strcpy(x->sch->buffer[question_id],result);
                //printf("result is %s\n",x->sch->buffer[question_id]);
                //free(result);
				strcpy(x->sch->q->jobs[thread_mutex_place].job_name,"-1");
			}
			//printf("\n");
			if(strncmp(x->sch->q->jobs[thread_mutex_place].job_name,"Find",4)==0)
			{
				//printf("threads exited=%d\nqueue=%s\nthesh=%d\n",threads_exited,x->sch->q->jobs[thread_mutex_place].job_name,thread_mutex_place);
				//printf("!!!!!!!!!!!!!!!!!@@@@@@@@@@@@@@@@@@@@@@@@@!!!!!!!!!!!!!!!!!!!!!!!!@@@@@@@@@@@@@@@@@@@@@@@\n");
				//printjobs(x->sch);
				strcpy(x->sch->q->jobs[thread_mutex_place].job_name,"-1");
				//pthread_mutex_lock( &id_counter_mutex );
				//id_counter++;
				//pthread_cond_broadcast(&write_cond);
				//pthread_mutex_unlock( &id_counter_mutex );
				//strcpy(x->sch->q->jobs[thread_mutex_place].job_name,"-1");
			}*/
			//pthread_cond_signal(&(read_cond[thread_mutex_place]));
			//printf("eimai to thread %d kai vgainw me %d\n",x->id,thread_mutex_place);
			//printf("i am here8\n");
		pthread_mutex_unlock( &(mutex[thread_mutex_place]));
	}
}

void execute_job(thread_param *x,int thread_mutex_place)
{
    char* result;
    int question_id;
    question_id=x->sch->q->jobs[thread_mutex_place].id;
    if(x->sch->q->jobs[thread_mutex_place].job_fun_st!=NULL)
    {
        result=x->sch->q->jobs[thread_mutex_place].job_fun_st(x->indx,x->sch->q->jobs[thread_mutex_place].job_work,x->hash,x->id);
    }
    else
    {
        result=x->sch->q->jobs[thread_mutex_place].job_fun_dy(x->indx,x->sch->q->jobs[thread_mutex_place].job_work,x->hash,x->sch->q->jobs[thread_mutex_place].current_version);
    }
    if(strcmp(result,"-1"))
    {
        //free(result);
        x->buffer[question_id]=result;
    }
    else if(!strcmp(result,"-1"))
    {
        x->buffer[question_id]=malloc(3);
        strcpy(x->buffer[question_id],result);
    }
    free(x->sch->q->jobs[thread_mutex_place].job_work);
    strcpy(x->sch->q->jobs[thread_mutex_place].job_name,"-1");
}

Job_Scheduler* initialize_scheduler(int execution_threads)
{
	Job_Scheduler* scheduler=malloc(sizeof(Job_Scheduler));
	pthread_t *workers;
	Queue* queue=malloc(sizeof(Queue));
	queue->start=0;
	queue->end=0;
	queue->size=100;
	queue->jobs=malloc(queue->size*sizeof(Job));
	mutex=malloc(queue->size*sizeof(pthread_mutex_t));
	int j;
	for(j=0;j<queue->size;j++)
	{
		pthread_mutex_init(&(mutex[j]),NULL);
		//pthread_mutex_init(&(read_mutex[i]),NULL);
	}

	for(j=0;j<queue->size;j++)
		strcpy(queue->jobs[j].job_name,"-1");
	scheduler->execution_threads=execution_threads;

	if (( workers = malloc ( execution_threads * sizeof ( pthread_t ))) == NULL )
	{
		perror ( " malloc ") ;
		exit (1) ;
	}
	int i;
	scheduler->q=queue;
	scheduler->tids=workers;
	return scheduler;
}
void submit_job(Job_Scheduler* schedule,Job* j,int id,int current_version)
{
	int i;
    if(schedule->q->end==schedule->q->size)
    {
        //printf("i am here\n");
        schedule->q->size=schedule->q->size*2;
        schedule->q->jobs=realloc(schedule->q->jobs,schedule->q->size*sizeof(Job));
        int j;
        for(j=schedule->q->end;j<schedule->q->size;j++)
        {
            strcpy(schedule->q->jobs[j].job_name,"-1");
        }
        mutex=realloc(mutex,schedule->q->size*sizeof(pthread_mutex_t));
        for(j=schedule->q->end;j<schedule->q->size;j++)
        {
            pthread_mutex_init(&(mutex[j]),NULL);
            //pthread_mutex_init(&(read_mutex[i]),NULL);
        }
        //printf("i am here1\n");
        //getchar();
    }
    schedule->q->jobs[schedule->q->end]=*j;
    schedule->q->jobs[schedule->q->end].id=id;
    if(!strcmp(schedule->q->jobs[schedule->q->end].job_name,"QS"))
    {
        schedule->q->jobs[schedule->q->end].job_fun_st=static_search;
        schedule->q->jobs[schedule->q->end].job_fun_dy=NULL;
    }
    if(!strcmp(schedule->q->jobs[schedule->q->end].job_name,"QD"))
    {
        schedule->q->jobs[schedule->q->end].job_fun_st=NULL;
        schedule->q->jobs[schedule->q->end].job_fun_dy=new_search;
        schedule->q->jobs[schedule->q->end].current_version=current_version;
    }
    schedule->q->end=schedule->q->end+1;

}
void execute_all_jobs(Job_Scheduler* schedule,hash_trie* indx,hash_keeper** hk,int counter,char **buf_res)
{
	int i;
	int err;
	void *Worker(void*) ;
	thread_param *temp;
	temp=malloc(schedule->execution_threads*sizeof(thread_param));
	//printjobs(schedule);
	//schedule->q->size=counter;
	//schedule->q->jobs=malloc(schedule->q->size*sizeof(Job));
	//temp->id=malloc(schedule->execution_threads*sizeof(int));
	for(i=0;i<schedule->execution_threads;i++)
	{
		temp[i].sch=schedule;
		temp[i].id=i;
		temp[i].hash=hk[i];
		temp[i].indx=indx;
		temp[i].buffer=buf_res;
		if ((err=pthread_create( (schedule->tids)+i , NULL , Worker ,(void*)(temp+i))))		/*ftiaxnw ta threads workers*/
		{
			perror (" pthread_create " );
			exit (1) ;
		}
	}
	//free(temp);

}
void printjobs(Job_Scheduler* schedule)
{
	int i;
	for(i=0;i<schedule->q->size;i++)
	{
		if(strcmp(schedule->q->jobs[i].job_name,"-1"))
			printf("Job:%s-->[%d]\n",schedule->q->jobs[i].job_name,i);
		if(schedule->q->jobs[i].job_work!=NULL)
		{
			//printf("Job_work:%s\n",schedule->q->jobs[i].job_work);
		}
	}
}
void wait_all_tasks_finish(Job_Scheduler* schedule)
{
	//printf("eimai sto wait all tasks&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
	int i,err;
	for (i =0 ; i <schedule->execution_threads ; i++)
	{
		if ( (err = pthread_join (*( schedule->tids+i) , NULL )))
		{
			perror (" pthread_join " );
			exit (1) ;
		}
	}
	for(i=0;i<schedule->q->size;i++)
	{
	//	printf("I=%d\n",i);
	//	if(buffer[i]==NULL)continue;
		//if(strcmp(buffer[i],"")!=0)
			//printf("%s\n",buffer[i]);
	}
//	printf("eimai sto wait all tasks########################################################################\n");
}
void destroy_scheduler(Job_Scheduler* schedule)
{
	free(schedule->q->jobs);
	free(schedule->tids);
}


void reset_queue(Queue *q)
{
	q->start=0;
	q->end=0;
	id_counter=0;
}
void delete_threads(Job_Scheduler* schedule)
{
	free(schedule->tids);
	free(schedule->q->jobs);
	int i;
	for(i=0;i<schedule->q->size;i++)
	{
		pthread_mutex_destroy(&mutex[i]);
	}
	pthread_mutex_destroy(mutex);
	pthread_mutex_destroy(&start_mutex);
}

