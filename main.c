#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "disk.h"
#define MAX_SIZE            64


// Super Block, first block of file system
typedef struct {
    int directory_location;
    int directory_size;
    int beginning_of_data;
} superBlock;

// Information about the file, the metadata for a file
typedef struct {
    char fileName[MAX_SIZE];
    int isOccupied; // 0 for empty, 1 for full
    int size;
    int beginningIndex; // where the data block begins
    int blockCount;
    int fdCount; // # of file descriptors
} metadata;

// File Descriptor, how to find said file
typedef struct {
    int isOccupied;  // check whether descriptor isOccupied
    int fileDescriptorNumber; // file description 0-63 inclusive
    int offset; // where to begin reading found by fs_offset
} fd;

fd fatTable[MAX_SIZE];
metadata*      directoryMetadata;
superBlock*    superBlock_ptr;

int lookupFile(char* fileName);
int lookupFileDescriptor(int fileIndex);
int lookupEmptyBlock(int fileIndex);
int fetchNextBlock(int currentBlockIndexBlockIndex, int fileIndex);

int createSuperBlock();

void initializeFirstWriteToSuperBlock();

int make_fs(char *disk_fileName)
{
    make_disk(disk_fileName);
    open_disk(disk_fileName);

    createSuperBlock();
    initializeFirstWriteToSuperBlock();
    free(superBlock_ptr);
    close_disk();

//    printf("make_fs()\t called successfully.\n");
    return 0;
}

void initializeFirstWriteToSuperBlock() {
    char *temp = malloc(BLOCK_SIZE);
    temp[0] = '\0';
// writes len bytes of value c (converted to an unsigned char) to the string b.
    memset(temp, 0, BLOCK_SIZE);
    // memcpy(void *restrict dst, const void *restrict src, size_t n);
// copies n bytes from memory area src to memory area dst.
    memcpy(temp, &superBlock_ptr, sizeof(superBlock));
    block_write(0, temp);
}

int createSuperBlock() {
    size_t size = sizeof(superBlock);
    superBlock_ptr = malloc(size);
    superBlock_ptr->directory_location = 1;  // self reference
    superBlock_ptr->beginning_of_data = 2; // after self reference
    superBlock_ptr->directory_size = 0;  // no files to begin with

    return 0;
}

int mount_fs(char *disk_fileName)
{
    if (strlen(disk_fileName) == 0 || disk_fileName == NULL) {
        return -1;
    }

    open_disk(disk_fileName);

    // add new disk to superblock
    char *temp = malloc(BLOCK_SIZE);
    temp[0] = '\0';
    memset(temp, 0, BLOCK_SIZE);  // clear string
    block_read(0, temp);  // the superblock
    memcpy(&superBlock_ptr, temp, sizeof(superBlock_ptr));

    // write metadata
    directoryMetadata = malloc(BLOCK_SIZE);
    memset(temp, 0, BLOCK_SIZE);  // clear string
//    block_read(superBlock_ptr->directory_location, temp);
//    memcpy(directoryMetadata, temp, sizeof(metadata) * superBlock_ptr->directory_size);

    // create usable space
    for (int i = 0; i < MAX_SIZE; i++) {
        fatTable[i].isOccupied = 0;  // not occupied
    }

    return 0;  // return 0 on success
}

int umount_fs(char *disk_fileName)
{
    if (strlen(disk_fileName) == 0 || disk_fileName == NULL) {
        return -1;
    }

    char *temp = malloc(BLOCK_SIZE);
    temp[0] = '\0';
    memset(temp, 0, BLOCK_SIZE);  // clear string with 0's

    // find metadata
    for (int i = 0; i < MAX_SIZE; i++) {
        if(directoryMetadata[i].isOccupied == 1) {
            memcpy(temp, &directoryMetadata[i], sizeof(directoryMetadata[i]));
            temp += sizeof(metadata);
        }
    }
//    block_write(superBlock_ptr->directory_location, temp);

    // remove all info from the file descriptors in the FAT table
    for (int k = 0; k < MAX_SIZE; k++) {
        fd *tempFd = &fatTable[k];
        if (tempFd->isOccupied == 1) {  // if this file descriptor is listed as occupied
            tempFd->offset = 0;
            tempFd->isOccupied = 0;
            tempFd->fileDescriptorNumber = -1;
        }
    }

//    free(directoryMetadata);
    close_disk();

    return 0;
}

int fs_open(char *fileName) {
    int fileIndex = lookupFile(fileName);
    int fd = lookupFileDescriptor(fileIndex);
    directoryMetadata[fileIndex].fdCount++;
    return fd;
}

int fs_close(int fildes) {
    fd *fd = &fatTable[fildes];

    directoryMetadata[fd->fileDescriptorNumber].fdCount -= 1;
    fd->isOccupied = 0;

    return 0;
}

int fs_create(char *fileName) {
    int fileIndex = lookupFile(fileName);

    if (fileIndex < 0) {  // Create file
        for (int i = 0; i < MAX_SIZE; i++) {
            metadata* tempFd = &directoryMetadata[i];
            if (tempFd->isOccupied == 0) {
                superBlock_ptr->directory_size++;
                /* Initialize file information */
                tempFd->isOccupied = 1;
                strcpy(directoryMetadata[i].fileName, fileName);
                tempFd->size = 0;
                tempFd->beginningIndex = -1;
                tempFd->blockCount = 0;
                tempFd->fdCount = 0;

                printf("File System create successful. FS %s created\n", fileName);
                return 0;
            }
        }

        return -1;  // no more space
    }
    return 0;
}

