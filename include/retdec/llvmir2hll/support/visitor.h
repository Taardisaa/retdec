/**
* @file include/retdec/llvmir2hll/support/visitor.h
* @brief A base class of all visitors.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_SUPPORT_VISITOR_H
#define RETDEC_LLVMIR2HLL_SUPPORT_VISITOR_H

#include "retdec/llvmir2hll/support/smart_ptr.h"

namespace retdec {
namespace llvmir2hll {

class AddOpExpr;
class AddressOpExpr;
class AndOpExpr;
class ArrayIndexOpExpr;
class ArrayType;
class AssignOpExpr;
class AssignStmt;
class BitAndOpExpr;
class BitCastExpr;
class BitOrOpExpr;
class BitShlOpExpr;
class BitShrOpExpr;
class BitXorOpExpr;
class BreakStmt;
class CallExpr;
class CallStmt;
class CommaOpExpr;
class ConstArray;
class ConstBool;
class ConstFloat;
class ConstInt;
class ConstNullPointer;
class ConstString;
class ConstStruct;
class ConstSymbol;
class ContinueStmt;
class DerefOpExpr;
class DivOpExpr;
class EmptyStmt;
class EqOpExpr;
class Expression;
class ExtCastExpr;
class FPToIntCastExpr;
class FloatConst;
class FloatType;
class ForLoopStmt;
class Function;
class FunctionType;
class GlobalVarDef;
class GotoStmt;
class GtEqOpExpr;
class GtOpExpr;
class IfStmt;
class IntToFPCastExpr;
class IntToPtrCastExpr;
class IntType;
class LtEqOpExpr;
class LtOpExpr;
class ModOpExpr;
class Module;
class MulOpExpr;
class NegOpExpr;
class NeqOpExpr;
class NotOpExpr;
class OrOpExpr;
class PointerType;
class PtrToIntCastExpr;
class ReturnStmt;
class Statement;
class StringType;
class StructIndexOpExpr;
class StructType;
class SubOpExpr;
class SwitchStmt;
class TernaryOpExpr;
class TruncCastExpr;
class UForLoopStmt;
class UnknownType;
class UnreachableStmt;
class VarDefStmt;
class Variable;
class VoidType;
class WhileLoopStmt;

/**
* @brief A base class of all visitors.
*
* This class implements the Visitor design pattern to circumvent the lack of
* double dispatch in C++.
*/
class Visitor {
public:
	virtual ~Visitor() = default;

	virtual void visit(const ShPtr<GlobalVarDef>& varDef) = 0;
	virtual void visit(const ShPtr<Function>& func) = 0;
	// Statements
	virtual void visit(const ShPtr<AssignStmt>& stmt) = 0;
	virtual void visit(const ShPtr<BreakStmt>& stmt) = 0;
	virtual void visit(const ShPtr<CallStmt>& stmt) = 0;
	virtual void visit(const ShPtr<ContinueStmt>& stmt) = 0;
	virtual void visit(const ShPtr<EmptyStmt>& stmt) = 0;
	virtual void visit(const ShPtr<ForLoopStmt>& stmt) = 0;
	virtual void visit(const ShPtr<UForLoopStmt>& stmt) = 0;
	virtual void visit(const ShPtr<GotoStmt>& stmt) = 0;
	virtual void visit(const ShPtr<IfStmt>& stmt) = 0;
	virtual void visit(const ShPtr<ReturnStmt>& stmt) = 0;
	virtual void visit(const ShPtr<SwitchStmt>& stmt) = 0;
	virtual void visit(const ShPtr<UnreachableStmt>& stmt) = 0;
	virtual void visit(const ShPtr<VarDefStmt>& stmt) = 0;
	virtual void visit(const ShPtr<WhileLoopStmt>& stmt) = 0;
	// Expressions
	virtual void visit(const ShPtr<AddOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<AddressOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<AndOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<ArrayIndexOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<AssignOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<BitAndOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<BitOrOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<BitShlOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<BitShrOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<BitXorOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<CallExpr>& expr) = 0;
	virtual void visit(const ShPtr<CommaOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<DerefOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<DivOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<EqOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<GtEqOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<GtOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<LtEqOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<LtOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<ModOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<MulOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<NegOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<NeqOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<NotOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<OrOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<StructIndexOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<SubOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<TernaryOpExpr>& expr) = 0;
	virtual void visit(const ShPtr<Variable>& var) = 0;
	// Casts
	virtual void visit(const ShPtr<BitCastExpr>& expr) = 0;
	virtual void visit(const ShPtr<ExtCastExpr>& expr) = 0;
	virtual void visit(const ShPtr<FPToIntCastExpr>& expr) = 0;
	virtual void visit(const ShPtr<IntToFPCastExpr>& expr) = 0;
	virtual void visit(const ShPtr<IntToPtrCastExpr>& expr) = 0;
	virtual void visit(const ShPtr<PtrToIntCastExpr>& expr) = 0;
	virtual void visit(const ShPtr<TruncCastExpr>& expr) = 0;
	// Constants
	virtual void visit(const ShPtr<ConstArray>& constant) = 0;
	virtual void visit(const ShPtr<ConstBool>& constant) = 0;
	virtual void visit(const ShPtr<ConstFloat>& constant) = 0;
	virtual void visit(const ShPtr<ConstInt>& constant) = 0;
	virtual void visit(const ShPtr<ConstNullPointer>& constant) = 0;
	virtual void visit(const ShPtr<ConstString>& constant) = 0;
	virtual void visit(const ShPtr<ConstStruct>& constant) = 0;
	virtual void visit(const ShPtr<ConstSymbol>& constant) = 0;
	// Types
	virtual void visit(const ShPtr<ArrayType>& type) = 0;
	virtual void visit(const ShPtr<FloatType>& type) = 0;
	virtual void visit(const ShPtr<IntType>& type) = 0;
	virtual void visit(const ShPtr<PointerType>& type) = 0;
	virtual void visit(const ShPtr<StringType>& type) = 0;
	virtual void visit(const ShPtr<StructType>& type) = 0;
	virtual void visit(const ShPtr<FunctionType>& type) = 0;
	virtual void visit(const ShPtr<VoidType>& type) = 0;
	virtual void visit(const ShPtr<UnknownType>& type) = 0;

protected:
	Visitor() = default;
};

} // namespace llvmir2hll
} // namespace retdec

#endif
