#define _GNU_SOURCE
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include "blocks.h"  
#include "func.h"
#include "bitmap.h"
#include "directory.h"

const int BLOCK_COUNT = 256;  
const int NUFS_SIZE = 4096 * 256; 
int BLOCK_SIZE = 4096;

static int   blocks_fd   = -1; 
static void* blocks_base =  0;  

/*
 * Initializes the blocks at the given path.
 *
 * This function creates a new file at the specified path, sets its size to NUFS_SIZE,
 * and maps it into memory for block storage. It also initializes the blocks' bitmap
 * and directory.
 */
void blocks_init(const char* path)  
{
    blocks_fd = open(path, O_CREAT | O_RDWR, 0644); 
    assert(blocks_fd != -1);

    int rv = ftruncate(blocks_fd, NUFS_SIZE);
    assert(rv == 0);

    blocks_base = mmap(0, NUFS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, blocks_fd, 0);
    assert(blocks_base != MAP_FAILED);

    void* bbm = get_blocks_bitmap(); 

    if (bitmap_get(bbm, 0) == 0) {
        bitmap_put(bbm, 0, 1);
        directory_init();
    }
}

/*
 * Frees the memory mapped blocks.
 *
 * This function unmaps the memory region used for block storage.
 */
void blocks_free()
{
    int rv = munmap(blocks_base, NUFS_SIZE);
    assert(rv == 0);
}

/*
 * Returns a pointer to the start of the specified block.
 */
void* blocks_get_block(int bnum)  
{
    return blocks_base + 4096 * bnum;
}

/*
 * Returns a pointer to the blocks' bitmap.
 */
void* get_blocks_bitmap() 
{
    return blocks_get_block(0);
}

/*
 * Returns a pointer to the inode bitmap.
 */
void* get_inode_bitmap()
{
    uint8_t* block = blocks_get_block(0);  
    return (void*)(block + 32);
}

/*
 * Allocates a block.
 *
 * This function searches for a free block in the blocks' bitmap, marks it as used,
 * and returns its block number.
 */
int alloc_block()
{
    void* bbm = get_blocks_bitmap(); 

    for (int ii = 1; ii < BLOCK_COUNT; ++ii) {
        if (!bitmap_get(bbm, ii)) {
            bitmap_put(bbm, ii, 1);
            printf("+ alloc_block() -> %d\n", ii);
            return ii;
        }
    }

    return -1;
}

/*
 * Frees a block.
 *
 * This function marks the specified block as free in the blocks' bitmap.
 */
void free_block(int bnum)
{
    printf("+ free_block(%d)\n", bnum);

    void* bbm = get_blocks_bitmap();  
    bitmap_put(bbm, bnum, 0);
}

/*
 * Get the number of blocks needed to store the given number of bytes.
 */
int bytes_to_blocks(int bytes) {
  int quo = bytes / BLOCK_SIZE;
  int rem = bytes % BLOCK_SIZE;
  if (rem == 0) {
    return quo;
  } else {
    return quo + 1;
  }
}
