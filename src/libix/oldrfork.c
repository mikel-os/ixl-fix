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
ixloldrfork(int cfd, int flags, long hash)
{
	Syscall	c;
	Sysreply	r;

	if(hash < 0){
		werrstr("invalid hash");
		return -1;
	}

	c.i = SYSOLDRFORK;
	c.rfork.hash = hash;
	c.rfork.flags = flags;

	if(syspC2R(cfd, &c, &r) < 0)
		return -1;

	if(r.i != SYSOLDRFORK){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	freeretR(&r);

	return hash;
}
