/*
 * Thompson NFA processor
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <limits.h>
#include <stdlib.h>

#include <peruse/bitset.h>
#include <peruse/nfa-proc.h>

#include "nfa-state.h"

struct nfa_proc {
	struct nfa_state *start;
	size_t count;
	const struct nfa_state **map;
	long *cset, *nset;
};

/*
 * The NFA processor constructor captures NFA, no one should try to use
 * the NFA passed to the constructor.
 */
struct nfa_proc *nfa_proc_alloc (struct nfa_state *nfa)
{
	struct nfa_proc *o;
	const struct nfa_state *p;
	size_t i;

	if ((o = malloc (sizeof (*o))) == NULL)
		goto no_obj;

	nfa_state_order (nfa);

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
no_obj:
	nfa_state_free (nfa);
	return NULL;
}

void nfa_proc_free (struct nfa_proc *o)
{
	bitset_free (o->nset);
	bitset_free (o->cset);
	free (o->map);
	nfa_state_free (o->start);
	free (o);
}

/* returns non-zero if stop state added */
static int add_state (struct nfa_proc *o, long *set, const struct nfa_state *s)
{
	if (s == NULL)
		return 1;

	if (bitset_is_member (set, s->index))
		return 0;

	if (s->from == NFA_SPLIT)
		return add_state (o, set, s->out[0]) |
		       add_state (o, set, s->out[1]);

	bitset_add (set, s->index);
	return 0;
}

/*
 * returns node color on match (stop state reached), zero otherwise
 */
int nfa_proc_start (struct nfa_proc *o)
{
	bitset_clear (o->cset, o->count);

	if (add_state (o, o->cset, o->start))
		return o->start->color;

	return 0;
}

/*
 * returns -1 on error (no match), node color on match, zero otherwise
 */
int nfa_proc_step (struct nfa_proc *o, int c)
{
	size_t i;
	const struct nfa_state *s;
	int match = 0, error = 1;
	long *t;

	bitset_clear (o->nset, o->count);

	for (
		i = bitset_find (o->cset, 0, o->count);
		i < o->count;
		i = bitset_find (o->cset, i + 1, o->count)
	) {
		s = o->map[i];

		if (s->from <= c && c <= s->to) {
			error = 0;

			/*
			 * Note that we remember the color of the first
			 * matching node. Thus, rules added earlier have
			 * a higher priority.
			 */
			if (add_state (o, o->nset, s->out[0]) && match == 0)
				match = s->color;
		}
	}

	if (error)
		return -1;

	t = o->cset; o->cset = o->nset; o->nset = t;  /* swap sets */
	return match;
}
