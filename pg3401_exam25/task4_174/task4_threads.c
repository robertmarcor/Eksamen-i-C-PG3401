#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include "task4_threads.h"

#define BUFFER_SIZE 4096
#define NUM_THREADS 2
#define BYTE_RANGE 256
#define TEA_BLOCK_SIZE 8 // for 64-bit block

// wrapped global variables in a struct to pass to threads
typedef struct
{
   int count[BYTE_RANGE];
   unsigned char buffer[BUFFER_SIZE];
   sem_t *mutex;
   sem_t *empty; // instead of cond_empty
   sem_t *full;  // instead of cond_full
   int bytes_in_buffer;
   // file data for hash,encryption and size
   char *filename;
   unsigned char *file_contents;
   size_t file_size;
} thread_data_t;

/*
   function to apply PKCS5 padding to input data.
   Calculates the number of padding bytes needed to reach a multiple of 8 (the TEA_BLOCK_SIZE)
   padding is applied at the end of the data, which allows it to easily be removed when decrypting.
*/
unsigned char *pkcs5_pad(unsigned char *data, size_t data_len, size_t *padded_len)
{
   // Calculate padding length
   size_t pad_len = TEA_BLOCK_SIZE - (data_len % TEA_BLOCK_SIZE);
   if (pad_len == 0)
   {
      pad_len = TEA_BLOCK_SIZE; // If already a multiple of 8, add a full block
   }

   // Allocate memory for padding
   *padded_len = data_len + pad_len;
   unsigned char *padded_data = malloc(*padded_len);
   if (!padded_data)
   {
      perror("Failed to allocate memory for padding");
      return NULL;
   }

   // Copy original data
   memcpy(padded_data, data, data_len);

   // Add padding bytes at the end
   for (size_t i = 0; i < pad_len; i++)
   {
      padded_data[data_len + i] = (unsigned char)pad_len;
   }

   return padded_data;
}

// Now accepts thread_data_t struct pointer instead of using globals
// All shared variables accessed through the data struct
// Removed the condition variables and replaced with semaphores
void *thread_A(void *arg)
{
   thread_data_t *data = (thread_data_t *)arg;
   FILE *fp = fopen(data->filename, "rb");
   if (!fp)
   {
      perror("Failed to open file");
      exit(EXIT_FAILURE);
   }
   printf("\nThread_A : %s opened successfully\n", data->filename);

   // Get file size for allocation
   fseek(fp, 0, SEEK_END);
   data->file_size = ftell(fp);
   rewind(fp);

   // Allocate memory to hold the entire file
   data->file_contents = malloc(data->file_size);
   if (!data->file_contents)
   {
      perror("Failed to allocate memory for file contents");
      fclose(fp);
      exit(EXIT_FAILURE);
   }

   // Read the entire file into memory
   fread(data->file_contents, 1, data->file_size, fp);

   while (1)
   {
      // Wait for empty space in buffer
      sem_wait(data->empty);
      sem_wait(data->mutex);

      int read_bytes = fread(data->buffer + data->bytes_in_buffer, 1,
                             BUFFER_SIZE - data->bytes_in_buffer, fp);
      data->bytes_in_buffer += read_bytes;

      // Exit
      sem_post(data->mutex);
      // Signal that buffer has data for thread B
      sem_post(data->full);

      // Check if we should exit loop
      if (read_bytes < BUFFER_SIZE - data->bytes_in_buffer)
      {
         break;
      }
   }
   sem_post(data->full);
   fclose(fp);
   pthread_exit(NULL);
}

