/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <stdint.h>
#include <libc.h>
#include <sysp.h>
#include "sysptest.h"

static
void
check(int v)
{
	char	ebuf[256];

	if(v)
		return;

	rerrstr(ebuf, 256);
	print("%s\n", ebuf);
	exits("failed");
}

static
void
checkC(void)
{
	Syscall	s, d;
	ulong	n, nn;
	uchar	*buf;

	print("Open\n");
	s.i = SYSOPEN;
	s.open.file = "one file";
	s.open.omode = OREAD | OEXCL;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Create\n");
	s.i = SYSCREATE;
	s.create.file = "new file";
	s.create.omode = ORDWR;
	s.create.perm = 1009;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Dup\n");
	s.i = SYSDUP;
	s.dup.oldfd = 8;
	s.dup.newfd = 10;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Close\n");
	s.i = SYSCLOSE;
	s.close.fd = 9;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Remove\n");
	s.i = SYSREMOVE;
	s.remove.file = "old file";
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Pread\n");
	s.i = SYSPREAD;
	s.pread.fd = 11;
	s.pread.n = 251;
	s.pread.offset = 4321;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Pwrite\n");
	s.i = SYSPWRITE;
	s.pwrite.fd = 9;
	s.pwrite.buf = "Hello, world!";
	s.pwrite.n = strlen(s.pwrite.buf);
	s.pwrite.offset = 1234;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Seek\n");
	s.i = SYSSEEK;
	s.seek.fd = 50;
	s.seek.n = 5544;
	s.seek.type = 0;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Stat\n");
	s.i = SYSSTAT;
	s.stat.file = "stat file";
	s.stat.edir = nil;	/* ignored field */
	s.stat.nedir = 152;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Wstat\n");
	s.i = SYSWSTAT;
	s.stat.file = "wstat file";
	s.stat.edir = (uchar *) "wstat entry";
	s.stat.nedir = strlen((char *) s.stat.edir);
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Fstat\n");
	s.i = SYSFSTAT;
	s.fstat.fd = 4;
	s.fstat.edir = nil;	/* ignored field */
	s.fstat.nedir = 152;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Fwstat\n");
	s.i = SYSFWSTAT;
	s.fstat.fd = 20;
	s.fstat.edir = (uchar *) "wstat entry";
	s.fstat.nedir = strlen((char *) s.stat.edir);
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Chdir\n");
	s.i = SYSCHDIR;
	s.chdir.path = "chdir file";
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Fd2path\n");
	s.i = SYSFD2PATH;
	s.fd2path.fd = 100;
	s.fd2path.nbuf = 40;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Bind\n");
	s.i = SYSBIND;
	s.bind.file = "bind file";
	s.bind.mtpt = "bind mount point";
	s.bind.flag = MREPL;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Mount\n");
	s.i = SYSMOUNT;
	s.mount.fd = 60;
	s.mount.afd = 0;
	s.mount.mtpt = "mount point";
	s.mount.flag = MREPL;
	s.mount.aname = "Aname";
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Mount (a = -1)\n");
	s.i = SYSMOUNT;
	s.mount.fd = 70;
	s.mount.afd = -1;
	s.mount.mtpt = "bind mount point";
	s.mount.flag = MBEFORE;
	s.mount.aname = nil;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Unmount\n");
	s.i = SYSUNMOUNT;
	s.unmount.file = "unmount file";
	s.unmount.mtpt = "old mount point";
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Unmount (file = all)\n");
	s.i = SYSUNMOUNT;
	s.unmount.file = nil;
	s.unmount.mtpt = "old mount point";
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Rfork (new)\n");
	s.i = SYSNEWRFORK;
	s.rfork.hash = 1230;
	s.rfork.flags = RFPROC | RFNOTEG | RFNOWAIT | RFFDG;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	s.rfork.hash = 0;
	check(syspcheckC(&s, &d));
	freeretC(&d);

	print("Rfork (old)\n");
	s.i = SYSOLDRFORK;
	s.rfork.hash = 1230;
	s.rfork.flags = RFPROC | RFNOTEG | RFNOWAIT | RFFDG;
	n = syspsizeC2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspC2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Syscall));
	nn = syspM2C(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckC(&s, &d));
	freeretC(&d);
}

static
void
checkR(void)
{
	Sysreply	s, d;
	ulong	n, nn;
	uchar	*buf;

	print("Error (reply)\n");
	s.i = SYSERROR;
	s.error.err = 150;
	s.error.errmsg = "This is an error";
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Open\n");
	s.i = SYSOPEN;
	s.fd.fd = 12;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Create\n");
	s.i = SYSCREATE;
	s.fd.fd = 21;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Dup\n");
	s.i = SYSDUP;
	s.fd.fd = 33;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Pread\n");
	s.i = SYSPREAD;
	s.pread.buf = "Hello, user!";
	s.pread.n = strlen(s.pread.buf);
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Pwrite\n");
	s.i = SYSPWRITE;
	s.pwrite.n = 42;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Seek\n");
	s.i = SYSSEEK;
	s.seek.offset = INT64_MAX;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Stat/Fstat\n");
	s.i = SYSSTAT;
	s.stat.edir = (uchar *) "stat entry";
	s.stat.nedir = strlen((char *) s.stat.edir);
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Wstat/Fwstat\n");
	s.i = SYSWSTAT;
	s.stat.nedir = 1701;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Fd2path\n");
	s.i = SYSFD2PATH;
	s.fd2path.buf = "file path";
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Rfork (new)\n");
	s.i = SYSNEWRFORK;
	s.newrfork.hash = 4130;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);

	print("Rfork (old)\n");
	s.i = SYSOLDRFORK;
	n = syspsizeR2M(&s);
	print("	Size\t%d\n", n);

	buf = malloc(n);
	nn = syspR2M(&s, buf, n);
	check(nn != 0);
	print("	Converted %d\n", nn);

	memset(&d, 0, sizeof(Sysreply));
	nn = syspM2R(buf, n, &d);
	free(buf);
	check(nn != 0);
	print("	Recovered %d\n", nn);

	check(syspcheckR(&s, &d));
	freeretR(&d);
}

void
main(int argc, char *argv[])
{
	print("[Sysp calls ...]\n");
	checkC();
	print("[Sysp replies ...]\n");
	checkR();
	exits(nil);
}
