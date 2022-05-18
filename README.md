# threads_C
copy.c is the main file, test.c is to be ignored.
copy.c copies file data cahracter by character to a new specified file using custom number of "in" and "out" threads
and creates a log file.
The more threads in and out the faster it will work.
The pgrogram is on C99 compiler for linux. Will not work on Windows!
launch 
./cpy 1 1  dataset4.txt target.txt 1521 mylog.txt
 cc -Wall -o cpy copy.c -lpthread
 
 //this program has no purpose.

Full description: 
I created the program that based on input reads number of threads in and out, input file and output file and a log file. It copies character(byte) by character from old file into the new one which it creates or replaces. Also, it makes a log file showing the progress of the work along the way.
1.	A clear statement about the assigned work/components you believe you have done/completed successfully.
1)	Read input 
2)	We create n in threads and m out threads 
3)	Read the whole file into temporary buffer
4)	Create threads n in and m out ones   
5)	IN producer threads get a character and its offset from file (temp buffer that we created) and put it in our main buffer which is a structure of structures which has a char and its offset as data. Also, puts info data into log file along the way.
6)	OUT consumer threads get a char and offset from main buffer (one thread – one char at once), and put it into a new created or replaced file in the same position as it was in the original file 
7)	We use mutex and semaphores to let threads handle their part without other interrupting 
So that everything is being done as the tasks requires.
8)	we wait for threads depending on the number of IN threads and OUT threads
No thread does everything at once, they do tasks one after another in parallel with synchronization.
9)	1 in and 1 out would not be super fast but let’s say 5 in and 5 out or 50 in and 50 out do job very fast
 ![image](https://user-images.githubusercontent.com/24215721/168939839-e40ec6a6-88d1-4562-a28f-34596705d6bd.png)
![image](https://user-images.githubusercontent.com/24215721/168939843-ec69e29d-9730-4f57-988d-87d7e98cc98b.png)
