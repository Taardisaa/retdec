/**
* @file src/llvmir2hll/support/expression_negater.cpp
* @brief Implementation of ExpressionNegater.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#include "retdec/llvmir2hll/ir/add_op_expr.h"
#include "retdec/llvmir2hll/ir/address_op_expr.h"
#include "retdec/llvmir2hll/ir/and_op_expr.h"
#include "retdec/llvmir2hll/ir/array_index_op_expr.h"
#include "retdec/llvmir2hll/ir/assign_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_and_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_cast_expr.h"
#include "retdec/llvmir2hll/ir/bit_or_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_shl_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_shr_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_xor_op_expr.h"
#include "retdec/llvmir2hll/ir/call_expr.h"
#include "retdec/llvmir2hll/ir/comma_op_expr.h"
#include "retdec/llvmir2hll/ir/const_array.h"
#include "retdec/llvmir2hll/ir/const_bool.h"
#include "retdec/llvmir2hll/ir/const_float.h"
#include "retdec/llvmir2hll/ir/const_int.h"
#include "retdec/llvmir2hll/ir/const_null_pointer.h"
#include "retdec/llvmir2hll/ir/const_string.h"
#include "retdec/llvmir2hll/ir/const_struct.h"
#include "retdec/llvmir2hll/ir/const_symbol.h"
#include "retdec/llvmir2hll/ir/deref_op_expr.h"
#include "retdec/llvmir2hll/ir/div_op_expr.h"
#include "retdec/llvmir2hll/ir/eq_op_expr.h"
#include "retdec/llvmir2hll/ir/ext_cast_expr.h"
#include "retdec/llvmir2hll/ir/fp_to_int_cast_expr.h"
#include "retdec/llvmir2hll/ir/gt_eq_op_expr.h"
#include "retdec/llvmir2hll/ir/gt_op_expr.h"
#include "retdec/llvmir2hll/ir/int_to_fp_cast_expr.h"
#include "retdec/llvmir2hll/ir/int_to_ptr_cast_expr.h"
#include "retdec/llvmir2hll/ir/lt_eq_op_expr.h"
#include "retdec/llvmir2hll/ir/lt_op_expr.h"
#include "retdec/llvmir2hll/ir/mod_op_expr.h"
#include "retdec/llvmir2hll/ir/mul_op_expr.h"
#include "retdec/llvmir2hll/ir/neg_op_expr.h"
#include "retdec/llvmir2hll/ir/neq_op_expr.h"
#include "retdec/llvmir2hll/ir/not_op_expr.h"
#include "retdec/llvmir2hll/ir/or_op_expr.h"
#include "retdec/llvmir2hll/ir/ptr_to_int_cast_expr.h"
#include "retdec/llvmir2hll/ir/struct_index_op_expr.h"
#include "retdec/llvmir2hll/ir/sub_op_expr.h"
#include "retdec/llvmir2hll/ir/ternary_op_expr.h"
#include "retdec/llvmir2hll/ir/trunc_cast_expr.h"
#include "retdec/llvmir2hll/ir/variable.h"
#include "retdec/llvmir2hll/support/debug.h"
#include "retdec/llvmir2hll/support/expression_negater.h"

namespace retdec {
namespace llvmir2hll {

/**
* @brief Constructs a new expression negater.
*/
ExpressionNegater::ExpressionNegater(): Visitor() {}

/**
* @brief Negates the given expression.
*
* See the class description for more details.
*
* @par Preconditions
*  - @a expr is non-null
*/
ShPtr<Expression> ExpressionNegater::negate(ShPtr<Expression> expr) {
	PRECONDITION_NON_NULL(expr);

	ShPtr<ExpressionNegater> negater(new ExpressionNegater());
	return negater->negateInternal(expr);
}

/**
* @brief Negates the given expression.
*
* Since visitation functions return void, we use the private variable @c
* exprStack to manually simulate recursion. Hence, instead of returning a value
* from a visitation function, we push it onto the stack.
*
* @par Preconditions
*  - @a expr is non-null
*/
ShPtr<Expression> ExpressionNegater::negateInternal(ShPtr<Expression> expr) {
	PRECONDITION_NON_NULL(expr);

	expr->accept(this);
	return exprStack.top();
}

void ExpressionNegater::visit(const ShPtr<Variable>& var) {
	exprStack.push(NotOpExpr::create(var));
}

void ExpressionNegater::visit(const ShPtr<AddressOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<AssignOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<ArrayIndexOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<StructIndexOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<DerefOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<NotOpExpr>& expr) {
	// not not expr -> expr
	// We have to clone the operand to prevent errors later.
	exprStack.push(ucast<Expression>(expr->getOperand()->clone()));
}

void ExpressionNegater::visit(const ShPtr<NegOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<EqOpExpr>& expr) {
	exprStack.push(NeqOpExpr::create(expr->getFirstOperand(), expr->getSecondOperand()));
}

