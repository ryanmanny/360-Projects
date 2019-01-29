#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>  // e2fslibs-dev

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#define BLKSIZE 1024

char buf[BLKSIZE];
int fd;

int get_block(int fd, int blk, char buf[])
{
    lseek(fd, (long) blk * BLKSIZE, 0);
    read(fd, buf, BLKSIZE);
    return 0;
}

INODE *get_inode(int ino)
{
    get_block(fd, 2, buf);

    gp = (GD *) buf;
    get_block(fd, gp->bg_inode_table + (ino - 1) / 8, buf);
    ip = (INODE *) buf + (ino - 1) % 8;

    return ip;
}

void print_dir(INODE *ip)
{
    char dbuf[BLKSIZE], temp[256];
    char *cp;

    for (int i = 0; i < 12; i++)  // Assume at most 12 direct blocks
    {  
        if (ip->i_block[i] == 0)  // INODE has no more data blocks
        {
            break;  // Stop printing
        }
        
        get_block(fd, ip->i_block[i], dbuf);
        dp = (DIR *)dbuf;
        cp = dbuf;

        while (cp < dbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = '\0';
            
            printf("%4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, temp);

            cp += dp->rec_len;  // Move to next record
            dp = (DIR *)cp;
        }
    }
}

void print_info(INODE *ip)
{
    int *bp, *bbp;
    int n = BLKSIZE / sizeof(int);
    char bbuf[BLKSIZE];

    puts("Direct blocks:");
    for (int i = 0; i < 12; i++)
    {
        printf("%d\n", ip->i_block[i]);
    }

    puts("Indirect blocks:");
    get_block(fd, ip->i_block[12], buf);
    bp = (int *) buf;        
    
    for (int i = 0; i < n; i++)
    {
        if (*bp)
        {
            printf("%d\n", *bp);
        }
        bp++;
    }

    puts("Double indirect blocks:");
    if (ip->i_block[13] == 101)
    {
        printf("Looking in block %d for double indirect\n", 334);
        get_block(fd, 334, buf);
        bp = (int *) buf;

        for (int i = 0; i < n; i++)
        {
            if (*bp)
            {
                get_block(fd, *bp, bbuf);
                bbp = (int *) bbuf;
                for (int j = 0; j < n; j++)
                {
                    if (*bbp)
                    {
                        printf("%d\n", *bbp);
                    }
                    bbp++;
                }
            }
            bp++;
        }
    }
}

INODE *search(INODE *ip, char *name)
{
    char dbuf[BLKSIZE], temp[256];
    char *cp, *cur_name;

    if (!name)
    {
        return ip;  // We found it
    }
    
    cur_name = strtok(name, "/");

    for (int i = 0; i < 12; i++)  // Assume at most 12 direct blocks
    {
        if (ip->i_block[i] == 0)  // INODE has no more data blocks
        {
            break;
        }
        
        get_block(fd, ip->i_block[i], dbuf);
        dp = (DIR *)dbuf;
        cp = dbuf;

        while (cp < dbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = '\0';  // Append NULL character
            
            if (!strcmp(cur_name, temp))  // Found the destination directory
            {
                ip = get_inode(dp->inode);
                return search(ip, (char *) strtok(NULL, "\0"));  // Recursively search that IP with rest of path
            }

            cp += dp->rec_len;  // Move to next record
            dp = (DIR *)cp;
        }
    }
    return NULL;
}

char *disk = "mydisk";

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        disk = argv[1];
    }

    fd = open(disk, O_RDWR);
    if (fd < 0)
    {
        printf("open %s failed\n", disk);
        exit(1);
    }

    ip = get_inode(2);
    ip = search(ip, argv[2]);
    print_info(ip);
}
