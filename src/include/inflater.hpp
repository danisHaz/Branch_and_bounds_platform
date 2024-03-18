#pragma once

#include <utility>
#include <string>
#include <queue>
#include <vector>

#include "algorithm.hpp"

namespace babp {
namespace core {

	static char const WHITESPACE = ' ';
	static char const COMMA = ',';
	static char const MINUS = '-';
	static char const PLUS = '+';
	static char const MULT = '*';
    static char const DIV = '/';
	static char const CIRCLE_PARENTH_OPEN = '(';
	static char const CIRCLE_PARENTH_CLOSE = ')';
	static char const ANGLE_PARENTH_OPEN = '<';
	static char const ANGLE_PARENTH_CLOSE = '>';

	struct Group {
		int const start, end;

		Group(int start, int end): start { start }, end { end } {}
	};

	enum class PointerType {
		BEGIN, END
	};

	struct Pointer {
		const int position;
		const PointerType type;

		Pointer(int position, PointerType type): position { position }, type { type } {}
    };

	class Inflater {

		void filterSpaces(std::string &fStr) const;
		std::vector<::Group> getGroupsList(std::string const& fStr) const;
		std::queue<::structural::OperationType> parseGroupToOperations(std::string const& fStr, int pos) const;

		std::queue<::structural::OperationType> getOperationList(std::string const& fStr) const;

		public:
		void inflate(std::string &functionStr) const;

		std::string inflate(std::string &&functionStr) const;
	};

} // namespace core
} // namespace babp