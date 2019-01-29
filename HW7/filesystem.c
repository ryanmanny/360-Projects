#include "header.h"
#include "globals.h"


void ls_file(int ino, char *filename)
{
    MINODE *mip = iget(ino);

    char *mask  = "rwxrwxrwx";
    char *bmask = "---------";
    int index = 0;
    u16 mode = mip->INODE.i_mode;

    for (int shift = 8; shift >= 0; shift--)
    {
        if ((mode >> shift) & 1)
        {
            printf("%c", mask[index]);
        }
        else
        {
            printf("%c", bmask[index]);
        }
        index++;
    }

    printf(" %6d %s\n", mip->INODE.i_size, filename);
    iput(mip);
}

void ls_dir(char *dirname)
{
    int ino;
    MINODE *mip;
    DIR *dp;
    char *cp, temp[256], dbuf[BLKSIZE];

    if (!dirname || !dirname[0])
    {
        ino = running->cwd->ino;
    }
    else
    {
        ino = getino(dirname);
    }

    mip = iget(ino);
    
    for (int i = 0; i < 12; i++)
    {
        if (mip->INODE.i_block[i] == 0)
            break;

        get_block(dev, mip->INODE.i_block[i], dbuf);
        dp = (DIR *) dbuf;
        cp = (char *) dbuf;

        while (cp < dbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = '\0';  // Append NULL character
            
            if (strcmp(temp, ".") && strcmp(temp, ".."))
            {
                ls_file(dp->inode, temp);
            }

            cp += dp->rec_len;  // Move to next record
            dp = (DIR *)cp;
        }
    }

    iput(mip);
}

void cd_dir(char *pathname)
{
    int ino;
    MINODE *mip;
    if (!pathname || !pathname[0])
        ino = root->ino;
    else
        ino = getino(pathname);

    if (ino == 0) return;  // Couldn't find path
    
    mip = iget(ino);
    if (S_ISDIR(mip->INODE.i_mode))
    {
        running->cwd = mip;
    }
    else
    {
        printf("not a dir!\n");
    }
}

void rpwd(MINODE *wd)
{
    MINODE *pip;
    DIR *dp;
    char *cp, temp[256];

    if (wd == root) return;

    pip = iget(getpino(wd));  // Get parent INODE

    for (int i = 0; i < 12; i++)  // Assume at most 12 direct blocks
    {
        get_block(dev, pip->INODE.i_block[i], buf);
        dp = (DIR *) buf;
        cp = buf;

        while (cp < buf + BLKSIZE)
        {
            if (dp->inode == wd->ino)  // Find own ino in parent to find name
            {
                strncpy(temp, dp->name, dp->name_len);
                temp[dp->name_len] = '\0';  // Append NULL character
                
                rpwd(pip);
                printf("/%s", temp);
                return;
            }
            
            cp += dp->rec_len;  // Move to next record
            dp = (DIR *) cp;
        }
    }
}

void pwd(MINODE *wd)
{
    if (wd == root)
    {
        printf("/\n");
    }
    else
    {
        rpwd(wd);
        printf("\n");
    }
}
 
int quit()
{
    int i;
    MINODE *mip;
    for (i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if (mip->refCount > 0)
            iput(mip);
    }
    exit(0);
}
