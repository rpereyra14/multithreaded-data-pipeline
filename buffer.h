/*
* Author: Renato Pereyra. UNC COMP 530: Operating Systems.
* This is a header file to be used with buffer.c.
* These files encapsulate the implementation of a queue-like buffer.
*/

/*Buffer structure.*/
typedef struct {
	char * buf;
} buffer;

/*Function declarations.*/
void initBuffer( buffer *Handler, int bufferSize );
void deposit ( buffer *Handler, int c );
int consume ( buffer *Handler );