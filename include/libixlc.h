/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.

	Included in "libix.h" - include directly only in its absence.
*/

#define IXLMSIZ	256
#define kiB	1024

static inline
int
intmin(int a, int b)
{
	return (a < b)? a: b;
}

static inline
int
intmax(int a, int b)
{
	return (a > b)? a: b;
}

static inline
ulong
ulongmin(ulong a, ulong b)
{
	return (a < b)? a: b;
}

static inline
ulong
ulongmax(ulong a, ulong b)
{
	return (a > b)? a: b;
}
