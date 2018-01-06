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
pthread_cond_t *mutex_cond;
pthread_cond_t  write_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t  *read_cond;//=PTHREAD_COND_INITIALIZER;
pthread_mutex_t init= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t init1= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t init_cond= PTHREAD_COND_INITIALIZER;
pthread_mutex_t end= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t end_cond= PTHREAD_COND_INITIALIZER;
pthread_mutex_t cntm= PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t cntc= PTHREAD_COND_INITIALIZER;
int ending=0;
int id_counter=0;
int threads_exited=0;
int beginning=0;
int broadcast_start=0;
pthread_mutex_t threads_exited_mutex=PTHREAD_MUTEX_INITIALIZER;
int threads_reached=0;
pthread_mutex_t trm= PTHREAD_MUTEX_INITIALIZER;
/*void *Worker(void* j)
{
    int thread_mutex_place=0;
	thread_param* x=(thread_param*)j;
	int question_id=-1;
	int flag1=0;
	while(1)
	{
        flag1=0;
		pthread_mutex_lock( &start_mutex );
		int start=x->sch->q->start;
		thread_mutex_place=start;
		start=(start+1)%(x->sch->q->size);
		x->sch->q->start=start;
		pthread_mutex_unlock( &start_mutex );
		pthread_mutex_lock( &(mutex[thread_mutex_place]));
        if(!strcmp(x->sch->q->jobs[thread_mutex_place].job_name,"-1"))
        {
            if(!beginning)
            {
                pthread_mutex_lock(&(trm));
                threads_reached=threads_reached+1;
                pthread_mutex_unlock(&(trm));
            }
            //printf("i am here2\n");
            pthread_cond_wait(&(read_cond[x->id]),&(mutex[thread_mutex_place]));
            printf("i  am here\n");
            flag1=1;
            if(ending)
            {
                pthread_cond_broadcast(&(read_cond[x->id]));
                pthread_mutex_unlock( &(mutex[thread_mutex_place]));
                pthread_exit(&x->id);
            }
        }
        if(beginning>1)
        {
            if(flag1)
            {
                pthread_cond_broadcast(&(read_cond[x->id]));
                pthread_mutex_unlock( &(mutex[thread_mutex_place]));
                continue;
            }
        }
        execute_job(x,thread_mutex_place);
        pthread_mutex_lock(&cntm);
        id_counter++;
        if(id_counter==x->flagslen)
        {
            pthread_cond_signal(&end_cond);
        }
        pthread_mutex_unlock(&cntm);
		pthread_mutex_unlock( &(mutex[thread_mutex_place]));
	}
}*/

