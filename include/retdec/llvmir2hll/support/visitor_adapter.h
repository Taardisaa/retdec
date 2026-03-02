/**
* @file include/retdec/llvmir2hll/support/visitor_adapter.h
* @brief A visitor whose visit methods do nothing.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_SUPPORT_VISITOR_ADAPTER_H
#define RETDEC_LLVMIR2HLL_SUPPORT_VISITOR_ADAPTER_H

#include "retdec/llvmir2hll/support/smart_ptr.h"
#include "retdec/llvmir2hll/support/visitor.h"

namespace retdec {
namespace llvmir2hll {

/**
* @brief A visitor whose visit methods do nothing by default.
*
* Concrete visitors should:
*  - subclass this class
*  - override the needed functions from the Visitor base class (remember that
*    non-overridden functions have to brought to scope using the <tt>using
*    VisitorAdapter::visit;</tt> declaration; otherwise, they'll be hidden by
*    the overridden ones)
*
* Use this class as a base class when you want the Visitor functionality, but
* you don't want to provide the default implementation for visit functions that
* you don't use.
*/
class VisitorAdapter: public Visitor {
public:
	/// @name Visitor Interface
	/// @{
	virtual void visit(const ShPtr<GlobalVarDef>& varDef) override {}
	virtual void visit(const ShPtr<Function>& func) override {}
	// Statements
	virtual void visit(const ShPtr<AssignStmt>& stmt) override {}
	virtual void visit(const ShPtr<BreakStmt>& stmt) override {}
	virtual void visit(const ShPtr<CallStmt>& stmt) override {}
	virtual void visit(const ShPtr<ContinueStmt>& stmt) override {}
	virtual void visit(const ShPtr<EmptyStmt>& stmt) override {}
	virtual void visit(const ShPtr<ForLoopStmt>& stmt) override {}
	virtual void visit(const ShPtr<UForLoopStmt>& stmt) override {}
	virtual void visit(const ShPtr<GotoStmt>& stmt) override {}
	virtual void visit(const ShPtr<IfStmt>& stmt) override {}
	virtual void visit(const ShPtr<ReturnStmt>& stmt) override {}
	virtual void visit(const ShPtr<SwitchStmt>& stmt) override {}
	virtual void visit(const ShPtr<UnreachableStmt>& stmt) override {}
	virtual void visit(const ShPtr<VarDefStmt>& stmt) override {}
	virtual void visit(const ShPtr<WhileLoopStmt>& stmt) override {}
	// Expressions
	virtual void visit(const ShPtr<AddOpExpr>& expr) override {}
	virtual void visit(const ShPtr<AddressOpExpr>& expr) override {}
	virtual void visit(const ShPtr<AssignOpExpr>& expr) override {}
	virtual void visit(const ShPtr<AndOpExpr>& expr) override {}
	virtual void visit(const ShPtr<ArrayIndexOpExpr>& expr) override {}
	virtual void visit(const ShPtr<BitAndOpExpr>& expr) override {}
	virtual void visit(const ShPtr<BitOrOpExpr>& expr) override {}
	virtual void visit(const ShPtr<BitShlOpExpr>& expr) override {}
	virtual void visit(const ShPtr<BitShrOpExpr>& expr) override {}
	virtual void visit(const ShPtr<BitXorOpExpr>& expr) override {}
	virtual void visit(const ShPtr<CallExpr>& expr) override {}
	virtual void visit(const ShPtr<CommaOpExpr>& expr) override {}
	virtual void visit(const ShPtr<DerefOpExpr>& expr) override {}
	virtual void visit(const ShPtr<DivOpExpr>& expr) override {}
	virtual void visit(const ShPtr<EqOpExpr>& expr) override {}
	virtual void visit(const ShPtr<GtEqOpExpr>& expr) override {}
	virtual void visit(const ShPtr<GtOpExpr>& expr) override {}
	virtual void visit(const ShPtr<LtEqOpExpr>& expr) override {}
	virtual void visit(const ShPtr<LtOpExpr>& expr) override {}
	virtual void visit(const ShPtr<ModOpExpr>& expr) override {}
	virtual void visit(const ShPtr<MulOpExpr>& expr) override {}
	virtual void visit(const ShPtr<NegOpExpr>& expr) override {}
	virtual void visit(const ShPtr<NeqOpExpr>& expr) override {}
	virtual void visit(const ShPtr<NotOpExpr>& expr) override {}
	virtual void visit(const ShPtr<OrOpExpr>& expr) override {}
	virtual void visit(const ShPtr<StructIndexOpExpr>& expr) override {}
	virtual void visit(const ShPtr<SubOpExpr>& expr) override {}
	virtual void visit(const ShPtr<TernaryOpExpr>& expr) override {}
	virtual void visit(const ShPtr<Variable>& var) override {}
	// Casts
	virtual void visit(const ShPtr<BitCastExpr>& expr) override {}
	virtual void visit(const ShPtr<ExtCastExpr>& expr) override {}
	virtual void visit(const ShPtr<FPToIntCastExpr>& expr) override {}
	virtual void visit(const ShPtr<IntToFPCastExpr>& expr) override {}
	virtual void visit(const ShPtr<IntToPtrCastExpr>& expr) override {}
	virtual void visit(const ShPtr<PtrToIntCastExpr>& expr) override {}
	virtual void visit(const ShPtr<TruncCastExpr>& expr) override {}
	// Constants
	virtual void visit(const ShPtr<ConstArray>& constant) override {}
	virtual void visit(const ShPtr<ConstBool>& constant) override {}
	virtual void visit(const ShPtr<ConstFloat>& constant) override {}
	virtual void visit(const ShPtr<ConstInt>& constant) override {}
	virtual void visit(const ShPtr<ConstNullPointer>& constant) override {}
	virtual void visit(const ShPtr<ConstString>& constant) override {}
	virtual void visit(const ShPtr<ConstStruct>& constant) override {}
	virtual void visit(const ShPtr<ConstSymbol>& constant) override {}
	// Types
	virtual void visit(const ShPtr<ArrayType>& type) override {}
	virtual void visit(const ShPtr<FloatType>& type) override {}
	virtual void visit(const ShPtr<IntType>& type) override {}
	virtual void visit(const ShPtr<PointerType>& type) override {}
	virtual void visit(const ShPtr<StringType>& type) override {}
	virtual void visit(const ShPtr<StructType>& type) override {}
	virtual void visit(const ShPtr<FunctionType>& type) override {}
	virtual void visit(const ShPtr<VoidType>& type) override {}
	virtual void visit(const ShPtr<UnknownType>& type) override {}
	/// @}
};

} // namespace llvmir2hll
} // namespace retdec

#endif
