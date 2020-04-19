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
	const struct nfa_rule *start, *stop;
	long *set;

	struct nfa_token token;
	size_t avail, size;
	int eof;
};

struct nfa_lexer *nfa_lexer_alloc (const struct nfa_rule *start)
{
	struct nfa_lexer *o;
	const struct nfa_rule *p;
	size_t i;

	if ((o = malloc (sizeof (*o))) == NULL)
		return NULL;

	o->start = start;

	for (p = o->start, i = 0; p != NULL; p = p->next, ++i) {}

	o->stop = NULL;

	if ((o->set = bitset_alloc (i)) == NULL)
		goto no_set;

	o->token.text = NULL;
	o->token.len = 0;
	o->token.id = 0;
	o->size = o->avail = 0;
	o->eof = 0;

	return o;
no_set:
	free (o);
	return NULL;
}

void nfa_lexer_free (struct nfa_lexer *o)
{
	free (o->token.text);
	bitset_free (o->set);
	free (o);
}

static int nfa_lexer_start (struct nfa_lexer *o)
{
	const struct nfa_rule *p;
	size_t i;
	int token = 0;

	o->stop = NULL;

	for (p = o->start, i = 0; p != o->stop; p = p->next, ++i)
		if (nfa_proc_start (p->proc) && token == 0) {
			o->stop = p->next;
			token = p->token;
		}

	bitset_clear (o->set, i);
	return token;
}

static int nfa_lexer_step (struct nfa_lexer *o, unsigned c)
{
	const struct nfa_rule *p;
	size_t i;
	int token = -1, state;

	for (p = o->start, i = 0; p != o->stop; p = p->next, ++i) {
		if (bitset_is_member (o->set, i))
			continue;

		if ((state = nfa_proc_step (p->proc, c)) < 0) {
			bitset_add (o->set, i);
			continue;
		}

		if (state > 0) {
			o->stop = p->next;
			return p->token;
		}

		if (token < 0)
			token = 0;
	}

	return token;
}

int nfa_lexer_write (struct nfa_lexer *o, const char *data, size_t len)
{
	size_t total = o->avail + len;
	char *text;

	if (len == 0) {
		o->eof = 1;
		return 1;
	}

	if (total > o->size) {
		if ((text = realloc (o->token.text, total)) == NULL)
			return 0;

		o->token.text = text;
		o->size = total;
	}

	memcpy (o->token.text + o->avail, data, len);
	o->avail += len;
	return 1;
}

int nfa_lexer_eof (struct nfa_lexer *o)
{
	return o->eof && o->avail == 0;
}

const struct nfa_token *nfa_lexer (struct nfa_lexer *o)
{
	size_t i = 0;
	int c, token;

	/* strip old token */
	memmove (o->token.text, o->token.text + o->token.len,
		 o->avail - o->token.len);
	o->avail -= o->token.len;
	o->token.len = 0;

	o->token.id = nfa_lexer_start (o);

	while (i < o->avail) {
		c = o->token.text[i++];

		if ((token = nfa_lexer_step (o, c)) < 0)
			return &o->token;

		if (token > 0) {
			o->token.id = token;
			o->token.len = i;
		}
	}

	if (!o->eof) {
		o->token.len = 0;
		return NULL;  /* want more data */
	}

	if (o->token.len != i)
		o->token.id = 0;

	return &o->token;
}
