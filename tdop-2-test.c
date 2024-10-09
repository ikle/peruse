/*
 * Sample Top-Down Operator Precedence parser
 *
 * Copyright (c) 2016-2024 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "se-node.h"

/*
 * Eqn Simple Lexer
 */

enum eqn_token {
	ERROR = -2, EOS = -1, EQN_VAR_TOK = 0,
	EQN_SEMI_TOK, EQN_LPAR_TOK, EQN_RPAR_TOK, EQN_EQU_TOK, EQN_COMMA_TOK,
	EQN_COND_TOK, EQN_COLON_TOK, EQN_PLUS_TOK, EQN_MINUS_TOK, EQN_MUL_TOK,
	EQN_POW_TOK, EQN_FACT_TOK,
	EQN_TOKEN_COUNT
};

static int la, value, x, y;
static const char *error;

static int get_token (int c)
{
	switch (c) {
	case EOF:	return EOS;
	case '!':	return EQN_FACT_TOK;	/* Factorial operator	*/
	case '(':	return EQN_LPAR_TOK;	/* Left parenthesis	*/
	case ')':	return EQN_RPAR_TOK;	/* Right parenthesis	*/
	case '*':	return EQN_MUL_TOK;	/* Multiplication operator */
	case '+':	return EQN_PLUS_TOK;
	case ',':	return EQN_COMMA_TOK;	/* Comma operator	*/
	case '-':	return EQN_MINUS_TOK;
	case ':':	return EQN_COLON_TOK;	/* Colon		*/
	case ';':	return EQN_SEMI_TOK;	/* Semicolon		*/
	case '=':	return EQN_EQU_TOK;	/* Equality operator	*/
	case '?':	return EQN_COND_TOK;	/* Condition operator	*/
	case '^':	return EQN_POW_TOK;	/* Power operator	*/
	default:	return isalpha (c) ? (value = c, EQN_VAR_TOK) : ERROR;
	}
}

static void eqn_lexer_next (FILE *in)
{
	int c;

	while (isspace (c = fgetc (in)))
		if (c == '\n')
			x = 0, ++y;
		else
			++x;
	++x;
	la = get_token (c);
}

static int lexer_expect (FILE *in, int tok, const char *reason)
{
	if (la == tok) {
		eqn_lexer_next (in);  /* eat token */
		return 1;
	}

	error = reason;
	return 0;
}

/*
 * Eqn Object Types
 */

enum eqn_type {
	EQN_SEQ = 0, EQN_GROUP, EQN_SET, EQN_PAIR, EQN_COND,
	EQN_ID, EQN_SUM, EQN_NEG, EQN_DIFF, EQN_PROD, EQN_POW, EQN_FACT,
	EQN_TYPE_COUNT
};

static const char *eqn_type_name[EQN_TYPE_COUNT] = {
	"seq", "group", "set", "pair", "cond",
	"id", "sum", "neg", "diff", "prod", "pow", "fact"
};

static const char *get_key_name (unsigned key)
{
	return key < EQN_TYPE_COUNT ? eqn_type_name[key] : NULL;
}

/*
 * Eqn Parsing Rules
 */

static struct se_node *fetch_atom (FILE *in)
{
	struct se_node *o;

	if (la == EQN_VAR_TOK) {
		o = se_node_alloc (-value, 0);
		eqn_lexer_next (in);
		return o;
	}

	error = "Unexpected sequence";
	return NULL;
}

static struct se_node *eqn_parse_exp (FILE *in, int prio);

#define LED_SEMI_BOUND	(10 + 0)	/* right assoc */

/*
 * exp -> exp a, ';', exp b : (seq a b)
 */
static struct se_node *led_semi (FILE *in, struct se_node *a)
{
	struct se_node *b;

	if ((b = eqn_parse_exp (in, LED_SEMI_BOUND)) == NULL)
		goto ep2;

	return se_node_alloc (EQN_SEQ, 2, a, b);
ep2:	se_node_free (a);
	return NULL;
}

#define NUD_LPAR_BOUND	(10 + 1)	/* seq does not allowed */

/*
 * exp -> '(', exp a, ')'
 */
static struct se_node *nud_lpar (FILE *in)
{
	struct se_node *a;

