/*
 * RE to NFA compiler
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RE_PARSER_H
#define RE_PARSER_H  1

#include "nfa-state.h"

struct nfa_state *re_parse (const char *re, int color);

#endif  /* RE_PARSER_H */
