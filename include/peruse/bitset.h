/*
 * Compact Binary Set
 *
 * Copyright (c) 2007-2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_BITSET_H
#define PERUSE_BITSET_H  1

#include <stddef.h>

long *bitset_alloc (size_t limit);
void bitset_free (long *o);

void bitset_clear (long *o, size_t limit);
void bitset_add (long *o, size_t x);

int bitset_is_member (const long *o, size_t x);
int bitset_is_empty  (const long *o, size_t limit);

size_t bitset_find (const long *o, size_t from, size_t limit);

#endif  /* PERUSE_BITSET_H */
