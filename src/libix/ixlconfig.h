/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.

	#include <stdio.h>
*/

/*
	IXLSIZ must be <= SIZE_MAX
*/
#define IXLSIZ	BUFSIZ
/*
	If writing on the ixlfd a positive (but unexpected) number of bytes
	is returned, the library calls can't be sure of getting the reply.

	That would block the program at "read". Instead the library
	returns an error letting the caller handle it (don't do that),
	OR calls sysfatal (if this macro is defined).

	Future implementations could do without plan 9 (p9p) "write".
*/
//#define IXLFATALWMISMATCH
