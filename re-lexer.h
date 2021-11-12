/*
 * RE simple lexer
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_RE_LEXER_H
#define PERUSE_RE_LEXER_H  1

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

static int re_lexer_escape (int c)
{
	switch (c) {
	case 'a':	return 0x07;
	case 'b':	return 0x08;
	case 't':	return 0x09;
	case 'n':	return 0x0a;
	case 'v':	return 0x0b;
	case 'f':	return 0x0c;
	case 'r':	return 0x0d;
	}

	return -1;
}

static int re_lexer_next (struct re_lexer *o)
{
	int c, e;

	if ((c = re_lexer_peek (o)) == '\0')
		return c;

	++o->p;

	if (c != '\\' || (e = re_lexer_escape (re_lexer_peek (o))) < 0)
		return c;

	++o->p;
	return e;
}

static int re_lexer_eat (struct re_lexer *o, int c)
{
	if (re_lexer_peek (o) != c)
		return 0;

	re_lexer_next (o);
	return 1;
}

#endif  /* PERUSE_RE_LEXER_H */
