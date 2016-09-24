/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>

long
ixlreadn(int fd, void *buf, long n)
{
	long	nn, acc;

	acc = 0;
	for(nn = 0; acc < n; acc += nn){
		nn = ixlread(fd, buf + acc, n - acc);
		if(nn > 0)
			continue;

		if(acc == 0)
			return nn;
		break;
	}

	return acc;
}
