#pragma once

#include <utility>
#include <string>
#include <vector>
#include <optional>
#include <climits>
#include <iostream>

#include "algorithm.hpp"

namespace babp {
namespace core {

	struct Group {
		std::size_t const start, end;

		Group(std::size_t const start, std::size_t const end): start { start }, end { end } {}
		Group(std::optional<std::size_t> start, std::optional<std::size_t> end): start { start.value_or(0) }, end { end.value_or(INT_MAX) } {}
		Group(): start { 0 }, end { INT_MAX } {}
	};

	enum class PointerType {
		BEGIN, END
	};

	struct Pointer {
		const std::size_t position;
		const PointerType type;

		Pointer(std::size_t position, PointerType type): position { position }, type { type } {}
    };

	template < typename T >
	struct TypeHolder {

		T construct() {
			return T { };
		}
	};

	using Statement = std::variant<std::string, structural::OperationType>;

	class Inflater {

		void pushVariableName(
			std::string const& fStr,
			Group const nameRange,
			std::vector<Statement>& sequence
		) const ;

		std::optional<structural::OperationType> getOperationOrNull(char const c) const;
		std::string filterSpaces(std::string const& fStr) const;
		void filterSpaces(std::string &fStr) const;
		std::vector<Group> getGroupsList(std::string const& fStr) const;
		std::vector<Statement> parseGroupToOperations(std::string const& fStr, Group const& group) const;
		std::vector<Statement> getOperationList(std::string const& fStr) const;

		public:
		std::vector<Statement> inflate(std::string const& functionStr) const;
		std::vector<Statement> inflate(std::string &&functionStr) const;

		void printStatement(std::ostream &output, Statement const& statement) const;
	};

} // namespace core
} // namespace babp