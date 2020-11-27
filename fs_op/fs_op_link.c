/*
 * fs_op_mkdir.c
 *
 * description: fs_mkdir function for CS 5600 / 7600 file system
 *
 * CS 5600, Computer Systems, Northeastern CCIS
 * Peter Desnoyers, November 2016
 * Philip Gust, March 2019, March 2020
 */

#include <sys/stat.h>
#include <string.h>
#include "fs_util_file.h"
#include "fs_util_dir.h"
#include "fs_util_path.h"
#include "fs_util_vol.h"
#include "fsx600.h"

/**
 *  link - create a Hard Link for a file
 *
 * Errors
 *   -ENOENT   - no directory exist
 *   -EISDIR   - the source file is a directory
 *
 * @param path path to file
 * @param linkpath path of the hard link
 * @return 0 if successful, or -error number
 */
int fs_link(const char* path, const char* linkpath)
{
    // get directory inode and leaf for source path
    char src_leaf[FS_FILENAME_SIZE];
    int srcdir_inum = get_inode_of_path_dir(path, src_leaf);

    /* find directory entry block and index */
    int blkno;
    char buf[FS_BLOCK_SIZE];
    int entno = get_dir_entry_block(srcdir_inum, buf, &blkno, src_leaf);
    if (entno < 0) {
        return -ENOENT;
    }
    // get inode of directory entry
    struct fs_dirent *de = (void*)buf;
    int inum = de[entno].inode;
    /* ensure that entry being removed is not a directory */
    if (S_ISDIR(fs.inodes[inum].mode)) {
        return -EISDIR;
    }

    char dst_leaf[FS_FILENAME_SIZE];
    int dstdir_inum = get_inode_of_path_dir(linkpath, dst_leaf);
    return do_link(inum, dstdir_inum, dst_leaf);

}
