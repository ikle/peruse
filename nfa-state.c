/*
 * Thompson NFA State
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>

#include "nfa-state.h"

static struct nfa_state *
nfa_state (int c, struct nfa_state *a, struct nfa_state *b)
{
	struct nfa_state *s;

	s = malloc (sizeof (*s));

	s->next = NULL;
	s->c = c;
	s->index = 0;

	s->out[0] = a;
	s->out[1] = b;

	s->color = 1;
	return s;
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

struct nfa_state *nfa_state_atom (unsigned c)
{
	return nfa_state (c, NULL, NULL);
}

static struct nfa_state *nfa_split (struct nfa_state *a, struct nfa_state *b)
{
	return nfa_state (NFA_SPLIT, a, b);
}

/* NFA node helper ops */

static void nfa_merge (struct nfa_state *a, struct nfa_state *b)
{
	for (; a->next != NULL; a = a->next) {}

	a->next = b;
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
	nfa_join(a, b);
	nfa_merge (a, b);
	return a;
}

struct nfa_state *nfa_state_union (struct nfa_state *a, struct nfa_state *b)
{
	struct nfa_state *s = nfa_split (a, b);

	nfa_merge (s, a);
	nfa_merge (s, b);
	return s;
}

struct nfa_state *nfa_state_opt (struct nfa_state *a)
{
	struct nfa_state *s = nfa_split (a, NULL);

	nfa_merge (s, a);
	return s;
}

struct nfa_state *nfa_state_star (struct nfa_state *a)
{
	struct nfa_state *s = nfa_split (a, NULL);

	nfa_join (a, s);
	nfa_merge (s, a);
	return s;
}

struct nfa_state *nfa_state_plus (struct nfa_state *a)
{
	struct nfa_state *s = nfa_split (a, NULL);

	nfa_join (a, s);
	nfa_merge (a, s);
	return a;
}
