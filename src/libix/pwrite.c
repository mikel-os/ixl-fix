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
ixlpwrite(int fd, void *buf, long n, vlong offset)
{
	Syscall	c;
	Sysreply	r;

	c.i = SYSPWRITE;
	c.pwrite.fd = fd;
	c.pwrite.buf = buf;
	c.pwrite.n = n;
	c.pwrite.offset = offset;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSPWRITE){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	if(r.pwrite.n > n){
		werrstr("too many bytes written (bug signal; r: %ld, e: %ld)",
			r.pwrite.n, n);
		freeretR(&r);
		return -1;
	}
	n = r.pwrite.n;
	freeretR(&r);

	return n;
}
