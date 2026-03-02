/**
* @file include/retdec/llvmir2hll/analysis/value_analysis.h
* @brief An analysis for obtaining information about a value.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_LLVMIR2HLL_ANALYSIS_VALUE_ANALYSIS_H
#define RETDEC_LLVMIR2HLL_ANALYSIS_VALUE_ANALYSIS_H

#include <cstddef>
#include <map>

#include "retdec/llvmir2hll/support/caching.h"
#include "retdec/llvmir2hll/support/smart_ptr.h"
#include "retdec/llvmir2hll/support/types.h"
#include "retdec/llvmir2hll/support/valid_state.h"
#include "retdec/llvmir2hll/support/visitors/ordered_all_visitor.h"
#include "retdec/utils/non_copyable.h"

namespace retdec {
namespace llvmir2hll {

class AliasAnalysis;
class Value;

/**
* @brief Information about a value.
*
* Instance of this class can be created only by using ValueAnalysis.
*
* Instances of this class have value object semantics.
*/
class ValueData {
	friend class ValueAnalysis;

public:
	/// Variables iterator.
	using var_iterator = VarSet::const_iterator;

	/// Calls iterator.
	using call_iterator = CallVector::const_iterator;

public:
	ValueData(const ValueData &other) = default;

	ValueData &operator=(const ValueData &other) = default;
	bool operator==(const ValueData &other) const;
	bool operator!=(const ValueData &other) const;

	/// @name Directly Used Variables Accessors
	/// @{
	const VarSet &getDirReadVars() const;
	const VarSet &getDirWrittenVars() const;
	const VarSet &getDirAccessedVars() const;
	std::size_t getNumOfDirReadVars() const;
	std::size_t getNumOfDirWrittenVars() const;
	std::size_t getNumOfDirAccessedVars() const;
	std::size_t getDirNumOfUses(ShPtr<Variable> var) const;
	bool isDirRead(ShPtr<Variable> var) const;
	bool isDirWritten(ShPtr<Variable> var) const;
	bool isDirAccessed(ShPtr<Variable> var) const;

	var_iterator dir_read_begin() const;
	var_iterator dir_read_end() const;

	var_iterator dir_written_begin() const;
	var_iterator dir_written_end() const;

	var_iterator dir_all_begin() const;
	var_iterator dir_all_end() const;
	/// @}

	/// @name Indirectly Used Variables Accessors
	/// @{
	// may
	const VarSet &getMayBeReadVars() const;
	const VarSet &getMayBeWrittenVars() const;
	const VarSet &getMayBeAccessedVars() const;
	bool mayBeIndirRead(ShPtr<Variable> var) const;
	bool mayBeIndirWritten(ShPtr<Variable> var) const;
	bool mayBeIndirAccessed(ShPtr<Variable> var) const;

	var_iterator may_be_read_begin() const;
	var_iterator may_be_read_end() const;

	var_iterator may_be_written_begin() const;
	var_iterator may_be_written_end() const;

	var_iterator may_be_accessed_begin() const;
	var_iterator may_be_accessed_end() const;

	// must
	const VarSet &getMustBeReadVars() const;
	const VarSet &getMustBeWrittenVars() const;
	const VarSet &getMustBeAccessedVars() const;
	bool mustBeIndirRead(ShPtr<Variable> var) const;
	bool mustBeIndirWritten(ShPtr<Variable> var) const;
	bool mustBeIndirAccessed(ShPtr<Variable> var) const;

	var_iterator must_be_read_begin() const;
	var_iterator must_be_read_end() const;

	var_iterator must_be_written_begin() const;
	var_iterator must_be_written_end() const;

	var_iterator must_be_accessed_begin() const;
	var_iterator must_be_accessed_end() const;
	/// @}

	/// @name Function Calls Accessors
	/// @{
	const CallVector &getCalls() const;
	bool hasCalls() const;
	std::size_t getNumOfCalls() const;

	call_iterator call_begin() const;
	call_iterator call_end() const;
	/// @}

