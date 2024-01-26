#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "blocks.h"

/*
 * Represents an Inode structure for a filesystem.
 */
typedef struct inode {
    int refs;            // Number of references to this inode
    int32_t mode;        // File mode (permissions and type)
    int size;            // Size of the file in bytes
    int ptrs[2];         // Direct pointers to data blocks
    int iptr;            // Indirect pointer to additional data blocks
    time_t time;         // Last modification time
} inode_t;

/*
 * Prints the attributes of the given inode.
 *
 * Parameters:
 *   node: Pointer to the inode structure
 *
 * Returns:
 *   None
 */
void print_inode(inode_t* node);

/*
 * Retrieves the inode associated with the given inode number.
 *
 * Parameters:
 *   inum: Inode number to look up
 *
 * Returns:
 *   Pointer to the inode structure with the specified inode number
 */
inode_t* get_inode(int inum);

/*
 * Allocates a new inode and returns its inode number.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Inode number of the newly allocated inode upon success, -1 otherwise
 */
int alloc_inode();

/*
 * Frees the inode associated with the given inode number.
 *
 * Parameters:
 *   inum: Inode number to free
 *
 * Returns:
 *   None
 */
void free_inode(int inum);

/*
 * Increases the size of the inode to accommodate the given size.
 *
 * Parameters:
 *   node: Pointer to the inode structure
 *   size: New size to accommodate
 *
 * Returns:
 *   New size of the inode upon success
 */
int grow_inode(inode_t* node, int size);

/*
 * Shrinks the size of the inode by the specified amount.
 *
 * Parameters:
 *   node: Pointer to the inode structure
 *   size: Size to shrink
 *
 * Returns:
 *   0 upon success
 */
int shrink_inode(inode_t* node, int size);

/*
 * Retrieves the page number of the given inode based on the file page number.
 *
 * Parameters:
 *   node: Pointer to the inode structure
 *   fpn: File page number
 *
 * Returns:
 *   Page number corresponding to the given file page number
 */
int inode_get_pnum(inode_t* node, int fpn);

#endif

