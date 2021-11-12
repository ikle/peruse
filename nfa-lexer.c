/*
 * Thompson NFA-based Lexer
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <peruse/nfa-lexer.h>
#include <peruse/nfa-proc.h>
#include <peruse/nfa-window.h>

struct nfa_lexer {
	struct nfa_window *in;
	struct nfa_proc *proc;

	struct nfa_token token;
	int eof;
};

/*
 * The NFA lexer constructor captures NFA, no one should try to use
 * the NFA passed to the constructor.
 */
struct nfa_lexer *nfa_lexer_alloc (struct nfa_state *start, size_t size,
				   peruse_reader *read, void *cookie)
{
	struct nfa_lexer *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	if ((o->in = nfa_window_alloc (size, read, cookie)) == NULL)
		goto no_window;

	if ((o->proc = nfa_proc_alloc (start)) == NULL)
		goto no_proc;

	o->token.color = 0;
	o->token.text = NULL;
	o->token.len = 0;
	o->eof = 0;

	return o;
no_proc:
	nfa_window_free (o->in);
no_window:
	free (o);
	return NULL;
}

void nfa_lexer_free (struct nfa_lexer *o)
{
	if (o == NULL)
		return;

	nfa_proc_free (o->proc);
	nfa_window_free (o->in);
	free (o);
}

int nfa_lexer_eof (struct nfa_lexer *o)
{
	size_t avail = 1;

	nfa_window_request (o->in, &avail);

	return o->eof && avail == 0;
}

const struct nfa_token *nfa_lexer (struct nfa_lexer *o)
{
	size_t i, avail;
	const unsigned char *cursor;
	int c, color;

	nfa_window_release (o->in, o->token.len);
start:
	o->token.color = nfa_proc_start (o->proc);
	o->token.len = 0;

	avail = SIZE_MAX;
	cursor = nfa_window_request (o->in, &avail);

	for (i = 0; i < avail;) {
		c = cursor[i++];

		if ((color = nfa_proc_step (o->proc, c)) < 0)
			return o->token.color == 0 ? NULL : &o->token;

		if (color > 0) {
			o->token.color = color;
			o->token.text = (void *) cursor;
			o->token.len = i;
		}
	}

	if (!o->eof) {
		if (!nfa_window_fill (o->in))
			o->eof = 1;

		goto start;
	}

	return o->token.color == 0 ? NULL : &o->token;
}
