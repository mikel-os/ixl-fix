/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <stdint.h>
#include <libc.h>
#include <fcall.h>
#include <sysp.h>

#define ADD_U32(n, v, val)\
{\
	v = val;\
	if(n > UINT32_MAX - v)\
		goto wraperror;\
	else\
		n += v;\
}\

#define MEMMOVE(p, v)\
	memmove(p, &v, sizeof(v));

#define MEMGET(p, v)\
	memmove(&v, p, sizeof(v));

void
freeretC(Syscall s[])
{
	switch(s->i){
	case SYSOPEN:
		free(s->open.file);
		break;
	case SYSCREATE:
		free(s->create.file);
		break;
	case SYSREMOVE:
		free(s->remove.file);
		break;
	case SYSPWRITE:
		free(s->pwrite.buf);
		break;
	case SYSWSTAT:
		free(s->stat.edir);
	case SYSSTAT:
		free(s->stat.file);
		break;
	case SYSFWSTAT:
		free(s->fstat.edir);
		break;
	case SYSCHDIR:
		free(s->chdir.path);
		break;
	case SYSBIND:
		free(s->bind.file);
		free(s->bind.mtpt);
		break;
	case SYSMOUNT:
		free(s->mount.mtpt);
		free(s->mount.aname);
		break;
	case SYSUNMOUNT:
		free(s->unmount.file);
		free(s->unmount.mtpt);
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSDUP:
	case SYSCLOSE:
	case SYSPREAD:
	case SYSSEEK:
	case SYSFSTAT:
	case SYSFD2PATH:
	case SYSNEWRFORK:
	case SYSOLDRFORK:
		break;
	default:
		/* This is an error. */
		break;
	}
}

void
freeretR(Sysreply r[])
{
	switch(r->i){
	case SYSERROR:
		free(r->error.errmsg);
		break;
	case SYSOPEN:
	case SYSCREATE:
	case SYSDUP:
	case SYSCLOSE:
	case SYSREMOVE:
	case SYSPWRITE:
	case SYSSEEK:
	case SYSWSTAT:
	case SYSFWSTAT:
	case SYSCHDIR:
	case SYSBIND:
	case SYSMOUNT:
	case SYSUNMOUNT:
	case SYSNEWRFORK:
	case SYSOLDRFORK:
		break;
	case SYSPREAD:
		free(r->pread.buf);
		break;
	case SYSSTAT:
	case SYSFSTAT:
		free(r->stat.edir);
		break;
	case SYSFD2PATH:
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	default:
		/* This is an error. */
		break;
	}
}

/*
	TODO: Inconsistent sizes of malloc types
*/

static
ulong
strbufsiz(char *s)
{
	/*
		String message buffers use an unsigned two-byte size field.
		This field doesn't include itself (+ 2 bytes).

	FIXME:
		Upgrade to a strnlen-based implementation.

		Note plan9port relies on the unix version of strlen.
		There, SIZE_MAX is >= UINT16_MAX, so it can wrap.
		Some days, it doesn't return as expected.
	*/
	if(!s || strlen(s) > UINT16_MAX)
		return BIT16SZ;

	return BIT16SZ + strlen(s);
}

/*
	These functions relies on s != nil: check it before use.

TODO:
	Try the general versions.
*/
static
uchar *
strbufmove(uchar *p, char s[], uint l)
{
	PBIT16(p, l);
	p += BIT16SZ;
	memmove(p, s, l);
	p += l;

	return p;
}

static
uchar *
strbufdup(uchar *p, char **s)
{
	uint	l;

	l = GBIT16(p);
	p += BIT16SZ;
	if(l == 0){
		*s = nil;
		return p;
	}
	*s = malloc(l + 1);
	memmove(*s, p, l);	/* memcpy should be valid */
	(*s)[l] = '\0';
	p += l;

	return p;
}

