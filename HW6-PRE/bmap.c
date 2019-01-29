/**************** imap.c file *************************/
#include "header.h"

char buf[BLKSIZE];
int fd;

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);
}

int tst_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;  j = bit % 8;
  if (buf[i] & (1 << j))
     return 1;
  return 0;
}

int bmap()
{
  char buf[BLKSIZE];
  int  bmap, nblocks;
  int  i;

  // read SUPER block
  get_block(fd, 1, buf);
  sp = (SUPER *)buf;

  nblocks = sp->s_blocks_count;
  printf("nblocks = %d\n", nblocks);

  // read Group Descriptor 0
  get_block(fd, 2, buf);
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  printf("bmap = %d\n", bmap);

  // read inode_bitmap block
  get_block(fd, bmap, buf);

  for (i=0; i < nblocks; i++){
    (tst_bit(buf, i)) ?	putchar('1') : putchar('0');
    if (i && ((i + 1) % 8)==0)
        if (((i + 1) % 48)==0)
            printf("\n");
        else
            printf(" ");
  }
  printf("\n");
}

char *disk = "mydisk";

int main(int argc, char *argv[ ])
{
  if (argc > 1)
    disk = argv[1];

  fd = open(disk, O_RDONLY);
  if (fd < 0){
    printf("open %s failed\n", disk);
    exit(1);
  }

  bmap();
}
