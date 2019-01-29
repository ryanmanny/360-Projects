#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

#include <string.h>

#define BLKSIZE 1024

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

int fd;
int iblock;

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd,(long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

int dir()
{
    int inode = 0;
    char buf[BUFSIZ];
    char *cp, name[256];

    get_block(fd, 2, buf);

    GD *gp = (GD *) buf;
    get_block(fd, gp->bg_inode_table, buf);
    INODE *ip = (INODE *) buf + 1;

    get_block(fd, ip->i_block[0], buf);
    
    dp = (DIR *) buf;
    cp = (char *) buf;

    for (int i = 0; i < 12; i++)
    {
        snprintf(name, 256, "%s\0", dp->name);
        puts(name);
        cp += dp->rec_len;
        dp = cp;
    }

    return 0;
}

char *disk = "mydisk";
int main(int argc, char *argv[])
{ 
  if (argc > 1)
    disk = argv[1];

  fd = open(disk, O_RDONLY);
  if (fd < 0){
    printf("open %s failed\n", disk);
    exit(1);
  }

  dir();
}
