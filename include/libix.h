/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

int	ixlopen(char file[], int omode);
int	ixlcreate(char file[], int omode, ulong perm);
int	ixldup(int oldfd, int newfd);
int	ixlclose(int fd);
int	ixlremove(char file[]);

long	ixlread(int fd, void *buf, long n);
long	ixlwrite(int fd, void *buf, long n);
long	ixlpread(int fd, void *buf, long n, vlong offset);
long	ixlpwrite(int fd, void *buf, long n, vlong offset);
vlong	ixlseek(int fd, vlong n, int type);

int	ixlstat(char file[], uchar edir[], int nedir);
int	ixlfstat(int fd, uchar edir[], int nedir);
int	ixlwstat(char file[], uchar edir[], int nedir);
int	ixlfwstat(int fd, uchar edir[], int nedir);
Dir *	ixldirstat(char file[]);
Dir *	ixldirfstat(int fd);
int	ixldirwstat(char file[], Dir *dir);
int	ixldirfwstat(int fd, Dir *dir);

int	ixlchdir(char path[]);
int	ixlfd2path(int fd, char buf[], int nbuf);

int	ixlbind(char file[], char mtpt[], int flag);
int	ixlmount(int fd, int afd, char mtpt[], int flag, char *aname);
int	ixlunmount(char *file, char mtpt[]);

int	ixlfversion(int fd, int bufsize, char version[], int nversion);
int	ixlfauth(int fd, char aname[]);

int	ixlfork(void);
int	ixlrfork(int flags);

long	ixlnewrfork(int cfd, int flags);
int	ixloldrfork(int cfd, int flags, long hash);

#include <libixlc.h>
#include <ixlconn.h>

long	ixlreadn(int fd, void *buf, long n);
