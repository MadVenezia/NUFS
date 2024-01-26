#include "inode.h"
#include "blocks.h"  
#include "bitmap.h"
#include <stdint.h>
#include "func.h"

// Maximum number of nodes (inodes)
int MAX_NODE = 4096 / sizeof(inode_t);  

/*
 * Retrieves the inode associated with the given inode number.
 *
 * Parameters:
 *   inum: Inode number to look up
 *
 * Returns:
 *   Pointer to the inode structure with the specified inode number
 */
inode_t* get_inode(int inum) { 
    return (inode_t*)(((uint8_t*)blocks_get_block(0)) + 64) + (inum * sizeof(inode_t));  
}

/*
 * Allocates an inode with the given inode number.
 *
 * Parameters:
 *   inum: Inode number to allocate
 *
 * Returns:
 *   Inode number of the allocated inode upon success, -1 otherwise
 */
int alloc_inode(int inum) {
    void* inbm = get_inode_bitmap();
  
    for (int i = 0; i < MAX_NODE; i++) {
        if (!bitmap_get(inbm, i)) {
            bitmap_put(inbm, i, 1);
            return i;
        }
    }
    return -1;
}

/*
 * Frees the inode associated with the given inode number.
 *
 * Parameters:
 *   inum: Inode number to free
 *
 * Returns:
 *   None
 */
void free_inode(int inum) {
    inode_t* node = get_inode(inum); 
    void* inbm = get_inode_bitmap();

    if (node->refs > 1) {
        node->refs = node->refs - 1;
        return;
    } else {
        free_block(node->ptrs[0]);
        free_block(node->ptrs[1]);
        memset(node, 0, sizeof(inode_t));  
        bitmap_put(inbm, inum, 0);
    }
}

/*
 * Expands the size of the inode to accommodate the given size.
 *
 * Parameters:
 *   node: Pointer to the inode structure
 *   size: New size to accommodate
 *
 * Returns:
 *   New size of the inode upon success
 */
int grow_inode(inode_t* node, int size) {  
    int new_blocks = bytes_to_blocks(size); 

    if (new_blocks > 2) {
        new_blocks = new_blocks - 2;
        if (node->iptr == 0) {
            node->iptr = alloc_block();
        }
        int* indirect = (int*)blocks_get_block(node->iptr); 

        for (int i = 0; i < new_blocks; i++) { 
            if (!indirect[i]) {
                indirect[i] = alloc_block();
            }
        }
    }
    node->size = size;
    return node->size;
}

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
int inode_get_pnum(inode_t* node, int fpn) {
    return fpn < 2 ? node->ptrs[fpn] : ((int*)blocks_get_block(node->iptr))[fpn - 2];  
}

/*
 * TODO: Shrink the size of the inode.
 *
 * Parameters:
 *   node: Pointer to the inode structure
 *   size: Size to shrink
 *
 * Returns:
 *   0 upon success
 */
int shrink_inode(inode_t* node, int size) {  
    node->size = node->size - size;
    return 0;
}
