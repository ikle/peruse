/*
 * NFA Lexer
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>

#include <peruse/bitset.h>
#include <peruse/nfa-proc.h>

#include "input.h"
#include "nfa-lexer.h"

struct nfa_lexer {
	struct nfa_proc *proc;
	struct input in;

	struct nfa_token token;
	int eof;
};

/*
 * The NFA lexer constructor captures NFA, no one should try to use
 * the NFA passed to the constructor.
 */
struct nfa_lexer *nfa_lexer_alloc (struct nfa_state *start,
				   peruse_reader *read, void *cookie)
{
	struct nfa_lexer *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	if (!input_init (&o->in, read, cookie))
		goto no_input;

	if ((o->proc = nfa_proc_alloc (start)) == NULL)
		goto no_proc;

	o->token.text = NULL;
	o->token.len = 0;
	o->token.id = 0;
	o->eof = 0;

	return o;
no_proc:
	input_fini (&o->in);
no_input:
	free (o);
	return NULL;
}

void nfa_lexer_free (struct nfa_lexer *o)
{
	if (o == NULL)
		return;

	nfa_proc_free (o->proc);
	input_fini (&o->in);
	free (o);
}

int nfa_lexer_eof (struct nfa_lexer *o)
{
	return o->eof && o->in.avail == 0;
}

const struct nfa_token *nfa_lexer (struct nfa_lexer *o)
{
	size_t i;
	int c, color;

	input_eat (&o->in, o->token.len);
start:
	o->token.id = nfa_proc_start (o->proc);
	o->token.len = 0;

	for (i = 0; i < o->in.avail;) {
		c = o->in.cursor[i++];

		if ((color = nfa_proc_step (o->proc, c)) < 0)
			return o->token.id == 0 ? NULL : &o->token;

		if (color > 0) {
			o->token.id = color;
			o->token.text = (void *) o->in.cursor;
			o->token.len = i;
		}
	}

	if (!o->eof) {
		if (input_fill (&o->in) == 0)
			o->eof = 1;

		goto start;
	}

	return o->token.id == 0 ? NULL : &o->token;
}
