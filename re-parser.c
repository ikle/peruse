/*
 * RE to NFA compiler
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "re-lexer.h"
#include "re-parser.h"

/* RE recursive descent parser */

static struct nfa_state *re_char (struct re_lexer *o)
{
	int c;

	if ((c = re_lexer_next (o)) == RE_EOI)
		return NULL;

	return nfa_state_atom (c);
}

static struct nfa_state *re_set (struct re_lexer *o)
{
	int c;
	struct nfa_state *a, *b;

	if ((a = re_char (o)) == NULL)
		return NULL;

	while ((c = re_lexer_peek (o)) != RE_SET_CLOSE && c != RE_EOI) {
		b = re_char (o);
		a = nfa_state_union (a, b);
	}

	return a;
}

static struct nfa_state *re_exp (struct re_lexer *o);

static struct nfa_state *re_atom (struct re_lexer *o)
{
	int c;
	struct nfa_state *a;

	if ((c = re_lexer_peek (o)) == RE_OPEN) {
		re_lexer_next (o);

		if ((a = re_exp (o)) == NULL)
			return NULL;

		if (!re_lexer_eat (o, RE_CLOSE))
			goto error;

		return a;
	}
	else if (c == RE_SET_OPEN) {
		re_lexer_next (o);

		if ((a = re_set (o)) == NULL)
			return NULL;

		if (!re_lexer_eat (o, RE_SET_CLOSE))
			goto error;

		return a;;
	}
	else if (c == '\\') {
		re_lexer_next (o);
		return re_char (o);
	}

	return re_char (o);
error:
	nfa_state_free (a);
	return NULL;
}

static struct nfa_state *re_piece (struct re_lexer *o)
{
	struct nfa_state *a;
	int c;

	if ((a = re_atom (o)) == NULL)
		return NULL;

	while ((c = re_lexer_peek (o)) != RE_EOI)
		switch (c) {
		case RE_OPT:
			re_lexer_next (o);
			a = nfa_state_opt (a);
			break;
		case RE_STAR:
			re_lexer_next (o);
			a = nfa_state_star (a);
			break;
		case RE_PLUS:
			re_lexer_next (o);
			a = nfa_state_plus (a);
			break;
		default:
			return a;
		}

	return a;
}

static struct nfa_state *re_branch (struct re_lexer *o)
{
	struct nfa_state *a, *b;
	int c;

	if ((a = re_piece (o)) == NULL)
		return NULL;

	while ((c = re_lexer_peek (o)) != RE_EOI && c != RE_CLOSE &&
	       c != RE_BRANCH) {
		if ((b = re_piece (o)) == NULL)
			goto error;

		a = nfa_state_cat (a, b);
	}

	return a;
error:
	nfa_state_free (a);
	return NULL;
}

static struct nfa_state *re_exp (struct re_lexer *o)
{
	struct nfa_state *a, *b;

	if ((a = re_branch (o)) == NULL)
		return NULL;

	while (re_lexer_peek (o) == RE_BRANCH) {
		re_lexer_next (o);
		if ((b = re_branch (o)) == NULL)
			goto error;

		a = nfa_state_union (a, b);
	}

	return a;
error:
	nfa_state_free (a);
	return NULL;
}

struct nfa_state *re_parse (const char *re, int color)
{
	struct re_lexer o;
	struct nfa_state *start;

	re_lexer_init (&o, re);

	if ((start = re_exp (&o)) == NULL)
		return NULL;

	nfa_state_color (start, color);
	return start;
}
