/*
 * RE to NFA compiler and NFA processor Sample
 *
 * Copyright (c) 20xy-2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "nfa-proc.h"
#include "re-parser.h"

int main (int argc, char *argv[])
{
	int i;
	struct nfa_state *nfa;
	struct nfa_proc *proc;
	size_t len;

	if (argc < 3) {
		fprintf (stderr, "usage:\n\tnfa-test RE string...\n");
		return 1;
	}

	if ((nfa = re_parse (argv[1])) == NULL) {
		fprintf (stderr, "nfa-test: cannot compile RE\n");
		return 1;
	}

	if ((proc = nfa_proc_alloc (nfa)) == NULL) {
		fprintf (stderr, "E: cannot initialize NFA processor\n");
		return 1;
	}

	for (i = 2; i < argc; ++i) {
		len = nfa_proc_match (proc, argv[i]);

		if (argv[i][len] == '\0')  /* test for full string match */
			printf ("%s\n", argv[i]);
	}

	nfa_proc_free (proc);
	nfa_state_free (nfa);
	return 0;
}
