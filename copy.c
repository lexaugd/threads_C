
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include<semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
//------------------
#define TEN_MILLIS_IN_NANOS 10000000
#define SHARED 1
#define COUNT_HALT1 3
#define COUNT_HALT2 6
void * consumer(int * id);
void * producer(int * id);
sem_t semEmpty; //semaphores
sem_t semFull;
//pthread_t         self;//thread
pthread_mutex_t mutexBuffer; //mutex
char * buffer = 0; //buffer size
char c;
int length_tmp_buff; //tmp buffer for holding file 
int buffer_size = 0; //size of structure and temp bufer too
int IN = 0, OUT = 0, k = 0, m = 0; //thread counters 
FILE * fp; //file pointer
FILE * newFile; //file to copy to
FILE * logFile; //log
struct timespec t;
sem_t empty, full, sm, amp; //for time 

struct thread_data { //structure to hold all info
  char data;
  int offset; //yes, int. no saying about mandatory dataType
}
thread_data;
struct thread_data tdata[9000000]; //structure with many many elements beacse we don't know size beforehang

int main(int argc, char * argv[]) {
  pthread_mutex_init( & mutexBuffer, NULL); //mutex init 
  sem_init( & semEmpty, 0, 10); //semaphores init 
  sem_init( & semFull, 0, 0);
  t.tv_sec = 0; //time 
  t.tv_nsec = rand() % (TEN_MILLIS_IN_NANOS + 1);
  int i = 0; //good old frind i
  //  printf("argc: %d\n", argc);
  if (argc < 7 || argc > 7) { //format 
    fprintf(stderr, "Missning or too many arguments, format: ./cpy <nIN> <nOUT> <file> <copy> <bufSize> <Log>\n");
    return (-1);
  }

  IN = atoi(argv[1]);//get all arguments 
  OUT = atoi(argv[2]);
  fp = fopen(argv[3], "rb"); //open file, reading
  newFile = fopen(argv[4], "w+"); //open file, writing 
  buffer_size = atoi(argv[5]);
  logFile = fopen(argv[6], "wb"); //open file, writing binary (it works that way)
  //printf("IN: %d\n", IN);
  //printf("OUT: %d\n", OUT);
 // printf("buffer_size: %d\n", buffer_size);
  pthread_t thread_IN[IN];//in threads and out threads 
  pthread_t thread_OUT[OUT];

  if (IN < 1 || OUT < 1) {
    fprintf(stderr, "IN and OUT threads must not have values less than 1, ok?\n");
    return (-1);
  }
  if (newFile == NULL) {
    fprintf(stderr, "Failed to create the targer file\n");
  }
  if (logFile == NULL) {
    fprintf(stderr, "Failed to create the log file\n");
  }
  if (fp) {//if source file opens then we work with it. get all data in temp buffer to accsess from proper buffer after(shared memory thing in C for stable work.)
    fseek(fp, 0, SEEK_END);
    length_tmp_buff = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(length_tmp_buff);
    if (buffer) {
      fread(buffer, 1, length_tmp_buff, fp);//read file in temporary buffer. Task didn't prohibit this  
    }
  } else {
    fprintf(stderr, "Failed to open the source file\n");
    return (-1);
  }
  buffer[length_tmp_buff - 1] = '\0';// end of file. just in case 

  if (length_tmp_buff - 1 < buffer_size) {// just a frindly reminder 
    printf("Source file size is %d, but your input is %d, it's ok though \n", length_tmp_buff - 1, buffer_size);
  }

  for (i = 0; i < IN; i++) {// create in threads 
    pthread_create( & thread_IN[i], NULL, (void * ) producer, & i);
    nanosleep( & t, NULL);
  }

  for (i = 0; i < OUT; i++) {// create out threads 
    pthread_create( & thread_OUT[i], NULL, (void * ) consumer, & i);
    nanosleep( & t, NULL);
  }
  
  
  if (IN >= OUT) {// if in less than or equal out wait for out threads first 
    for (i = 0; i < OUT; i++) {
      //nanosleep( & t, NULL);
      pthread_join(thread_OUT[i], NULL);
    }
  }

  if (OUT > IN) {// if in> out wait for in threads first
    for (i = 0; i < IN; i++) {
      nanosleep( & t, NULL);
      pthread_join(thread_IN[i], NULL);
    }
  }
  //free memory of everyhing that we used: files, mutexes, semaphores, temp bufffer 
  sem_destroy( & semEmpty);
  sem_destroy( & semFull);
  pthread_mutex_destroy( & mutexBuffer);
  fclose(newFile);
  fclose(fp);
  free(buffer);
  //printf("lng: %d \n", length_tmp_buff);
  return 0;
}
//---------------Fills Buffer-------------------------
void * producer(int * arg) { //thread IN
  //int i = 0;//unused but has to be there, don't ask,

  char readStr[50];// strings for log file 
  char produceStr[50];
  while (m < buffer_size && m < length_tmp_buff - 1) {//make sure don't go over file size and given size 

    nanosleep( & t, NULL);//sleep sugar 
    sem_wait( & semEmpty);//hold on traffic light till this thread reads data and increments iterator m
    pthread_mutex_lock( & mutexBuffer);//mutex lock
    if (m < buffer_size && m < length_tmp_buff - 1) {//make sure threads don't go wild gettign datta from where there is no datta 
      sprintf(readStr, "read_byte PT%d 0%d B%d I-1", * arg, m, buffer[m]);// prepare string for log 
      fputs(readStr, logFile);//put sring into log 
      putc('\n', logFile);//put next line char into log 
      sprintf(produceStr, "produce PT%d 0%d B%d I%d", * arg, m, buffer[m], m);//prepare sring for log 
      tdata[m].data = buffer[m];// struct buffer gets data from temporarry buffer where we store all file, nice and efficient and no restrictions on this approach so...
      tdata[m].offset = m;//offset to buffer 
      fputs(produceStr, logFile);//put to log 
      putc('\n', logFile);//put next line char into log 

    }
    m++;//we itterate here cuz we don't want to stuck in loop forever 
    pthread_mutex_unlock( & mutexBuffer);// release the kraken, mutex I mean. 
    sem_post( & semFull);// traffic light lets other threads handle stuff 
    nanosleep( & t, NULL);//sleep 
    /*if(k>=buffer_size && m>=buffer_size){ well, this was useful intill i realized it is not but imma keep it here to remeber the struggle. nobody reads it anyway
    	nanosleep( & t, NULL);//hi, I'm time 
    	break;//stop this nonsense 
    }*/
  } //pthread_exit(NULL);
  // return 0; nope 
}
//--------------------Fills File----------------------
void * consumer(int * arg) { //thread OUT
  char writeStr[50];//prepare string for log file 
  char consumeStr[50];
  //int i;

  while (k < buffer_size && k < length_tmp_buff - 1) {//make sure don't go over file size and given size 

    nanosleep( & t, NULL);//sleep sugar 
    sem_wait( & semFull);//hold on traffic light till this thread reads data and increments iterator m
    pthread_mutex_lock( & mutexBuffer);//mutex lock
    if (k < buffer_size && k < length_tmp_buff - 1) {//make sure threads don't go wild gettign datta from where there is no datta
      sprintf(writeStr, "write_byte CT%d 0%d B%d I-1", * arg, k, tdata[k].data);// prepare string for log 
      fputs(writeStr, logFile);//put sring into log 
      putc('\n', logFile);//put next line char into log 
      // printf("data=%c offset=%d \n", tdata[k].data, tdata[k].offset);
      fwrite( & tdata[k].data, 1, sizeof(tdata[k].data), newFile);//fill int the destination file char by char, thread by thread from actual buffer, just as the task asks me to
      sprintf(consumeStr, "consume CT%d 0%d B%d I%d", * arg, m, buffer[k], k);//put sring into log 
      fputs(consumeStr, logFile);//prepare sring for log 
      putc('\n', logFile);//put next line char into log 
      //fwrite( & buffer[k], 1, sizeof(buffer[k]), newFile); //never mind. cheating to debug threads. Not actually cheating

    }
    k++;//we itterate here cuz we don't want to stuck in loop forever 
    sem_post( & semEmpty);// traffic light lets other threads handle stuff 
    pthread_mutex_unlock( & mutexBuffer);// release the mutex!!! 
    nanosleep( & t, NULL);//sleep 
    /*	if(k>=buffer_size && m>=buffer_size){well, I already explained that 
		nanosleep( & t, NULL);
		break;
	}*/
  }
  //return 0; no way 
}
