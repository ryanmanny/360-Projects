#include "header.h"

/******************* in <ext2fs/ext2_fs.h>*******************************
struct ext2_group_desc
{
  u32  bg_block_bitmap;          // Bmap block number
  u32  bg_inode_bitmap;          // Imap block number
  u32  bg_inode_table;           // Inodes begin block number
  u16  bg_free_blocks_count;     // THESE are OBVIOUS
  u16  bg_free_inodes_count;
  u16  bg_used_dirs_count;        

  u16  bg_pad;                   // ignore these 
  u32  bg_reserved[3];
};
**********************************************************************/

char buf[BLKSIZE];
int fd;

int get_block(int fd, int blk, char buf[ ])
{
    lseek(fd, (long)blk*BLKSIZE, 0);
    read(fd, buf, BLKSIZE);
}

int group_desc()
{
    // read group_desc block
    get_block(fd, 2, buf);  
    gp = (GD *)buf;

    printf("bg_block_bitmap = %d\n", gp->bg_block_bitmap);
    printf("bg_inode_bitmap = %d\n", gp->bg_inode_bitmap);
    printf("bg_inode_table = %d\n", gp->bg_inode_table);
    printf("bg_free_blocks_count = %d\n", gp->bg_free_blocks_count);
    printf("bg_free_inodes_count = %d\n", gp->bg_free_inodes_count);
    printf("bg_used_dirs_count = %d\n", gp->bg_used_dirs_count);
}

char *disk = "mydisk";

int main(int argc, char *argv[ ])
{ 
    if (argc > 1)
        disk = argv[1];
    fd = open(disk, O_RDONLY);
    if (fd < 0){
        printf("open failed\n");
        exit(1);
    }
    group_desc();
}