// Now accepts thread_data_t struct pointer instead of using globals
// All shared variables accessed through the data struct
// Count array is now part of the thread_data struct
// Removed the condition variables and replaced with semaphores
void *thread_B(void *arg)
{
   thread_data_t *data = (thread_data_t *)arg;
   memset(data->count, 0, sizeof(data->count));

   while (1)
   {
      // Wait for data in buffer
      sem_wait(data->full);
      sem_wait(data->mutex);

      // Check there is data in buffer, if no data then break loop
      if (data->bytes_in_buffer == 0)
      {
         sem_post(data->mutex);
         break;
      }

      for (int i = 0; i < data->bytes_in_buffer; i++)
         data->count[data->buffer[i]]++;

      data->bytes_in_buffer = 0;

      // Exit
      sem_post(data->mutex);

      // Signal that buffer is empty so thread A can fill it
      sem_post(data->empty);
   }

   // REMOVED
   // for (int i = 0; i < BYTE_RANGE; i++)
   //   printf("%d: %d\n", i, data->count[i]);

   // Calculate DJB2 hash of the file instead
   FILE *fp = fopen(data->filename, "rb");
   if (!fp)
   {
      perror("Failed to open file for hashing");
      pthread_exit(NULL);
   }
   printf("\nThread_B: file for hashing opened\n");
   int hash_value;
   int result = Task2_SimpleDjb2Hash(fp, &hash_value);
   fclose(fp);

   // Write hash to file only if hash calculation succeeded
   if (result == 0)
   {
      printf("Thread_B: hash calculated successfully with DJB2\n");
      FILE *hash_file = fopen("task4_pg2265.hash", "w");
      if (!hash_file)
      {
         perror("Failed to open hash file for writing");
         pthread_exit(NULL);
      }
      fprintf(hash_file, "%d", hash_value);
      fclose(hash_file);
      printf("Thread_B: operation succeeded and saved to task4_pg2265.hash\n");

      // Info to user
      printf("\n--- output of hash file ---\n");
      system("xxd task4_pg2265.hash");

      // Encryption process
      printf("\n--- Starting encryption process ---\n");
      printf("Original file size: %zu bytes\n", data->file_size);

      // read file from memory and check it byte size and if its a multiple of 8
      if (data->file_size % TEA_BLOCK_SIZE == 0)
      {
         printf("File is already a multiple of %d\n", TEA_BLOCK_SIZE);
      }
      else
      {
         printf("File is not a multiple of %d\n", TEA_BLOCK_SIZE);
      }

      // Calculate padding and apply PKCS5
      printf("Applying PKCS5 padding...\n");
      size_t padded_size;
      unsigned char *padded_data = pkcs5_pad(data->file_contents, data->file_size, &padded_size);
      size_t padding_bytes = padded_size - data->file_size;
      printf("Added %zu bytes of padding\n", padding_bytes);
      printf("File size after padding: %zu bytes\n", padded_size);
      // Create encryption key
      unsigned char key_bytes[16];
      FILE *keyfile = fopen("tea.key", "rb");
      if (!keyfile)
      {
         perror("Failed to open tea.key\n is file missing?");
         exit(1);
      }
      fread(key_bytes, 1, 16, keyfile);
      fclose(keyfile);
      unsigned int key[4];
      memcpy(key, key_bytes, 16);
      printf("encrypted with key: [ ");
      for (int i = 0; i < 16; i++)
      {
         printf("%02x", key_bytes[i]);
      }
      printf(" ]\n");
      // Encrypt blocks in-place
      for (size_t i = 0; i < padded_size; i += TEA_BLOCK_SIZE)
      {
         encipher((unsigned int *)(padded_data + i),
                  (unsigned int *)(padded_data + i), key);
      }

      printf("Encryption complete\n");

      // Save encrypted file
      FILE *enc_file = fopen("task4_pg2265.enc", "wb");
      if (!enc_file)
      {
         printf("Failed to create encrypted file\n");
      }
      else
      {
         fwrite(padded_data, 1, padded_size, enc_file);
         fclose(enc_file);
         printf("Encrypted data saved to task4_pg2265.enc\n");
      }

      free(padded_data);
   }
   else
   {
      perror("Failed to calculate hash");
   }

   pthread_exit(NULL);
}

// All previously global variables are now in main
// Initializes the struct for shared data and allocates memory for it
// Passes the thread_data struct to both threads
// cleans up resources at the end
int main(int argc, char **argv)
{
   pthread_t threadA, threadB;

   if (argc != 2)
   {
      printf("Usage: %s <filename.txt>\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   printf("Filename to read: %s\n", argv[1]);

   // Create and initialize the shared data
   thread_data_t *thread_data = malloc(sizeof(thread_data_t));
   if (!thread_data)
   {
      perror("Failed to allocate memory");
      exit(EXIT_FAILURE);
   }
   thread_data->bytes_in_buffer = 0;

   // Allocate memory for semaphores
   thread_data->mutex = malloc(sizeof(sem_t));
   thread_data->empty = malloc(sizeof(sem_t));
   thread_data->full = malloc(sizeof(sem_t));

   // Failesafe guards for init
   // Initialize mutex semaphore (controls access to buffer)
   if (sem_init(thread_data->mutex, 0, 1) != 0) // Initial value 1
   {
      perror("Failed to initialize mutex semaphore");
      exit(EXIT_FAILURE);
   }

   // Initialize empty semaphore (counts empty slots in buffer)
   if (sem_init(thread_data->empty, 0, BUFFER_SIZE) != 0) // Initial value = BUFFER_SIZE
   {
      perror("Failed to initialize empty semaphore");
      exit(EXIT_FAILURE);
   }

   // Initialize full semaphore (counts filled slots in buffer)
   if (sem_init(thread_data->full, 0, 0) != 0) // Initial value 0
   {
      perror("Failed to initialize full semaphore");
      exit(EXIT_FAILURE);
   }

   thread_data->filename = argv[1];

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
   sem_destroy(thread_data->mutex);
   sem_destroy(thread_data->empty);
   sem_destroy(thread_data->full);
   free(thread_data->mutex);
   free(thread_data->empty);
   free(thread_data->full);
   free(thread_data->file_contents);
   free(thread_data);

   return 0;
}
