/*
 * Thompson NFA State
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
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
	int c, id;
	struct nfa_state *out[2];
};

struct nfa_state *nfa_state_atom  (unsigned c);
struct nfa_state *nfa_state_cat   (struct nfa_state *a, struct nfa_state *b);
struct nfa_state *nfa_state_union (struct nfa_state *a, struct nfa_state *b);
struct nfa_state *nfa_state_opt   (struct nfa_state *a);
struct nfa_state *nfa_state_star  (struct nfa_state *a);
struct nfa_state *nfa_state_plus  (struct nfa_state *a);

void nfa_state_free (struct nfa_state *o);

void nfa_state_order (struct nfa_state *o);
size_t nfa_state_count (const struct nfa_state *o);

#endif  /* NFA_STATE_H */
