//
// Created by tug65404 on 12/10/2019.
//

#ifndef FILE_SYSTEM_DISK_H
#define FILE_SYSTEM_DISK_H

/******************************************************************************/
#define DISK_BLOCKS  16384      /* number of blocks on the disk                */
#define BLOCK_SIZE   4096      /* block size on "disk"                        */

/******************************************************************************/
int make_disk(char *fileName);     /* create an empty, virtual disk file          */
int open_disk(char *fileName);     /* open a virtual disk (file)                  */
int close_disk();              /* close a previously opened disk (file)       */

int block_write(int block, char *buf);
/* write a block of size BLOCK_SIZE to disk    */
int block_read(int block, char *buf);
/* read a block of size BLOCK_SIZE from disk   */
/******************************************************************************/


#endif //FILE_SYSTEM_DISK_H
