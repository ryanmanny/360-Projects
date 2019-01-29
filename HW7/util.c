#include "header.h"
#include "globals.h"


void get_block(int fd, int blk, char buf[])
{
    // Store contents of block in buffer
    lseek(fd, (long) blk * BLKSIZE, 0);
    read(fd, buf, BLKSIZE);
}

void put_block(int fd, int blk, char buf[])
{
    // Store contents of buffer in block
    lseek(fd, (long) blk * BLKSIZE, 0);
    write(fd, buf, BLKSIZE);
}

int tokenize(char *src, char *parts[], char *delim)
{
    int i = 0;
    char *part;

    if (!*src) return 0;  // Empty string

    parts[i++] = strtok(src, delim);
    while ((part = strtok(NULL, delim)) && i < NTOK)
    {
        parts[i++] = part;
    }
    return i;  // Returns number of tokens
}

int getino(char *pathname)
{
    int ino, n;
    MINODE *mip;
    DIR *dp;
    char *cp, temp[256];

    if (pathname[0] == '/')
    {
        // Absolute
        ino = root->ino;
        pathname++;
    }
    else
    {
        // Relative
        ino = running->cwd->ino;
    }
    
    n = tokenize(pathname, name, "/");

    for (int i = 0; i < n; i++)  // Goes through names
    {
        mip = iget(ino);

        for (int b = 0; b < 12; b++)  // Access data blocks of INODE
        {
            if (mip->INODE.i_block[b] == 0)
            {
                printf("%s not found!\n", name[i]);
                return 0;  // Not found
            }
            else
            {
                get_block(dev, mip->INODE.i_block[b], buf);
                dp = (DIR *) buf;
                cp = buf;

                while (cp < buf + BLKSIZE)  // Search dir blocks for entries
                {
                    strncpy(temp, dp->name, dp->name_len);
                    temp[dp->name_len] = '\0';  // Append NULL character

                    if (!strcmp(temp, name[i]))  // Found the destination directory
                    {
                        ino = dp->inode;
                        iput(mip);
                        goto NEXT_NAME;
                    }

                    cp += dp->rec_len;  // Move to next record
                    dp = (DIR *) cp;
                }
            }
        }
        NEXT_NAME:;
    }
    
    return ino;
}

int getpino(MINODE* mip)
{
    // Gets parent inode for a directory (doesn't work for files)
    char temp[256];
    DIR *dp = (DIR *) buf;
    char *cp = buf;

    get_block(dev, mip->INODE.i_block[0], buf);  // .. should be in block one

    while (cp < buf + BLKSIZE)
    {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = '\0';  // Append NULL character
        
        if (!strcmp(temp, ".."))  // Found ye parent
        {
            return dp->inode;
        }
        
        cp += dp->rec_len;  // Move to next record
        dp = (DIR *) cp;
    }
    return 0;
}

MINODE *iget(int ino)
{
    int block, offset;
    MINODE *mip;
    INODE *ip;

    // return minode pointer to loaded INODE
    for (int i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount > 0 && mip->dev == dev && mip->ino == ino)
        {
            mip->refCount++;
            return mip;
        }
    }

    // needed entry not in memory
    for (int i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount == 0)
        {
            mip->refCount++;
            mip->dev = dev;
            mip->ino = ino;

            block = inode_start + (mip->ino - 1) / 8;
            get_block(dev, block, buf);

            offset = (mip->ino - 1) % 8;
            ip = (INODE *) buf + offset;

            mip->INODE = *ip;  // Copy INODE from block

            return mip;
        }
    }

    return NULL;  // No more room for MINODES
}

void iput(MINODE *mip)
{
    INODE *ip;
    int block, offset;
    int dev = mip->dev;
    int ino = mip->ino;

    mip->refCount--;

    if (mip->refCount > 0) return;  // References still exist
    if (!mip->dirty)       return;  // Doesn't need to be written back

    block = inode_start + (ino - 1) / 8;
    get_block(dev, block, buf);

    offset = (ino - 1) % 8;
    ip = (INODE *) buf + offset;

    *ip = mip->INODE;  // Copies INODE back to block
    put_block(dev, block, buf);
}
