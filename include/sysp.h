/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

/*
	Warning: libsysp *is* machine-dependent.
*/

enum{
	SYSERROR,	/* not a syscall! */
	SYSOPEN,
	SYSCREATE,
	SYSDUP,
	SYSCLOSE,
	SYSREMOVE,
	SYSPREAD,
	SYSPWRITE,
	SYSSEEK,
	SYSSTAT,
	SYSFSTAT,
	SYSWSTAT,
	SYSFWSTAT,
	SYSCHDIR,
	SYSFD2PATH,
	SYSBIND,
	SYSMOUNT,
	SYSUNMOUNT,
	SYSFVERSION,	/* reserved */
	SYSFAUTH,	/* reserved */

	/* These two sysp codes are part of the "rfork" syscall. */
	SYSNEWRFORK,
	SYSOLDRFORK
};

typedef struct Syscall	Syscall;
typedef struct Syscallcreate	Syscallcreate;	/* open, remove */
typedef struct Syscalldup	Syscalldup;
typedef struct Syscallclose	Syscallclose;
typedef struct Syscallpwrite	Syscallpwrite;	/* pread */
typedef struct Syscallseek	Syscallseek;
typedef struct Syscallwstat	Syscallwstat;	/* stat */
typedef struct Syscallfwstat	Syscallfwstat;	/* fstat */
typedef struct Syscallchdir	Syscallchdir;
typedef struct Syscallfd2path	Syscallfd2path;
typedef struct Syscallbind	Syscallbind;	/* unmount */
typedef struct Syscallmount	Syscallmount;
typedef struct Syscallrfork	Syscallrfork;

typedef struct Sysreply	Sysreply;
typedef struct Sysreplyerror	Sysreplyerror;
typedef struct Sysreplyfd	Sysreplyfd;	/* open, create, dup */
typedef struct Sysreplypread	Sysreplypread;	/* pwrite */
typedef struct Sysreplyseek	Sysreplyseek;
typedef struct Sysreplystat	Sysreplystat;	/* wstat, fstat, fwstat */
typedef struct Sysreplyfd2path	Sysreplyfd2path;
typedef struct Sysreplynewrfork	Sysreplynewrfork;

/* TODO:
	The fs API of Plan 9 uses signed types for sizes,
	but 9P uses unsigned values as machine-independent representations.

	Fcall uses some macros to fill/extract a 9P buffer from/to a struct.
	These could not take care of size/sign discrepancies.

	1. Double-check fcall implementation.
	2. After version one, API can suffer deeper changes,
		that will be an opportunity to rethink
		(but this is not considered a bug).
*/

struct Syscallcreate{
	char	*file;
	int	omode;
	ulong	perm;
};

struct Syscalldup{
	int	oldfd;
	int	newfd;
};

struct Syscallclose{
	int	fd;
};

struct Syscallpwrite{
	int	fd;
	void	*buf;
	long	n;
	vlong	offset;
};

struct Syscallseek{
	int	fd;
	vlong	n;
	int	type;
};

struct Syscallwstat{
	char	*file;
	uchar	*edir;
	int	nedir;
};

struct Syscallfwstat{
	int	fd;
	uchar	*edir;
	int	nedir;
};

struct Syscallchdir{
	char	*path;
};

struct Syscallfd2path{
	int	fd;
	int	nbuf;
};

struct Syscallbind{
	char	*file;
	char	*mtpt;
	int	flag;
};

struct Syscallmount{
	int	fd;
	int	afd;
	char	*mtpt;
	int	flag;
	char	*aname;
};

struct Syscallrfork{
	long	hash;
	int	flags;
};

/*
	Syscall replies
*/
struct Sysreplyerror{
	int	err;	/* non-negative */
	char	*errmsg;
};

struct Sysreplyfd{
	int	fd;
};

struct Sysreplypread{
	void	*buf;
	long	n;
};

struct Sysreplyseek{
	vlong	offset;
};

struct Sysreplystat{
	uchar	*edir;
	int	nedir;
};

struct Sysreplyfd2path{
	char	*buf;
	/* int	nbuf; */
};

struct Sysreplynewrfork{
	long	hash;
};

/*
	Remember that p9p turns the syscall names into macros.
	So open is expanded as p9open, etc. It should not be a problem.
*/

struct Syscall{
	uchar	i;	/* syscall number */
	union{
		Syscallcreate	open;
		Syscallcreate	create;
		Syscalldup	dup;
		Syscallclose	close;
		Syscallcreate	remove;
		Syscallpwrite	pread;
		Syscallpwrite	pwrite;
		Syscallseek	seek;
		Syscallwstat	stat;
		Syscallfwstat	fstat;
		Syscallchdir	chdir;
		Syscallfd2path	fd2path;
		Syscallbind	bind;
		Syscallmount	mount;
		Syscallbind	unmount;
		Syscallrfork	rfork;
	};
};

struct Sysreply{
	uchar	i;	/* syscall number */
	union{
		Sysreplyerror	error;
		Sysreplyfd	fd;
		Sysreplypread	pread;
		Sysreplypread	pwrite;
		Sysreplyseek	seek;
		Sysreplystat	stat;
		Sysreplyfd2path	fd2path;
		Sysreplynewrfork	newrfork;
	};
};

/*
	WARNING: set member pointers as nil before to use.
*/
void	freeretC(Syscall s[]);
void	freeretR(Sysreply r[]);

/*
	These functions return zero on error.
*/
ulong	syspsizeC2M(Syscall s[]);
ulong	syspC2M(Syscall s[], uchar buf[], uint nbuf);
ulong	syspM2C(uchar buf[], uint nbuf, Syscall s[]);

ulong	syspsizeR2M(Sysreply r[]);
ulong	syspR2M(Sysreply r[], uchar buf[], uint nbuf);
ulong	syspM2R(uchar buf[], uint nbuf, Sysreply r[]);
