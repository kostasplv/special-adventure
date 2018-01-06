/*
 * Job_Scheduler.h
 *
 *  Created on: Dec 19, 2017
 *      Author: user
 */

#ifndef JOB_SCHEDULER_H_
#define JOB_SCHEDULER_H_
#include <pthread.h>
#include "dy_ngrams.h"


typedef struct Job Job;
typedef struct Job_Scheduler Job_Scheduler;
typedef struct Queue Queue;
typedef struct thread_param thread_param;
struct Queue
{
	Job* jobs;
	int size;
	int start;
	int end;
};
struct Job_Scheduler
{
	int execution_threads;
	Queue* q;
	pthread_t* tids;
};
struct Job
{
	char job_name[100];
	char* job_work;
	int id;
	int current_version;
    char* (*job_fun_st)(hash_trie*,char*,hash_keeper*,int);
    char* (*job_fun_dy)(hash_trie*,char*,hash_keeper*,int);
};
struct thread_param
{
	Job_Scheduler* sch;
	int id;
	hash_trie* indx;
	hash_keeper* hash;
    char **buffer;
    int flagslen;
};

void printjobs(Job_Scheduler* schedule);
Job_Scheduler* initialize_scheduler(int,hash_trie*,hash_keeper**,int,thread_param **);
void *Worker(void* i);
void submit_job(Job_Scheduler* schedule,Job* j,int id,int current_version);
void execute_all_jobs(Job_Scheduler*,thread_param *,char **);
void wait_all_tasks_finish(Job_Scheduler* schedule);
void destroy_scheduler(Job_Scheduler* schedule);
void reset_queue(Queue *);
void execute_job(thread_param *,int);
void delete_threads(Job_Scheduler **,thread_param **);
#endif /* JOB_SCHEDULER_H_ */
