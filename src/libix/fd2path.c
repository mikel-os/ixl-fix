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
ixlfd2path(int fd, char buf[], int nbuf)
{
	Syscall	c;
	Sysreply	r;

	c.i = SYSFD2PATH;
	c.fd2path.fd = fd;
	c.fd2path.buf = buf;
	c.fd2path.nbuf = nbuf;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSFD2PATH){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	if(!r.fd2path.buf){
		werrstr("empty buffer");
		freeretR(&r);
		return -1;
	}
	strncpy(buf, r.fd2path.buf, nbuf);
	freeretR(&r);

	return 0;
}
