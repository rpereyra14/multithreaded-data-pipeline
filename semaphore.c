/*
* Author: Renato Pereyra. UNC COMP 530: Operating Systems.
* This is the source file to be used with semaphore.h.
* These files encapsulate the implementation of a general semaphore.
*/
#include <stdlib.h>

#include "st.h"

/*Semaphore structure.*/
typedef struct {
	int val;
	st_cond_t cond;
} semaphore;

/*Initialize a semaphore. Initial value must be nonnegative.*/
void createSem( semaphore *s, int value ){

	if ( value < 0 ){
		perror("Semaphore value cannot be negative.");
		exit(1);
	}

	s->val = value;
	s->cond = st_cond_new();

}

/*Increase a semaphore value and signal treads waiting on the semaphore.*/
void up ( semaphore *s ){

	s->val = s->val + 1;
	st_cond_signal( s->cond );

}

/*Decrease a semaphore value if it is greater than zero. Else, wait until it becomes greater than zero.*/
void down ( semaphore *s ){

	while ( 1 ){
		if ( s->val > 0 ){
			s->val = s->val - 1;
			break;
		}else{
			st_cond_wait( s->cond );
		}
	}

}