/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <sysp.h>
#include "sysptest.h"

#define CMPMEMB(ls, rs, field, memb)\
	if(ls->field.memb != rs->field.memb){\
		werrstr("Mismatch: " #field " " #memb);\
		return 0;\
	}\

#define CMPSTR(ls, rs, field, str)\
	if(ls->field.str == nil){\
		if(rs->field.str != nil){\
			werrstr("Empty: " #field " left " #str);\
			return 0;\
		}\
	}else if(rs->field.str == nil){\
		werrstr("Empty: " #field " right " #str);\
		return 0;\
	}else if(strcmp(ls->field.str, rs->field.str) != 0){\
		werrstr("Mismatch: " #field " " #str);\
		return 0;\
	}\

#define CMPBUF(ls, rs, field, size, buf)\
	if(ls->field.size != rs->field.size){\
		werrstr("Mismatch: " #field " " #buf " size " #size);\
		return 0;\
	}\
	if(memcmp(ls->field.buf, rs->field.buf, ls->field.size) != 0){\
		werrstr("Mismatch: " #field " " #buf);\
		return 0;\
	}\

int
syspcheckC(Syscall ls[], Syscall rs[])
{
	if(ls->i != rs->i){
		werrstr("Syscall number mismatch");
		return 0;
	}

	switch(ls->i){
	case SYSOPEN:
		CMPSTR(ls, rs, open, file)
		CMPMEMB(ls, rs, open, omode)
		break;
	case SYSCREATE:
		CMPSTR(ls, rs, create, file)
		CMPMEMB(ls, rs, create, omode)
		CMPMEMB(ls, rs, create, perm)
		break;
	case SYSDUP:
		CMPMEMB(ls, rs, dup, oldfd)
		CMPMEMB(ls, rs, dup, newfd)
		break;
	case SYSCLOSE:
		CMPMEMB(ls, rs, close, fd)
		break;
	case SYSREMOVE:
		CMPSTR(ls, rs, remove, file)
		break;
	case SYSPREAD:
		CMPMEMB(ls, rs, pread, fd)
		CMPMEMB(ls, rs, pread, n)
		CMPMEMB(ls, rs, pread, offset)
		break;
	case SYSPWRITE:
		CMPMEMB(ls, rs, pwrite, fd)
		CMPBUF(ls, rs, pwrite, n, buf)
		CMPMEMB(ls, rs, pwrite, offset)
		break;
	case SYSSEEK:
		CMPMEMB(ls, rs, seek, fd)
		CMPMEMB(ls, rs, seek, n)
		CMPMEMB(ls, rs, seek, type)
		break;
	case SYSSTAT:
		CMPSTR(ls, rs, stat, file)
		CMPMEMB(ls, rs, stat, nedir)
		break;
	case SYSFSTAT:
		CMPMEMB(ls, rs, fstat, fd)
		CMPMEMB(ls, rs, fstat, nedir)
		break;
	case SYSWSTAT:
		CMPSTR(ls, rs, stat, file)
		CMPBUF(ls, rs, stat, nedir, edir)
		break;
	case SYSFWSTAT:
		CMPMEMB(ls, rs, fstat, fd)
		CMPBUF(ls, rs, fstat, nedir, edir)
		break;
	case SYSCHDIR:
		CMPSTR(ls, rs, chdir, path)
		break;
	case SYSFD2PATH:
		CMPMEMB(ls, rs, fd2path, fd)
		CMPMEMB(ls, rs, fd2path, nbuf)
		break;
	case SYSBIND:
		CMPSTR(ls, rs, bind, file)
		CMPSTR(ls, rs, bind, mtpt)
		CMPMEMB(ls, rs, bind, flag)
		break;
	case SYSMOUNT:
		CMPMEMB(ls, rs, mount, fd)
		CMPMEMB(ls, rs, mount, afd)
		CMPSTR(ls, rs, mount, mtpt)
		CMPMEMB(ls, rs, mount, flag)
		CMPSTR(ls, rs, mount, aname)
		break;
	case SYSUNMOUNT:
		CMPSTR(ls, rs, unmount, file)
		CMPSTR(ls, rs, unmount, mtpt)
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSNEWRFORK:
	case SYSOLDRFORK:
		CMPMEMB(ls, rs, rfork, hash)	/* not used on newrfork */
		CMPMEMB(ls, rs, rfork, flags)
		break;
	default:
		werrstr("Unknown syscall index");
		return 0;
	}
	return 1;
}

int
syspcheckR(Sysreply lr[], Sysreply rr[])
{
	if(lr->i != rr->i){
		werrstr("Syscall number mismatch");
		return 0;
	}

	switch(lr->i){
	case SYSERROR:
		CMPMEMB(lr, rr, error, err)
		CMPSTR(lr, rr, error, errmsg)
	case SYSOPEN:
	case SYSCREATE:
	case SYSDUP:
		CMPMEMB(lr, rr, fd, fd)
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
		CMPBUF(lr, rr, pread, n, buf)
		break;
	case SYSPWRITE:
		CMPMEMB(lr, rr, pwrite, n)
		break;
	case SYSSEEK:
		CMPMEMB(lr, rr, seek, offset)
		break;
	case SYSSTAT:
	case SYSFSTAT:
		CMPBUF(lr, rr, stat, nedir, edir)
		break;
	case SYSWSTAT:
	case SYSFWSTAT:
		CMPMEMB(lr, rr, stat, nedir)
		break;
	case SYSFD2PATH:
		CMPSTR(lr, rr, fd2path, buf)
		break;
/*	reserved
	case SYSFVERSION:
		break;
	case SYSFAUTH:
		break;
*/
	case SYSNEWRFORK:
		CMPMEMB(lr, rr, newrfork, hash)
		break;
	default:
		werrstr("Unknown syscall index");
		return 0;
	}
	return 1;
}
