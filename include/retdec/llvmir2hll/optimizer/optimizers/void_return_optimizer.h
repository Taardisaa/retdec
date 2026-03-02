/**
* @file include/retdec/llvmir2hll/optimizer/optimizers/void_return_optimizer.h
* @brief Optimizes redundant void returns.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_OPTIMIZER_OPTIMIZERS_VOID_RETURN_OPTIMIZER_H
#define RETDEC_LLVMIR2HLL_OPTIMIZER_OPTIMIZERS_VOID_RETURN_OPTIMIZER_H

#include "retdec/llvmir2hll/optimizer/func_optimizer.h"
#include "retdec/llvmir2hll/support/smart_ptr.h"

namespace retdec {
namespace llvmir2hll {

/**
* @brief Optimizes redundant void returns.
*
* This optimizer removes redundant void returns. For example,
* @code
* def func():
*     a = 1
*     return
* @endcode
* can be optimized into
* @code
* def func():
*     a = 1
* @endcode
*
* Instances of this class have reference object semantics.
*
* This is a concrete optimizer which should not be subclassed.
*/
class VoidReturnOptimizer final: public FuncOptimizer {
public:
	VoidReturnOptimizer(ShPtr<Module> module);

	virtual std::string getId() const override { return "VoidReturn"; }

private:
	/// @name Visitor Interface
	/// @{
	using OrderedAllVisitor::visit;
	virtual void visit(const ShPtr<AssignStmt>& stmt) override;
	virtual void visit(const ShPtr<VarDefStmt>& stmt) override;
	virtual void visit(const ShPtr<CallStmt>& stmt) override;
	virtual void visit(const ShPtr<ReturnStmt>& stmt) override;
	virtual void visit(const ShPtr<IfStmt>& stmt) override;
	virtual void visit(const ShPtr<SwitchStmt>& stmt) override;
	virtual void visit(const ShPtr<WhileLoopStmt>& stmt) override;
	virtual void visit(const ShPtr<ForLoopStmt>& stmt) override;
	virtual void visit(const ShPtr<UForLoopStmt>& stmt) override;
	/// @}

private:
	/// Counter of the current level of nesting.
	int nestingLevel;
};

} // namespace llvmir2hll
} // namespace retdec

#endif
