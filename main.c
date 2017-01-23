#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sync.h"


void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   
}



void *pthreadMutexTest(){
	
    int i;
	int j;
	int k;
	
	localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_mutex_unlock(&count_mutex);    
	}   
}

void *pthreadSpinTest(){
	
	int i;
	int j;
	int k;
	
	localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_spin_lock(&count_spin);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_spin_unlock(&count_spin);    
	}   
}

void *mySpinlockTASTest(){
	
	int i;
	int j;
	int k;
	
	localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_spinlock_lockTAS(&my_spinlock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_spinlock_unlock(&my_spinlock);    
	}   
}

void *mySpinlockTTASTest(){
	
	int i;
	int j;
	int k;
	
	localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_spinlock_lockTTAS(&my_spinlock2);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_spinlock_unlock(&my_spinlock2);    
	}   

}

void *myMutexTest(){
	int i;
	int j;
	int k;
	
	localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_mutex_lock(&my_own_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_mutex_unlock(&my_own_mutex);    
	}   
}

void *myQueueTest()
{
	int i;
	int j;
	int k;
	
	localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_queuelock_lock(&count_queue);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_queuelock_unlock(&count_queue);    
	}   
}

int runTest(int testID)
{

/*You must create all data structures and threads for you experiments*/
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
	{
		c=0;
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
			if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL))){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);
		//printf("Threaded Run Pthread (Mutex) Local Count: %lld\n", localCount);
		printf("Threaded Run Pthread (Mutex) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Mutex time(ms): %llu\n",result/1000000);

	}

	if(testID == 0 || testID == 2) /*Pthread Spinlock*/
	{
		c=0;
		
		pthread_spin_init(&count_spin,0);

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
			if( rt=(pthread_create( threads+i, NULL, &pthreadSpinTest, NULL))){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);
		//printf("Threaded Run Pthread (Spin) Local Count: %lld\n", localCount);
		printf("Threaded Run Pthread (Spin) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("PthreadSpin time(ms): %llu\n",result/1000000);

	}

	if(testID == 0 || testID == 3) /*MySpinlockTAS*/
	{
		c=0;
		
		my_spinlock_init(&my_spinlock);



		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
			if( rt=(pthread_create( threads+i, NULL, &mySpinlockTASTest, NULL))){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);
		//printf("Threaded Run mySpin (TAS) Local Count: %lld\n", localCount);
		printf("Threaded Run mySpin (TAS) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("mySpinTAS time(ms): %llu\n",result/1000000);

		
	}


	if(testID == 0 || testID == 4) /*mySpinlockTTAS*/
	{
		c=0;
		
		my_spinlock_init(&my_spinlock2);



		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
			if( rt=(pthread_create( threads+i, NULL, &mySpinlockTTASTest, NULL))){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);
		//printf("Threaded Run mySpin (TTAS) Local Count: %lld\n", localCount);
		printf("Threaded Run mySpin (TTAS) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("mySpinTTAS time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 5) /*myMutexTTAS*/
	{
		c=0;
		
		my_mutex_init(&my_own_mutex);



		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
			if( rt=(pthread_create( threads+i, NULL, &myMutexTest, NULL))){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);
		//printf("Threaded Run myMutex Local Count: %lld\n", localCount);
		printf("Threaded Run myMutex (TTAS) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("my_mutexTTAS time(ms): %llu\n",result/1000000);

	}

	if(testID == 0 || testID == 6) /*myQueuelock*/
	{
		c=0;
	
		my_queuelock_init(&count_queue);



		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
			if( rt=(pthread_create( threads+i, NULL, &myQueueTest, NULL))){
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);
		//printf("Threaded Run myQueue Local Count: %lld\n", localCount);
		printf("Threaded Run myQueue Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("my_queue time(ms): %llu\n",result/1000000);

	}
	


	return 0;
}

int testAndSetExample()
{
	volatile long test = 0; //Test is set to 0
	printf("Test before atomic OP:%ld\n",test);
	tas(&test);
	printf("Test after atomic OP:%ld\n",test);
}

int processInput(int argc, char *argv[])
{

/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/
	// -t -i -o -c -d
	
	
	if(argv[1]==NULL){
		numThreads=4;
		numItterations=1000000;
		testID=0;
		workOutsideCS=0;
		workInsideCS=1;
	}
	else if(strcmp(argv[1],"-t")==0 && strcmp(argv[3],"-i")==0 && strcmp(argv[5],"-o")==0 && strcmp(argv[7],"-c")==0 && strcmp(argv[9],"-d")==0){
		numThreads = atoi(argv[2]);
		numItterations = atoi(argv[4]);
		workOutsideCS = atoi(argv[6]);
		workInsideCS = atoi(argv[8]);
		testID = atoi(argv[10]);
	}
	

	printf("\n\n\n\nThe number of threads is %d\n", numThreads );


	return 0;
}


int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");	
	
	//testAndSetExample(); //Uncomment this line to see how to use TAS
	
	processInput(argc,argv);
	runTest(testID);
	return 0;

}
