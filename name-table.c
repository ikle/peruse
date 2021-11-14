/*
 * Simple Name Table
 *
 * Copyright (c) 2006-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <peruse/name-table.h>

struct name_table {
	size_t last, avail;
	char **name;
};

struct name_table *name_table_alloc (void)
{
	struct name_table *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	o->last  = 0;
	o->avail = 8;

	if ((o->name = malloc (sizeof (o->name[0]) * o->avail)) != NULL)
		return o;

	free (o);
	return NULL;
}

void name_table_free (struct name_table *o)
{
	if (o == NULL)
		return;

	free (o->name);
	free (o);
}

size_t name_table_max (struct name_table *o)
{
	return o->last;
}

static char *name_clone (const char *name, size_t len)
{
	char *p;

	if (len == 0)
		len = strlen (name);

	if ((p = malloc (len + 1)) == NULL)
		return NULL;

	strncpy (p, name, len + 1);
	return p;
}

size_t name_table_add (struct name_table *o, const char *name, size_t len)
{
	size_t curr, next;
	char **p;

	if (o->last == o->avail) {
		curr = o->avail * sizeof (o->name[0]);
		
		if ((next = curr * 2) < curr) {
			errno = ENOMEM;
			return 0;
		}

		if ((p = realloc (o->name, next)) == NULL)
			return 0;

		o->name = p;
		o->avail *= 2;
	}

	if ((o->name[o->last] = name_clone (name, len)) == NULL)
		return 0;

	return ++o->last;
}

const char *name_table_get (struct name_table *o, size_t i)
{
	if (i == 0 || i > o->last)
		return NULL;

	return o->name[i - 1];
}
