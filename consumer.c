#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define BUFFER_SIZE 10

struct buffer {
    int data[BUFFER_SIZE];
    int count;

    int index;
};

void consume(struct buffer* buffer) {
    // Remove item from the buffer
    int item = buffer->data[buffer->index];
    buffer->index = (buffer->index + 1) % BUFFER_SIZE;
    buffer->count--;
    
    //sleep(2);
    printf("Consumed item  : %d\n", item);

}

void consumer(struct buffer* buffer) { 

    while(1){
        if (buffer->count > 0) {
            consume(buffer);
        }
        else {
 	
            printf("Buffer is empty. Consumer waiting... !\n");
            sleep(8); // Wait for the producer to add items 
           
        }
        
    }
}




int main() {


    const char* shm_name = "/shm_buffer";
    const int shm_size = sizeof(struct buffer);

    // Open the shared memory for reading and writing
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Map the shared memory segment into the address space
    struct buffer* buffer = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
       exit(1);
    }

	consumer(buffer);
	
	
	
	 // Cleanup and close the shared memory
    if (munmap(buffer, shm_size) == -1) {
        perror("munmap");
        exit(1);
    }



    return 0;
    }
