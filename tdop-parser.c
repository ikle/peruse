/*
 * Sample Top-Down Operator Precedence parser
 *
 * Copyright (c) 2016-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "se-node.h"

/* lexer/parser context */
static int la;
static int x, y;
static const char *error;

static void next (FILE *in)
{
	while (isspace (la = fgetc (in)))
		if (la == '\n')
			x = 0, ++y;
		else
			++x;

	++x;
}

static int get_prio (int type)
{
	switch (type) {
	case '=':  return 1;
	case ',':  return 2;
	case '?':  return 3;
	case '+':
	case '-':  return 4;
	case '*':  return 5;
	case '+' + 0x80:
	case '-' + 0x80: return 6;
	case '^':  return 7;
	case '!':  return 8;
	}

	return 0;
}

static int get_prefix_type (int token)
{
	switch (token) {
	case '+':
	case '-':  return token + 0x80;  /* uplus/uminus */
	case '(':  return token;
	}

	return 0;
}

static int get_trailer (int type)
{
	switch (type) {
	case '(':  return ')';
	}

	return 0;
}

static int is_postfix (int type)
{
	switch (type) {
	case '!': return 1;
	}

	return 0;
}

static int is_right (int type)
{
	switch (type) {
	case '=':
	case ',':
	case '^':  return 1;
	}

	return 0;
}

static struct se_node *parse_exp (FILE *in, int prio);

static struct se_node *parse_prefix_op (FILE *in, int op)
{
	struct se_node *o;
	int trailer;

	next (in);  /* eat op */

	if ((o = parse_exp (in, get_prio (op) + 1)) == NULL)
		return NULL;

	if ((trailer = get_trailer (op)) != 0) {
		if (la != trailer) {
			error = "trailer expected";
			se_node_free (o);
			return NULL;
		}

		next (in);  /* eat trailer */
	}

	return se_node_alloc (op, 1, o);
}

static struct se_node *parse_primary (FILE *in)
{
	int op;
	struct se_node *o;

	if ((op = get_prefix_type (la)) != 0)
		return parse_prefix_op (in, op);

	/* fetch object from lexer */
#if 1
	if (isalpha (la)) {
		o = se_node_alloc (-la, 0);
#else
	if (object != NULL) {
		o = object;
#endif
		next (in);
		return o;
	}

	error = "unexpected sequence";
	return NULL;
}

static struct se_node *parse_exp (FILE *in, int prio)
{
	struct se_node *a, *b, *c;
	int op, bound;

	if ((a = parse_primary (in)) == NULL)
		return NULL;

	while (la != EOF && get_prio (la) >= prio) {
		op = la;  /* get type */
		next (in);  /* eat op */

		if (is_postfix (op)) {
			if ((a = se_node_alloc (op, 1, a)) == NULL)
				return NULL;

			continue;
		}

		bound = is_right (op) ? get_prio (op) : get_prio (op) + 1;

		if ((b = parse_exp (in, bound)) == NULL)
			goto no_b;

		/* NOTE: else part is optional here */
		if (!(op == '?' && la == ':')) {
			if ((a = se_node_alloc (op, 2, a, b)) == NULL)
				return NULL;

			continue;
		}

		next (in);  /* eat ':' */

		if ((c = parse_exp (in, bound)) == NULL)
			goto no_c;

		if ((a = se_node_alloc (op, 3, a, b, c)) == NULL)
			return NULL;
	}

	return a;
no_c:
	se_node_free (b);
no_b:
	se_node_free (a);
	return NULL;
}

struct se_node *parse (FILE *in)
{
	struct se_node *o;

	next (in);

	if ((o = parse_exp (in, 0 + 1)) == NULL)
		return NULL;

	if (la == EOF)
		return o;

	error = "unexpected sequence after expression";
	se_node_free (o);
	return NULL;
}

static const char *get_name (int token)
{
	switch (token) {
	case ',':  return "pair";
	case '?':  return "if-then-else";
	case '!':  return "fact";
	case '=':  return "set";
	case '+':  return "sum";
	case '-':  return "diff";
	case '*':  return "prod";
	case '^':  return "pow";
	case '(':
	case '+' + 0x80: return "id";
	case '-' + 0x80: return "neg";
	}

	return NULL;
}

static void show (struct se_node *o, size_t indent)
{
	size_t i;

	for (i = 0; i < indent; ++i)
		printf ("    ");

	if (o->type < 0)
		printf ("var %c\n", -o->type);
	else {
		printf ("%s\n", get_name (o->type));

		for (i = 0; i < o->size; ++i)
			show (o->arg[i], indent + 1);
	}
}

int main (int argc, char *argv[])
{
	struct se_node *o;

	if ((o = parse (stdin)) == NULL) {
		fprintf (stderr, "E: %s at (%d,%d)\n", error, x, y);
		return 1;
	}

	show (o, 0);
	se_node_free (o);
	return 0;
}
