/*
 * NFA Input Window (Buffer)
 *
 * Copyright (c) 2016-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <peruse/nfa-window.h>

static size_t stdio_read (void *to, size_t count, void *cookie)
{
	return fread (to, 1, count, cookie);
}

struct nfa_window {
	char *data, *cursor;
	size_t size, avail;
	nfa_window_reader *read;
	void *cookie;
};

struct nfa_window *nfa_window_alloc (size_t size, nfa_window_reader *read,
				     void *cookie)
{
	struct nfa_window *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	o->size = size == 0 ? BUFSIZ : size;

	if ((o->data = malloc (o->size)) == NULL)
		goto no_data;

	o->cursor = o->data;
	o->avail  = 0;

	o->read   = read == NULL ? stdio_read : read;
	o->cookie = cookie;

	return o;
no_data:
	free (o);
	return NULL;
}

void nfa_window_free (struct nfa_window *o)
{
	if (o == NULL)
		return;

	free (o->data);
	free (o);
}

int nfa_window_fill (struct nfa_window *o)
{
	size_t count;

	/*
	 * move existing data into head of buffer
	 */
	memmove (o->data, o->cursor, o->avail);
	o->cursor = o->data;

	count = o->read (o->cursor + o->avail, o->size - o->avail, o->cookie);
	o->avail += count;

	return count > 0;
}

void *nfa_window_request (struct nfa_window *o, size_t *len)
{
	if (*len > o->avail)
		*len = o->avail;

	return o->cursor;
}

void nfa_window_release (struct nfa_window *o, size_t len)
{
	if (len > o->avail)
		len = o->avail;

	o->avail  -= len;
	o->cursor += len;
}
