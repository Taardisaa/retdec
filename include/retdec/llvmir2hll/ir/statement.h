/**
* @file include/retdec/llvmir2hll/ir/statement.h
* @brief A representation of a program statement.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_IR_STATEMENT_H
#define RETDEC_LLVMIR2HLL_IR_STATEMENT_H

#include <cstddef>
#include <iterator>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "retdec/llvmir2hll/ir/value.h"
#include "retdec/llvmir2hll/support/smart_ptr.h"
#include "retdec/llvmir2hll/support/types.h"

namespace retdec {
namespace llvmir2hll {

class GotoStmt;

/**
* @brief A representation of a program statement.
*
* Each statement can have at most one parent, i.e. it has to be used at most
* once in the whole program. If you want to have the same statement on multiple
* places, clone it by calling clone().
*
* Instances of this class have reference object semantics.
*
* Statement successors have to be set in subclasses by calling setSuccessor()
* (the reason is that @c shared_from_this() cannot be called from a
* constructor).
*
* Other functions for manipulating statements can be found in @c
* Utils/IR.h.
*/
class Statement: public Value {
private:
	/// Weak-pointer storage for predecessors to avoid shared_ptr cycles.
	using WeakStmtVec = std::vector<WkPtr<Statement>>;

public:
	/// Iterator that locks weak predecessor pointers on dereference.
	class predecessor_iterator {
		using base_iter = WeakStmtVec::const_iterator;
		base_iter it_;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = ShPtr<Statement>;
		using difference_type = std::ptrdiff_t;
		using pointer = void;
		using reference = ShPtr<Statement>;

		predecessor_iterator() = default;
		explicit predecessor_iterator(base_iter it) : it_(it) {}

		ShPtr<Statement> operator*() const { return it_->lock(); }
		predecessor_iterator& operator++() { ++it_; return *this; }
		predecessor_iterator operator++(int) { auto tmp = *this; ++it_; return tmp; }
		bool operator==(const predecessor_iterator& o) const { return it_ == o.it_; }
		bool operator!=(const predecessor_iterator& o) const { return it_ != o.it_; }
	};

public:
	/**
	* @brief Replaces all occurrences of @a oldExpr with @a newExpr in the
	*        current statement.
	*
	* @param[in] oldExpr Old expression to be replaced.
	* @param[in] newExpr Replacement.
	*
	* In compound statements, their body is left untouched, i.e. this function
	* doesn't replace expressions in the bodies of statements.
	*
	* @par Preconditions
	*  - @a oldExpr is non-null
	*/
	virtual void replace(ShPtr<Expression> oldExpr,
		ShPtr<Expression> newExpr) = 0;

	/**
	* @brief Returns @c true if the statement is a compound statement, @c false
	*        otherwise.
	*
	* A compound statement is a statement with nested statements, e.g. an if
	* statement, a switch statement, a while loop, and a for loop.
	*/
	virtual bool isCompound() = 0;

	/**
	* @brief Returns the statement as an expression.
	*
	* If the statement cannot be converted into an expression, the null pointer
	* is returned.
	*
	* Parts of the statement are not cloned (if you want a clone, call @c
	* clone() on the returned expression).
	*/
	virtual ShPtr<Expression> asExpression() const = 0;

	/// @name Successor Management
	/// @{
	bool hasSuccessor() const;
	ShPtr<Statement> getSuccessor() const;
	void setSuccessor(ShPtr<Statement> newSucc);
	void removeSuccessor();
	void appendStatement(ShPtr<Statement> stmt);
	/// @}

	/// @name Predecessor Management
	/// @{
	bool hasPredecessors() const;
	std::size_t getNumberOfPredecessors() const;
	void addPredecessor(ShPtr<Statement> stmt);
	ShPtr<Statement> getUniquePredecessor() const;
	void removePredecessor(ShPtr<Statement> stmt);
	void removePredecessors(bool onlyNonGoto = false);
	void prependStatement(ShPtr<Statement> stmt);

	predecessor_iterator predecessor_begin() const;
	predecessor_iterator predecessor_end() const;
	/// @}

	/// @name Label
	/// @{
	bool hasLabel() const;
	std::string getLabel() const;
	void setLabel(const std::string &newLabel);
	void removeLabel();
	void transferLabelFrom(ShPtr<Statement> stmt);
	void transferLabelTo(ShPtr<Statement> stmt);
	/// @}

	ShPtr<Statement> getParent() const;
	Address getAddress() const;

	/// @name Goto Targets
	/// @{
	bool isGotoTarget() const;
	void redirectGotosTo(ShPtr<Statement> stmt);
	/// @}

	static void removeStatement(ShPtr<Statement> stmt);
	static void removeStatementButKeepDebugComment(ShPtr<Statement> stmt);
	static bool areEqualStatements(ShPtr<Statement> stmts1, ShPtr<Statement> stmts2);
	static bool isStatementInStatements(ShPtr<Statement> stmt,
		ShPtr<Statement> stmts);
	static void replaceStatement(ShPtr<Statement> oldStmt,
		ShPtr<Statement> newStmt);
	static void removeLastStatement(ShPtr<Statement> stmts);
	static ShPtr<Statement> mergeStatements(
		ShPtr<Statement> stmt1, ShPtr<Statement> stmt2);
	static ShPtr<Statement> cloneStatements(ShPtr<Statement> stmts);
	static ShPtr<Statement> getLastStatement(ShPtr<Statement> stmts);

	/// Break all shared_ptr cycles in the statement graph rooted at @a body.
	/// For use during Function destruction only.
	static void breakCycles(ShPtr<Statement> body);

	/// Clear targets of all orphaned gotos still alive after per-function
	/// cycle breaking.  Called once during Module teardown.
	static void breakAllOrphanedCycles();

protected:
	Statement(Address a = Address::Undefined);

protected:
	/// Successor statement.
	ShPtr<Statement> succ;

	/// Predecessor statements (weak pointers to break shared_ptr cycles).
	WeakStmtVec preds;

	/// Label (lazily allocated — only goto targets have labels).
	std::unique_ptr<std::string> label;

	/// Address of ASM instruction from which this statement was created from.
	Address address;

private:
	bool targetIsCurrentStatement(ShPtr<GotoStmt> gotoStmt) const;
	bool containsJustGotosToCurrentStatement(const WeakStmtVec &stmts) const;
};

} // namespace llvmir2hll
} // namespace retdec

#endif
