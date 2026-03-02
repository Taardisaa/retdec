/**
* @file src/llvmir2hll/optimizer/optimizers/self_assign_optimizer.cpp
* @brief Implementation of SelfAssignOptimizer.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#include "retdec/llvmir2hll/ir/assign_stmt.h"
#include "retdec/llvmir2hll/ir/expression.h"
#include "retdec/llvmir2hll/optimizer/optimizers/self_assign_optimizer.h"
#include "retdec/llvmir2hll/support/debug.h"

namespace retdec {
namespace llvmir2hll {

/**
* @brief Constructs a new optimizer.
*
* @param[in] module Module to be optimized.
*
* @par Preconditions
*  - @a module is non-null
*/
SelfAssignOptimizer::SelfAssignOptimizer(ShPtr<Module> module):
	FuncOptimizer(module) {
		PRECONDITION_NON_NULL(module);
	}

void SelfAssignOptimizer::visit(const ShPtr<AssignStmt>& stmt) {
	// Save successor before potential removal.
	ShPtr<Statement> stmtSucc(stmt->getSuccessor());

	if (stmt->getLhs()->isEqualTo(stmt->getRhs())) {
		Statement::removeStatementButKeepDebugComment(stmt);
	}

	if (stmtSucc) {
		nextStmtToVisit = stmtSucc;
	}
}

} // namespace llvmir2hll
} // namespace retdec
