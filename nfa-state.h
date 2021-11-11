/*
 * Thompson NFA State Internals
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_NFA_STATE_INT_H
#define PERUSE_NFA_STATE_INT_H  1

#include <peruse/nfa-state.h>

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

/* set up indexes for NFA state list */
void nfa_state_order (struct nfa_state *o);
size_t nfa_state_count (const struct nfa_state *o);

#endif  /* PERUSE_NFA_STATE_INT_H */
