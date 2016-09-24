/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>

int
ixlrfork(int flags)
{
	int	cfd, ixlfd;
	long	hash;
	int	pid;

	ixlfd = getixlfd();

	if((flags & RFPROC) == 0)
		return ixloldrfork(ixlfd, flags, 0);

	cfd = getixlconn();
	if(cfd < 0 || cfd == ixlfd){
		close(cfd);
		werrstr("no connection");
		return -1;
	}
	hash = ixlnewrfork(cfd, flags);
	if(hash <= 0){
		close(cfd);
		if(hash == 0)
			werrstr("invalid hash (bug symptom)");
		return -1;
	}
	if(ixloldrfork(ixlfd, hash, flags) < 0){
		close(cfd);
		return -1;
	}
	/* the new connection has been successfully forked from ixlfd */

	pid = rfork(flags);
	if(pid == 0)
		dup(cfd, ixlfd);
	close(cfd);

	return pid;
}
