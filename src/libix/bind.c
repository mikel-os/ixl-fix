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
ixlbind(char file[], char mtpt[], int flag)
{
	Syscall	c;
	Sysreply	r;

	c.i = SYSBIND;
	c.bind.file = file;
	c.bind.mtpt = mtpt;
	c.bind.flag = flag;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSBIND){
		werrstr("wrong reply (bug signal), n: %d", r.i);
		freeretR(&r);
		return -1;
	}
	freeretR(&r);

	/*
		The Plan 9 manual talks about positive and unique
		sequence numbers.
	*/
	return 0;
}
