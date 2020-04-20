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

enum re_token {
	RE_EOI		= '\0',
	RE_BRANCH	= '|',
	RE_OPEN		= '(',
	RE_CLOSE	= ')',
	RE_SET_OPEN	= '[',
	RE_SET_CLOSE	= ']',
	RE_OPT		= '?',
	RE_STAR		= '*',
	RE_PLUS		= '+',
};

void re_lexer_init (struct re_lexer *o, const char *re);

int re_lexer_peek (struct re_lexer *o);
int re_lexer_next (struct re_lexer *o);
int re_lexer_eat  (struct re_lexer *o, int c);

#endif  /* RE_LEXER_H */
