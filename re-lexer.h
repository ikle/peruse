/*
 * RE simple lexer
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RE_LEXER_H
#define RE_LEXER_H  1

struct re_lexer {
	const char *p;
};

static void re_lexer_init (struct re_lexer *o, const char *re)
{
	o->p = re;
}

static int re_lexer_peek (struct re_lexer *o)
{
	return *o->p;
}

static int re_lexer_next (struct re_lexer *o)
{
	int c;

	if ((c = re_lexer_peek (o)) == '\0')
		return c;

	++o->p;
	return c;
}

static int re_lexer_eat (struct re_lexer *o, int c)
{
	if (re_lexer_peek (o) != c)
		return 0;

	re_lexer_next (o);
	return 1;
}

#endif  /* RE_LEXER_H */
