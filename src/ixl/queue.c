/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <libix.h>
#include <sysp.h>
#include "queue.h"

extern	int	debug;

void
rqinit(RQueue *q)
{
	memset(q, 0, sizeof(RQueue));
	q->empty.l = &q->lck;
	q->full.l = &q->lck;
}

void
rqput(RQueue *q, void *e)
{
	assert(e);
	qlock(&q->lck);
	while(q->e)
		rsleep(&q->full);
	q->e= e;
	rwakeup(&q->empty);
	qunlock(&q->lck);
}

int
rqcanput(RQueue *q, void *e)
{
	assert(e);
	qlock(&q->lck);
	if(q->e)
		return -1;
	q->e = e;
	rwakeup(&q->empty);
	qunlock(&q->lck);

	return 0;
}

void *
rqget(RQueue *q)
{
	void	*e;

	qlock(&q->lck);
	while(!q->e)
		rsleep(&q->empty);
	e = q->e;
	q->e = nil;
	rwakeup(&q->full);
	qunlock(&q->lck);
	assert(e);
	return e;
}
