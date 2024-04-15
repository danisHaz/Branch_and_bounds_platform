#pragma once

namespace babp {
namespace core {

	// operations
	static char const WHITESPACE = ' ';
	static char const COMMA = ',';
	static char const MINUS = '-';
	static char const PLUS = '+';
	static char const MULT = '*';
    static char const DIV = '/';
	static char const CIRCLE_BRACKET_OPEN = '(';
	static char const CIRCLE_BRACKET_CLOSE = ')';
	static char const ANGLE_BRACKET_OPEN = '<';
	static char const ANGLE_BRACKET_CLOSE = '>';

	static char const VARIABLE_NAME_SIGN = '$';

	// declared types
	static char const* INT_T = "Int";
	static char const* DOUBLE_T = "Double";
	static char const* MATRIX_T = "Matrix";
	static char const* VECTOR_T = "Vector";

	static char const SEMICOLON = ':';

	// other
	static int KEY_LEN = 32;
	static constexpr int KEY_ALPHABET_START = int('a');
	static constexpr int KEY_ALPHABET_LEN = ('z' - 'a') + 1;

} // namespace core
} // namespace babp