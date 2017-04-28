/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

typedef struct ClientConn	ClientConn;
typedef struct ClientConnBox	ClientConnBox;
typedef struct ServerConn	ServerConn;
typedef struct Call	Call;

struct ClientConn{
	int	ref;
	int	fd;
	ClientConnBox	*box;
	QLock	lck;
	Rendez	empty;
	Sysreply	r;
	char	dir[NETPATHLEN];
};

struct ClientConnBox{
	QLock	lck;
	Rendez	full;
	int	size;
	ClientConn	*ctab;
};

struct Call{
	ClientConn	*c;
	Syscall	sysc;
};

void	initclientc(ClientConnBox cbox[], int size, ClientConn ctab[]);
int	getclientcfrom(ClientConnBox cbox[]);
void	putclientcto(ClientConnBox cbox[], int conn);
void	putclientc(ClientConn *cc);
