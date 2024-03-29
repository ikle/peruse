/*
 * S-Expression Node helpers
 *
 * Copyright (c) 2016-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdarg.h>
#include <stdlib.h>

#include "se-node.h"

/*
 * NOTE: Ownership of arguments transferred, and in case of errors all
 * arguments will be freed.
 */
struct se_node *se_node_alloc (int type, int rank, ...)
{
	struct se_node *o;
	va_list ap;
	size_t i;

	va_start (ap, rank);

	if ((o = malloc (sizeof (*o) + sizeof (o->arg[0]) * rank)) == NULL)
		goto error;

	o->type = type;
	o->rank = rank;

	for (i = 0; i < rank; ++i)
		o->arg[i] = va_arg (ap, struct se_node *);

	va_end (ap);
	return o;
error:
	for (i = 0; i < rank; ++i)
		se_node_free (va_arg (ap, struct se_node *));

	va_end (ap);
	return NULL;
}

void se_node_free (struct se_node *o)
{
	size_t i;

	if (o == NULL)
		return;

	for (i = 0; i < o->rank; ++i)
		se_node_free (o->arg[i]);

	free (o);
}
