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
	unsigned len;
	int last, in_set;
};

enum re_token {
	RE_EOI		= -1,
	RE_BRANCH	= -2,
	RE_OPEN		= -3,
	RE_CLOSE	= -4,
	RE_SET_OPEN	= -5,
	RE_SET_CLOSE	= -6,
	RE_OPT		= -7,
	RE_STAR		= -8,
	RE_PLUS		= -9,
};

void re_lexer_init (struct re_lexer *o, const char *re);

int re_lexer_peek (struct re_lexer *o);
int re_lexer_next (struct re_lexer *o);
int re_lexer_eat  (struct re_lexer *o, int c);

#endif  /* RE_LEXER_H */