void ExpressionNegater::visit(const ShPtr<NeqOpExpr>& expr) {
	exprStack.push(EqOpExpr::create(expr->getFirstOperand(), expr->getSecondOperand()));
}

void ExpressionNegater::visit(const ShPtr<LtEqOpExpr>& expr) {
	exprStack.push(GtOpExpr::create(expr->getFirstOperand(), expr->getSecondOperand()));
}

void ExpressionNegater::visit(const ShPtr<GtEqOpExpr>& expr) {
	exprStack.push(LtOpExpr::create(expr->getFirstOperand(), expr->getSecondOperand()));
}

void ExpressionNegater::visit(const ShPtr<LtOpExpr>& expr) {
	exprStack.push(GtEqOpExpr::create(expr->getFirstOperand(), expr->getSecondOperand()));
}

void ExpressionNegater::visit(const ShPtr<GtOpExpr>& expr) {
	exprStack.push(LtEqOpExpr::create(expr->getFirstOperand(), expr->getSecondOperand()));
}

void ExpressionNegater::visit(const ShPtr<AddOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<SubOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<MulOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<ModOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<DivOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<AndOpExpr>& expr) {
	// Use De-Morgan laws.
	expr->getFirstOperand()->accept(this);
	ShPtr<Expression> firstOperandNegated(exprStack.top());
	exprStack.pop();

	expr->getSecondOperand()->accept(this);
	ShPtr<Expression> secondOperandNegated(exprStack.top());
	exprStack.pop();

	exprStack.push(OrOpExpr::create(firstOperandNegated, secondOperandNegated));
}

void ExpressionNegater::visit(const ShPtr<OrOpExpr>& expr) {
	// Use De-Morgan laws.
	expr->getFirstOperand()->accept(this);
	ShPtr<Expression> firstOperandNegated(exprStack.top());
	exprStack.pop();

	expr->getSecondOperand()->accept(this);
	ShPtr<Expression> secondOperandNegated(exprStack.top());
	exprStack.pop();

	exprStack.push(AndOpExpr::create(firstOperandNegated, secondOperandNegated));
}

void ExpressionNegater::visit(const ShPtr<BitAndOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<BitOrOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<BitXorOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<BitShlOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<BitShrOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<TernaryOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<CallExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<CommaOpExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

// Casts.
void ExpressionNegater::visit(const ShPtr<BitCastExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<ExtCastExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<TruncCastExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<FPToIntCastExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<IntToFPCastExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<IntToPtrCastExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}

void ExpressionNegater::visit(const ShPtr<PtrToIntCastExpr>& expr) {
	exprStack.push(NotOpExpr::create(expr));
}
// End of casts.

void ExpressionNegater::visit(const ShPtr<ConstBool>& constant) {
	// true -> false, false -> true
	exprStack.push(ConstBool::create(!constant->getValue()));
}

void ExpressionNegater::visit(const ShPtr<ConstFloat>& constant) {
	exprStack.push(NotOpExpr::create(constant));
}

void ExpressionNegater::visit(const ShPtr<ConstInt>& constant) {
	exprStack.push(NotOpExpr::create(constant));
}

void ExpressionNegater::visit(const ShPtr<ConstNullPointer>& constant) {
	exprStack.push(NotOpExpr::create(constant));
}

void ExpressionNegater::visit(const ShPtr<ConstString>& constant) {
	exprStack.push(NotOpExpr::create(constant));
}

void ExpressionNegater::visit(const ShPtr<ConstArray>& constant) {
	exprStack.push(NotOpExpr::create(constant));
}

void ExpressionNegater::visit(const ShPtr<ConstStruct>& constant) {
	exprStack.push(NotOpExpr::create(constant));
}

void ExpressionNegater::visit(const ShPtr<ConstSymbol>& constant) {
	exprStack.push(NotOpExpr::create(constant));
}

void ExpressionNegater::visit(const ShPtr<GlobalVarDef>& varDef) {
	FAIL("you cannot negate a global variable definition");
}

void ExpressionNegater::visit(const ShPtr<Function>& func) {
	FAIL("you cannot negate a function");
}

void ExpressionNegater::visit(const ShPtr<AssignStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<VarDefStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<CallStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<ReturnStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<EmptyStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<IfStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<SwitchStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<WhileLoopStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<ForLoopStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<UForLoopStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<BreakStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<ContinueStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<GotoStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<UnreachableStmt>& stmt) {
	FAIL("you cannot negate a statement");
}

void ExpressionNegater::visit(const ShPtr<FloatType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<IntType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<PointerType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<StringType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<ArrayType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<StructType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<FunctionType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<VoidType>& type) {
	FAIL("you cannot negate a type");
}

void ExpressionNegater::visit(const ShPtr<UnknownType>& type) {
	FAIL("you cannot negate a type");
}

} // namespace llvmir2hll
} // namespace retdec
