/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

typedef struct RQueue	RQueue;
struct RQueue{
	QLock	lck;
	Rendez	empty;
	Rendez	full;
	void	*e;
};

void	rqinit(RQueue *q);
void	rqput(RQueue *q, void *e);
int	rqcanput(RQueue *q, void *e);
void *	rqget(RQueue *q);
