/*
 * RE to NFA compiler
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "re-parser.h"

/* parser state and helpers */

struct re_parser {
	const char *in, *p;
};

static int re_peek (struct re_parser *o)
{
	return *o->p;
}

static int re_next (struct re_parser *o)
{
	int c;

	if ((c = re_peek (o)) == '\0')
		return '\0';

	++o->p;
	return c;
}

static int re_eat (struct re_parser *o, int c)
{
	if (re_peek (o) != c)
		return 0;

	re_next (o);
	return 1;
}

/* RE recursive descent parser */

static struct nfa_state *re_exp (struct re_parser *o);

static struct nfa_state *re_base (struct re_parser *o)
{
	struct nfa_state *a;

	if (re_peek (o) == '(') {
		re_next (o);
		a = re_exp (o);
		re_eat (o, ')');  /* OOPS: throw error here */
		return a;
	}

	return nfa_state_atom (re_next (o));
}

static struct nfa_state *re_piece (struct re_parser *o)
{
	struct nfa_state *a = re_base (o);
	int c;

	while ((c = re_peek (o)) != '\0')
		switch (c) {
		case '?':
			re_next (o);
			a = nfa_state_opt (a);
			break;
		case '*':
			re_next (o);
			a = nfa_state_star (a);
			break;
		case '+':
			re_next (o);
			a = nfa_state_plus (a);
			break;
		default:
			return a;
		}

	return a;
}

static struct nfa_state *re_branch (struct re_parser *o)
{
	struct nfa_state *a = re_piece (o);
	int c;

	while ((c = re_peek (o)) != '\0' && c != ')' && c != '|')
		a = nfa_state_cat (a, re_piece (o));

	return a;
}

static struct nfa_state *re_exp (struct re_parser *o)
{
	struct nfa_state *a = re_branch (o);

	while (re_peek (o) == '|') {
		re_next (o);
		a = nfa_state_union (a, re_branch (o));
	}

	return a;
}

struct nfa_state *re_parse (const char *re)
{
	struct re_parser o;
	struct nfa_state *start;

	o.p = o.in = re;

	start = re_exp (&o);
	return start;
}