/*
	These functions return 0 on error.
	Returning < (BIT32SZ + BIT8SZ) would be a bug.
*/
ulong
syspsizeC2M(Syscall s[])
{
	ulong	n, v;

	n = 0;
	ADD_U32(n, v, BIT32SZ)	/* size */
	ADD_U32(n, v, BIT8SZ)	/* syscall number */

	switch(s->i){
	case SYSOPEN:
		ADD_U32(n, v, strbufsiz(s->open.file))
		ADD_U32(n, v, BIT16SZ)	/* omode */
		break;
	case SYSCREATE:
		ADD_U32(n, v, strbufsiz(s->create.file))
		ADD_U32(n, v, BIT8SZ)
		ADD_U32(n, v, BIT32SZ)	/* perm */
		break;
	case SYSDUP:
		ADD_U32(n, v, 2 * sizeof(int))
		break;
	case SYSCLOSE:
		ADD_U32(n, v, sizeof(int))
		break;
	case SYSREMOVE:
		ADD_U32(n, v, strbufsiz(s->remove.file))
		break;
	case SYSPWRITE:
		if(s->pwrite.n < 0){
			werrstr("pwrite nbytes < 0");
			return 0;
		}
		ADD_U32(n, v, s->pwrite.n)
	case SYSPREAD:
		ADD_U32(n, v, sizeof(int))
		ADD_U32(n, v, BIT32SZ)	/* nbytes */
		ADD_U32(n, v, BIT64SZ)	/* offset */
		break;
	case SYSSEEK:
		ADD_U32(n, v, sizeof(int))
		ADD_U32(n, v, BIT64SZ)	/* offset */
		ADD_U32(n, v, BIT8SZ);	/* type */
		break;
	/*
		The "nedir" parameter used on stat and variants
		seems to be STATMAX.

		STATMAX is uint16_max, and on 9P,
		the buffer size is a two-byte field.

		This code assumes a direct relation between both facts.
	*/
	case SYSWSTAT:
		if(s->stat.nedir < 0){
			werrstr("wstat nedir < 0");
			return 0;
		}
		ADD_U32(n, v, s->stat.nedir)
	case SYSSTAT:
		ADD_U32(n, v, strbufsiz(s->stat.file))
		ADD_U32(n, v, BIT16SZ)	/* nedir */
		break;
	case SYSFWSTAT:
		if(s->fstat.nedir < 0){
			werrstr("fwstat nedir < 0");
			return 0;
		}
		ADD_U32(n, v, s->fstat.nedir)
	case SYSFSTAT:
		ADD_U32(n, v, sizeof(int))
		ADD_U32(n, v, BIT16SZ)	/* nedir */
		break;
	case SYSCHDIR:
		ADD_U32(n, v, strbufsiz(s->chdir.path))
		break;
	case SYSFD2PATH:
		ADD_U32(n, v, sizeof(int))
		ADD_U32(n, v, BIT16SZ)	/* nbuf */
		break;
	case SYSBIND:
	/*
		Mount flags use just one byte.
		If that change, this have to be updated.
	*/
		ADD_U32(n, v, strbufsiz(s->bind.file))
		ADD_U32(n, v, strbufsiz(s->bind.mtpt))
		ADD_U32(n, v, BIT8SZ);	/* flag */
		break;
	case SYSMOUNT:
		ADD_U32(n, v, 2 * sizeof(int))	/* fd, afd */
		ADD_U32(n, v, strbufsiz(s->mount.mtpt))
		ADD_U32(n, v, BIT8SZ);	/* flag */
		if(s->mount.afd < 0)
			ADD_U32(n, v, BIT16SZ)	/* see strbufsiz */
		else
			ADD_U32(n, v, strbufsiz(s->mount.aname))
		break;
	case SYSUNMOUNT:
		ADD_U32(n, v, strbufsiz(s->unmount.file))
		ADD_U32(n, v, strbufsiz(s->unmount.mtpt))
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSOLDRFORK:
	/*
		Rfork flags use just one byte.
		If that change, this have to be updated.
	*/
		ADD_U32(n, v, BIT32SZ)	/* hash */
	case SYSNEWRFORK:
		ADD_U32(n, v, BIT8SZ)	/* flags */
		break;
	default:
		werrstr("unknown syscall index");
		return 0;
	}
	return n;

wraperror:
	werrstr("message too long");
	return 0;
}

