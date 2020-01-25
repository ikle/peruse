/*
 * Thompson NFA processor
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <limits.h>
#include <stdlib.h>

#include "bitset.h"
#include "nfa-proc.h"
#include "nfa-state.h"

struct nfa_proc {
	const struct nfa_state *start;
	size_t count;
	const struct nfa_state **map;
	long *cset, *nset;
};

struct nfa_proc *nfa_proc_alloc (const struct nfa_state *nfa)
{
	struct nfa_proc *o;
	const struct nfa_state *p;
	size_t i;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	o->start = nfa;
	o->count = nfa_state_count (nfa);

	if ((o->map = malloc (o->count * sizeof (o->map[0]))) == NULL)
		goto no_map;

	for (p = o->start, i = 0; p != NULL; p = p->next, ++i)
		o->map[i] = p;

	if ((o->cset = bitset_alloc (o->count)) == NULL)
		goto no_cset;

	if ((o->nset = bitset_alloc (o->count)) == NULL)
		goto no_nset;

	return o;
no_nset:
	bitset_free (o->cset);
no_cset:
	free (o->map);
no_map:
	free (o);
	return NULL;
}

void nfa_proc_free (struct nfa_proc *o)
{
	bitset_free (o->nset);
	bitset_free (o->cset);
	free (o->map);
	free (o);
}

/* returns non-zero if stop state added */
static int add_state (struct nfa_proc *o, long *set, const struct nfa_state *s)
{
	if (s == NULL)
		return 1;

	if (bitset_is_member (set, s->id))
		return 0;

	if (s->c == NFA_SPLIT)
		return add_state (o, set, s->out[0]) |
		       add_state (o, set, s->out[1]);

	bitset_add (set, s->id);
	return 0;
}

int nfa_proc_start (struct nfa_proc *o)
{
	bitset_clear (o->cset, o->count);

	return add_state (o, o->cset, o->start);
}

int nfa_proc_step (struct nfa_proc *o, unsigned c)
{
	size_t i;
	const struct nfa_state *s;
	int match = 0;
	long *t;

	if (bitset_is_empty (o->cset, o->count))
		return -1;

	bitset_clear (o->nset, o->count);

	for (
		i = bitset_find (o->cset, 0, o->count);
		i < o->count;
		i = bitset_find (o->cset, i + 1, o->count)
	) {
		s = o->map[i];

		if (s->c == c)
			match |= add_state (o, o->nset, s->out[0]);
	}

	t = o->cset; o->cset = o->nset; o->nset = t;  /* swap sets */
	return match;
}

int nfa_proc_match (struct nfa_proc *o, const char *s)
{
	const char *p;
	int state = nfa_proc_start (o);

	for (p = s; *p != '\0'; ++p)
		if ((state = nfa_proc_step (o, *p)) < 0)
			return 0;

	/* test for full string match */
	return state > 0;
}

/*
 * To Do:
 *
 * 3. Move match function into test sample.
 *
 */
