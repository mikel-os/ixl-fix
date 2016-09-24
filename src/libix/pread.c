/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>
#include <sysp.h>
#include "fns.h"

long
ixlpread(int fd, void *buf, long n, vlong offset)
{
	Syscall	c;
	Sysreply	r;

	c.i = SYSPREAD;
	c.pread.fd = fd;
	c.pread.n = n;
	c.pread.offset = offset;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSPREAD){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	if(!r.pread.buf){
		werrstr("empty buffer");
		freeretR(&r);
		return -1;
	}
	if(r.pread.n > n){
		werrstr("buffer too long (bug signal; r: %ld, e: %ld)",
			r.pread.n, n);
		freeretR(&r);
		return -1;
	}
	n = r.pread.n;
	memmove(buf, r.pread.buf, n);
	freeretR(&r);

	return n;
}
