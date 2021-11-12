/*
 * NFA Lexer
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef NFA_LEXER_H
#define NFA_LEXER_H  1

#include <peruse/nfa-state.h>

#include "input.h"

/*
 * The NFA lexer constructor captures NFA, no one should try to use
 * the NFA passed to the constructor.
 */
struct nfa_lexer *nfa_lexer_alloc (struct nfa_state *start,
				   struct input *in);
void nfa_lexer_free (struct nfa_lexer *o);

int nfa_lexer_eof (struct nfa_lexer *o);

struct nfa_token {
	char *text;
	size_t len;
	int id;
};

/* returns NULL on error, matched token overwise */
const struct nfa_token *nfa_lexer (struct nfa_lexer *o);

#endif  /* NFA_LEXER_H */
