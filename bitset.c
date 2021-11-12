/*
 * Compact Binary Set
 *
 * Copyright (c) 2007-2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <peruse/bitset.h>

static size_t bitset_get_size (size_t limit)
{
	const size_t size = sizeof (long) * CHAR_BIT;

	return (limit + size - 1) / size;
}

void bitset_clear (long *o, size_t limit)
{
	memset (o, 0, sizeof (o[0]) * bitset_get_size (limit));
}

long *bitset_alloc (size_t limit)
{
	long *o = calloc (bitset_get_size (limit), sizeof (o[0]));

	return o;
}

void bitset_free (long *o)
{
	free (o);
}

void bitset_add (long *o, size_t x)
{
	const size_t size = sizeof (o[0]) * CHAR_BIT;
	const size_t pos = x / size;
	const size_t bit = x % size;

	o[pos] |= (1L << bit);
}

int bitset_is_member (const long *o, size_t x)
{
	const size_t size = sizeof (o[0]) * CHAR_BIT;
	const size_t pos = x / size;
	const size_t bit = x % size;

	return (o[pos] & (1L << bit)) != 0;
}

int bitset_is_empty (const long *o, size_t limit)
{
	size_t i, count = bitset_get_size (limit);

	for (i = 0; i < count; ++i)
		if (o[i] != 0)
			return 0;

	return 1;
}

size_t bitset_find (const long *o, size_t from, size_t limit)
{
	size_t x;

	for (x = from; x < limit; ++x)
		if (bitset_is_member (o, x))
			return x;

	return limit;
}
