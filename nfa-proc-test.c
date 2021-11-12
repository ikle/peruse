/*
 * RE to NFA compiler and NFA processor Sample
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>

#include <peruse/nfa-proc.h>
#include <peruse/re-parser.h>

static int nfa_proc_match (struct nfa_proc *o, const char *s)
{
	int state = nfa_proc_start (o);

	for (; *s != '\0'; ++s)
		if ((state = nfa_proc_step (o, *s)) < 0)
			return 0;

	return state > 0;
}

int main (int argc, char *argv[])
{
	struct nfa_state *nfa;
	struct nfa_proc *proc;
	int i;

	if (argc < 3) {
		fprintf (stderr, "usage:\n\tnfa-test RE string...\n");
		return 1;
	}

	if ((nfa = re_parse (argv[1], 1)) == NULL) {
		fprintf (stderr, "nfa-test: cannot compile RE\n");
		return 1;
	}

	if ((proc = nfa_proc_alloc (nfa)) == NULL) {
		fprintf (stderr, "E: cannot initialize NFA processor\n");
		return 1;
	}

	for (i = 2; i < argc; ++i)
		if (nfa_proc_match (proc, argv[i]))
			printf ("%s\n", argv[i]);

	nfa_proc_free (proc);
	return 0;
}
