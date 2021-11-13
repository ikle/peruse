/*
 * S-Expression Node helpers
 *
 * Copyright (c) 2016-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_SE_NODE_H
#define PERUSE_SE_NODE_H  1

#include <stddef.h>

struct se_node {
	short type;
	unsigned short size;
	struct se_node *arg[];
};

/*
 * NOTE: Ownership of arguments transferred, and in case of errors all
 * arguments will be freed.
 */
struct se_node *se_node_alloc (short type, unsigned short size, ...);
void se_node_free (struct se_node *o);

#endif  /* PERUSE_SE_NODE_H */
