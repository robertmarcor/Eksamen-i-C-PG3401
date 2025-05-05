#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define NUM_THREADS 2
#define BYTE_RANGE 256

// wrapped global variables in a struct since pthread_create() only accepts void* arguments
typedef struct
{
   int count[BYTE_RANGE];
   unsigned char buffer[BUFFER_SIZE];
   pthread_mutex_t mutex;
   pthread_cond_t cond_full, cond_empty;
   int bytes_in_buffer;
} thread_data_t;

// Now accepts thread_data_t struct pointer instead of using globals
// All shared variables accessed through the data struct
void *thread_A(void *arg)
{
   thread_data_t *data = (thread_data_t *)arg;
   FILE *fp = fopen("task4_pg2265.txt", "rb");
   if (!fp)
   {
      perror("Failed to open file");
      exit(EXIT_FAILURE);
   }

   while (1)
   {
      pthread_mutex_lock(&(data->mutex));
      while (data->bytes_in_buffer == BUFFER_SIZE)
         pthread_cond_wait(&(data->cond_empty), &(data->mutex));

      int read_bytes = fread(data->buffer + data->bytes_in_buffer, 1,
                             BUFFER_SIZE - data->bytes_in_buffer, fp);
      data->bytes_in_buffer += read_bytes;

      if (read_bytes < BUFFER_SIZE - data->bytes_in_buffer)
      {
         pthread_mutex_unlock(&(data->mutex));
         break;
      }
      pthread_cond_signal(&(data->cond_full));
      pthread_mutex_unlock(&(data->mutex));
   }
   fclose(fp);
   pthread_exit(NULL);
}

// Now accepts thread_data_t struct pointer instead of using globals
// All shared variables accessed through the data struct
// Count array is now part of the thread_data struct
void *thread_B(void *arg)
{
   thread_data_t *data = (thread_data_t *)arg;
   memset(data->count, 0, sizeof(data->count));

   while (1)
   {
      pthread_mutex_lock(&(data->mutex));
      while (data->bytes_in_buffer == 0)
         pthread_cond_wait(&(data->cond_full), &(data->mutex));

      for (int i = 0; i < data->bytes_in_buffer; i++)
         data->count[data->buffer[i]]++;

      data->bytes_in_buffer = 0;
      pthread_cond_signal(&(data->cond_empty));
      pthread_mutex_unlock(&(data->mutex));

      if (data->bytes_in_buffer == 0)
         break;
   }
   for (int i = 0; i < BYTE_RANGE; i++)
      printf("%d: %d\n", i, data->count[i]);
   pthread_exit(NULL);
}

// All previously global variables are now in main
// Initializes the struct for shared data and allocates memory for it
// Passes the thread_data struct to both threads
// cleans up resources at the end
int main(void)
{
   pthread_t threadA, threadB;

   // Create and initialize the shared data
   thread_data_t *thread_data = malloc(sizeof(thread_data_t));
   if (!thread_data)
   {
      perror("Failed to allocate memory");
      exit(EXIT_FAILURE);
   }

   // Initialize shared variables
   thread_data->bytes_in_buffer = 0;
   pthread_mutex_init(&(thread_data->mutex), NULL);
   pthread_cond_init(&(thread_data->cond_full), NULL);
   pthread_cond_init(&(thread_data->cond_empty), NULL);

   if (pthread_create(&threadA, NULL, thread_A, (void *)thread_data) != 0)
   {
      perror("Could not create thread A");
      exit(1);
   }

   if (pthread_create(&threadB, NULL, thread_B, (void *)thread_data) != 0)
   {
      perror("Could not create thread B");
      exit(1);
   }

   if (pthread_join(threadA, NULL) != 0)
   {
      perror("Could not join thread A");
      exit(1);
   }
   if (pthread_join(threadB, NULL) != 0)
   {
      perror("Could not join thread B");
      exit(1);
   }

   // Clean up
   pthread_mutex_destroy(&(thread_data->mutex));
   pthread_cond_destroy(&(thread_data->cond_full));
   pthread_cond_destroy(&(thread_data->cond_empty));
   free(thread_data);

   return 0;
}
