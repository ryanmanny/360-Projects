#ifndef GLOBAL_H
#define GLOBAL_H

#include "header.h"

/* GLOBALS */
char buf[BLKSIZE];

char *name[NTOK]; // assume at most 64 components in pathnames
char line[256], cmd[32], pathname[256];

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

int  dev;
int  nblocks, ninodes, bmap, imap, inode_start;

#endif
