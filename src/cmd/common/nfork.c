/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>

void
usage()
{
	fprint(2, "usage: %s n program [...]\n"
		"\tn must be positive\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int	i, n;

	ARGBEGIN{
	}ARGEND

	if(argc < 2)
		usage();

	n = atoi(argv[0]);
	if(n <= 0)
		usage();
	argc--; argv++;

	for(i = 0; i < n; i++)
		if(fork() == 0){
			exec(argv[0], argv);
			sysfatal("exec failure: %r");
		}

	exits(nil);
}