ulong
syspC2M(Syscall s[], uchar buf[], uint nbuf)
{
	uchar	*p;
	ulong	n;

	if(!buf){
		werrstr("null buffer");
		return 0;
	}
	p = buf;
	n = syspsizeC2M(s);

	if(n < BIT32SZ + BIT8SZ){
		if(n > 0)
			werrstr("bug on syspsizeC2M");
		return 0;
	}
	if(n > nbuf){
		werrstr("syspC2M: buffer too short");
		return 0;
	}

	PBIT32(p, n);
	p += BIT32SZ;
	PBIT8(p, s->i);
	p += BIT8SZ;

	switch(s->i){
	case SYSOPEN:
		if(!s->open.file){
			werrstr("open null file");
			return 0;
		}
		p = strbufmove(p, s->open.file, strlen(s->open.file));
		PBIT16(p, s->open.omode);
		p += BIT16SZ;
		break;
	case SYSCREATE:
		if(!s->create.file){
			werrstr("create null file");
			return 0;
		}
		p = strbufmove(p, s->create.file, strlen(s->create.file));
		PBIT8(p, s->create.omode);
		p += BIT8SZ;
		PBIT32(p, s->create.perm);
		p += BIT32SZ;
		break;
	case SYSDUP:
		MEMMOVE(p, s->dup.oldfd)
		p += sizeof(int);
		MEMMOVE(p, s->dup.newfd)
		p += sizeof(int);
		break;
	case SYSCLOSE:
		MEMMOVE(p, s->close.fd)
		p += sizeof(int);
		break;
	case SYSREMOVE:
		if(!s->remove.file){
			werrstr("remove null file");
			return 0;
		}
		p = strbufmove(p, s->remove.file, strlen(s->remove.file));
		break;
	case SYSPREAD:
		MEMMOVE(p, s->pread.fd)
		p += sizeof(int);
		PBIT32(p, s->pread.n);
		p += BIT32SZ;
		PBIT64(p, s->pread.offset);
		p += BIT64SZ;
		break;
	case SYSPWRITE:
		if(s->pwrite.n < 0){
			werrstr("pwrite nbytes < 0");
			return 0;
		}
		if(!s->pwrite.buf){
			werrstr("pwrite null buf");
			return 0;
		}
		MEMMOVE(p, s->pwrite.fd)
		p += sizeof(int);
		PBIT32(p, s->pwrite.n);
		p += BIT32SZ;
		memmove(p, s->pwrite.buf, s->pwrite.n);
		p += s->pwrite.n;
		PBIT64(p, s->pwrite.offset);
		p += BIT64SZ;
		break;
	case SYSSEEK:
		MEMMOVE(p, s->seek.fd)
		p += sizeof(int);
		PBIT64(p, s->seek.n);
		p += BIT64SZ;
		PBIT8(p, s->seek.type);
		p += BIT8SZ;
		break;
	case SYSSTAT:
		if(!s->stat.file){
			werrstr("stat null file");
			return 0;
		}
		p = strbufmove(p, s->stat.file, strlen(s->stat.file));
		PBIT16(p, s->stat.nedir);
		p += BIT16SZ;
		break;
	case SYSFSTAT:
		MEMMOVE(p, s->fstat.fd)
		p += sizeof(int);
		PBIT16(p, s->fstat.nedir);
		p += BIT16SZ;
		break;
	case SYSWSTAT:
		if(s->stat.nedir < 0){
			werrstr("wstat nedir < 0");
			return 0;
		}
		if(!s->stat.edir){
			werrstr("wstat null edir");
			return 0;
		}
		if(!s->stat.file){
			werrstr("wstat null file");
			return 0;
		}
		p = strbufmove(p, s->stat.file, strlen(s->stat.file));

		PBIT16(p, s->stat.nedir);
		p += BIT16SZ;

		memmove(p, s->stat.edir, s->stat.nedir);
		p += s->stat.nedir;
		break;
	case SYSFWSTAT:
		if(s->fstat.nedir < 0){
			werrstr("wstat nedir < 0");
			return 0;
		}
		if(!s->fstat.edir){
			werrstr("wstat null edir");
			return 0;
		}
		MEMMOVE(p, s->fstat.fd)
		p += sizeof(int);

		PBIT16(p, s->fstat.nedir);
		p += BIT16SZ;

		memmove(p, s->fstat.edir, s->fstat.nedir);
		p += s->fstat.nedir;
		break;
	case SYSCHDIR:
		if(!s->chdir.path){
			werrstr("chdir null path");
			return 0;
		}
		p = strbufmove(p, s->chdir.path, strlen(s->chdir.path));
		break;
	case SYSFD2PATH:
		MEMMOVE(p, s->fd2path.fd)
		p += sizeof(int);
		PBIT16(p, s->fd2path.nbuf);
		p += BIT16SZ;
		break;
	case SYSBIND:
		if(!s->bind.file){
			werrstr("bind null file");
			return 0;
		}
		if(!s->bind.mtpt){
			werrstr("bind null mtpt");
			return 0;
		}
		p = strbufmove(p, s->bind.file, strlen(s->bind.file));
		p = strbufmove(p, s->bind.mtpt, strlen(s->bind.mtpt));
		PBIT8(p, s->bind.flag);
		p += BIT8SZ;
		break;
	case SYSMOUNT:
		if(!s->mount.mtpt){
			werrstr("mount null mtpt");
			return 0;
		}
		MEMMOVE(p, s->mount.fd)
		p += sizeof(int);
		MEMMOVE(p, s->mount.afd)
		p += sizeof(int);
		p = strbufmove(p, s->mount.mtpt, strlen(s->mount.mtpt));
		PBIT8(p, s->mount.flag);
		p += BIT8SZ;
		if(s->mount.afd < 0 || !s->mount.aname){
			PBIT16(p, 0);
			p += BIT16SZ;
			break;
		}
		p = strbufmove(p, s->mount.aname, strlen(s->mount.aname));
		break;
	case SYSUNMOUNT:
		if(!s->unmount.mtpt){
			werrstr("unmount null mtpt");
			return 0;
		}
		if(!s->unmount.file){
			PBIT16(p, 0);
			p += BIT16SZ;
		}else{
			p = strbufmove(p, s->unmount.file, strlen(s->unmount.file));
		}
		p = strbufmove(p, s->unmount.mtpt, strlen(s->unmount.mtpt));
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSOLDRFORK:
		PBIT32(p, s->rfork.hash);
		p += BIT32SZ;
	case SYSNEWRFORK:
		PBIT8(p, s->rfork.flags);
		p += BIT8SZ;
		break;
	default:
		werrstr("unknown syscall index");
		return 0;
	}

	if(p - buf != n){
		werrstr("syspC2M: unexpected size (r: %lud, e:%lud)", p - buf, n);
		return 0;
	}
	return n;
}

