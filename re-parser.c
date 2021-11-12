/*
 * Regular Expression to Thompson NFA compiler
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <limits.h>

#include <peruse/re-parser.h>

#include "re-lexer.h"

/* RE recursive descent parser */

static struct nfa_state *re_char (struct re_lexer *o)
{
	int c;

	if ((c = re_lexer_next (o)) == '\0')
		return NULL;

	return nfa_state_atom (c);
}

static struct nfa_state *re_range (struct re_lexer *o)
{
	int a, b;

	if ((a = re_lexer_next (o)) == '\0')
		return NULL;

	if (re_lexer_peek (o) != '-')
		return nfa_state_atom (a);

	re_lexer_next (o);

	if ((b = re_lexer_next (o)) == '\0')
		return NULL;

	return nfa_state_range (a, b);
}

static struct nfa_state *re_set (struct re_lexer *o)
{
	int c;
	struct nfa_state *a, *b;

	if ((a = re_range (o)) == NULL)
		return NULL;

	while ((c = re_lexer_peek (o)) != ']' && c != '\0') {
		if ((b = re_range (o)) == NULL)
			goto error;

		a = nfa_state_union (a, b);
	}

	return a;
error:
	nfa_state_free (a);
	return NULL;
}

static struct nfa_state *re_exp (struct re_lexer *o);

static struct nfa_state *re_atom (struct re_lexer *o)
{
	int c;
	struct nfa_state *a;

	if ((c = re_lexer_peek (o)) == '(') {
		re_lexer_next (o);

		if ((a = re_exp (o)) == NULL)
			return NULL;

		if (!re_lexer_eat (o, ')'))
			goto error;

		return a;
	}
	else if (c == '[') {
		re_lexer_next (o);

		if ((a = re_set (o)) == NULL)
			return NULL;

		if (!re_lexer_eat (o, ']'))
			goto error;

		return a;;
	}
	else if (c == '\\') {
		re_lexer_next (o);
		return re_char (o);
	}
	else if (c == '.') {
		re_lexer_next (o);
		return nfa_state_range (0, INT_MAX);
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

	while ((c = re_lexer_peek (o)) != '\0')
		switch (c) {
		case '?':
			re_lexer_next (o);
			a = nfa_state_opt (a);
			break;
		case '*':
			re_lexer_next (o);
			a = nfa_state_star (a);
			break;
		case '+':
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

	while ((c = re_lexer_peek (o)) != '\0' && c != ')' && c != '|') {
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

	while (re_lexer_peek (o) == '|') {
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