void *Worker(void* j)
{
    int thread_mutex_place=0;
	thread_param* x=(thread_param*)j;
	int question_id=-1;
	int flag1=0;
	int start;
    /*pthread_mutex_lock( &start_mutex );
    int start=x->sch->q->start;
    thread_mutex_place=start;
    start=(start+1)%(x->sch->q->size);
    x->sch->q->start=start;
    pthread_mutex_unlock( &start_mutex );*/
    /*pthread_mutex_lock(&(trm));
    threads_reached=threads_reached+1;
    pthread_mutex_unlock(&(trm));*/
	while(1)
	{
        flag1=0;
        pthread_mutex_lock( &start_mutex );
        start=x->sch->q->start;
        thread_mutex_place=start;
        start=(start+1)%(x->sch->q->size);
        x->sch->q->start=start;
        pthread_mutex_unlock( &start_mutex );
        //printf("1 id:%d\n",x->id);
        pthread_mutex_lock( &(mutex[thread_mutex_place]));
		//printf("1.5 id:%d\n",x->id);
        while(!strcmp(x->sch->q->jobs[thread_mutex_place].job_name,"-1"))
        {
            if(!beginning)
            {
                pthread_mutex_lock(&(trm));
                threads_reached=threads_reached+1;
                pthread_mutex_unlock(&(trm));
            }
           // printf("2 id:%d-->%d\n",x->id,thread_mutex_place);
            pthread_cond_wait(&(read_cond[thread_mutex_place]),&(mutex[thread_mutex_place]));
           // printf("eimai to thread %d\n",x->id);
            //printf("2.5 id:%d\n",x->id);
            if(ending)
            {
                //pthread_cond_broadcast(&(read_cond[x->id]));
               // printf("3 id:%d\n",x->id);
                pthread_mutex_unlock( &(mutex[thread_mutex_place]));
                //printf("3.5 id:%d\n",x->id);
                pthread_exit(&x->id);
            }
            if(beginning>1)
            {
                //pthread_cond_broadcast(&(read_cond[x->id]));
            	//printf("4 id:%d\n",x->id);
                pthread_mutex_unlock( &(mutex[thread_mutex_place]));
                //printf("4.5 id:%d\n",x->id);
                pthread_mutex_lock( &start_mutex );
                start=x->sch->q->start;
                thread_mutex_place=start;
                start=(start+1)%(x->sch->q->size);
                x->sch->q->start=start;
                pthread_mutex_unlock( &start_mutex );
                //printf("5 id:%d\n",x->id);
                pthread_mutex_lock(&(mutex[thread_mutex_place]));
                //printf("5.5 id:%d\n",x->id);
            }
        }
       // printf("i am here2\n");
        execute_job(x,thread_mutex_place);
        //printf("i am here3\n");
        pthread_mutex_lock(&cntm);
        id_counter++;
        if(id_counter==x->flagslen)
        {
            //printf("i am here in flagslen\n");
            pthread_cond_signal(&end_cond);
        }
        pthread_mutex_unlock(&cntm);
        //printf("i am here4\n");
        //printf("7 id:%d\n",x->id);
		pthread_mutex_unlock( &(mutex[thread_mutex_place]));
		//printf("7.5 id:%d\n",x->id);
        //printf("i am here5\n");

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

Job_Scheduler* initialize_scheduler(int execution_threads,hash_trie* indx,hash_keeper** hk,int counter,thread_param **temp)
{
	Job_Scheduler* scheduler=malloc(sizeof(Job_Scheduler));
	pthread_t *workers;
	Queue* queue=malloc(sizeof(Queue));
	queue->start=0;
	queue->end=0;
	queue->size=100;
	queue->jobs=malloc(queue->size*sizeof(Job));
	mutex=malloc(5000*sizeof(pthread_mutex_t));
	//mutex_cond=malloc(queue->size*sizeof(pthread_cond_t));
	read_cond=malloc(5000*sizeof(pthread_cond_t));
	//read_mutex=malloc(execution_threads*sizeof(pthread_mutex_t));
	int j;
	for(j=0;j<5000;j++)
	{
		pthread_mutex_init(&(mutex[j]),NULL);
		//pthread_cond_init(&(mutex_cond[j]),NULL);
		pthread_cond_init(&(read_cond[j]),NULL);
	}
	/*for(j=0;j<execution_threads;j++)
	{
        pthread_cond_init(&(read_cond[j]),NULL);
        //pthread_mutex_init(&(read_mutex[j]),NULL);
	}*/
    //printf("i am here4\n");
	for(j=0;j<queue->size;j++)
		strcpy(queue->jobs[j].job_name,"-1");
	scheduler->execution_threads=execution_threads;

	if (( workers = malloc ( execution_threads * sizeof ( pthread_t ))) == NULL )
	{
		perror ( " malloc ") ;
		exit (1) ;
	}
	//printf("i am here5\n");
	int i;
	scheduler->q=queue;
	scheduler->tids=workers;
	int err;
	void *Worker(void*) ;
	//thread_param *temp;
	*temp=malloc(scheduler->execution_threads*sizeof(thread_param));
	thread_param *temp1;
	temp1=*temp;
    //printf("i am here7\n");
    for(i=0;i<scheduler->execution_threads;i++)
	{
		temp1[i].sch=scheduler;
		temp1[i].id=i;
		temp1[i].hash=hk[i];
		temp1[i].indx=indx;
		temp1[i].buffer=NULL;
		if ((err=pthread_create( (scheduler->tids)+i , NULL , Worker ,(void*)(temp1+i))))		/*ftiaxnw ta threads workers*/
		{
			perror (" pthread_create " );
			exit (1) ;
		}
	}
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
        /*mutex=realloc(mutex,schedule->q->size*sizeof(pthread_mutex_t));
        mutex_cond=realloc(mutex_cond,schedule->q->size*sizeof(pthread_cond_t));
        for(j=0;j<schedule->q->size;j++)
        {
            pthread_mutex_init(&(mutex[j]),NULL);
            //pthread_cond_init(&(mutex_cond[j]),NULL);
            pthread_cond_init(&(read_cond[j]),NULL);
        }*/
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

void execute_all_jobs(Job_Scheduler* schedule,thread_param *temp,char **buf_res)
{
    int i;
    //pthread_cond_broadcast(&init_cond);
	//printf("eimai sto execute all\n");
    if(!beginning)
    {
    	//printf("beggining\n");
        //pthread_cond_broadcast(&init_cond);
        int flag1;
        flag1=0;
        while(1)
        {
            pthread_mutex_lock( &(trm));
            if(threads_reached==schedule->execution_threads)
            {
                flag1=1;
            }
            pthread_mutex_unlock( &(trm));
            if(flag1)
            {
                break;
            }
        }
    }
    beginning=beginning+1;
    i=0;
   // printf("%d-%d\n",schedule->q->start,schedule->execution_threads);
   // printf("%d--%d\n",broadcast_start,broadcast_start+schedule->execution_threads+1);
    /*for(i=broadcast_start;i<broadcast_start+schedule->execution_threads+1;i++)
    {
    	//printf("i=%d\n",i);
        pthread_cond_broadcast(&(read_cond[i]));
       // printf("i am here2\n");
    }*/
    for(i=0;i<5000;i++)
    {
    	//printf("i=%d\n",i);
        pthread_cond_broadcast(&(read_cond[i]));
       // printf("i am here2\n");
    }
    //pthread_cond_broadcast(&init_cond);
    //printf("i am here2\n");
	return;
}

void printjobs(Job_Scheduler* schedule)
{
	int i;
	printf("Jobs:\n");
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
    pthread_mutex_lock(&init);
    //printf("i am here3\n");
    //printf("i am here in  wait\n");
    pthread_cond_wait(&end_cond,&init);
    pthread_mutex_unlock(&init);
}
void destroy_scheduler(Job_Scheduler* schedule)
{
	free(schedule->q->jobs);
	free(schedule->tids);
}


void reset_queue(Queue *q)
{
	//printf("start=%d\nend=%d\n",q->start,q->end);
	broadcast_start=q->end;
	q->start=0;
	q->end=0;
	id_counter=0;
}
void delete_threads(Job_Scheduler** schedule,thread_param **temp)
{
    int i;
	int err;
	ending=1;
    /*for(i=broadcast_start;i<(*schedule)->execution_threads+broadcast_start+1;i++)
    {
        pthread_cond_broadcast(&(read_cond[i]));
    }*/
    //printf("i am here\n");
    for(i=0;i<5000;i++)
    {
        pthread_cond_broadcast(&(read_cond[i]));
    }
	for (i =0 ; i <(*schedule)->execution_threads ; i++)
	{
		if ( (err = pthread_join (*( (*schedule)->tids+i) , NULL )))
		{
			perror (" pthread_join " );
			exit (1) ;
		}
	}
	free((*schedule)->tids);
	free((*schedule)->q->jobs);
	free((*schedule)->q);
	for(i=0;i<5000;i++)
	{
		pthread_mutex_destroy(&mutex[i]);
		pthread_cond_destroy(&read_cond[i]);
	}
	free((*schedule));
	free((*temp));
	//pthread_mutex_destroy(mutex);
	pthread_mutex_destroy(&start_mutex);
}
