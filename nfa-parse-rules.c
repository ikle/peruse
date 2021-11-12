/*
 * Colored Regular Expression List to Thompson NFA compiler
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <peruse/nfa-parse.h>

struct nfa_state *nfa_parse_rules (const struct nfa_rule *rules)
{
	struct nfa_state *nfa, *head = NULL;
	const struct nfa_rule *p;

	for (p = rules; p != NULL; p = p->next) {
		if ((nfa = nfa_parse_re (p->re, p->color)) == NULL)
			goto error;

		head = head == NULL ? nfa : nfa_state_union (head, nfa);
	}

	return head;
error:
	nfa_state_free (head);
	return NULL;
}
