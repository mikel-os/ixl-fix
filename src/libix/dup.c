/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>
#include <sysp.h>
#include "fns.h"

int
ixldup(int oldfd, int newfd)
{
	Syscall	c;
	Sysreply	r;
	int	fd;

	c.i = SYSDUP;
	c.dup.oldfd = oldfd;
	c.dup.newfd = newfd;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSDUP){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	fd = r.fd.fd;
	freeretR(&r);

	return fd;
}
