/*
* Author: Renato Pereyra. UNC COMP 530: Operating Systems.
* This is the source file to be used with buffer.h.
* These files encapsulate the implementation of a queue-like buffer.
*/

#include <stdlib.h>
#include <string.h>

/*Buffer structure.*/
typedef struct {
	char * buf;
} buffer;

/*Initialize buffer.*/
void initBuffer( buffer *Handler, int bufferSize ){

	/*Buffer is initialized to contain null terminator.*/
	Handler->buf = malloc( bufferSize + 1);
	*(Handler->buf) = '\0';

}

/*Deposit a char into the buffer.*/
void deposit ( buffer *Handler, int c ){

	int length = strlen(Handler->buf);
	*(Handler->buf + length) = c;
	*(Handler->buf + length + 1) = '\0';

}

/*Consume a char from the buffer.*/
int consume ( buffer *Handler ){

	int toReturn = *(Handler->buf);
	int i = 1, length = strlen(Handler->buf);

	/*Shift all buffer elements down by one index.*/
	while ( i <= length ){
		*(Handler->buf + i - 1) = *(Handler->buf + i);
		i++;
	}

	return toReturn;

}