ulong
syspM2C(uchar buf[], uint nbuf, Syscall s[])
{
	uchar	*p;
	ulong	n;

	if(!buf){
		werrstr("null buffer");
		return 0;
	}
	p = buf;
	n = GBIT32(p);
	p += BIT32SZ;

	if(n < BIT32SZ + BIT8SZ){
		werrstr("ill formed message");
		return 0;
	}
	if(n > nbuf){
		werrstr("syspM2C: buffer too short");
		return 0;
	}

	s->i = GBIT8(p);
	p += BIT8SZ;

	switch(s->i){
	case SYSOPEN:
		p = strbufdup(p, &s->open.file);
		s->open.omode = GBIT16(p);
		p += BIT16SZ;
		break;
	case SYSCREATE:
		p = strbufdup(p, &s->create.file);
		s->create.omode = GBIT8(p);
		p += BIT8SZ;
		s->create.perm = GBIT32(p);
		p += BIT32SZ;
		break;
	case SYSDUP:
		MEMGET(p, s->dup.oldfd)
		p += sizeof(int);
		MEMGET(p, s->dup.newfd)
		p += sizeof(int);
		break;
	case SYSCLOSE:
		MEMGET(p, s->close.fd)
		p += sizeof(int);
		break;
	case SYSREMOVE:
		p = strbufdup(p, &s->remove.file);
		break;
	case SYSPREAD:
		MEMGET(p, s->pread.fd)
		p += sizeof(int);
		s->pread.n = GBIT32(p);
		p += BIT32SZ;
		s->pread.offset = GBIT64(p);
		p += BIT64SZ;
		break;
	case SYSPWRITE:
		MEMGET(p, s->pwrite.fd)
		p += sizeof(int);
		s->pwrite.n = GBIT32(p);
		p += BIT32SZ;
		if(s->pwrite.n > 0){
			s->pwrite.buf = malloc(s->pwrite.n);
			memmove(s->pwrite.buf, p, s->pwrite.n);
		}
		p += s->pwrite.n;
		s->pwrite.offset = GBIT64(p);
		p += BIT64SZ;
		break;
	case SYSSEEK:
		MEMGET(p, s->seek.fd)
		p += sizeof(int);
		s->seek.n = GBIT64(p);
		p += BIT64SZ;
		s->seek.type = GBIT8(p);
		p += BIT8SZ;
		break;
	case SYSSTAT:
		p = strbufdup(p, &s->stat.file);
		s->stat.nedir = GBIT16(p);
		p += BIT16SZ;
		break;
	case SYSFSTAT:
		MEMGET(p, s->fstat.fd)
		p += sizeof(int);
		s->fstat.nedir = GBIT16(p);
		p += BIT16SZ;
		break;
	case SYSWSTAT:
		p = strbufdup(p, &s->stat.file);

		s->stat.nedir = GBIT16(p);
		p += BIT16SZ;

		if(s->stat.nedir > 0){
			s->stat.edir = malloc(s->stat.nedir);
			memmove(s->stat.edir, p, s->stat.nedir);
		}
		p += s->stat.nedir;
		break;
	case SYSFWSTAT:
		MEMGET(p, s->fstat.fd)
		p += sizeof(int);

		s->fstat.nedir = GBIT16(p);
		p += BIT16SZ;

		if(s->fstat.nedir > 0){
			s->fstat.edir = malloc(s->fstat.nedir);
			memmove(s->fstat.edir, p, s->fstat.nedir);
		}
		p += s->fstat.nedir;
		break;
	case SYSCHDIR:
		p = strbufdup(p, &s->chdir.path);
		break;
	case SYSFD2PATH:
		MEMGET(p, s->fd2path.fd)
		p += sizeof(int);
		s->fd2path.nbuf = GBIT16(p);
		p += BIT16SZ;
		break;
	case SYSBIND:
		p = strbufdup(p, &s->bind.file);
		p = strbufdup(p, &s->bind.mtpt);
		s->bind.flag = GBIT8(p);
		p += BIT8SZ;
		break;
	case SYSMOUNT:
		MEMGET(p, s->mount.fd)
		p += sizeof(int);
		MEMGET(p, s->mount.afd)
		p += sizeof(int);
		p = strbufdup(p, &s->mount.mtpt);
		s->mount.flag = GBIT8(p);
		p += BIT8SZ;
		//if(s->mount.afd >= 0)
		p = strbufdup(p, &s->mount.aname);
		break;
	case SYSUNMOUNT:
		p = strbufdup(p, &s->unmount.file);
		p = strbufdup(p, &s->unmount.mtpt);
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSOLDRFORK:
		s->rfork.hash = GBIT32(p);
		p += BIT32SZ;
		s->rfork.flags = GBIT8(p);
		p += BIT8SZ;
		break;
	case SYSNEWRFORK:
		s->rfork.hash = 0;
		s->rfork.flags = GBIT8(p);
		p += BIT8SZ;
		break;
	default:
		werrstr("unknown syscall index");
		return 0;
	}

	if(p - buf != n){
		werrstr("syspM2C: unexpected size (r: %lud, e:%lud)", p - buf, n);
		return 0;
	}
	return n;
}

