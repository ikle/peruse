/*
 * NFA Input Window (Buffer)
 *
 * Copyright (c) 2016-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_NFA_WINDOW_H
#define PERUSE_NFA_WINDOW_H  1

#include <stddef.h>

/*
 * The function nfa_window_reader reads up to the count bytes into the
 * specified buffer. Returns the number of bytes written, or zero on
 * EOF or errors.
 */
typedef size_t nfa_window_reader (void *to, size_t count, void *cookie);

/*
 * The function nfa_window_alloc creates the NFA Input Window context.
 * If no maximum window size is specified, then the buffer size for
 * standard I/O is used.
 *
 * The function nfa_window_free destroys the NFA Input Window context.
 */
struct nfa_window *nfa_window_alloc (size_t size, nfa_window_reader *read,
				     void *cookie);
void nfa_window_free (struct nfa_window *o);

/*
 * The function nfa_window_fill fills the buffer with new data. Returns
 * 1 on success, or zero on EOF or errors.
 */
int nfa_window_fill (struct nfa_window *o);

/*
 * The function nfa_window_request requests a region for reading. Returns
 * a pointer to the window and the size of the window for reading.
 *
 * The function nfa_window_release releases readed data. The size should
 * be less or equal to the requested region size.
 */
void *nfa_window_request (struct nfa_window *o, size_t *len);
void  nfa_window_release (struct nfa_window *o, size_t  len);

#endif  /* PERUSE_NFA_WINDOW_H */
