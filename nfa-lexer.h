/*
 * NFA Lexer
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef NFA_LEXER_H
#define NFA_LEXER_H  1

#include "nfa-proc.h"

struct nfa_rule {
	struct nfa_rule *next;
	struct nfa_proc *proc;
	int token;
};

struct nfa_lexer *nfa_lexer_alloc (const struct nfa_rule *start);
void nfa_lexer_free (struct nfa_lexer *o);

int nfa_lexer_write (struct nfa_lexer *o, const char *data, size_t len);
int nfa_lexer_eof (struct nfa_lexer *o);

struct nfa_token {
	char *text;
	size_t len;
	int id;		/* zero on error */
};

const struct nfa_token *nfa_lexer (struct nfa_lexer *o);

#endif  /* NFA_LEXER_H */
