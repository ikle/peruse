#!/usr/bin/python3
#
# Top Down Operator Precedence
#
# Copyright (c) 2016 Alexei A. Smekalkine <ikle@ikle.ru>
#

class TDOP:
	class Op:
		lbp = 0

		def nud (o):
			name = type (o).__name__
			raise SyntaxError ('%s requires left operand' % name)

		def get_right (o, parser, rbp):
			try:
				value = parser.expression (rbp)
			except StopIteration as e:
				name = type (o).__name__
				message = '%s requires right operand' % name
				raise SyntaxError (message) from e

			return value

	class EOI (Op): pass

	def expression (o, rbp = 0):
		left, o.right = (o.right, next (o.tokens))
		value = left.nud ()

		while rbp < o.right.lbp:
			left, o.right = (o.right, next (o.tokens))
			value = left.led (o, value)

		return value

	def parse (o, input):
		o.tokens = o.tokenize (input)
		o.right = next (o.tokens)

		value = o.expression ()

		if type (o.right) != o.EOI:
			message = 'extra token at end: %s' % repr (o.right)
			raise SyntaxError (message)

		return value

import re

class Calc (TDOP):
	class Int (TDOP.Op):
		def __init__ (o, value):
			o.value = int (value)

		def __repr__ (o):
			return repr (o.value)

		def nud (o):
			return o.value

	class Add (TDOP.Op):
		lbp = 10

		def led (o, parser, value):
			return value + o.get_right (parser, 10)

	class Mul (TDOP.Op):
		lbp = 20

		def led (o, parser, value):
			return value * o.get_right (parser, 20)

	def tokenize (o, input):

		pattern = r'\s*(?:(\d+)|(.))'

		for number, op in re.findall (pattern, input):
			if number:
				yield o.Int (number)
			elif op == '+':
				yield o.Add ()
			elif op == '*':
				yield o.Mul ()
			else:
				raise SyntaxError ('unknown operetor')

		yield o.EOI ()

tests = [
	'1 + 2 * 3',
	'+ 2 * 3',
	'1 + 2 *',
	'13 7',
]

for program in tests:
	try:
		print (program, '=', Calc ().parse (program))
	except SyntaxError as e:
		print ('%s:' % program, e)

