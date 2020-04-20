/*
 * RE simple lexer
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "re-lexer.h"

void re_lexer_init (struct re_lexer *o, const char *re)
{
	o->p = re;
	o->last = RE_EOI;
	o->in_set = 0;
}

int re_lexer_peek (struct re_lexer *o)
{
	if (*o->p == '\0')
		return RE_EOI;

	o->len = 1;

	if (o->in_set) {
		if (o->last != RE_SET_OPEN && *o->p == ']')
			return RE_SET_CLOSE;

		return *o->p;
	}

	switch (*o->p) {
	case '|':	return RE_BRANCH;
	case '(':	return RE_OPEN;
	case ')':	return RE_CLOSE;
	case '[':	return RE_SET_OPEN;
	case '?':	return RE_OPT;
	case '*':	return RE_STAR;
	case '+':	return RE_PLUS;
	case '\\':
		o->len = 2;
		return o->p[1];
	}

	return *o->p;
}

int re_lexer_next (struct re_lexer *o)
{
	int c;

	if ((c = re_lexer_peek (o)) == RE_EOI)
		return c;

	switch (c) {
	case RE_SET_OPEN:	o->in_set = 1; break;
	case RE_SET_CLOSE:	o->in_set = 0; break;
	}

	o->p += o->len;
	o->last = c;
	return c;
}

int re_lexer_eat (struct re_lexer *o, int c)
{
	if (re_lexer_peek (o) != c)
		return 0;

	re_lexer_next (o);
	return 1;
}
