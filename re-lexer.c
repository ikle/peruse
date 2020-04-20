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
}

int re_lexer_peek (struct re_lexer *o)
{
	return *o->p;
}

int re_lexer_next (struct re_lexer *o)
{
	int c;

	if ((c = re_lexer_peek (o)) == '\0')
		return c;

	++o->p;
	return c;
}

int re_lexer_eat (struct re_lexer *o, int c)
{
	if (re_lexer_peek (o) != c)
		return 0;

	re_lexer_next (o);
	return 1;
}
