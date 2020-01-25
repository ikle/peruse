/*
 * Thompson NFA processor
 *
 * Copyright (c) 2020 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef NFA_PROC_H
#define NFA_PROC_H  1

#include <stddef.h>

#include "nfa-state.h"

struct nfa_proc *nfa_proc_alloc (const struct nfa_state *nfa);
void nfa_proc_free (struct nfa_proc *o);

int nfa_proc_start (struct nfa_proc *o);
int nfa_proc_step  (struct nfa_proc *o, unsigned c);

int nfa_proc_match (struct nfa_proc *o, const char *s);

#endif  /* NFA_PROC_H */
