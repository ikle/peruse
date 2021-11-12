/*
 * Regular Expression to Thompson NFA compiler
 *
 * Copyright (c) 2020-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_RE_PARSER_H
#define PERUSE_RE_PARSER_H  1

#include <peruse/nfa-state.h>

struct nfa_state *re_parse (const char *re, int color);

struct re_rule {
	struct re_rule *next;
	char *re;
	int color;
};

struct nfa_state *re_parse_list (const struct re_rule *rules);

#endif  /* PERUSE_RE_PARSER_H */
