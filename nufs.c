#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <bsd/string.h>
#include <assert.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include "inode.h"
#include "func.h"
#include "bitmap.h"
#include "blocks.h"  
#include "slist.h"
#include "directory.h"


//int BLOCK_SIZE = 4096;  

// implementation for: man 2 access
// Checks if a file exists.
// Checks if a file exists.
int nufs_access(const char *path, int mask)
{
    // Placeholder for return value
    int rv = 0;

    // Check if the file exists in the filesystem
    if (tree_lookup(path) < 0) {
        rv = -ENOENT;
    }

    // Print debugging information
    printf("access(%s, %04o) -> %d\n", path, mask, rv);
    return rv;
}

// Gets an object's attributes (type, permissions, size, etc).
int nufs_getattr(const char *path, struct stat *st)
{
    // Placeholder for return value
    int rv = 0;

    // Retrieve the inode number associated with the file path
    int inum = tree_lookup(path);

    // Check if the file does not exist
    if (inum == -ENOENT) {
        return rv = -ENOENT;
    } else {
        // Retrieve the inode and populate the stat structure
        inode_t* node = get_inode(inum);
        st->st_mode = node->mode;
        st->st_size = node->size;
        st->st_nlink = node->refs;
        st->st_mtime = node->time;
    }

    // Print debugging information
    printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode, st->st_size);
    return rv;
}

// Lists the contents of a directory.
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    struct stat st;

    // Iterate through the directory entries and fill the buffer
    for (slist_t* list = directory_list(path); list; list = list->next) {
        filler(buf, list->data, &st, 0);
    }

    // Print debugging information
    printf("readdir(%s) -> %d\n", path, 0);
    return 0;
}

// Makes a filesystem object like a file or directory.
int nufs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    // Placeholder for return value
    int rv = -1;

    // Allocate a new inode and initialize its attributes
    int inum = alloc_inode();
    inode_t* newnode = get_inode(inum);
    int dirNum = directory_get_super(path);
    inode_t* node = get_inode(dirNum);

    newnode->ptrs[0] = alloc_block();
    newnode->ptrs[1] = alloc_block();
    newnode->refs = 1;
    newnode->mode = mode;
    newnode->iptr = 0;
    newnode->size = 0;
    newnode->time = time(0);

    // Update the directory entry with the new inode number
    rv = directory_put(get_inode(directory_get_super(path)), directory_get_name(path), inum);

    // Print debugging information
    printf("mknod(%s, %04o) -> %d\n", path, mode, rv);

    return rv;
}

// Creates a directory.
int nufs_mkdir(const char *path, mode_t mode)
{
    // Use mknod to create a directory
    int rv = nufs_mknod(path, mode | 040000, 0);
    printf("mkdir(%s) -> %d\n", path, rv);
    return rv;
}

// Removes a file or directory.
int remove(const char* path) {
    // Placeholder for return value
    int rv = -1;

    // Retrieve the inode number and free the corresponding inode
    int inum = tree_lookup(path);
    free_inode(inum);

    // Delete the directory entry
    rv =  directory_delete(get_inode(directory_get_super(path)), directory_get_name(path));
    return rv;
}

// Removes a file.
int nufs_unlink(const char *path)
{
    // Remove the file
    int rv = remove(path);
    printf("unlink(%s) -> %d\n", path, rv);
    return rv;
}

// Creates a hard link.
int nufs_link(const char *from, const char *to)
{
    // Placeholder for return value
    int rv = -1;

    // Retrieve inode numbers
    int fromNum = tree_lookup(from);
    inode_t* parentNode = get_inode(directory_get_super(to));
    inode_t* fromNode = get_inode(fromNum);
    fromNode->refs += 1;

    // Update the directory entry
    rv = directory_put(parentNode, directory_get_name(to), fromNum);

    // Print debugging information
    printf("link(%s => %s) -> %d\n", from, to, rv);
    return rv;
}

// Removes a directory.
int nufs_rmdir(const char *path)
{
    // Remove the directory
    int rv = remove(path);
    printf("rmdir(%s) -> %d\n", path, rv);
    return rv;
}

// Renames a file or directory.
int nufs_rename(const char *from, const char *to)
{
    // Placeholder for return value
    int rv = 0;

    // Retrieve inode numbers
    int from_node_num = tree_lookup(from);
    inode_t* from_node = get_inode(from_node_num);
    inode_t* from_parent_node = get_inode(directory_get_super(from));
    inode_t* to_parent_node = get_inode(directory_get_super(to));

    // Update directory entries
    directory_put(to_parent_node, directory_get_name(to), from_node_num);
    directory_delete(from_parent_node, directory_get_name(from));

    // Print debugging information
    printf("rename(%s => %s) -> %d\n", from, to, rv);
    return rv;
}

// Changes file permissions.
int nufs_chmod(const char *path, mode_t mode)
{
    // Placeholder for return value
    int rv = 0;

    // Check if the file exists
    if (tree_lookup(path) < 0) {
        rv = -1;
    } else {
        // Update the mode of the file
        inode_t* node  = get_inode(tree_lookup(path));
        node->mode = mode;
    }
    // Print debugging information
    printf("chmod(%s, %04o) -> %d\n", path, mode, rv);
    return rv;
}

// Truncates a file to the specified size.
int nufs_truncate(const char *path, off_t size)
{
    // Placeholder for return value
    int rv = 0;

    // Update the file size
    get_inode(tree_lookup(path))->size = size;

    // Print debugging information
    printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
    return rv;
}

