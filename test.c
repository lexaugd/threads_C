
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

#define TEN_MILLIS_IN_NANOS 10000000
#define SHARED 1
#define COUNT_HALT1  3
#define COUNT_HALT2  6
void * consumer(int * id);
void * producer(int * id);
sem_t semEmpty;
sem_t semFull;
pthread_t         self;

pthread_mutex_t mutexBuffer;
char * buffer = 0;
char c;
long length_tmp_buff;
int buffer_size = 0; //size of structure and temp bufer too
int IN = 0, OUT = 0, k=0, m=0;//thread counters
FILE * fp; //file pointer
FILE * newFile;//file to copy to
FILE * logFile;//log
struct timespec t;
sem_t empty, full, sm, amp;

struct thread_data { //structure to hold all info
  char data;
  int offset;
}
thread_data;

//  struct thread_data * tdata = (struct thread_data * ) arg;//struct init
struct thread_data tdata[1000000];

int main(int argc, char * argv[]) {
  pthread_mutex_init(&mutexBuffer, NULL);
   sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);
  t.tv_sec = 0;
  t.tv_nsec = rand() % (TEN_MILLIS_IN_NANOS + 1);

  int j = 0, i = 0;
  
  
  if (argc < 2) {
    printf("Missing File\n");
    return (1);
  }
  fp = fopen(argv[1], "rb"); //open file, reading
  IN = atoi(argv[2]);
  OUT = atoi(argv[3]);
  buffer_size = atoi(argv[4]);
  pthread_t thread_IN[IN];
   pthread_t thread_OUT[OUT];
  int thread_id_IN[IN];
  int thread_id_OUT[OUT];

  struct thread_data tdata[buffer_size];
  
  if (fp) {
    fseek(fp, 0, SEEK_END);
    length_tmp_buff = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(length_tmp_buff);
    if (buffer) {
      fread(buffer, 1, length_tmp_buff, fp);
    }
  } else {
    fprintf(stderr, "Failed to open the file\n");
  }
  buffer[length_tmp_buff-1]='\0';

    /*else{
	  fprintf(stderr, "Failed to open the taarget\n");  
}*/
   newFile = fopen("target.txt", "w+");//wb
   logFile = fopen("myLog.txt", "wb");
 // if (newFile) {
  for (i = 0; i < IN; i++) {
   pthread_create( & thread_IN[i], NULL, (void * ) producer, & i);
	nanosleep( & t, NULL);
  }
 
  for (i = 0; i < OUT; i++) {
    pthread_create( & thread_OUT[i], NULL, (void * ) consumer, & i);
	nanosleep( & t, NULL);
  }
	if(IN>OUT){
    for (i = 0; i < OUT; i++) {
	 nanosleep( & t, NULL);
	 pthread_join(thread_OUT[i], NULL);
    } 
	 } 

	   if(OUT>=IN){ 
	for (i = 0; i < IN; i++) {
	 nanosleep( & t, NULL);
	 pthread_join(thread_IN[i], NULL);
	}
	 }
	


  
	sem_destroy(&semEmpty);
    sem_destroy(&semFull);
	pthread_mutex_destroy(&mutexBuffer);
    fclose(newFile);
	fclose(fp);
	free(buffer);
		printf("lng: %d \n", length_tmp_buff);
  return 0;
}
//---------------Fills Buffer-------------------------
void * producer(int * arg) { //thread IN
int i=0;

char readStr[50];
char produceStr[50];
  while (m < buffer_size) {
 
	nanosleep( & t, NULL);
	sem_wait(&semEmpty);
	pthread_mutex_lock(&mutexBuffer);
	if(m<buffer_size){
	sprintf(readStr, "read_byte PT%d 0%d B%d I-1", *arg, m, buffer[m]);
	fputs(readStr, logFile);
	putc('\n', logFile);
	sprintf(produceStr, "produce PT%d 0%d B%d I%d", *arg, m, buffer[m], m);
    tdata[m].data = buffer[m];
    tdata[m].offset = m;
	fputs(produceStr, logFile);
	putc('\n', logFile);
    
	}
	m++;
	pthread_mutex_unlock(&mutexBuffer);
	sem_post(&semFull);
	nanosleep( & t, NULL);
	if(k>=buffer_size && m>=buffer_size){
		nanosleep( & t, NULL);
		break;
	}
  }//pthread_exit(NULL);
}
//--------------------Fills File----------------------
void * consumer(int * arg) { //thread OUT
char writeStr[50];
char consumeStr[50];
int temp=0, i;

   while (k < buffer_size) {

	  nanosleep( & t, NULL);
	  sem_wait(&semFull);
	  pthread_mutex_lock(&mutexBuffer);
	  if(k<buffer_size){
	  sprintf(writeStr, "write_byte CT%d 0%d B%d I-1", *arg, k, tdata[k].data);
	  fputs(writeStr, logFile);
	  putc('\n', logFile);
      printf("data=%c offset=%d \n", tdata[k].data, tdata[k].offset);
	  fwrite( & tdata[k].data, 1, sizeof(tdata[k].data), newFile); 
	  sprintf(consumeStr, "consume CT%d 0%d B%d I%d", *arg, m, buffer[k], k);
	  fputs(consumeStr, logFile);
	  putc('\n', logFile);
	  //fwrite( & buffer[k], 1, sizeof(buffer[k]), newFile); 
	 
	  
	  }
      k++;
	sem_post(&semEmpty);
	pthread_mutex_unlock(&mutexBuffer);
	nanosleep( & t, NULL);
 		if(k>=buffer_size && m>=buffer_size){
		nanosleep( & t, NULL);
		break;
	}
}
}
