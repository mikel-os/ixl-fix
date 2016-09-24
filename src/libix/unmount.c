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
ixlunmount(char *file, char mtpt[])
{
	Syscall	c;
	Sysreply	r;

	c.i = SYSUNMOUNT;
	c.unmount.file = file;
	c.unmount.mtpt = mtpt;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSUNMOUNT){
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
