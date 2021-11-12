/*
 * Thompson NFA State
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_NFA_STATE_H
#define PERUSE_NFA_STATE_H  1

#include <stddef.h>

/*
 * NFA leaf node constructors
 */
struct nfa_state *nfa_state_atom  (int c);
struct nfa_state *nfa_state_range (int from, int to);

/*
 * NFA compound node constructors
 */
struct nfa_state *nfa_state_cat   (struct nfa_state *a, struct nfa_state *b);
struct nfa_state *nfa_state_union (struct nfa_state *a, struct nfa_state *b);
struct nfa_state *nfa_state_opt   (struct nfa_state *a);
struct nfa_state *nfa_state_star  (struct nfa_state *a);
struct nfa_state *nfa_state_plus  (struct nfa_state *a);

/*
 * NFA destructor
 */
void nfa_state_free (struct nfa_state *o);

/*
 * Paint NFA root node to a specified color
 */
void nfa_state_color (struct nfa_state *o, int color);

/*
 * Get total number of states in NFA
 */
size_t nfa_state_count (const struct nfa_state *o);

#endif  /* PERUSE_NFA_STATE_H */
