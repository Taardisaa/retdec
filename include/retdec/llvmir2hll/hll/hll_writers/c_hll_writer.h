/**
* @file include/retdec/llvmir2hll/hll/hll_writers/c_hll_writer.h
* @brief Implementation of the HLL writer for the C language.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_HLL_HLL_WRITERS_C_HLL_WRITER_H
#define RETDEC_LLVMIR2HLL_HLL_HLL_WRITERS_C_HLL_WRITER_H

#include <cstddef>
#include <map>
#include <string>

#include "retdec/llvmir2hll/hll/hll_writer.h"
#include "retdec/llvmir2hll/support/smart_ptr.h"

namespace retdec {
namespace llvmir2hll {

class BracketManager;
class CastExpr;
class CompoundOpManager;
class OutputManager;
class StructType;

/**
* @brief A HLL writer for the C language (C99).
*
* Use create() to create instances. Instances of this class have
* reference object semantics.
*/
class CHLLWriter: public HLLWriter {
public:
	static ShPtr<HLLWriter> create(
		llvm::raw_ostream &out,
		const std::string& outputFormat = "");

	virtual std::string getId() const override;

	/// @name Options
	/// @{
	void setOptionEmitFunctionPrototypesForNonLibraryFuncs(bool emit = true);
	/// @}

private:
	/// Mapping of a structured type into its name.
	using StructTypeNameMap = std::map<ShPtr<StructType>, std::string>;

private:
	CHLLWriter(
		llvm::raw_ostream &out,
		const std::string& outputFormat = "");

	virtual std::string getCommentPrefix() override;
	virtual bool emitFileHeader() override;
	virtual bool emitGlobalVariables() override;
	virtual bool emitFunctionPrototypesHeader() override;
	virtual bool emitFunctionPrototypes() override;
	virtual bool emitExternalFunction(ShPtr<Function> func) override;
	virtual bool emitTargetCode(ShPtr<Module> module) override;

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
	virtual void visit(const ShPtr<FunctionType>& type) override;
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

	bool shouldEmitFunctionPrototypesHeader() const;
	bool emitFunctionPrototypes(const FuncSet &funcs);
	bool emitStandardFunctionPrototypes();
	bool emitFunctionPrototypesForNonLibraryFuncs();
	bool emitFunctionPrototype(ShPtr<Function> func);
	void emitFunctionDefinition(ShPtr<Function> func);
	void emitFunctionHeader(ShPtr<Function> func);
	void emitHeaderOfFuncReturningPointerToFunc(ShPtr<Function> func);
	void emitHeaderOfFuncReturningPointerToArray(ShPtr<Function> func);
	void emitFunctionParameters(ShPtr<Function> func);
	void emitVarWithType(ShPtr<Variable> var);
	void emitPointerToFunc(ShPtr<PointerType> pointerToFuncType,
		ShPtr<Variable> var = nullptr);
	void emitArrayOfFuncPointers(ShPtr<ArrayType> arrayType,
		ShPtr<Variable> var = nullptr);
	void emitPointerToArray(ShPtr<PointerType> pointerToArrayType,
		ShPtr<Variable> var = nullptr);
	void emitArrayDimensions(ShPtr<ArrayType> arrayType);
	void emitArrayDimension(std::size_t size);
	void emitInitializedConstArray(ShPtr<ConstArray> array);
	void emitInitializedConstArrayInline(ShPtr<ConstArray> array);
	void emitInitializedConstArrayInStructuredWay(ShPtr<ConstArray> array);
	void emitUninitializedConstArray(ShPtr<ConstArray> array);
	void emitTypeOfElementsInArray(ShPtr<ArrayType> arrayType);
	void emitCastInStandardWay(ShPtr<CastExpr> expr);
	void emitStarsBeforePointedValue(ShPtr<PointerType> ptrType);
	void emitFunctionParameters(ShPtr<FunctionType> funcType);
	void emitReturnType(ShPtr<FunctionType> funcType);
	void emitNameOfVarIfExists(ShPtr<Variable> var);
	void emitAssignment(ShPtr<Expression> lhs, ShPtr<Expression> rhs);
	void emitInitVarDefWhenNeeded(ShPtr<UForLoopStmt> loop);
	void emitConstStruct(ShPtr<ConstStruct> constant, bool emitCast = true);
	void emitStructDeclaration(ShPtr<StructType> structType,
		bool emitInline = false);
	void emitBlock(ShPtr<Statement> stmt);
	void emitGlobalDirectives(ShPtr<Function> func);
	void emitDebugComment(std::string comment, bool indent = true);
	void emitGotoLabelIfNeeded(ShPtr<Statement> stmt);

	std::string getConstFloatSuffixIfNeeded(ShPtr<ConstFloat> constant);
	std::string genNameForUnnamedStruct(const StructTypeVector &usedStructTypes);

private:
	/// Optimizes AssignStmt to compound operator when possible.
	ShPtr<CompoundOpManager> compoundOpManager;

	/// Mapping of a structured type into its name.
	StructTypeNameMap structNames;

	/// A counter for unnamed structures.
	/// It is needed for assigning names to unnamed structures.
	std::size_t unnamedStructCounter;

	/// Are we emitting global variables?
	/// This variable is needed because we cannot emit casts of structures
	/// inside the initialization of global variables. See emitConstStruct()
	/// for more details.
	bool emittingGlobalVarDefs;

	/// Emit prototypes for functions which do not have any associated header
	/// file?
	bool optionEmitFunctionPrototypesForNonLibraryFuncs;
};

} // namespace llvmir2hll
} // namespace retdec

#endif
