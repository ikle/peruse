/*
 * Thompson NFA State
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef NFA_STATE_H
#define NFA_STATE_H  1

#include <stddef.h>

enum nfa_type {
	NFA_SPLIT = -1,
};

struct nfa_state {
	struct nfa_state *next;
	short from, to;
	int index;	/* used by processor to map state to state index */
	struct nfa_state *out[2];
	short color;	/* used by lexer to distinguish rules, 1 by default */
};

struct nfa_state *nfa_state_atom  (int c);
struct nfa_state *nfa_state_range (int from, int to);

struct nfa_state *nfa_state_cat   (struct nfa_state *a, struct nfa_state *b);
struct nfa_state *nfa_state_union (struct nfa_state *a, struct nfa_state *b);
struct nfa_state *nfa_state_opt   (struct nfa_state *a);
struct nfa_state *nfa_state_star  (struct nfa_state *a);
struct nfa_state *nfa_state_plus  (struct nfa_state *a);

void nfa_state_free (struct nfa_state *o);

/* set up indexes for NFA state list */
void nfa_state_order (struct nfa_state *o);
size_t nfa_state_count (const struct nfa_state *o);

void nfa_state_color (struct nfa_state *o, int color);

#endif  /* NFA_STATE_H */
