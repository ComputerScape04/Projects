#include <stdio.h>
#include <string.h>
#include <unistd.h> // contains brk() and sbrk()

// GLOBAL VARS
void *mem_start;
void *last_available_address_space;
int has_init = 0;

// this will point to a newly allocated memory location
typedef struct {
  size_t size; // size (in bytes) of each block
  int free_m;  // whether the block is in use
} mem_control_block;

// function to initialize malloc
void init_smalloc() {
  /*
   * get the last addr
   * */
  last_available_address_space = sbrk(0);

  // since we don't have any memory to manage at the beginning
  // we set the mem_start = last_available_address_space
  mem_start = last_available_address_space;

  // set init = 1
  has_init = 1;
}

void *smalloc(size_t size) {
  // define a memory location
  // this is going to be returned at the end of smalloc()
  void *memory_location;

  // init to 0 because if no available addr_ space,
  // we will extend the program break
  memory_location = 0;

  // initialize a mem_control_block
  // this will point to the memory_location that we utilize
  mem_control_block *current_location_mem_control_block;

  // initalize smalloc() if it hasn't been initialized yet
  if (!has_init) {
    init_smalloc();
  }

  /*
   * program loop
   *
   * 1. keep looping from mem_start till last address space
   * 2. if we find that there is a large enough addr space,
   *    then we use that
   * 3. Otherwise, we keep looking for a big enough space by adding "size"
   * 4. if we reach the end of the addr space, we'll allocate more via the OS
   *    by using sbrk(size)
   */

  // tells our current location in memory
  void *current_location;

  current_location = mem_start;

  // the amount of effective memory to allocate (size + the mem_ctrl_block)
  size_t num_bytes = size + sizeof(mem_control_block);

  while (current_location != last_available_address_space) {

    // cast the current location to mem_control_block
    current_location_mem_control_block = (mem_control_block *)current_location;

    // check if there's available memory
    if (current_location_mem_control_block->free_m) {

      // we found the mem location -> now check for availability

      if (current_location_mem_control_block->size >= num_bytes) {
        // mark it as unavailable
        current_location_mem_control_block->free_m = 0;

        memory_location = current_location;

        break;
      }
    }

    // if the mem block we were at was not available or of enough size
    // we will increment to the next mem block

    current_location += current_location_mem_control_block->size;
  }

  // if we don't find any valid addr space
  // allocate more via sbrk()

  if (!memory_location) {

    memory_location = sbrk(num_bytes);

    last_available_address_space = memory_location + num_bytes;

    // assign the mem control block to the newly mapped memory
    current_location_mem_control_block = memory_location;

    // mark it unavailable
    current_location_mem_control_block->free_m = 0;

    // set the size to num_bytes = size + sizeof(mem_control_block)
    current_location_mem_control_block->size = num_bytes;
  }

  // we need to move the ptr past the mem_control_block
  memory_location += sizeof(mem_control_block);

  // return the start of the newly allocated memory
  return memory_location;
}

/*
 * the implementation of free does not actually clear the memory.
 * it sets the allocated memory's mem_control_block as "free"
 * meaning it is ready to be used again.
 *
 * but the data present there is going to persist until the end of the program
 *
 **/

void sfree(void *ptr) {
  /*
   *  ptr ---> points to the block of memory allocated
   *  back up by sizeof(mem_control_block)
   *  set mcb -> free = 0;
   *
   */

  mem_control_block *mcb;

  mcb = ptr - sizeof(mem_control_block);

  // set is as free (1)
  mcb->free_m = 1;

  // brk((void*)mcb);

  return;
}

int main() {

  char *m = smalloc(13);

  strcpy(m, "Hello World!");

  // printf("%s\n", m);

  char *new_m = smalloc(10);

  strcpy(new_m, "pavin");

  sfree(new_m);

  sfree(m);
  
  char* final_m = smalloc(5);

  strcpy(final_m, "Hey!");

  sfree(final_m);

  return 0;
}