int fs_delete(char *fileName) {
    for(int i = 0; i < MAX_SIZE; i++) {
        metadata* tempFi = &directoryMetadata[i];
        if(strcmp(tempFi->fileName, fileName) == 0) {
            int fileIndex = i;
            int block_index = tempFi->beginningIndex;
            int block_count = tempFi->blockCount;

            // Remove file information
            superBlock_ptr->directory_size--;
            tempFi->isOccupied = 0;
            strcpy(tempFi->fileName, "");
            tempFi->size = 0;
            tempFi->fdCount = 0;

            return 0;
        }

    }

    printf("Delete was successful. %s was deleted\n", fileName);
    return -1;
}

int fs_read(int fildes, void *buf, size_t nbyte) {
    char *dst = buf;
    char block[BLOCK_SIZE];
    int fileIndex = fatTable[fildes].fileDescriptorNumber;
    metadata* file = &directoryMetadata[fileIndex];
    int block_index = file->beginningIndex;
    int block_count = 0;
    int offset = fatTable[fildes].offset;

    while (offset >= BLOCK_SIZE){
        block_index = fetchNextBlock(block_index, fileIndex);
        block_count++;
        offset -= BLOCK_SIZE;
    }
    block_read(block_index, block);

    int read_count = 0;
    for(int i = offset; i < BLOCK_SIZE; i++) {
        dst[read_count++] = block[i];
        if(read_count == (int)nbyte) {
            fatTable[fildes].offset += read_count;
            return read_count;
        }
    }
    block_count++;

    strcpy(block,"");
    while(read_count < (int)nbyte && block_count <= file->blockCount) {
        block_index = fetchNextBlock(block_index, fileIndex);
        strcpy(block,"");
        block_read(block_index, block);
        for(int j=0; j < BLOCK_SIZE; j++) {
            dst[read_count++] = block[j];
            if(read_count == (int)nbyte ) {
                fatTable[fildes].offset += read_count;
                return read_count;
            }
        }
        block_count++;
    }
    fatTable[fildes].offset += read_count;

    return read_count;
}

int fs_write(int fildes, void *buf, size_t nbyte) {
    return 0;
}

int fs_get_filesize(int fildes) {
    return directoryMetadata[fatTable[fildes].fileDescriptorNumber].size;
}

int fs_lseek(int fildes, off_t offset) {
        fatTable[fildes].offset = offset;
        return 0;
}

int fs_truncate(int fildes, off_t length) {
    return 0;
}

int lookupFile(char* fileName) {
    for(int i = 0; i < MAX_SIZE; i++) {
        if(strcmp(directoryMetadata[i].fileName, fileName) == 0 && directoryMetadata[i].isOccupied == 1) {
            return i;  // return the file index
        }
    }

    return -1;         // file not found
}

int lookupFileDescriptor(int fileIndex) {
    for(int i = 0; i < MAX_SIZE; i++) {
        fd* tempFd = &fatTable[i];
        if(tempFd->isOccupied == 0) {
            tempFd->isOccupied = 1;
            tempFd->fileDescriptorNumber = fileIndex;
            tempFd->offset = 0;
            return i;  // return the file descriptor number
        }
    }

    return -1;         // no empty file descriptor available
}

int lookupEmptyBlock(int fileIndex) {
    char *temp = malloc(BLOCK_SIZE);
    temp[0] = '\0';
    char *otherTemp = malloc(BLOCK_SIZE);
    temp[0] = '\0';
    block_read(superBlock_ptr->beginning_of_data, temp);
    block_read(superBlock_ptr->beginning_of_data + 1, otherTemp);
    int i;

    for(i = 4; i < BLOCK_SIZE; i++) {
        if(temp[i] == '\0') {
            temp[i] = (char)(fileIndex + 1);
            block_write(superBlock_ptr->beginning_of_data, temp);
            return i;  // return block number
        }
    }
    for(i = 0; i < BLOCK_SIZE; i++) {
        if(otherTemp[i] == '\0') {
            otherTemp[i] = (char)(fileIndex + 1);
            block_write(superBlock_ptr->beginning_of_data, otherTemp);
            return i;  // return block number
        }
    }
    return -1;         // no free blocks
}

int fetchNextBlock(int currentBlockIndex, int fileIndex) {
    char *temp = malloc(BLOCK_SIZE);
    temp[0] = '\0';

    if (currentBlockIndex < BLOCK_SIZE){
        block_read(superBlock_ptr->beginning_of_data, temp);
        for(int i = currentBlockIndex + 1; i < BLOCK_SIZE; i++) {
            if (temp[i] == (fileIndex + 1)){
                return i;
            }
        }
    } else {
        block_read(superBlock_ptr->beginning_of_data + 1, temp);
        for(int i = currentBlockIndex - BLOCK_SIZE + 1; i < BLOCK_SIZE; i++) {
            if (temp[i] == (fileIndex + 1)){
                return i + BLOCK_SIZE;
            }
        }
    }

    return -1; // no next block
}

int main()
{
    char* disk_fileName = "rootDirectory";
    make_fs(disk_fileName);
    mount_fs(disk_fileName);
    fs_create("testFile.txt");
    fs_delete("testFile.txt");

    fs_create("testFile.txt");

    int fileDescriptor;
    fileDescriptor = fs_open("testFile.txt");

    int fileDescriptor2;
    fileDescriptor2 = fs_open("testFile.txt");


    fs_close(fileDescriptor);
    fs_close(fileDescriptor2);

    umount_fs(disk_fileName);

    mount_fs(disk_fileName);
    fileDescriptor = fs_open("testFile.txt");

    int file_size = fs_get_filesize(fileDescriptor);
    printf("file size of testFile.txt: %d\n", file_size);

    umount_fs(disk_fileName);

    return 0;
}
