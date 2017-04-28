/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>
#include <sysp.h>
#include "conn.h"

extern	int	debug;

void
initclientc(ClientConnBox cbox[], int size, ClientConn ctab[])
{
	int	i;

	for(i = 0; i < size; i++){
		memset(&ctab[i], 0, sizeof(ClientConn));
		ctab[i].empty.l = &ctab[i].lck;
		ctab[i].ref = 0;
		ctab[i].fd = -1;
		ctab[i].box = cbox;
	}

	memset(cbox, 0, sizeof(ClientConnBox));
	cbox->full.l = &cbox->lck;
	cbox->size = size;
	cbox->ctab = ctab;
}

static
int
_getclientcfrom(ClientConnBox cbox[])
{
	int	i;
	ClientConn	*ctab;

	ctab = cbox->ctab;
	for(i = 0; i < cbox->size; i++)
		if(ctab[i].ref == 0){
			memset(&ctab[i].r, 0, sizeof(Sysreply));
			ctab[i].r.i = SYSDUP;	/* nothing to free */
			ctab[i].ref++;
			return i;
		}
	if(debug)
		fprint(2, "cc get failed (full)\n");
	return -1;
}

int
getclientcfrom(ClientConnBox cbox[])
{
	int	conn;

	if(debug)
		fprint(2, "cc get\n");

	qlock(&cbox->lck);
	while((conn = _getclientcfrom(cbox)) < 0){
		rsleep(&cbox->full);
		if(debug)
			fprint(2, "cc get retry\n");
	}
	qunlock(&cbox->lck);

	if(debug)
		fprint(2, "cc get %d\n", conn);
	return conn;
}

void
putclientcto(ClientConnBox cbox[], int conn)
{
	ClientConn	*ctab;

	if(debug)
		fprint(2, "cc put %d\n", conn);

	qlock(&cbox->lck);
	ctab = cbox->ctab;
	assert(ctab[conn].ref > 0);
	ctab[conn].ref--;
	if(ctab[conn].ref == 0){
		if(debug)
			fprint(2, "cc free %d\n", conn);
		if(ctab[conn].fd > 2){
			if(debug)
				fprint(2, "cc close fd %d\n", ctab[conn].fd);
			close(ctab[conn].fd);
			ctab[conn].fd = -1;
		}
		freeretR(&ctab[conn].r);
		if(debug)
			fprint(2, "cc wake up\n");
		rwakeup(&cbox->full);
	}else if(debug){
		fprint(2, "cc %d remaining refs %d\n", conn, ctab[conn].ref);
	}
	qunlock(&cbox->lck);
}

void
putclientc(ClientConn *cc)
{
	putclientcto(cc->box, cc - cc->box->ctab);
}
