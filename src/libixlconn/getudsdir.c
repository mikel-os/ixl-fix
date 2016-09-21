/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <ixlconn.h>

/*
	While NETPATHLEN is used on "line directory" names,
	using short paths for the UDS directory can be handy.
*/
#define PATHSIZ	NETPATHLEN

char *
getudsdir(char name[])
{
	char	*p;
	int	n, fd;
	Dir	*d;

	if(!name || strlen(name) == 0){
		werrstr("empty name");
		return nil;
	}
	
	if(strchr(name, '/') || strchr(name, '#')){
		werrstr("uds dir name mustn't use \'/\' nor \'#\'");
		return nil;
	}

	p = malloc(PATHSIZ);
	if(!p)
		return nil;

	n = snprint(p, PATHSIZ, "/tmp/%s", name);
	if(n != strlen("/tmp/") + strlen(name)){
		free(p);
		if(n > 0) /* mustn't be zero */
			werrstr("dir name too long");
		return nil;
	}

	fd = create(p, OREAD, 0755 | DMDIR);
	if(fd >= 0){
		close(fd);
		return p;
	}

	d = dirstat(p);
	if(!d || (d->mode & 0777) != 0755){
		if(d)
			werrstr("wrong permissions (r: %o, e: 0755)", d->mode);
		else
			werrstr("file doesn't exist");
		free(p);
		p = nil;
	}

	free(d);
	return p;
}
