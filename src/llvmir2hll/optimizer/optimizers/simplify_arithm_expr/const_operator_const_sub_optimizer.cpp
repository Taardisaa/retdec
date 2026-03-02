/**
* @file src/llvmir2hll/optimizer/optimizers/simplify_arithm_expr/const_operator_const_sub_optimizer.cpp
* @brief Implementation of ConstOperatorConstSubOptimizer.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#include "retdec/llvmir2hll/evaluator/arithm_expr_evaluator.h"
#include "retdec/llvmir2hll/ir/add_op_expr.h"
#include "retdec/llvmir2hll/ir/and_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_and_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_or_op_expr.h"
#include "retdec/llvmir2hll/ir/bit_xor_op_expr.h"
#include "retdec/llvmir2hll/ir/const_float.h"
#include "retdec/llvmir2hll/ir/const_int.h"
#include "retdec/llvmir2hll/ir/div_op_expr.h"
#include "retdec/llvmir2hll/ir/eq_op_expr.h"
#include "retdec/llvmir2hll/ir/expression.h"
#include "retdec/llvmir2hll/ir/gt_eq_op_expr.h"
#include "retdec/llvmir2hll/ir/gt_op_expr.h"
#include "retdec/llvmir2hll/ir/lt_eq_op_expr.h"
#include "retdec/llvmir2hll/ir/lt_op_expr.h"
#include "retdec/llvmir2hll/ir/mul_op_expr.h"
#include "retdec/llvmir2hll/ir/neg_op_expr.h"
#include "retdec/llvmir2hll/ir/neq_op_expr.h"
#include "retdec/llvmir2hll/ir/or_op_expr.h"
#include "retdec/llvmir2hll/ir/sub_op_expr.h"
#include "retdec/llvmir2hll/ir/value.h"
#include "retdec/llvmir2hll/optimizer/optimizers/simplify_arithm_expr/const_operator_const_sub_optimizer.h"

namespace retdec {
namespace llvmir2hll {

REGISTER_AT_FACTORY("ConstOperatorConst", CONST_OPERATOR_CONST_SUB_OPTIMIZER_ID,
	SubOptimizerFactory, ConstOperatorConstSubOptimizer::create);

/**
* @brief Constructs the ConstOperatorConstSubOptimizer.
*
* @param[in] arithmExprEvaluator @a The used evaluator of arithmetical
*            expressions.
*/
ConstOperatorConstSubOptimizer::ConstOperatorConstSubOptimizer(
		ShPtr<ArithmExprEvaluator> arithmExprEvaluator):
			SubOptimizer(arithmExprEvaluator) {}

/**
* @brief Creates a new ConstOperatorConstSubOptimizer.
*
* @param[in] arithmExprEvaluator @a The used evaluator of arithmetical
*            expressions.
*/
ShPtr<SubOptimizer> ConstOperatorConstSubOptimizer::create(
		ShPtr<ArithmExprEvaluator> arithmExprEvaluator) {
	return std::make_shared<ConstOperatorConstSubOptimizer>(arithmExprEvaluator);
}

std::string ConstOperatorConstSubOptimizer::getId() const {
	return CONST_OPERATOR_CONST_SUB_OPTIMIZER_ID;
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<AddOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<SubOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<MulOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<DivOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<BitAndOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<BitOrOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<BitXorOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<LtOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<LtEqOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<GtOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<GtEqOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<EqOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	// This is resolved in EqualOperandsSubOptimizer.
	// This case has some special conditions which are resolved there.
	if (expr->getFirstOperand()->isEqualTo(expr->getSecondOperand())) {
		return;
	}

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<NeqOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	// This is resolved in EqualOperandsSubOptimizer.
	// This case has some special conditions which are resolved there.
	if (expr->getFirstOperand()->isEqualTo(expr->getSecondOperand())) {
		return;
	}

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<AndOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

void ConstOperatorConstSubOptimizer::visit(const ShPtr<OrOpExpr>& expr) {
	OrderedAllVisitor::visit(expr);

	tryOptimizeConstConstOperand(expr);
}

/**
* @brief Try to optimize expression like ConstInt/ConstFloat operator
*        ConstInt/ConstFloat
*
* @param[in] expr A @c BinaryOpExpr to optimize.
*/
void ConstOperatorConstSubOptimizer::tryOptimizeConstConstOperand(
		ShPtr<BinaryOpExpr> expr) {
	// Try to evaluate expression.
	ShPtr<Constant> result(arithmExprEvaluator->evaluate(expr));
	if (result) {
		// Expression was evaluated, so optimize it to this new expression.
		optimizeExpr(expr, result);
	}
}

} // namespace llvmir2hll
} // namespace retdec