	if ((a = eqn_parse_exp (in, NUD_LPAR_BOUND)) == NULL)
		goto ep1;

	if (!lexer_expect (in, EQN_RPAR_TOK, "Right parenthesis" " expected"))
		goto ep2;

	return se_node_alloc (EQN_GROUP, 1, a);
ep2:	se_node_free (a);
ep1:	return NULL;
}

#define LED_EQU_BOUND	(20 + 0)	/* right assoc */

/*
 * exp -> exp a, '=', exp b : (set a b)
 */
static struct se_node *led_equ (FILE *in, struct se_node *a)
{
	struct se_node *b;

	if ((b = eqn_parse_exp (in, LED_EQU_BOUND)) == NULL)
		goto ep2;

	return se_node_alloc (EQN_SET, 2, a, b);
ep2:	se_node_free (a);
	return NULL;
}

#define LED_COMMA_BOUND	(30 + 0)	/* right assoc */

/*
 * exp -> exp a, ',', exp b : (pair a b)
 */
static struct se_node *led_comma (FILE *in, struct se_node *a)
{
	struct se_node *b;

	if ((b = eqn_parse_exp (in, LED_COMMA_BOUND)) == NULL)
		goto ep2;

	return se_node_alloc (EQN_PAIR, 2, a, b);
ep2:	se_node_free (a);
	return NULL;
}

#define LED_COND_BOUND	(40 + 0)	/* right assoc */

/*
 * exp -> exp a, '?', exp b, ':', exp c : (cond a b c)
 */
static struct se_node *led_cond (FILE *in, struct se_node *a)
{
	struct se_node *b, *c;

	if ((b = eqn_parse_exp (in, NUD_LPAR_BOUND)) == NULL)
		goto ep2;

	if (!lexer_expect (in, EQN_COLON_TOK, "Colon" " expected"))
		goto ep3;

	if ((c = eqn_parse_exp (in, LED_COND_BOUND)) == NULL)
		goto ep4;

	return se_node_alloc (EQN_COND, 3, a, b, c);
ep4:	se_node_free (b);
ep3:
ep2:	se_node_free (a);
	return NULL;
}

#define LED_PLUS_BOUND	(50 + 1)	/* left assoc */

/*
 * exp -> exp a, '+', exp b : (sum a b)
 */
static struct se_node *led_plus (FILE *in, struct se_node *a)
{
	struct se_node *b;

	if ((b = eqn_parse_exp (in, LED_PLUS_BOUND)) == NULL)
		goto ep2;

	return se_node_alloc (EQN_SUM, 2, a, b);
ep2:	se_node_free (a);
	return NULL;
}

#define LED_MINUS_BOUND	(50 + 1)	/* left assoc */

/*
 * exp -> exp a, '-', exp b : (diff a b)
 */
static struct se_node *led_minus (FILE *in, struct se_node *a)
{
	struct se_node *b;

	if ((b = eqn_parse_exp (in, LED_MINUS_BOUND)) == NULL)
		goto ep2;

	return se_node_alloc (EQN_DIFF, 2, a, b);
ep2:	se_node_free (a);
	return NULL;
}

#define LED_MUL_BOUND	(60 + 1)	/* left assoc */

/*
 * exp -> exp a, '*', exp b : (mul a b)
 */
static struct se_node *led_mul (FILE *in, struct se_node *a)
{
	struct se_node *b;

	if ((b = eqn_parse_exp (in, LED_MUL_BOUND)) == NULL)
		goto ep2;

	return se_node_alloc (EQN_PROD, 2, a, b);
ep2:	se_node_free (a);
	return NULL;
}

#define NUD_PLUS_BOUND	(70 + 1)

/*
 * exp -> '+', exp a : (id a)
 */
static struct se_node *nud_plus (FILE *in)
{
	struct se_node *a;

	if ((a = eqn_parse_exp (in, NUD_PLUS_BOUND)) == NULL)
		goto ep1;

	return se_node_alloc (EQN_ID, 1, a);
ep1:	return NULL;
}

#define NUD_MINUS_BOUND	(70 + 1)

/*
 * exp -> '-', exp a : (neg a)
 */
static struct se_node *nud_minus (FILE *in)
{
	struct se_node *a;