	/// @name Address Operators Accessors
	/// @{
	bool hasAddressOps() const;
	bool hasAddressTaken(ShPtr<Variable> var) const;
	/// @}

	/// @name Dereferences Accessors
	/// @{
	bool hasDerefs() const;
	/// @}

	/// @name Array Accesses Accessors
	/// @{
	bool hasArrayAccesses() const;
	/// @}

	/// @name Struct Accesses Accessors
	/// @{
	bool hasStructAccesses() const;
	/// @}

private:
	/// Mapping of a variable into a count.
	using VarCountMap = std::map<ShPtr<Variable>, std::size_t>;

private:
	ValueData();

	void clear();

private:
	/// Set of variables that are directly read.
	VarSet dirReadVars;

	/// Set of variables into which something is directly written.
	VarSet dirWrittenVars;

	/// Set of all directly accessed variables (read or written).
	VarSet dirAllVars;

	/// Number of uses of a variable in direct accesses.
	VarCountMap dirNumOfVarUses;

	/// Set of variables that may be read.
	VarSet mayBeReadVars;

	/// Set of variables into which something may be written.
	VarSet mayBeWrittenVars;

	/// Set of variables which may be accessed.
	VarSet mayBeAccessedVars;

	/// Set of variables that must be read.
	VarSet mustBeReadVars;

	/// Set of variables into which something must be written.
	VarSet mustBeWrittenVars;

	/// Set of variables which must be accessed.
	VarSet mustBeAccessedVars;

	/// List of function calls.
	CallVector calls;

	/// Set of variables whose address is taken.
	VarSet addressTakenVars;

	/// Are there any dereferences?
	bool containsDerefs;

	/// Are there any array accesses?
	bool containsArrayAccesses;

	/// Are there any struct accesses?
	bool containsStructAccesses;
};

