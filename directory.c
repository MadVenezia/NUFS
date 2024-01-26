#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "directory.h"
#include "slist.h"
#include "blocks.h"  
#include "inode.h"
#include "func.h"
#include "bitmap.h"

/*
 * Represents functions for managing directories.
 */

// Current root inode
int rooti = 0;
// Maximum number of entries in a directory
int MAX_ENTR = 4096 / sizeof(dirent_t); 


/*
 * Initializes the root directory.
 *
 * This function initializes the root directory by creating the root inode,
 * setting its attributes, allocating necessary blocks, and initializing the
 * root directory entries.
 */
void directory_init() {
    // Inode bitmap
    void* ibm = get_inode_bitmap();	
    // Creates the root inode
    rooti = alloc_inode();
    
    inode_t* root = get_inode(rooti);

    root->refs = 1;

    // Root is the inode root
    root->mode = 040755;
    root->size = 0;
    root->ptrs[0] = alloc_block();  
    root->ptrs[1] = alloc_block(); 
    root->iptr = 0;
    root->time = time(0);

    // Sets the inode bitmap first spot to 1, because root was created
    bitmap_put(ibm, 0, 1);

    // Root points to itself
    directory_put(root, ".", rooti);
}

/*
 * Looks up the specified directory inode and name.
 *
 * This function searches for the given directory name within the specified
 * directory inode and returns the inode number of the directory if found.
 */
int directory_lookup(inode_t* dd, const char* name) {  
    dirent_t* entries = (dirent_t*) blocks_get_block(dd->ptrs[0]);  
    for (int i = 0; i < MAX_ENTR; i++, entries = (void*)entries + sizeof(dirent_t)) {
        if(streq(entries->name, name)) {
            return entries->inum;
        }
    }
    return -ENOENT;
}

/*
 * Looks up the root of the tree.
 *
 * This function parses the given path and returns the inode number of the
 * corresponding root directory.
 */
int tree_lookup(const char* path) {

    int result = 0;
    inode_t* root = get_inode(rooti); 

    slist_t* list = s_split(path, '/');
    list = list->next;

    if (streq(path, "/")) {
        s_free(list);
        return rooti;
    } else {
        char* filename = directory_get_name(path);
        result = directory_lookup(get_inode(directory_get_super(path)), filename);
        s_free(list);
        return result;
    }
}

/*
 * Places an inode into the directory with the given inode number and name.
 *
 * This function adds a directory entry with the specified inode number and
 * name to the given directory inode.
 */
int directory_put(inode_t* dd, const char* name, int inum) {  
    dirent_t* entries = (dirent_t*) blocks_get_block(dd->ptrs[0]); 
    for (int i = 0; i < MAX_ENTR; i++) {
        if(entries[i].inum == 0) {
            entries[i].inum = inum;
            strcpy(entries[i].name, name);
            dd->size += sizeof(dirent_t); 
            dd->time = time(0);
            return 0;
        }
    }
    return -1;
}

/*
 * Deletes the given directory with the given name.
 */
int directory_delete(inode_t* dd, const char* name) {  
    int rv = 0;
    printf(" + directory_delete(%s)\n", name);
    int rm = 0;
    dirent_t* entries = (dirent_t*) blocks_get_block(dd->ptrs[0]);
    for (int i = 0; i < MAX_ENTR; i++) {
        if(!(strcmp(entries[i].name, name))) {
            rm = i;
            goto delete_found;
        }
    }
    delete_found:
    dd->time = time(0);
    memcpy(&entries[rm], &entries[rm + 1], 4096 - ((rm + 1) * sizeof(dirent_t)));  
    return rv;
}

/*
 * Creates a list of all the directories in a particular path.
 */
slist_t* directory_list(const char* path) {
    int index = get_inode(tree_lookup(path))->ptrs[0];
    dirent_t* entries = (dirent_t*) blocks_get_block(index);  

    slist_t* result = 0;
    for (int i = 0; i < MAX_ENTR && strcmp(entries[i].name, ""); i++) {
        result = s_cons(entries[i].name, result);
    }
    return result;
}

/*
 * Retrieves the inode number of the parent directory.
 *
 * This function parses the given path and returns the inode number of the
 * parent directory of the specified path.
 */
int directory_get_super(const char* path) {

    int result = 0;
    slist_t* list = s_split(path, '/');
    list = list->next;
    
    // in root directory
    if (!list->next) {
        return 0;
    }

    inode_t* parent = get_inode(rooti);  // Updated type
    for (; list->next && list->next->next && result > 0; list = list->next) {
        result = directory_lookup(parent, list->data);
        parent = get_inode(result);
    }
    result = directory_lookup(parent, list->data);
    s_free(list);

    return result;
}

/*
 * Returns the name of the furthest node in the given path.
 */
char* directory_get_name(const char* path) {
    slist_t* list = s_split(path, '/');
    while(list->next) {
        list = list->next;
    }
    return list->data;
}
