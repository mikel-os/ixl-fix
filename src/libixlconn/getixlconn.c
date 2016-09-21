/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <ixlconn.h>

int
getixlconn(void)
{
	char	*dir;
	char	addr[sizeof("unix!") + NETPATHLEN + sizeof(getixlname()) + 2];

	dir = getudsdir(getixldir());
	if(!dir)
		return -1;

	snprint(addr, sizeof(addr), "unix!%s/%s", dir, getixlname());
	free(dir);

	return dial(addr, nil, nil, nil);
}
