/*
 * Copyright 2008, Ingo Weinhold, ingo_weinhold@gmx.de. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include <errno.h>
#include <unistd.h>

#include <errno_private.h>
#include <syscalls.h>


static int
common_pipe(int streams[2], int flags)
{
	status_t error = _kern_create_pipe(streams, flags);
	if (error != B_OK) {
		__set_errno(error);
		return -1;
	}

	return 0;
}


int
pipe2(int streams[2], int flags)
{
	return common_pipe(streams, flags);
}


int
pipe(int streams[2])
{
	return common_pipe(streams, 0);
}
