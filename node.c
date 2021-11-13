/*
 * Operator-precedence parser Node object
 *
 * Copyright (c) 2016-2018 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdarg.h>
#include <stdlib.h>

#include "node.h"

/*
 * NOTE: Ownership of arguments transferred, and in case of errors all
 * arguments will be freed.
 */
struct node *node_alloc (short type, unsigned short size, ...)
{
	struct node *o;
	va_list ap;
	size_t i;

	va_start (ap, size);

	if ((o = malloc (sizeof (*o) + sizeof (o->arg[0]) * size)) == NULL)
		goto error;

	o->type = type;
	o->size = size;

	for (i = 0; i < o->size; ++i)
		o->arg[i] = va_arg (ap, struct node *);

	va_end (ap);
	return o;
error:
	for (i = 0; i < o->size; ++i)
		node_free (va_arg (ap, struct node *));

	va_end (ap);
	return NULL;
}

void node_free (struct node *o)
{
	size_t i;

	if (o == NULL)
		return;

	for (i = 0; i < o->size; ++i)
		node_free (o->arg[i]);

	free (o);
}
