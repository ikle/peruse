/*
 * Thompson NFA State
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdlib.h>

#include "nfa-state.h"

static struct nfa_state *
nfa_state (int from, int to, struct nfa_state *a, struct nfa_state *b)
{
	struct nfa_state *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	o->next = NULL;
	o->from = from;
	o->to   = to;
	o->index = 0;

	o->out[0] = a;
	o->out[1] = b;

	o->color = 1;
	return o;
}

void nfa_state_free (struct nfa_state *o)
{
	struct nfa_state *next;

	for (; o != NULL; o = next) {
		next = o->next;
		free (o);
	}
}

void nfa_state_order (struct nfa_state *o)
{
	size_t i;

	for (i = 0; o != NULL; ++i, o = o->next)
		o->index = i;
}

size_t nfa_state_count (const struct nfa_state *o)
{
	size_t count;

	for (count = 0; o != NULL; o = o->next)
		++count;

	return count;
}

void nfa_state_color (struct nfa_state *o, int color)
{
	for (; o != NULL; o = o->next)
		o->color = color;
}

/* NFA leaf node constructors */

struct nfa_state *nfa_state_atom (int c)
{
	if (c < 0) {
		errno = EINVAL;
		return NULL;
	}

	return nfa_state (c, c, NULL, NULL);
}

struct nfa_state *nfa_state_range (int from, int to)
{
	if (from < 0 || to < 0) {
		errno = EINVAL;
		return NULL;
	}

	return nfa_state (from, to, NULL, NULL);
}

/* NFA node helper ops */

static struct nfa_state *nfa_split (struct nfa_state *a, struct nfa_state *b)
{
	struct nfa_state *o;

	if ((o = nfa_state (NFA_SPLIT, 0, a, b)) == NULL)
		goto no_state;

	return o;
no_state:
	nfa_state_free (a);
	nfa_state_free (b);
	return NULL;
}

static void nfa_merge (struct nfa_state *o, struct nfa_state *b)
{
	struct nfa_state *tail;

	for (tail = o; tail->next != NULL; tail = tail->next) {}

	tail->next = b;
}

static void nfa_join (struct nfa_state *a, struct nfa_state *b)
{
	for (; a != NULL; a = a->next) {
		if (a->out[0] == NULL)
			a->out[0] = b;

		if (a->out[1] == NULL)
			a->out[1] = b;
	}
}

/* NFA compound node constructors */

struct nfa_state *nfa_state_cat (struct nfa_state *a, struct nfa_state *b)
{
	nfa_join  (a, b);
	nfa_merge (a, b);
	return a;
}

struct nfa_state *nfa_state_union (struct nfa_state *a, struct nfa_state *b)
{
	struct nfa_state *o;

	if ((o = nfa_split (a, b)) == NULL)
		return NULL;

	nfa_merge (o, a);
	nfa_merge (o, b);
	return o;
}

struct nfa_state *nfa_state_opt (struct nfa_state *a)
{
	struct nfa_state *o;

	if ((o = nfa_split (a, NULL)) == NULL)
		return NULL;

	nfa_merge (o, a);
	return o;
}

struct nfa_state *nfa_state_star (struct nfa_state *a)
{
	struct nfa_state *o;

	if ((o = nfa_split (a, NULL)) == NULL)
		return NULL;

	nfa_join  (a, o);
	nfa_merge (o, a);
	return o;
}

struct nfa_state *nfa_state_plus (struct nfa_state *a)
{
	struct nfa_state *o;

	if ((o = nfa_split (a, NULL)) == NULL)
		return NULL;

	nfa_join  (a, o);
	nfa_merge (a, o);
	return a;
}
