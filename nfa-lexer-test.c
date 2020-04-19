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

static struct nfa_state *compile_nfa (void)
{
	size_t i;
	struct nfa_state *nfa, *head = NULL;

	for (i = 0; i < ARRAY_SIZE (rules); ++i) {
		if ((nfa = re_parse (rules[i].re, rules[i].token)) == NULL)
			goto error;

		head = head == NULL ? nfa : nfa_state_union (head, nfa);
	}

	return head;
error:
	nfa_state_free (head);
	return NULL;
}

int main (int argc, char *argv[])
{
	struct input in;
	struct nfa_state *set;
	struct nfa_lexer *lex;
	const struct nfa_token *tok;

	if (!input_init (&in, 0, NULL, stdin)) {
		perror ("nfa-lexer-test");
		return 1;
	}

	if ((set = compile_nfa ()) == NULL) {
		fprintf (stderr, "nfa-lexer-test: cannot compile lexer\n");
		return 1;
	}

	if ((lex = nfa_lexer_alloc (set, &in)) == NULL) {
		fprintf (stderr, "nfa-lexer-test: cannot construct lexer\n");
		return 1;
	}

	while ((tok = nfa_lexer (lex)) != NULL)
		printf ("%d: '%.*s'\n", tok->id, (int) tok->len, tok->text);

	if (!nfa_lexer_eof (lex)) {
		fprintf (stderr, "E: lexical error\n");
		return 1;
	}

	nfa_lexer_free (lex);
	input_fini (&in);
	return 0;
}
