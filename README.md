+ This is a multithreaded program that will read user input from stdin as a stream, parse it, and output it with small modifications, as outlined below. Data flow is controlled through the use of semaphores.

1. Upon encountering a newline in the input, it will output a space. Upon encountering **, it will output ^.

2. This implimentation will output lines of size BUFFER_SIZE. Nothing less and nothing more. If input line is shorter than BUFFER_SIZE, nothing will be outputted until more characters are supplied. If longer, output will be cut at BUFFER_SIZE. This is not an implementation limitation per se, but it was a requirement of the project at the time.
