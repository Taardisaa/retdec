/**
* @file src/llvmir2hll/support/statements_counter.cpp
* @brief Implementation of StatementsCounter.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#include "retdec/llvmir2hll/ir/assign_stmt.h"
#include "retdec/llvmir2hll/ir/break_stmt.h"
#include "retdec/llvmir2hll/ir/call_stmt.h"
#include "retdec/llvmir2hll/ir/const_array.h"
#include "retdec/llvmir2hll/ir/const_bool.h"
#include "retdec/llvmir2hll/ir/const_float.h"
#include "retdec/llvmir2hll/ir/const_int.h"
#include "retdec/llvmir2hll/ir/const_null_pointer.h"
#include "retdec/llvmir2hll/ir/const_string.h"
#include "retdec/llvmir2hll/ir/const_struct.h"
#include "retdec/llvmir2hll/ir/const_symbol.h"
#include "retdec/llvmir2hll/ir/continue_stmt.h"
#include "retdec/llvmir2hll/ir/empty_stmt.h"
#include "retdec/llvmir2hll/ir/for_loop_stmt.h"
#include "retdec/llvmir2hll/ir/function.h"
#include "retdec/llvmir2hll/ir/global_var_def.h"
#include "retdec/llvmir2hll/ir/goto_stmt.h"
#include "retdec/llvmir2hll/ir/if_stmt.h"
#include "retdec/llvmir2hll/ir/module.h"
#include "retdec/llvmir2hll/ir/pointer_type.h"
#include "retdec/llvmir2hll/ir/return_stmt.h"
#include "retdec/llvmir2hll/ir/statement.h"
#include "retdec/llvmir2hll/ir/switch_stmt.h"
#include "retdec/llvmir2hll/ir/ufor_loop_stmt.h"
#include "retdec/llvmir2hll/ir/unreachable_stmt.h"
#include "retdec/llvmir2hll/ir/var_def_stmt.h"
#include "retdec/llvmir2hll/ir/variable.h"
#include "retdec/llvmir2hll/ir/while_loop_stmt.h"
#include "retdec/llvmir2hll/support/debug.h"
#include "retdec/llvmir2hll/support/smart_ptr.h"
#include "retdec/llvmir2hll/support/statements_counter.h"

namespace retdec {
namespace llvmir2hll {

/**
* @brief Constructs a new statements counter.
*/
StatementsCounter::StatementsCounter(): OrderedAllVisitor(),
	numOfStmts(0), recursive(true), includeEmptyStmts(false) {}

/**
* @brief Returns the number of statements in @a block.
*
* @param[in] block Sequence of statements (possibly empty).
* @param[in] recursive Visit also nested statements (in compound statements)?
* @param[in] includeEmptyStmts Count also empty statements?
*/
unsigned StatementsCounter::count(ShPtr<Statement> block, bool recursive,
		bool includeEmptyStmts) {
	ShPtr<StatementsCounter> counter(new StatementsCounter());
	return counter->countInternal(block, recursive, includeEmptyStmts);
}

/**
* @brief Internal implementation of count().
*
* See the description of count() for more info.
*/
unsigned StatementsCounter::countInternal(ShPtr<Statement> block, bool recursive_,
		bool includeEmptyStmts_) {
	if (!block) {
		return 0;
	}

	numOfStmts = 0;
	recursive = recursive_;
	includeEmptyStmts = includeEmptyStmts_;
	OrderedAllVisitor::visitStmt(block);
	return numOfStmts;
}

void StatementsCounter::visit(const ShPtr<GlobalVarDef>& varDef) {
	FAIL("this function should never be called");
}

void StatementsCounter::visit(const ShPtr<Function>& func) {
	FAIL("this function should never be called");
}

void StatementsCounter::visit(const ShPtr<Variable>& var) {}

void StatementsCounter::visit(const ShPtr<AddressOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<AssignOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<ArrayIndexOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<StructIndexOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<DerefOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<NotOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<NegOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<EqOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<NeqOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<LtEqOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<GtEqOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<LtOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<GtOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<AddOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<SubOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<MulOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<ModOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<DivOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<AndOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<OrOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<BitAndOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<BitOrOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<BitXorOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<BitShlOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<BitShrOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<TernaryOpExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<CallExpr>& expr) {}

void StatementsCounter::visit(const ShPtr<CommaOpExpr>& expr) {}

// Casts.
void StatementsCounter::visit(const ShPtr<BitCastExpr>& expr) { }

void StatementsCounter::visit(const ShPtr<ExtCastExpr>& expr) { }

void StatementsCounter::visit(const ShPtr<TruncCastExpr>& expr) { }

void StatementsCounter::visit(const ShPtr<FPToIntCastExpr>& expr) { }

void StatementsCounter::visit(const ShPtr<IntToFPCastExpr>& expr) { }

void StatementsCounter::visit(const ShPtr<IntToPtrCastExpr>& expr) { }

void StatementsCounter::visit(const ShPtr<PtrToIntCastExpr>& expr) { }
// End of casts

void StatementsCounter::visit(const ShPtr<ConstBool>& constant) {}

void StatementsCounter::visit(const ShPtr<ConstFloat>& constant) {}

void StatementsCounter::visit(const ShPtr<ConstInt>& constant) {}

void StatementsCounter::visit(const ShPtr<ConstNullPointer>& constant) {}

void StatementsCounter::visit(const ShPtr<ConstString>& constant) {}

void StatementsCounter::visit(const ShPtr<ConstArray>& constant) {}

void StatementsCounter::visit(const ShPtr<ConstStruct>& constant) {}

void StatementsCounter::visit(const ShPtr<ConstSymbol>& constant) {}

void StatementsCounter::visit(const ShPtr<AssignStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<VarDefStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<CallStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<ReturnStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<EmptyStmt>& stmt) {
	if (includeEmptyStmts) {
		numOfStmts++;
	}
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<IfStmt>& stmt) {
	numOfStmts++;

	if (recursive) {
		// For each clause...
		for (auto i = stmt->clause_begin(), e = stmt->clause_end(); i != e; ++i) {
			visitStmt(i->second);
		}
		visitStmt(stmt->getElseClause());
	}

	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<SwitchStmt>& stmt) {
	numOfStmts++;

	if (recursive) {
		// For each clause...
		for (auto i = stmt->clause_begin(), e = stmt->clause_end(); i != e; ++i) {
			visitStmt(i->second);
		}
	}

	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<WhileLoopStmt>& stmt) {
	numOfStmts++;

	if (recursive) {
		visitStmt(stmt->getBody());
	}

	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<ForLoopStmt>& stmt) {
	numOfStmts++;

	if (recursive) {
		visitStmt(stmt->getBody());
	}

	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<UForLoopStmt>& stmt) {
	numOfStmts++;

	if (recursive) {
		visitStmt(stmt->getBody());
	}

	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<BreakStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<ContinueStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<GotoStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<UnreachableStmt>& stmt) {
	numOfStmts++;
	visitStmt(stmt->getSuccessor());
}

void StatementsCounter::visit(const ShPtr<FloatType>& type) {}

void StatementsCounter::visit(const ShPtr<IntType>& type) {}

void StatementsCounter::visit(const ShPtr<PointerType>& type) {}

void StatementsCounter::visit(const ShPtr<StringType>& type) {}

void StatementsCounter::visit(const ShPtr<ArrayType>& type) {}

void StatementsCounter::visit(const ShPtr<StructType>& type) {}

void StatementsCounter::visit(const ShPtr<VoidType>& type) {}

void StatementsCounter::visit(const ShPtr<UnknownType>& type) {}

} // namespace llvmir2hll
} // namespace retdec
