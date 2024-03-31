#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>


#define BUFFER_SIZE 10

struct buffer {
    int data[BUFFER_SIZE];
    int count;
    int index;// for reading and writing
};

void produce(struct buffer* buffer) {
    // Generate random integer
    int item = rand() % 100;
  
    // Add item to the buffer
    buffer->data[buffer->index] = item;
    buffer->index = (buffer->index + 1) % BUFFER_SIZE;
    buffer->count++;
  
    printf("Produced item : %d\n", item); 
    usleep(500000);
                        
}

void producer(struct buffer* buffer) {
int j=0;
    while(1){
  // while(j<15){
        // Check if the buffer has empty space
        if (buffer->count < BUFFER_SIZE) {
           // usleep(500000);
            produce(buffer);

        }
        else {
            printf("Buffer is full. Producer waiting...!\n");
             sleep(1); // Wait for the consumer to read items
        }
    
    }

}



int main() {
    const char* shm_name = "/shm_buffer";
    const int shm_size = sizeof(struct buffer);

    // Create or open the shared memory for reading and writing
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Set the size of the shared memory segment
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Map the shared memory segment into the address space
    struct buffer* buffer = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }



    // Initialize the buffer
    memset(buffer, 0, shm_size);


     buffer->index = 0;

    pid_t pid ;
    
  //  for(int i=0 ; i <= 1 ; i++){
    
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
    sleep(6);
        // Child process (consumer)
         char *args[]={"./consumer",NULL};
        if (execv(args[0],args) == -1) {
            perror("execv");
            exit(1);
        }
    }
    else {
        // Parent process (producer)
        producer(buffer);
       // wait(NULL); // Wait for the child process to terminate
     //  sleep(6);// wait for child process to finish
     //   kill(pid,SIGTERM);// to kill child 
        
    }

 // Cleanup and close the shared memory
    if (munmap(buffer, shm_size) == -1) {
        perror("munmap");
        exit(1);
    }

    if (shm_unlink(shm_name) == -1) {
        perror("shm_unlink");
        exit(1);
    }
    
  //  }
     
    return 0;
}

