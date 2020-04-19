/*
 * NFA Lexer
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>

#include "bitset.h"
#include "nfa-lexer.h"

struct nfa_lexer {
	const struct nfa_rule *start;
	long *set;
	struct input *in;

	struct nfa_token token;
	int eof;
};

struct nfa_lexer *nfa_lexer_alloc (const struct nfa_rule *start,
				   struct input *in)
{
	struct nfa_lexer *o;
	const struct nfa_rule *p;
	size_t i;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	o->start = start;

	for (p = o->start, i = 0; p != NULL; p = p->next, ++i) {}

	if ((o->set = bitset_alloc (i)) == NULL)
		goto no_set;

	o->in = in;

	o->token.text = NULL;
	o->token.len = 0;
	o->token.id = 0;
	o->eof = 0;

	return o;
no_set:
	free (o);
	return NULL;
}

void nfa_lexer_free (struct nfa_lexer *o)
{
	bitset_free (o->set);
	free (o);
}

static int nfa_lexer_start (struct nfa_lexer *o)
{
	const struct nfa_rule *p;
	size_t i;
	int token = 0;

	for (p = o->start, i = 0; p != NULL; p = p->next, ++i)
		if (nfa_proc_start (p->proc) && token == 0)
			token = p->token;

	bitset_clear (o->set, i);
	return token;
}

static int nfa_lexer_step (struct nfa_lexer *o, unsigned c)
{
	const struct nfa_rule *p;
	size_t i;
	int token = -1, state;

	for (p = o->start, i = 0; p != NULL; p = p->next, ++i) {
		if (bitset_is_member (o->set, i))
			continue;

		if ((state = nfa_proc_step (p->proc, c)) < 0) {
			bitset_add (o->set, i);
			continue;
		}

		if (state > 0) {
			if (token <= 0)
				token = p->token;

			continue;
		}

		if (token < 0)
			token = 0;
	}

	return token;
}

int nfa_lexer_eof (struct nfa_lexer *o)
{
	return o->eof && o->in->avail == 0;
}

const struct nfa_token *nfa_lexer (struct nfa_lexer *o)
{
	size_t i;
	int c, token;

	input_eat (o->in, o->token.len);
start:
	o->token.id = nfa_lexer_start (o);
	o->token.len = 0;

	for (i = 0; i < o->in->avail;) {
		c = o->in->cursor[i++];

		if ((token = nfa_lexer_step (o, c)) < 0)
			return &o->token;

		if (token > 0) {
			o->token.id = token;
			o->token.text = (void *) o->in->cursor;
			o->token.len = i;
		}
	}

	if (!o->eof) {
		if (input_fill (o->in) == 0)
			o->eof = 1;

		goto start;
	}

	if (o->token.id == 0)
		return NULL;

	return &o->token;
}
