#include "header.h"
#include "globals.h"


int init()
{
    MINODE *mip;
    PROC   *p;

    printf("init()\n");

    for (int i = 0; i < NMINODE; i++)
    {
        // set all entries to 0;
        mip = &minode[i];
        mip->refCount = 0;
    }
    for (int i = 0; i < NPROC; i++)
    {
        // set pid = i; uid = i; cwd = 0;
        p = &proc[i];
        p->pid = i;
        p->uid = i;
        p->cwd = NULL;
    }
    root = NULL;

    return 0;
}

// load root INODE and set root pointer to it
int mount_root()
{  
    SUPER *sp;
    GD    *gp;

    printf("mount_root()\n");

    // (1). read super block in block #1;
    //      verify it is an EXT2 FS;
    //      record nblocks, ninodes as globals;
    get_block(dev, 1, buf);
    sp = (SUPER *) buf;
    
    if (sp->s_magic != EXT2_SUPER_MAGIC)
    {
        printf("%d is not an EXT2 file system\n", dev);
        return 1;
    }
    else
    {
        printf("%d is an EXT2 file system!\n", dev);
    }
    
    nblocks = sp->s_blocks_count;
    ninodes = sp->s_inodes_count;

    // (2). get GD0 in Block #2:
    //      record bmap, imap, inodes_start as globals
    get_block(dev, 2, buf);
    gp = (GD *) buf;

    bmap = gp->bg_block_bitmap;
    imap = gp->bg_inode_bitmap;
    inode_start = gp->bg_inode_table;

    // (3). root = iget(dev, 2);       // get #2 INODE into minoe[ ]
    //      printf("mounted root OK\n");
    root = iget(2);

    for (int i = 0; i < NPROC; i++)
    {
        proc[i].cwd = iget(2);
    }

    return 0;
}

char *disk = "mydisk";
int main(int argc, char *argv[ ])
{
    if (argc > 1)
        disk = argv[1];

    dev = open(disk, O_RDWR);
    if (dev < 0)
    {
        printf("open %s failed\n", disk);  
        exit(1);
    }

    init();  
    mount_root();

    // running->status = READY;
    running = &proc[0];

    while(1)
    {
        printf("input command : [ls|cd|pwd|quit] ");
        fgets(line, 128, stdin);
        line[strlen(line)-1] = 0;  // Removes newline I hope or FUCK LS

        if (line[0] == '\0')
            continue;
        strcpy(pathname, "");

        sscanf(line, "%s %s", cmd, pathname);
        printf("cmd=%s pathname=%s\n", cmd, pathname);

        if (!strcmp(cmd, "ls"))
            ls_dir(pathname);

        if (!strcmp(cmd, "cd"))
            cd_dir(pathname);

        if (!strcmp(cmd, "pwd"))
            pwd(running->cwd);

        if (!strcmp(cmd, "quit"))
            quit();
    }
}
