/*
 * Colored Regular Expression List to Thompson NFA compiler
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <peruse/re-parser.h>

struct nfa_state *re_parse_list (const struct re_rule *rules)
{
	struct nfa_state *nfa, *head = NULL;
	const struct re_rule *p;

	for (p = rules; p != NULL; p = p->next) {
		if ((nfa = re_parse (p->re, p->color)) == NULL)
			goto error;

		head = head == NULL ? nfa : nfa_state_union (head, nfa);
	}

	return head;
error:
	nfa_state_free (head);
	return NULL;
}
