#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdio.h>

/*
 * Represents a block in the file system.
 */

// Externally defined block size.
extern int BLOCK_SIZE;

/*
 * Get the number of blocks needed to store the given number of bytes.
 *
 * This function calculates the number of blocks required to store a given
 * number of bytes based on the predefined block size.
 */
int bytes_to_blocks(int bytes);

/*
 * Initializes the file system blocks.
 *
 * This function creates a new file at the specified path, sets its size to
 * NUFS_SIZE, and maps it into memory for block storage. It also initializes
 * the blocks' bitmap and directory.
 */
void blocks_init(const char* path);

/*
 * Frees the memory mapped file system blocks.
 *
 * This function unmaps the memory region used for block storage.
 */
void blocks_free();

/*
 * Returns a pointer to the start of the specified block.
 *
 * This function calculates the memory address of the given block number.
 */
void* blocks_get_block(int bnum);

/*
 * Returns a pointer to the blocks' bitmap.
 *
 * This function provides access to the blocks' bitmap.
 */
void* get_blocks_bitmap();

/*
 * Returns a pointer to the inode bitmap.
 *
 * This function provides access to the inode bitmap.
 */
void* get_inode_bitmap();

/*
 * Allocates a free block.
 *
 * This function searches for a free block in the blocks' bitmap, marks it as
 * used, and returns its block number.
 */
int alloc_block();

/*
 * Frees the specified block.
 *
 * This function marks the specified block as free in the blocks' bitmap.
 */
void free_block(int bnum);

#endif
