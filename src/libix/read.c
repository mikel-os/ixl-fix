/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>

long
ixlread(int fd, void *buf, long n)
{
	return ixlpread(fd, buf, n, -1);	/* yes, negative, is right */
}
