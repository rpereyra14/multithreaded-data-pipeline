/*
* Author: Renato Pereyra. UNC COMP 530: Operating Systems.
* This is a multithreaded program that will read user input from stdin as a stream, parse it, and output it.
* Upon encountering a newline in the input, it will output a space. Upon encountering **, it will output ^.
* It will output lines of size BUFFER_SIZE. Nothing less and nothing more.
* This will be accomplished by implementing a data pipeline.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st.h"
#include "semaphore.h"
#include "buffer.h"

#define BUFFER_SIZE 80

/*
* Declare function prototypes.
* This program will have three threads. Each thread will execute one of these functions.
*/
void *readInput( void *ptr );
void *parseInput( void *ptr );
void *printOutput( void *ptr );

/*ThreadMono will be passed to threads that act solely as producers or solely as consumers.*/
typedef struct {
	semaphore *fullSlots;
	semaphore *emptySlots;
	semaphore *mutex;
	buffer *buf;
} ThreadMono;

/*ThreadDual will be passed to threads that act both as producers and as consumers.*/
typedef struct {
	semaphore *fullSlots1;
	semaphore *emptySlots1;
	semaphore *mutex1;
	buffer *buf1;
	semaphore *fullSlots2;
	semaphore *emptySlots2;
	semaphore *mutex2;
	buffer *buf2;
} ThreadDual;

/*
* Manages the creation of the 3 threads of execution.
* Ensures these threads are passed the semaphores and buffers they require.
*/
void main ( void ){

	/*Initialize the ST library runtime.*/
	st_init();

	/*Variable Conventions:
	* Numbers in semaphore variable names correspond to the buffer number to which the semaphore will be applied.
	* Buffers are numbered 1 and 2, according to the order in which they are reached in the data pipeline.
	*/

	/*
	* Initialize semaphores.
	* mutex semaphores (binary) will wrap critical sections and will ensure mutual exclusion between threads.
	* fullSlots semaphores (general) will count the number of slots in a buffer available for writing.
	* emptySlots semaphores (general) will count the number of slots in a buffer available for consumption.
	*/
	semaphore mutex1, mutex2, fullSlots1, fullSlots2, emptySlots1, emptySlots2;
	createSem(&mutex1, 1);
	createSem(&mutex2, 1);
	createSem(&fullSlots1, 0);
	createSem(&fullSlots2, 0);
	createSem(&emptySlots1, BUFFER_SIZE);
	createSem(&emptySlots2, BUFFER_SIZE);

	/*
	* Initialize buffers.
	* Buffer 1 will contain data prior to processing.
	* Buffer 2 will contain data ready for output.
	*/
	buffer buf1, buf2;
	initBuffer(&buf1, BUFFER_SIZE);
	initBuffer(&buf2, BUFFER_SIZE);

	/*Struct for Thread 1. Thread 1 will solely produce to buffer 1.*/
	ThreadMono threadOneStruct = {
		&fullSlots1,
		&emptySlots1,
		&mutex1,
		&buf1
	};

	/*Struct for Thread 2. Thread 2 will consume from buffer 1 and produce to buffer 2.*/
	ThreadDual threadTwoStruct = {
		&fullSlots1,
		&emptySlots1,
		&mutex1,
		&buf1,
		&fullSlots2,
		&emptySlots2,
		&mutex2,
		&buf2,
	};

	/*Struct for Thread 3. Thread 3 will solely consume from buffer 2.*/
	ThreadMono threadThreeStruct = {
		&fullSlots2,
		&emptySlots2,
		&mutex2,
		&buf2
	};

	/*Create threads.*/
	if ( st_thread_create(readInput, &threadOneStruct, 0, 0) == NULL) {
		perror("st_thread_create for reading thread failed.");
		exit(1);
	}
	if ( st_thread_create(parseInput, &threadTwoStruct, 0, 0) == NULL) {
		perror("st_thread_create for parsing thread failed.");
		exit(1);
	}
	if ( st_thread_create(printOutput, &threadThreeStruct, 0, 0) == NULL) {
		perror("st_thread_create for printing thread failed.");
		exit(1);
	}

	st_thread_exit(NULL);

}

