# semaphores

This is a multithreaded program that will read user input from stdin as a stream, parse it, and output it.
Upon encountering a newline in the input, it will output a space. Upon encountering **, it will output ^.
It will output lines of size BUFFER_SIZE. Nothing less and nothing more.
This example will be accomplished by implementing a data pipeline in which data flow is controlled by semaphores.
