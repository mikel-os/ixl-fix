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
ixlclose(int fd)
{
	Syscall	c;
	Sysreply	r;

	c.i = SYSCLOSE;
	c.close.fd = fd;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return 0;

	if(r.i != SYSCLOSE){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	freeretR(&r);

	return 0;
}