/*
* Implementation Details:
* Before depositing a char, threads will ensure emptySlots is not zero. After depositing, they up fullSlots.
* Before consuming a char, threads will ensure fullSlots is not zero. After consuming, they up emptySlots.
* Deposits and consumptions are wrapped with mutex to ensure buffer integrety.
* These conditions ensure the correctness of the consumer-producer systems in this program.
*/

/*
* Read input data from stdin and deposit chars read into buffer 1.
*/
void *readInput( void *ptr ){

	ThreadMono *pkg1 = ptr;

	int c;

	while( 1 ){

		c = fgetc( stdin );

		/*Deposit char read. Refer to comments above for implementation details.*/
		down( pkg1->emptySlots );
		down( pkg1->mutex );
		deposit( pkg1->buf, c );
		up( pkg1->mutex );
		up( pkg1->fullSlots );

		/*Exit thread upon encountering EOF. Note that EOF was passed to buffer 1.*/
		if ( c == EOF ){
			break;
		}

	}

	st_thread_exit(NULL);

}

/*
* Parse input data deposited into buffer 1, process it, and then deposit the processed data into buffer 2.
*/
void *parseInput( void *ptr ){

	ThreadDual *pkg2 = ptr;
	int c, d;

	while( 1 ){

		/*Consumer a char from buffer 1.*/
		down( pkg2->fullSlots1 );
		down( pkg2->mutex1 );
		c = consume( pkg2->buf1 );
		up( pkg2->mutex1 );
		up( pkg2->emptySlots1 );

		/*Convert newlines to spaces.*/
		if ( c == '\n' ){
			c = ' ';
		}

		if ( c == '*' ){

			/*If an asterisk was consumed, check if the next input is an asterisk as well.*/
			down( pkg2->fullSlots1 );
			down( pkg2->mutex1 );
			d = consume( pkg2->buf1 );
			up( pkg2->mutex1 );
			up( pkg2->emptySlots1 );

			/*The next input need not be an asterisk.*/
			if ( d == '\n' ){
				d = ' ';
			}

			/*If another asterisk was consumed, deposit a caret in buffer 2.*/
			if ( d == '*' ){

				down( pkg2->emptySlots2 );
				down( pkg2->mutex2 );
				deposit( pkg2->buf2, '^' );
				up( pkg2->mutex2 );
				up( pkg2->fullSlots2 );

			/*Else, deposit both chars consumed into buffer 2.*/
			}else{

				down( pkg2->emptySlots2 );
				down( pkg2->mutex2 );
				deposit( pkg2->buf2, c );
				up( pkg2->mutex2 );
				up( pkg2->fullSlots2 );

				down( pkg2->emptySlots2 );
				down( pkg2->mutex2 );
				deposit( pkg2->buf2, d );
				up( pkg2->mutex2 );
				up( pkg2->fullSlots2 );

				/*Exit thread on EOF.*/
				if ( d == EOF ){
					break;
				}

			}

		/*First char consumed was not an asterisk. Deposit it into buffer 2.*/
		}else{

			down( pkg2->emptySlots2 );
			down( pkg2->mutex2 );
			deposit( pkg2->buf2, c );
			up( pkg2->mutex2 );
			up( pkg2->fullSlots2 );

			/*Exit thread on EOF.*/
			if ( c == EOF ){
				break;
			}

		}

	}

	st_thread_exit(NULL);	

}

/*
* Print the processed data stored in buffer 2 in BUFFER_SIZE increments.
*/
void *printOutput( void *ptr ){

	ThreadMono *pkg3 = ptr;

	/*Temporary storage for consumed chars until BUFFER_SIZE is reached.*/
	char toPrint[ BUFFER_SIZE ];

	int c, j = 0;

	while( 1 ){

		/*Consume from buffer 2.*/
		down( pkg3->fullSlots );
		down( pkg3->mutex );
		c = consume( pkg3->buf );
		up( pkg3->mutex );
		up( pkg3->emptySlots );

		/*If BUFFER_SIZE has not yet been reached, store consumed char.*/
		if ( j < BUFFER_SIZE ){

			toPrint[ j ] = c;
			j++;

		/*Print and clear toPrint.*/
		}else{

			printf("%s\n", toPrint);
			memset( toPrint, 0, sizeof(toPrint) );
			j = 0;
			toPrint[ j ] = c;
			j++;

		}

		/*Exit thread on EOF.*/
		if ( c == EOF ){
			break;
		}

	}

	st_thread_exit(NULL);

}