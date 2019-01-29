#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include <stdbool.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define BLKSIZE  1024

#define NMINODE    64
#define NOFT       64
#define NFD        10
#define NMOUNT      4
#define NPROC       2
#define NTOK       64

typedef struct minode
{
    INODE INODE;
    int dev, ino;
    int refCount;
    int dirty;
    int mounted;
    struct mntable *mptr;
}MINODE;

typedef struct oft
{
    int  mode;
    int  refCount;
    MINODE *mptr;
    int  offset;
}OFT;

typedef struct proc
{
    struct proc *next;
    int          pid;
    int          uid, gid;
    MINODE      *cwd;
    OFT         *fd[NFD];
}PROC;

// MOUNTROOT.C
int init();
int mount_root();

// FILESYSTEM.C
void ls_file(int, char *);
void ls_dir(char *);
void cd_dir(char *);
void rpwd(MINODE *);
void pwd(MINODE *);
int quit();

// UTIL.C
void get_block(int, int, char[]);
void put_block(int, int, char[]);

int tokenize(char *, char *[], char *);
int getino(char *);
int getpino(MINODE*);

MINODE *iget(int);
void iput(MINODE *);


// GLOBALS
extern char buf[BLKSIZE];
extern char *name[NTOK]; // assume at most 64 components in pathnames
extern char line[256], cmd[32], pathname[256];
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern int  dev;
extern int  nblocks, ninodes, bmap, imap, inode_start;

#endif
