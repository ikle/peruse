/*
 * Node object
 *
 * Copyright (c) 2016-2018 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef OPP_NODE_H
#define OPP_NODE_H  1

#include <stddef.h>

struct node {
	short type;
	unsigned short size;
	struct node *arg[];
};

/*
 * NOTE: Ownership of arguments transferred, and in case of errors all
 * arguments will be freed.
 */
struct node *node_alloc (short type, unsigned short size, ...);
void node_free (struct node *o);

#endif  /* OPP_NODE_H */
