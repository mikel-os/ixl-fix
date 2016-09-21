/*
	Copyright © 2016 Mikel Cazorla Pérez
	This file is part of Ixl, licensed under the terms of GPLv2.
*/

#include <u.h>
#include <libc.h>
#include <ixlconn.h>
#include "ixlconfig.h"

int
getixlfd(void)
{
	return IXLFD;
}
