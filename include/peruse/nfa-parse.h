/*
 * Regular Expression to Thompson NFA compiler
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_NFA_PARSE_H
#define PERUSE_NFA_PARSE_H  1

#include <peruse/nfa-state.h>

struct nfa_state *nfa_parse_re (const char *re, int color);

struct nfa_rule {
	struct nfa_rule *next;
	char *re;
	int color;
};

struct nfa_state *nfa_parse_rules (const struct nfa_rule *rules);

#endif  /* PERUSE_NFA_PARSE_H */