/*
	Some functions expecting negative values on failure
	and non-negative otherwise get nothing at all.

	This *could* be undesirable, in which case upgrade the library.
*/
ulong
syspsizeR2M(Sysreply r[])
{
	ulong	n, v;

	n = 0;
	ADD_U32(n, v, BIT32SZ)	/* size */
	ADD_U32(n, v, BIT8SZ)	/* syscall number */

	/*
		Now to read is to write,
		and to write is to read.
	*/
	switch(r->i){
	case SYSERROR:
		ADD_U32(n, v, BIT8SZ)	/* error code */
		ADD_U32(n, v, strbufsiz(r->error.errmsg))
		break;
	case SYSOPEN:
	case SYSCREATE:
	case SYSDUP:
		ADD_U32(n, v, sizeof(int))
		break;
	case SYSCLOSE:
	case SYSREMOVE:
	case SYSCHDIR:
	case SYSBIND:
	case SYSMOUNT:
	case SYSUNMOUNT:
	case SYSOLDRFORK:
		break;
	case SYSPREAD:
		if(r->pread.n < 0){
			werrstr("pread nbytes < 0");
			return 0;
		}
		ADD_U32(n, v, r->pread.n)
	case SYSPWRITE:
		ADD_U32(n, v, BIT32SZ)	/* nbytes */
		break;
	case SYSSEEK:
		ADD_U32(n, v, BIT64SZ)
		break;
	case SYSSTAT:
	case SYSFSTAT:
		if(r->stat.nedir < 0){
			werrstr("stat nedir < 0");
			return 0;
		}
		ADD_U32(n, v, r->stat.nedir)
	case SYSWSTAT:
	case SYSFWSTAT:
		ADD_U32(n, v, BIT16SZ)
		break;
	case SYSFD2PATH:
		ADD_U32(n, v, strbufsiz(r->fd2path.buf))
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSNEWRFORK:
		ADD_U32(n, v, BIT32SZ)
		break;
	default:
		werrstr("unknown syscall index");
		return 0;
	}
	return n;

wraperror:
	werrstr("too long message");
	return 0;
}

