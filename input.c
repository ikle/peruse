/*
 * Input Buffer
 *
 * Copyright (c) 2016-2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"

static size_t stdio_read (void *to, size_t count, void *ctx)
{
	FILE *in = ctx;

	return fread (to, 1, count, in);
}

int input_init (struct input *o, size_t size, input_reader *read, void *ctx)
{
	o->size = size == 0 ? BUFSIZ : size;

	if ((o->data = malloc (o->size)) == NULL)
		return 0;

	o->cursor = o->data;
	o->avail = 0;

	o->read = read == NULL ? stdio_read : read;
	o->ctx  = ctx;

	return 1;
}

void input_fini (struct input *o)
{
	if (o == NULL)
		return;

	free (o->data);
}

int input_fill (struct input *o)
{
	size_t count;

	/* move existing data into head of buffer */
	memmove (o->data, o->cursor, o->avail);
	o->cursor = o->data;

	count = o->read (o->cursor + o->avail, o->size - o->avail, o->ctx);
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
