/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <stdio.h>
#include <libc.h>
#include <libix.h>

void
cat(int in, int out)
{
	long	n;
	char	buf[BUFSIZ];

	for(;;){
		n = ixlread(in, buf, BUFSIZ);
		if(n <= 0)
			break;
		if(ixlwrite(out, buf, n) != n)
			sysfatal("write error: %r");
	}
	if(n < 0)
		sysfatal("read error: %r");

}

void
catfile(char file[], int out)
{
	int	in;

	in = ixlopen(file, OREAD);
	if(in < 0)
		sysfatal("can't open file %s: %r", file);
	cat(in, out);
	ixlclose(in);
}

void
main(int argc, char *argv[])
{
	argv0 = argv[0];
	if(argc == 1)
		cat(0, 1);
	else
		for(argc--, argv++; argc > 0; argc--, argv++)
			catfile(*argv, 1);
	exits(nil);
}
