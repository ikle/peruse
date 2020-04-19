/*
 * NFA Lexer Sample
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "nfa-lexer.h"
#include "re-parser.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)  (sizeof (a) / sizeof ((a)[0]))
#endif

struct rule {
	int token;
	const char *re;
};

static const struct rule rules[] = {
	{ 10,	"if" },
	{ 11,	"then" },
	{ 12,	"else" },

	{ 40,	"( |\t|\n)+" },
	{ 41,	"0|(1(0|1)*)" },
	{ 42,	"(a|b)(-?(a|b|0|1))*" },
};

static struct nfa_rule *compile (void)
{
	struct nfa_rule *set;
	size_t i;
	struct nfa_state *nfa;

	assert (ARRAY_SIZE (rules) > 0);

	if ((set = malloc (sizeof (set[0]) * ARRAY_SIZE (rules))) == NULL)
		return NULL;

	for (i = 0; i < ARRAY_SIZE (rules); ++i) {
		set[i].next = set + i + 1;
		set[i].token = rules[i].token;

		if ((nfa = re_parse (rules[i].re)) == NULL ||
		    (set[i].proc = nfa_proc_alloc (nfa)) == NULL)
			goto error;
	}

	set[i - 1].next = NULL;
	return set;
error:
	for (; i > 0; --i)
		nfa_proc_free (set[i - 1].proc);

	free (set);
	return NULL;
}

int main (int argc, char *argv[])
{
	struct input in;
	struct nfa_rule *set;
	struct nfa_lexer *lex;
	const struct nfa_token *tok;

	if (!input_init (&in, 0, NULL, stdin)) {
		perror ("nfa-lexer-test");
		return 1;
	}

	if ((set = compile ()) == NULL) {
		fprintf (stderr, "nfa-lexer-test: cannot compile lexer\n");
		return 1;
	}

	if ((lex = nfa_lexer_alloc (set, &in)) == NULL) {
		fprintf (stderr, "nfa-lexer-test: cannot construct lexer\n");
		return 1;
	}

	for (;;) {
		tok = nfa_lexer (lex);

		if (nfa_lexer_eof (lex))
			break;

		if (tok == NULL) {
			fprintf (stderr, "E: lexical error\n");
			return 1;
		}

		printf ("%d: '%.*s'\n", tok->id, (int) tok->len, tok->text);
	}

	nfa_lexer_free (lex);
	input_fini (&in);
	return 0;
}
