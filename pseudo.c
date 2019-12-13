#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "disk.h"

/******************************************************************************/
static int active = 0;  /* is the virtual disk open (active) */
static int handle;      /* file handle to virtual disk       */

/******************************************************************************/
int make_disk(char *fileName)
{ 
  int f, cnt;
  char buf[BLOCK_SIZE];

  if (!fileName) {
    fprintf(stderr, "make_disk: invalid file fileName\n");
    return -1;
  }

  if ((f = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
    perror("make_disk: cannot open file");
    return -1;
  }

  memset(buf, 0, BLOCK_SIZE);
  for (cnt = 0; cnt < DISK_BLOCKS; ++cnt)
    write(f, buf, BLOCK_SIZE);

  close(f);

  return 0;
}

int open_disk(char *fileName)
{
  int f;

  if (!fileName) {
    fprintf(stderr, "open_disk: invalid file fileName\n");
    return -1;
  }  
  
  if (active) {
    fprintf(stderr, "open_disk: disk is already open\n");
    return -1;
  }
  
  if ((f = open(fileName, O_RDWR, 0644)) < 0) {
    perror("open_disk: cannot open file");
    return -1;
  }

  handle = f;
  active = 1;

  return 0;
}

int close_disk()
{
  if (!active) {
    fprintf(stderr, "close_disk: no open disk\n");
    return -1;
  }
  
  close(handle);

  active = handle = 0;

  return 0;
}

int block_write(int block, char *buf)
{
  if (!active) {
    fprintf(stderr, "block_write: disk not active\n");
    return -1;
  }

  if ((block < 0) || (block >= DISK_BLOCKS)) {
    fprintf(stderr, "block_write: block index out of bounds\n");
    return -1;
  }

  if (lseek(handle, block * BLOCK_SIZE, SEEK_SET) < 0) {
    perror("block_write: failed to lseek");
    return -1;
  }

  if (write(handle, buf, BLOCK_SIZE) < 0) {
    perror("block_write: failed to write");
    return -1;
  }

  return 0;
}

int block_read(int block, char *buf)
{
  if (!active) {
    fprintf(stderr, "block_read: disk not active\n");
    return -1;
  }

  if ((block < 0) || (block >= DISK_BLOCKS)) {
    fprintf(stderr, "block_read: block index out of bounds\n");
    return -1;
  }

  if (lseek(handle, block * BLOCK_SIZE, SEEK_SET) < 0) {
    perror("block_read: failed to lseek");
    return -1;
  }

  if (read(handle, buf, BLOCK_SIZE) < 0) {
    perror("block_read: failed to read");
    return -1;
  }

  return 0;
}

/**
 * This is where project 4A is
 * */

/* This will be isOccupied to help with development and testFileing of our implementation.
 * Will return the status of the disk and inodes
 * */
void check_disk(void) {
  return NULL;
}

/**
 * Delete all data stored and represented on the disk/inodes
 * */

void format_disk(void) {
  return NULL;
}

/** 
 * Clears the data stored at the specified inode
 * */
void clear_inode(int inode_number) {
  return NULL;
}

// Returns the size/information of the specified inode
int get_inode_size(int inode_number) {
  return 0;
}

// Method that will be a act as a driver for the program and run various testFiles
// Return 0 if it doesn't work, 1 if it does
int testFile_file_system(void) {
  assert(0);
}

// Data structure to represent superblock
struct SuperBlock{};

// Data structure to represent root directory
struct Root{};

// Data structure that represents the status of the disk
struct FileSystemStatus{};

// Data structure to represent the abstraction of a "file" to create cleaner code
struct CustomFile{};

// Data structure to map which files belond to which blocks of the file system partition
struct DataMap{};

// Data structure to contain information about files and their statuses
enum FileDescriptors{};