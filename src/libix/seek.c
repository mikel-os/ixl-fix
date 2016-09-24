/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>
#include <sysp.h>
#include "fns.h"

vlong
ixlseek(int fd, vlong n, int type)
{
	Syscall	c;
	Sysreply	r;
	vlong	off;

	c.i = SYSSEEK;
	c.seek.fd = fd;
	c.seek.n = n;
	c.seek.type = type;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSSEEK){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	off = r.seek.offset;
	freeretR(&r);

	return off;
}
