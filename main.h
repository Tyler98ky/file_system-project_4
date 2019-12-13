//
// Created by tug65404 on 12/10/2019.
//

#ifndef FILE_SYSTEM_MAIN_H
#define FILE_SYSTEM_MAIN_H

//#define MAX_SIZE    15
//#define MAX_SIZE 32
//#define MAX_SIZE            64


//typedef struct
//{
//    // Directory info
//    int directory_location;
//    int directory_size;
//
//    // Data info
//    int beginning_of_data;
//} superBlock;
//
///* file information */
//typedef struct
//{
//    int isOccupied;                   // whether the file is being isOccupied, 0 for no, 1 for yes
//    char fileName[MAX_SIZE]; // file fileName
//    int size;                    // file size
//    int beginningIndex;                    // first data block
//    int blockCount;              // number of blocks
//    int fdCount;                // number of file descriptors using this file
//} metadata;
//
///* file descriptor */
//typedef struct
//{
//    int isOccupied;                   // whether the file descriptor is being isOccupied
//    int file;                   // file index from 0-63 inclusive
//    int offset;             // read offset isOccupied by fs_read()
//} fd;

//int make_fs(char *disk_fileName);
//int mount_fs(char *disk_fileName);
//int umount_fs(char *disk_fileName);
//int fs_open(char *fileName);
//int fs_close(int fildes);
//int fs_create(char *fileName);
//int fs_delete(char *fileName);
//int fs_read(int fildes, void *buf, size_t nbyte);
//int fs_write(int fildes, void *buf, size_t nbyte);
//int fs_get_filesize(int fildes);
//int fs_lseek(int fildes, off_t offset);
//int fs_truncate(int fildes, off_t length);
//
///* Helper function */
//int lookupFile(char* fileName);
//int lookupFileDescriptor(int fileIndex);
//int lookupEmptyBlock(int fileIndex);
//int fetchNextBlock(int currentBlockIndex, int fileIndex);

#endif //FILE_SYSTEM_MAIN_H
