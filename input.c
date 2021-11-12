/*
 * Input Buffer
 *
 * Copyright (c) 2016-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"

#ifdef PERUSE_BUFSIZE
#define SIZE  PERUSE_BUFSIZE
#else
#define SIZE  BUFSIZ
#endif

static size_t stdio_read (void *to, size_t count, void *cookie)
{
	FILE *in = cookie;

	return fread (to, 1, count, in);
}

int input_init (struct input *o, peruse_reader *read, void *cookie)
{
	if ((o->data = malloc (SIZE)) == NULL)
		return 0;

	o->cursor = o->data;
	o->avail  = 0;

	o->read   = read == NULL ? stdio_read : read;
	o->cookie = cookie;

	return 1;
}

void input_fini (struct input *o)
{
	if (o == NULL)
		return;

	free (o->data);
}

size_t input_fill (struct input *o)
{
	size_t count;

	/* move existing data into head of buffer */
	memmove (o->data, o->cursor, o->avail);
	o->cursor = o->data;

	count = o->read (o->cursor + o->avail, SIZE - o->avail, o->cookie);
	o->avail += count;

	return count;
}

int input_eat (struct input *o, size_t count)
{
	if (count > o->avail)
		return 0;

	o->avail  -= count;
	o->cursor += count;

	return 1;
}