ulong
syspR2M(Sysreply r[], uchar buf[], uint nbuf)
{
	uchar	*p;
	ulong	n;

	if(!buf){
		werrstr("null buffer");
		return 0;
	}
	p = buf;
	n = syspsizeR2M(r);

	if(n < BIT32SZ + BIT8SZ){
		if(n > 0)
			werrstr("bug on syspsizeR2M");
		return 0;
	}
	if(n > nbuf){
		werrstr("syspsizeR2M: buffer too short");
		return 0;
	}

	PBIT32(p, n);
	p += BIT32SZ;
	PBIT8(p, r->i);
	p += BIT8SZ;

	switch(r->i){
	case SYSERROR:
		PBIT8(p, r->error.err);
		p += BIT8SZ;
		p = strbufmove(p, r->error.errmsg, strlen(r->error.errmsg));
		break;
	case SYSOPEN:
	case SYSCREATE:
	case SYSDUP:
		MEMMOVE(p, r->fd.fd)
		p += sizeof(int);
		break;
	case SYSCLOSE:
	case SYSREMOVE:
	case SYSCHDIR:
	case SYSBIND:
	case SYSMOUNT:
	case SYSUNMOUNT:
	case SYSOLDRFORK:
		break;
	case SYSPREAD:
		if(r->pread.n < 0){
			werrstr("pread nbytes < 0");
			return 0;
		}
		if(!r->pread.buf){
			werrstr("pread null buf");
			return 0;
		}
		PBIT32(p, r->pread.n);
		p += BIT32SZ;
		memmove(p, r->pread.buf, r->pread.n);
		p += r->pread.n;
		break;
	case SYSPWRITE:
		PBIT32(p, r->pwrite.n);
		p += BIT32SZ;
		break;
	case SYSSEEK:
		PBIT64(p, r->seek.offset);
		p += BIT64SZ;
		break;
	case SYSSTAT:
	case SYSFSTAT:
		if(r->stat.nedir < 0){
			werrstr("fstat/stat nedir < 0");
			return 0;
		}

		PBIT16(p, r->stat.nedir);
		p += BIT16SZ;

		memmove(p, r->stat.edir, r->stat.nedir);
		p += r->stat.nedir;
		break;
	case SYSWSTAT:
	case SYSFWSTAT:
		PBIT16(p, r->stat.nedir);
		p += BIT16SZ;
		break;
	case SYSFD2PATH:
		p = strbufmove(p, r->fd2path.buf, strlen(r->fd2path.buf));
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSNEWRFORK:
		PBIT32(p, r->newrfork.hash);
		p += BIT32SZ;
		break;
	default:
		werrstr("unknown syscall index");
		return 0;
	}

	if(p - buf != n){
		werrstr("syspR2M: unexpected size (r: %lud, e:%lud)", p - buf, n);
		return 0;
	}
	return n;
}