// Opens a file.
int nufs_open(const char *path, struct fuse_file_info *fi)
{
    // Placeholder for return value
    int rv = 0;
    // Print debugging information
    printf("open(%s) -> %d\n", path, rv);
    return rv;
}

int nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    // Placeholder value for return
    int rv = 6;

    // Retrieve the inode associated with the file path
    inode_t* node = get_inode(tree_lookup(path));

    // Calculate initial block and remainder for efficient block access
    int remainder = offset % BLOCK_SIZE;
    int initialBlock = offset / BLOCK_SIZE;

    // Track the current pointer for buffer manipulation
    int currentPtr = 0;

    // Loop through the necessary blocks to read data
    for (int i = initialBlock; i < bytes_to_blocks(size) + initialBlock; i++) {
        char* data = blocks_get_block(inode_get_pnum(node, i));

        // Handle the first page's data read
        if (i == initialBlock) {
            int length = BLOCK_SIZE - remainder;
            memcpy(buf, data + remainder, length);
            currentPtr += length;
        } else {
            // Copy the entire block to the buffer
            memcpy(buf + currentPtr, data, BLOCK_SIZE);
            currentPtr += BLOCK_SIZE;
        }
    }

    // Print debugging information
    printf("read(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
    return currentPtr;
}

int nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    // Retrieve the inode associated with the file path
    inode_t* node = get_inode(tree_lookup(path));

    // Resize the inode if necessary
    grow_inode(node, size + offset);

    // Track the current pointer for buffer manipulation
    int currentPtr = 0;

    // Calculate initial page and remainder for efficient block access
    int initialPage = offset / BLOCK_SIZE;
    int remainder = offset % BLOCK_SIZE;

    // Loop through the necessary pages to write data
    for (int i = initialPage; i < bytes_to_blocks(size) + initialPage; i++) {
        char* data = blocks_get_block(inode_get_pnum(node, i));

        // Handle the first page's data write
        if (i == initialPage) {
            int length = BLOCK_SIZE - remainder;
            memcpy(data + remainder, buf + currentPtr, length);
            currentPtr += length;
        } else {
            // Copy the entire block from the buffer
            memcpy(data, buf + currentPtr, BLOCK_SIZE);
            currentPtr += BLOCK_SIZE;
        }
    }

    // Update inode size and modification time
    node->size = size + offset;
    node->time = time(0);
    // Placeholder value for return
    int rv = size;

    // Print debugging information
    printf("node size: %d\n", node->size);
    printf("write(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);
    return rv;
}

int nufs_utimens(const char* path, const struct timespec ts[2])
{
    // Placeholder value for return
    int rv = 0;

    // Retrieve the inode associated with the file path
    inode_t* node = get_inode(tree_lookup(path));
    
    // Update inode modification time
    node->time = ts[1].tv_sec;

    // Print debugging information
    printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n",
           path, ts[0].tv_sec, ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
    return rv;
}

int nufs_ioctl(const char* path, int cmd, void* arg, struct fuse_file_info* fi,
           unsigned int flags, void* data)
{
    // Placeholder value for return
    int rv = -1;

    // Print debugging information
    printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
    return rv;
}

int nufs_symlink(const char* to, const char* from) {
    // Placeholder value for return
    int rv = 0;

    // Create a symbolic link by creating a new inode and writing the target path
    rv = nufs_mknod(from, 0120000, 0);
    if (rv < 0) {
        return rv;
    }
    rv = nufs_write(from, to , strlen(to), 0, 0);

    // Print debugging information
    printf("symlink(%s, %s) -> (%d)\n", from, to, rv);
    return rv;
}

int nufs_readlink(const char* path, char* buf, size_t size) {
    // Placeholder value for return
    int rv = 0;

    // Read the content of a symbolic link
    rv = nufs_read(path, buf, size, 0 , 0);

    // Print debugging information
    printf("readlink(%s) -> (%d)\n", path, rv);
    return rv;
}

void nufs_init_ops(struct fuse_operations* ops)
{
    // Initialize the FUSE operations structure with implemented callbacks
    memset(ops, 0, sizeof(struct fuse_operations));
    ops->access   = nufs_access;
    ops->getattr  = nufs_getattr;
    ops->readdir  = nufs_readdir;
    ops->mknod    = nufs_mknod;
    ops->mkdir    = nufs_mkdir;
    ops->link     = nufs_link;
    ops->unlink   = nufs_unlink;
    ops->rmdir    = nufs_rmdir;
    ops->rename   = nufs_rename;
    ops->chmod    = nufs_chmod;
    ops->truncate = nufs_truncate;
    ops->open     = nufs_open;
    ops->read     = nufs_read;
    ops->write    = nufs_write;
    ops->utimens  = nufs_utimens;
    ops->ioctl    = nufs_ioctl;
    ops->readlink = nufs_readlink;
    ops->symlink  = nufs_symlink;
}

struct fuse_operations nufs_ops;

int main(int argc, char *argv[])
{
    // Ensure valid command line arguments
    assert(argc > 2 && argc < 6);

    // Print information about mounting data file
    printf("TODO: mount %s as data file\n", argv[argc-1]);

    // Initialize block system and FUSE operations
    blocks_init(argv[--argc]);
    nufs_init_ops(&nufs_ops);

    // Run FUSE with the specified operations
    return fuse_main(argc, argv, &nufs_ops, NULL);
}
