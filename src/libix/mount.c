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
ixlmount(int fd, int afd, char mtpt[], int flag, char *aname)
{
	Syscall	c;
	Sysreply	r;

	c.i = SYSMOUNT;
	c.mount.fd = fd;
	c.mount.afd = afd;
	c.mount.mtpt = mtpt;
	c.mount.flag = flag;
	c.mount.aname = aname;

	if(syspC2R(getixlfd(), &c, &r) < 0)
		return -1;

	if(r.i != SYSMOUNT){
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
