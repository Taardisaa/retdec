/**
* @file include/retdec/llvmir2hll/support/statements_counter.h
* @brief Counter of the number of statements in a block.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_SUPPORT_STATEMENTS_COUNTER_H
#define RETDEC_LLVMIR2HLL_SUPPORT_STATEMENTS_COUNTER_H

#include "retdec/llvmir2hll/support/smart_ptr.h"
#include "retdec/llvmir2hll/support/visitors/ordered_all_visitor.h"
#include "retdec/utils/non_copyable.h"

namespace retdec {
namespace llvmir2hll {

/**
* @brief Counter of the number of statements in a block.
*
* This class implements the "static helper" (or "library") design pattern (it
* has just static functions and no instances can be created).
*/
class StatementsCounter: private OrderedAllVisitor,
		private retdec::utils::NonCopyable {
public:
	static unsigned count(ShPtr<Statement> block, bool recursive = true,
		bool includeEmptyStmts = false);

private:
	StatementsCounter();

	unsigned countInternal(ShPtr<Statement> block, bool recursive,
		bool includeEmptyStmts);

	/// @name Visitor Interface
	/// @{
	virtual void visit(const ShPtr<GlobalVarDef>& varDef) override;
	virtual void visit(const ShPtr<Function>& func) override;
	virtual void visit(const ShPtr<Variable>& var) override;
	virtual void visit(const ShPtr<AddressOpExpr>& expr) override;
	virtual void visit(const ShPtr<AssignOpExpr>& expr) override;
	virtual void visit(const ShPtr<ArrayIndexOpExpr>& expr) override;
	virtual void visit(const ShPtr<StructIndexOpExpr>& expr) override;
	virtual void visit(const ShPtr<DerefOpExpr>& expr) override;
	virtual void visit(const ShPtr<NotOpExpr>& expr) override;
	virtual void visit(const ShPtr<NegOpExpr>& expr) override;
	virtual void visit(const ShPtr<EqOpExpr>& expr) override;
	virtual void visit(const ShPtr<NeqOpExpr>& expr) override;
	virtual void visit(const ShPtr<LtEqOpExpr>& expr) override;
	virtual void visit(const ShPtr<GtEqOpExpr>& expr) override;
	virtual void visit(const ShPtr<LtOpExpr>& expr) override;
	virtual void visit(const ShPtr<GtOpExpr>& expr) override;
	virtual void visit(const ShPtr<AddOpExpr>& expr) override;
	virtual void visit(const ShPtr<SubOpExpr>& expr) override;
	virtual void visit(const ShPtr<MulOpExpr>& expr) override;
	virtual void visit(const ShPtr<ModOpExpr>& expr) override;
	virtual void visit(const ShPtr<DivOpExpr>& expr) override;
	virtual void visit(const ShPtr<AndOpExpr>& expr) override;
	virtual void visit(const ShPtr<OrOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitAndOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitOrOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitXorOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitShlOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitShrOpExpr>& expr) override;
	virtual void visit(const ShPtr<TernaryOpExpr>& expr) override;
	virtual void visit(const ShPtr<CallExpr>& expr) override;
	virtual void visit(const ShPtr<CommaOpExpr>& expr) override;
	virtual void visit(const ShPtr<ConstBool>& constant) override;
	virtual void visit(const ShPtr<ConstFloat>& constant) override;
	virtual void visit(const ShPtr<ConstInt>& constant) override;
	virtual void visit(const ShPtr<ConstNullPointer>& constant) override;
	virtual void visit(const ShPtr<ConstString>& constant) override;
	virtual void visit(const ShPtr<ConstArray>& constant) override;
	virtual void visit(const ShPtr<ConstStruct>& constant) override;
	virtual void visit(const ShPtr<ConstSymbol>& constant) override;
	virtual void visit(const ShPtr<AssignStmt>& stmt) override;
	virtual void visit(const ShPtr<VarDefStmt>& stmt) override;
	virtual void visit(const ShPtr<CallStmt>& stmt) override;
	virtual void visit(const ShPtr<ReturnStmt>& stmt) override;
	virtual void visit(const ShPtr<EmptyStmt>& stmt) override;
	virtual void visit(const ShPtr<IfStmt>& stmt) override;
	virtual void visit(const ShPtr<SwitchStmt>& stmt) override;
	virtual void visit(const ShPtr<WhileLoopStmt>& stmt) override;
	virtual void visit(const ShPtr<ForLoopStmt>& stmt) override;
	virtual void visit(const ShPtr<UForLoopStmt>& stmt) override;
	virtual void visit(const ShPtr<BreakStmt>& stmt) override;
	virtual void visit(const ShPtr<ContinueStmt>& stmt) override;
	virtual void visit(const ShPtr<GotoStmt>& stmt) override;
	virtual void visit(const ShPtr<UnreachableStmt>& stmt) override;
	// Types
	virtual void visit(const ShPtr<FloatType>& type) override;
	virtual void visit(const ShPtr<IntType>& type) override;
	virtual void visit(const ShPtr<PointerType>& type) override;
	virtual void visit(const ShPtr<StringType>& type) override;
	virtual void visit(const ShPtr<ArrayType>& type) override;
	virtual void visit(const ShPtr<StructType>& type) override;
	virtual void visit(const ShPtr<VoidType>& type) override;
	virtual void visit(const ShPtr<UnknownType>& type) override;
	// Casts
	virtual void visit(const ShPtr<BitCastExpr>& expr) override;
	virtual void visit(const ShPtr<ExtCastExpr>& expr) override;
	virtual void visit(const ShPtr<TruncCastExpr>& expr) override;
	virtual void visit(const ShPtr<FPToIntCastExpr>& expr) override;
	virtual void visit(const ShPtr<IntToFPCastExpr>& expr) override;
	virtual void visit(const ShPtr<IntToPtrCastExpr>& expr) override;
	virtual void visit(const ShPtr<PtrToIntCastExpr>& expr) override;
	/// @}

private:
	/// Counter of the number of statements.
	unsigned numOfStmts;

	/// Should we also visit nested statements (in compound statements)?
	bool recursive;

	/// Should we also count empty statements?
	bool includeEmptyStmts;
};

} // namespace llvmir2hll
} // namespace retdec

#endif
