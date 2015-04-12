/*
* Author: Renato Pereyra. UNC COMP 530: Operating Systems.
* This is a header file to be used with semaphore.c.
* These files encapsulate the implementation of a general semaphore.
*/

/*Semaphore structure.*/
typedef struct {
	int val;
	st_cond_t cond;
} semaphore;

/*Function declarations.*/
void createSem( semaphore *s, int value );
void up ( semaphore *s );
void down ( semaphore *s );