# threads_C
copy.c is the main file, test.c is to be ignored
copy.c copies file data cahracter by character to a new specified file using custom number of "in" and "out" threads
and creates a log file.
The more threads in and out the faster it will work.
The pgrogram is on C99 compiler for linux. Will not work on Windows!
launch 
./cpy 1 1  dataset4.txt target.txt 1521 mylog.txt
 cc -Wall -o cpy copy.c -lpthread
 
 //this program has no purpose.