	if ((a = eqn_parse_exp (in, NUD_MINUS_BOUND)) == NULL)
		goto ep1;

	return se_node_alloc (EQN_NEG, 1, a);
ep1:	return NULL;
}

#define LED_POW_BOUND	(80 + 0)	/* right assoc */

/*
 * exp -> exp a, '^', exp b : (pow a b)
 */
static struct se_node *led_pow (FILE *in, struct se_node *a)
{
	struct se_node *b;

	if ((b = eqn_parse_exp (in, LED_POW_BOUND)) == NULL)
		goto ep2;

	return se_node_alloc (EQN_POW, 2, a, b);
ep2:	se_node_free (a);
	return NULL;
}

#define LED_FACT_BOUND	(90 + 1)	/* not assoc */

/*
 * exp -> exp a, '!' : (fact a)
 */
static struct se_node *led_fact (FILE *in, struct se_node *a)
{
	return se_node_alloc (EQN_FACT, 1, a);
}

/*
 * TDOP Parser Table
 */

typedef struct se_node *nud_fn (FILE *in);
typedef struct se_node *led_fn (FILE *in, struct se_node *a);

struct tdop_entry {
	nud_fn	*nud;
	led_fn	*led;
	int	led_prio;
};

static const struct tdop_entry table[EQN_TOKEN_COUNT] = {
	[EQN_SEMI_TOK]	= { NULL,	led_semi,	10	},
	[EQN_LPAR_TOK]	= { nud_lpar,	NULL,			},
	[EQN_EQU_TOK]	= { NULL,	led_equ,	20	},
	[EQN_COMMA_TOK]	= { NULL,	led_comma,	30	},
	[EQN_COND_TOK]	= { NULL,	led_cond,	40	},
	[EQN_PLUS_TOK]	= { nud_plus,	led_plus,	50	},
	[EQN_MINUS_TOK]	= { nud_minus,	led_minus,	50	},
	[EQN_MUL_TOK]	= { NULL,	led_mul,	60	},
	[EQN_POW_TOK]	= { NULL,	led_pow,	80	},
	[EQN_FACT_TOK]	= { NULL,	led_fact,	90	},
};

static nud_fn *get_nud (unsigned tok)
{
	return tok < EQN_TOKEN_COUNT ? table[tok].nud : NULL;
}

static led_fn *get_led (unsigned tok, int prio)
{
	int led_prio = tok < EQN_TOKEN_COUNT ? table[tok].led_prio : 0;

	return (led_prio >= prio && tok < EQN_TOKEN_COUNT) ? table[tok].led : NULL;
}

/*
 * TDOP Parser Core
 */

static struct se_node *parse_primary (FILE *in)
{
	nud_fn *nud;

	if ((nud = get_nud (la)) != NULL) {
		eqn_lexer_next (in);  /* eat nud op token */
		return nud (in);
	}

	return fetch_atom (in);
}

static struct se_node *eqn_parse_exp (FILE *in, int prio)
{
	struct se_node *a;
	led_fn *led;

	for (
		a = parse_primary (in);
		a != NULL && (led = get_led (la, prio)) != NULL;
		a = led (in, a)
	)
		eqn_lexer_next (in);  /* eat led op token */

	return a;
}

/*
 * TDOP Parser Test Code
 */

struct se_node *parse (FILE *in)
{
	struct se_node *o;

	eqn_lexer_next (in);

	if ((o = eqn_parse_exp (in, 0 + 1)) == NULL || la == EOS)
		return o;

	error = "Unexpected sequence after expression";
	se_node_free (o);
	return NULL;
}

static void se_node_show (struct se_node *o, size_t indent)
{
	size_t i;

	for (i = 0; i < indent; ++i)
		printf ("    ");

	if (o->type < 0)
		printf ("var %c\n", -o->type);
	else {
		printf ("%s\n", get_key_name (o->type));

		for (i = 0; i < o->rank; ++i)
			se_node_show (o->arg[i], indent + 1);
	}
}

int main (int argc, char *argv[])
{
	struct se_node *o;

	if ((o = parse (stdin)) == NULL) {
		fprintf (stderr, "E: %s at (%d,%d)\n", error, x, y);
		return 1;
	}

	se_node_show (o, 0);
	se_node_free (o);
	return 0;
}
