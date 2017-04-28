/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <stdio.h>
#include <libc.h>
#include <thread.h>
#include <libixlc.h>
#include <ixlconn.h>
#include <sysp.h>
#include "ixlconfig.h"
#include "queue.h"
#include "conn.h"

int	debug;
int	afd;
char	adir[NETPATHLEN];
//ServerConn	sctab[SERVERMAX];
ClientConn	cctab[CLIENTMAX];
ClientConnBox	ccbox;
RQueue	inputq, outputq;

int
notehandler(void *none, char s[])
{
	if(debug)
		fprint(2, "note: %s\n", s);
	if(strcmp(s, "sys: write on closed pipe") == 0)
		return 1;
	/* BSD/SUN specific: */
	if(strcmp(s, "sys: window size change") == 0)
		return 1;
	fprint(2, "unhandled note!\n");
	return 0;
}

long
ioaccept(va_list *arg)
{
	char	ctl;
	char	*dir;

	ctl = va_arg(*arg, int);
	dir = va_arg(*arg, char *);

	return accept(ctl, dir);
}

void
clientthread(void *arg)
{
	Ioproc	*io;
	long	n;
	int	fd;
	Call	*call;
	ClientConn	*c;
	
	uchar	buf[IXLSIZ];

	c = arg;
	io = ioproc();
	if(!io){
		fprint(2, "ioproc: %r");
		putclientc(c);
		threadexits("clientthread halted");
	}
	if(debug)
		fprint(2, "accepting connection %d\n", c - cctab);
	fd = iocall(io, ioaccept, c->fd, c->dir);
	close(c->fd);
	c->fd = fd;

	call = nil;
	while((n = ioread(io, fd, buf, IXLSIZ)) > 0){
		if(!call){
			call = malloc(sizeof(Call));
			if(!call){
				putclientc(c);
				closeioproc(io);
				fprint(2, "syscall malloc: %r\n");
				threadexits("clientthread malloc");
			}
		}
		qlock(&c->lck);
		freeretR(&c->r);
		if(syspM2C(buf, n, &call->sysc) > 0){
			c->r.i = SYSDUP;	/* nothing to free */
			c->ref++;
			call->c = c;
			rqput(&inputq, call);
			call = nil;
			rsleep(&c->empty);
		}else{
			c->r.i = SYSERROR;
			c->r.error.errmsg = strdup("broken syscall");
		}
		syspR2M(&c->r, buf, syspsizeR2M(&c->r));
		qunlock(&c->lck);

		n = iowrite(io, fd, buf, syspsizeR2M(&c->r));
		if(n < 0){
			putclientc(c);
			closeioproc(io);
			fprint(2, "write error: %r\n");
			threadexits("clientthread write error");
		}
	}
	putclientc(c);
	closeioproc(io);
	if(n < 0){
		fprint(2, "read error: %r\n");
		threadexits("clientthread read error");
	}
	threadexits(nil);
}

long
iolisten(va_list *arg)
{
	char	*dir;
	char	*newdir;

	dir = va_arg(*arg, char *);
	newdir = va_arg(*arg, char *);

	return listen(dir, newdir);
}

void
listenthread(void *arg)
{
	Ioproc	*io;
	int	conn;
	ClientConn	*c;

	io = ioproc();
	if(!io){
		fprint(2, "ioproc: %r");
		threadexits("listenthread halted");
	}
	for(;;){
		conn = getclientcfrom(&ccbox);
		c = &cctab[conn];
		c->fd = iocall(io, iolisten, adir, c->dir);
		if(c->fd < 0){
			putclientc(c);
			break;
		}
		threadcreate(clientthread, c, 32 * kiB);
	}
	closeioproc(io);
	fprint(2, "can't listen: %r");
	threadexits("listenthread halted");
}

void
inputdispatcher(void *arg)
{
	Call	*call;
	ClientConn	*c;

	for(;;){
		call = rqget(&inputq);
		c = call->c;

		qlock(&c->lck);
		c->r.i = SYSERROR;
		c->r.error.errmsg = strdup("Not implemented, yet");
		rwakeup(&c->empty);
		qunlock(&c->lck);
		putclientc(c);

		freeretC(&call->sysc);
		free(call);
	}
}

void
threadmain(int argc, char *argv[])
{
	char	*ixldir;
	char	*ixlname;
	char	*udsdir;

	ARGBEGIN{
	case 'D':
		debug++;
		break;
	}ARGEND

	print("Ixl starting\n");
	atnotify(notehandler, 1);
	ixldir = getixldir();
	ixlname = getixlname();

	char	addr[sizeof("unix!")
			+ NETPATHLEN
			+ sizeof(ixlname)
			+ 2];	/* slash + '\0' */

	udsdir = getudsdir(ixldir);
	if(!udsdir)
		sysfatal("getudsdir: %r");
	snprint(addr, sizeof(addr), "unix!%s/%s", udsdir, ixlname);
	free(udsdir);

	initclientc(&ccbox, CLIENTMAX, cctab);
	rqinit(&inputq);
	rqinit(&outputq);
	afd = announce(addr, adir);
	if(afd < 0)
		sysfatal("announce: %r");

	threadcreate(listenthread, nil, 8 * kiB);
	threadcreate(inputdispatcher, nil, 32 * kiB);

	//threadexits(nil);

	/*
		Test code here: comment out on full versions.
		Then restore the previous line.
	*/
	for(;;){
		sleep(100);
		yield();
	}
}