/**
* @brief An analysis for obtaining information about a value.
*
* Use create() to create instances of this class. Instances of this class have
* reference object semantics.
*
* When you change a module in a way that may affect the results returned by
* getValueData() when caching is enabled, you have to call invalidate(). This
* will bring the analysis into an invalid state so other users of a shared
* analysis will know that they have to validate the analysis before using it.
* Upon calling clearCache(), the analysis gets validated automatically. If you
* modify or remove a statement and call removeFromCache(), then you do not have
* to call invalidate().
*/
class ValueAnalysis: private OrderedAllVisitor,
	private retdec::utils::NonCopyable, public ValidState,
	public Caching<ShPtr<Value>, ShPtr<ValueData>> {

public:
	ShPtr<ValueData> getValueData(ShPtr<Value> value);

	/// @name Caching
	/// @{
	void clearCache();
	void removeFromCache(ShPtr<Value> value, bool recursive = true);
	/// @}

	/// @name Access To Alias Analysis
	/// @{
	void initAliasAnalysis(ShPtr<Module> module);
	const VarSet &mayPointTo(ShPtr<Variable> var) const;
	ShPtr<Variable> pointsTo(ShPtr<Variable> var) const;
	bool mayBePointed(ShPtr<Variable> var) const;
	/// @}

	static ShPtr<ValueAnalysis> create(ShPtr<AliasAnalysis> aliasAnalysis,
		bool enableCaching = false);

private:
	explicit ValueAnalysis(ShPtr<AliasAnalysis> aliasAnalysis,
		bool enableCaching = false);

	void computeAndStoreIndirectlyUsedVars(ShPtr<DerefOpExpr> expr);

	/// @name Visitor Interface
	/// @{
	using OrderedAllVisitor::visit;
	virtual void visit(const ShPtr<Function>& func) override;
	// Statements
	virtual void visit(const ShPtr<AssignStmt>& stmt) override;
	virtual void visit(const ShPtr<BreakStmt>& stmt) override;
	virtual void visit(const ShPtr<CallStmt>& stmt) override;
	virtual void visit(const ShPtr<ContinueStmt>& stmt) override;
	virtual void visit(const ShPtr<EmptyStmt>& stmt) override;
	virtual void visit(const ShPtr<ForLoopStmt>& stmt) override;
	virtual void visit(const ShPtr<UForLoopStmt>& stmt) override;
	virtual void visit(const ShPtr<GotoStmt>& stmt) override;
	virtual void visit(const ShPtr<IfStmt>& stmt) override;
	virtual void visit(const ShPtr<ReturnStmt>& stmt) override;
	virtual void visit(const ShPtr<SwitchStmt>& stmt) override;
	virtual void visit(const ShPtr<UnreachableStmt>& stmt) override;
	virtual void visit(const ShPtr<VarDefStmt>& stmt) override;
	virtual void visit(const ShPtr<WhileLoopStmt>& stmt) override;
	// Expressions
	virtual void visit(const ShPtr<AddOpExpr>& expr) override;
	virtual void visit(const ShPtr<AddressOpExpr>& expr) override;
	virtual void visit(const ShPtr<AndOpExpr>& expr) override;
	virtual void visit(const ShPtr<ArrayIndexOpExpr>& expr) override;
	virtual void visit(const ShPtr<AssignOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitAndOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitOrOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitShlOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitShrOpExpr>& expr) override;
	virtual void visit(const ShPtr<BitXorOpExpr>& expr) override;
	virtual void visit(const ShPtr<CallExpr>& expr) override;
	virtual void visit(const ShPtr<CommaOpExpr>& expr) override;
	virtual void visit(const ShPtr<DerefOpExpr>& expr) override;
	virtual void visit(const ShPtr<DivOpExpr>& expr) override;
	virtual void visit(const ShPtr<EqOpExpr>& expr) override;
	virtual void visit(const ShPtr<GtEqOpExpr>& expr) override;
	virtual void visit(const ShPtr<GtOpExpr>& expr) override;
	virtual void visit(const ShPtr<LtEqOpExpr>& expr) override;
	virtual void visit(const ShPtr<LtOpExpr>& expr) override;
	virtual void visit(const ShPtr<ModOpExpr>& expr) override;
	virtual void visit(const ShPtr<MulOpExpr>& expr) override;
	virtual void visit(const ShPtr<NegOpExpr>& expr) override;
	virtual void visit(const ShPtr<NeqOpExpr>& expr) override;
	virtual void visit(const ShPtr<NotOpExpr>& expr) override;
	virtual void visit(const ShPtr<OrOpExpr>& expr) override;
	virtual void visit(const ShPtr<StructIndexOpExpr>& expr) override;
	virtual void visit(const ShPtr<SubOpExpr>& expr) override;
	virtual void visit(const ShPtr<TernaryOpExpr>& expr) override;
	virtual void visit(const ShPtr<Variable>& var) override;
	// Casts
	virtual void visit(const ShPtr<BitCastExpr>& expr) override;
	virtual void visit(const ShPtr<ExtCastExpr>& expr) override;
	virtual void visit(const ShPtr<FPToIntCastExpr>& expr) override;
	virtual void visit(const ShPtr<IntToFPCastExpr>& expr) override;
	virtual void visit(const ShPtr<IntToPtrCastExpr>& expr) override;
	virtual void visit(const ShPtr<PtrToIntCastExpr>& expr) override;
	virtual void visit(const ShPtr<TruncCastExpr>& expr) override;
	// Constants
	virtual void visit(const ShPtr<ConstArray>& constant) override;
	virtual void visit(const ShPtr<ConstBool>& constant) override;
	virtual void visit(const ShPtr<ConstFloat>& constant) override;
	virtual void visit(const ShPtr<ConstInt>& constant) override;
	virtual void visit(const ShPtr<ConstNullPointer>& constant) override;
	virtual void visit(const ShPtr<ConstString>& constant) override;
	virtual void visit(const ShPtr<ConstStruct>& constant) override;
	/// @}

private:
	/// The used alias analysis.
	ShPtr<AliasAnalysis> aliasAnalysis;

	/// Information about the currently computed value.
	ShPtr<ValueData> valueData;

	/// Are we writing into a variable?
	bool writing;

	/// Are we removing values from the cache?
	bool removingFromCache;
};

} // namespace llvmir2hll
} // namespace retdec

#endif
