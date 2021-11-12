/*
 * Thompson NFA-based Lexer Sample
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>

#include <peruse/nfa-lexer.h>
#include <peruse/re-parser.h>

static struct re_rule rules[] = {
	{ rules + 1,	"if",			10 },
	{ rules + 2,	"then",			11 },
	{ rules + 3,	"else",			12 },

	{ rules + 4,	"[ \t\\n]+",		40 },
	{ rules + 5,	"0|(1[01]*)",		41 },
	{ NULL,		"[ab](-?[a-z0-9])*",	42 },
};

int main (int argc, char *argv[])
{
	struct nfa_state *set;
	struct nfa_lexer *lex;
	const struct nfa_token *tok;

	if ((set = re_parse_list (rules)) == NULL) {
		fprintf (stderr, "nfa-lexer-test: cannot compile lexer\n");
		return 1;
	}

	if ((lex = nfa_lexer_alloc (set, 0, NULL, stdin)) == NULL) {
		fprintf (stderr, "nfa-lexer-test: cannot construct lexer\n");
		return 1;
	}

	while ((tok = nfa_lexer (lex)) != NULL)
		printf ("%d: '%.*s'\n", tok->color, (int) tok->len, tok->text);

	if (!nfa_lexer_eof (lex)) {
		fprintf (stderr, "E: lexical error\n");
		return 1;
	}

	nfa_lexer_free (lex);
	return 0;
}
