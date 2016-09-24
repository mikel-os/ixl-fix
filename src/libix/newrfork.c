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
ixlnewrfork(int cfd, int flags)
{
	Syscall	c;
	Sysreply	r;
	long	hash;

	c.i = SYSNEWRFORK;
	c.rfork.flags = flags;

	if(syspC2R(cfd, &c, &r) < 0)
		return -1;

	if(r.i != SYSNEWRFORK){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	hash = r.newrfork.hash;
	freeretR(&r);

	if(hash <= 0)
		return -1;

	return hash;
}