ulong
syspM2R(uchar buf[], uint nbuf, Sysreply r[])
{
	uchar	*p;
	ulong	n;

	if(!buf){
		werrstr("null buffer");
		return 0;
	}
	p = buf;
	n = GBIT32(p);
	p += BIT32SZ;

	if(n < BIT32SZ + BIT8SZ){
		werrstr("ill formed message");
		return 0;
	}
	if(n > nbuf){
		werrstr("syspM2R: buffer too short %lud %ud", n, nbuf);
		return 0;
	}

	r->i = GBIT8(p);
	p += BIT8SZ;

	switch(r->i){
	case SYSERROR:
		r->error.err = GBIT8(p);
		p += BIT8SZ;
		p = strbufdup(p, &r->error.errmsg);
		break;
	case SYSOPEN:
	case SYSCREATE:
	case SYSDUP:
		MEMGET(p, r->fd.fd)
		p += sizeof(int);
		break;
	case SYSCLOSE:
	case SYSREMOVE:
	case SYSCHDIR:
	case SYSBIND:
	case SYSMOUNT:
	case SYSUNMOUNT:
	case SYSOLDRFORK:
		break;
	case SYSPREAD:
		r->pread.n = GBIT32(p);
		p += BIT32SZ;
		if(r->pread.n > 0){
			r->pread.buf = malloc(r->pread.n);
			memmove(r->pread.buf, p, r->pread.n);
		}
		p += r->pread.n;
		break;
	case SYSPWRITE:
		r->pwrite.n = GBIT32(p);
		p += BIT32SZ;
		break;
	case SYSSEEK:
		r->seek.offset = GBIT64(p);
		p += BIT64SZ;
		break;
	case SYSSTAT:
	case SYSFSTAT:
		r->stat.nedir = GBIT16(p);
		p += BIT16SZ;

		if(r->stat.nedir > 0){
			r->stat.edir = malloc(r->stat.nedir);
			memmove(r->stat.edir, p, r->stat.nedir);
		}
		p += r->stat.nedir;
		break;
	case SYSWSTAT:
	case SYSFWSTAT:
		r->stat.nedir = GBIT16(p);
		p += BIT16SZ;
		break;
	case SYSFD2PATH:
		p = strbufdup(p, &r->fd2path.buf);
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSNEWRFORK:
		r->newrfork.hash = GBIT32(p);
		p += BIT32SZ;
		break;
	default:
		werrstr("unknown syscall index");
		return 0;
	}

	if(p - buf != n){
		werrstr("syspM2R: unexpected size (r: %lud, e:%lud)", p - buf, n);
		return 0;
	}
	return n;
}
