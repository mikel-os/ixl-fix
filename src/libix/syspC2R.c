/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <libc.h>
#include <libix.h>
#include <sysp.h>
#include "fns.h"
#include "ixlconfig.h"

int
syspC2R(int ixlfd, Syscall *c, Sysreply *r)
{
	ulong	size;
	uchar	msg[IXLSIZ];
	ssize_t	n;

	assert(IXLSIZ <= _POSIX_SSIZE_MAX);	/* awful */
	size = syspsizeC2M(c);

	if(size == 0){
		return -1;
	}
	if(size > IXLSIZ){
		if(size > SIZE_MAX)
			werrstr("too much for size_t (wrap error)");
		else
			werrstr("msg buffer too short (call)");
		return -1;
	}

	if(!syspC2M(c, msg, size))
		return -1;

	n = write(ixlfd, msg, size);
	if(n < 0)
		return -1;

	/*
		WARNING: This error prevents to read the reply.
		Avoid writing again with the ixlfd (stop the program).
		See "ixlconfig.h".
	*/
	if(n != (ssize_t) size){	/* absolutely awful */
		werrstr("write: size mismatch on (r: %lud, e:%lud)", n, size);
#ifdef IXLFATALWMISMATCH
		sysfatalerr();
#else
		return -1;
#endif
	}

	n = read(ixlfd, msg, IXLSIZ);
	if(n <= 0){
		if(n == 0)
			werrstr("end of connection");
		return -1;
	}
	if(n > IXLSIZ){
		werrstr("msg buffer too short (reply)");
		return -1;
	}

	memset(r, 0, sizeof(Sysreply));
	if(!syspM2R(msg, sizeof(msg), r))
		return -1;

	if(r->i == SYSERROR){
		/* r.error.err number ignored */
		werrstr("%s", r->error.errmsg);
		freeretR(r);
		return -1;
	}

	return 0;
}
