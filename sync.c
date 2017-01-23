/*
 * NAME, etc.
 *
 * sync.c
 *
 */


#define _REENTRANT

#include "sync.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
	lock = (my_spinlock_t *)malloc(sizeof(my_spinlock_t*)); 
	lock->is_locked = 0;
	lock->tid = syscall(SYS_gettid);
	lock->count_self=0;
	
	return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
	free(lock);
	return 0;

}

int my_spinlock_unlock(my_spinlock_t *lock)
{
	if(lock->count_self==0){
		
		lock->tid=0;
		lock->is_locked=0;
		lock->count_self=0;
		return 0;
	}
	else if(lock->count_self>0){
		
		lock->count_self--;
		my_spinlock_unlock(lock);
	}
	return -1;
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
	
	while(1){
		if(lock->tid!=syscall(SYS_gettid)){
			while(tas(&lock->is_locked)){};
			lock->tid=syscall(SYS_gettid);
			return 0;
		}
		lock->count_self++;
		return 0;
	}
	return -1;
} 

int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
	if(lock->tid!=syscall(SYS_gettid)){	
		while(1){
			while(lock->is_locked==1){};
			if(tas(&lock->is_locked)==0){
				lock->tid=syscall(SYS_gettid);
				return 0;
			}
		}
		return -1;
	}
	lock->count_self++;
	return 0;
}
	
	

int my_spinlock_trylock(my_spinlock_t *lock)
{
	if(lock->is_locked==0){
		if(tas(&lock->is_locked)==0){
			lock->tid=syscall(SYS_gettid);
		}
		return 0;
	}
	return -1;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
	lock=(my_mutex_t *)malloc(sizeof(my_mutex_t *));
	lock->is_locked=0;
	lock->count_self=0;
	lock->tid=syscall(SYS_gettid);
	
	return 0;
}

int my_mutex_destroy(my_mutex_t *lock)
{
	free(lock);
	return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{
	
		if(lock->count_self==0){
			
			lock->is_locked=0;
			lock->tid=0;
			lock->count_self=0;
			return 0;
		}
		else{
			lock->count_self--;
			my_mutex_unlock(lock);
		}
	
		return -1;
	
}

int my_mutex_lock(my_mutex_t *lock)
{
	lock->wait_interval.tv_nsec=2;
	
	if(lock->tid!=syscall(SYS_gettid)){
		while(1){
			while(lock->is_locked){
				nanosleep((const struct timespec*)(rand()%(lock->wait_interval.tv_nsec)),NULL);
				if(lock->wait_interval.tv_nsec<16){
					lock->wait_interval.tv_nsec = 2*lock->wait_interval.tv_nsec;
				}
			}
			if(tas(&lock->is_locked)==0){
				lock->tid=syscall(SYS_gettid);
					
				return 0;
			}
		}	
		return -1;	
	}
	lock->count_self++;
	return 0;
}
int my_mutex_trylock(my_mutex_t *lock)
{
	
		if(tas(&lock->is_locked)==0){
			lock->tid=syscall(SYS_gettid);
		}
		return 0;
	
	return -1;
}

/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *lock)
{
	lock->now_serving=0;
	lock->next_in_queue=0;
	return 0;
}


int my_queuelock_destroy(my_queuelock_t *lock)
{
	return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
	lock->now_serving++;
	return 0;
}

int my_queuelock_lock(my_queuelock_t *lock)
{
	int my_ticket = __sync_fetch_and_add(&lock->next_in_queue, 1);
	while(lock->now_serving!=my_ticket){};
}

int my_queuelock_trylock(my_queuelock_t *lock)
{
	return lock->now_serving;
}

