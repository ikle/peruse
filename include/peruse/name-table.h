/*
 * Simple Name Table
 *
 * Copyright (c) 2006-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PERUSE_NAME_TABLE_H
#define PERUSE_NAME_TABLE_H  1

#include <stddef.h>
#include <stdint.h>

struct name_table *name_table_alloc (void);
void name_table_free (struct name_table *o);

size_t name_table_max (struct name_table *o);
size_t name_table_add (struct name_table *o, const char *name, size_t len);

const char *name_table_get (struct name_table *o, size_t i);

#endif  /* PERUSE_NAME_TABLE_H */
