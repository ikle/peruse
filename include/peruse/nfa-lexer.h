/*
 * Thompson NFA-based Lexer
 *
 * Copyright (c) 2020-2024 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_NFA_LEXER_H
#define PERUSE_NFA_LEXER_H  1

#include <peruse/nfa-state.h>

/*
 * The peruse_reader function reads upto count bytes into buffer.
 * Returns count of bytes stored into buffer or zero on EOF.
 */
typedef size_t peruse_reader (void *to, size_t count, void *cookie);

/*
 * The function nfa_lexer_alloc creates NFA Lexer context with the specified
 * start state. If the reader is NULL then the standard I/O reader is used
 * and cookie should points to FILE object.
 *
 * If no maximum window size is specified, then the buffer size for
 * standard I/O is used.
 *
 * NOTE: The NFA lexer constructor captures NFA, no one should try to use
 * the NFA passed to the constructor.
 */
struct nfa_lexer *nfa_lexer_alloc (struct nfa_state *start, size_t size,
				   peruse_reader *read, void *cookie);
/*
 * The function nfa_lexer_free destroys NFA Lexer context.
 */
void nfa_lexer_free (struct nfa_lexer *o);

/*
 * The function nfa_lexer_eof returns 1 if end of input stream reached
 * or zero otherwise.
 */
int nfa_lexer_eof (struct nfa_lexer *o);

/*
 * NFA Lexer Token
 */
struct nfa_token {
	int color;	/* token identifier */
	char *text;
	size_t len;
};

/*
 * The function nfa_lexer_get returns last matched token or NULL on error.
 *
 * The function nfa_lexer returns next matched token or NULL on error.
 */
const struct nfa_token *nfa_lexer_get (struct nfa_lexer *o);
const struct nfa_token *nfa_lexer     (struct nfa_lexer *o);

#endif  /* PERUSE_NFA_LEXER_H */
