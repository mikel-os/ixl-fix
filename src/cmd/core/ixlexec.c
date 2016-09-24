/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>

void
usage()
{
	fprint(2, "usage: %s program [...]\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int	fd, ixlfd;

	ARGBEGIN{
	}ARGEND

	if(argc < 1)
		usage();

	fd = getixlconn();
	if(fd < 0)
		sysfatal("connection failure: %r");
	ixlfd = getixlfd();
	if(fd != ixlfd){
		dup(fd, ixlfd);
		close(fd);
	}

	exec(argv[0], argv);
	sysfatal("connection failure: %r");
}
