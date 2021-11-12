/*
 * Input Buffer
 *
 * Copyright (c) 2016-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef INPUT_H
#define INPUT_H  1

#include <stddef.h>

typedef size_t peruse_reader (void *to, size_t count, void *ctx);

struct input {
	unsigned char *data, *cursor;
	size_t size, avail;
	peruse_reader *read;
	void *cookie;
};

int input_init (struct input *o, size_t size, peruse_reader *read, void *cookie);
void input_fini (struct input *o);

size_t input_fill (struct input *o);
int input_eat  (struct input *o, size_t count);

#endif  /* INPUT_H */
