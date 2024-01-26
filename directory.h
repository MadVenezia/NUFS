#ifndef DIRECTORY_H
#define DIRECTORY_H

#define DIR_NAME 48

#include "inode.h"  
#include "slist.h"  
#include "blocks.h"
#include "func.h"
#include "bitmap.h"

/*
 * Represents functions for managing directories.
 */

typedef struct dirent {
    char name[DIR_NAME];
    int inum;
    char _reserved[12];
} dirent_t;

/*
 * Initializes the root directory.
 *
 * This function creates the root inode, sets its attributes, allocates
 * necessary blocks, and initializes the root directory entries.
 */
void directory_init();

/*
 * Looks up the specified directory inode and name.
 *
 * This function searches for the given directory name within the specified
 * directory inode and returns the inode number of the directory if found.
 */
int directory_lookup(inode_t* dd, const char* name);

/*
 * Looks up the root of the directory tree.
 *
 * This function parses the given path and returns the inode number of the
 * corresponding root directory.
 */
int tree_lookup(const char* path);

/*
 * Places an inode into the directory with the given inode number and name.
 *
 * This function adds a directory entry with the specified inode number and
 * name to the given directory inode.
 */
int directory_put(inode_t* dd, const char* name, int inum);

/*
 * Deletes the specified directory entry.
 *
 * This function removes the directory entry with the given name from the
 * specified directory inode.
 */
int directory_delete(inode_t* dd, const char* name);


/*
 * Creates a list of all directories in a particular path.
 *
 * This function generates a list of directory names within the specified
 * path based on the directory entries.
 */
slist_t* directory_list(const char* path);


void print_directory(inode_t* dd);

/*
 * Retrieves the inode number of the parent directory.
 *
 * This function parses the given path and returns the inode number of the
 * parent directory of the specified path.
 */
int directory_get_super(const char* path);

/*
 * Returns the name of the furthest node in the given path.
 *
 * This function parses the given path and returns the name of the furthest
 * node in the path.
 */
char* directory_get_name(const char* path);


#endif
