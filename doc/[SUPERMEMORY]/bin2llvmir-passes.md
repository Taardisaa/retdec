# bin2llvmir Pass Map

This document summarizes `bin2llvmir` LLVM passes registered in `src/bin2llvmir/optimizations/**`.
Pass IDs and one-line descriptions come from each pass registration (`RegisterPass<...>`), and ordering context comes from `src/retdec-decompiler/decompiler-config.json`.

## Passes (brief meaning)

| Pass ID | Class | Meaning (brief) | Evidence |
|---|---|---|---|
| `retdec-provider-init` | `ProviderInitialization` | Initializes core providers (config, image, ABI/calling convention, debug, names) needed by later passes. | `src/bin2llvmir/optimizations/provider_init/provider_init.cpp` |
| `retdec-decoder` | `Decoder` | Core lifter: decodes machine instructions/basic blocks/functions into LLVM IR. | `src/bin2llvmir/optimizations/decoder/decoder.cpp` |
| `retdec-x86-addr-spaces` | `X86AddressSpacesPass` | Normalizes/fixes x86 address-space usage in generated IR memory operations. | `src/bin2llvmir/optimizations/x86_addr_spaces/x86_addr_spaces_pass.cpp` |
| `retdec-x87-fpu` | `X87FpuAnalysis` | Reconstructs x87 FPU register-stack semantics and rewrites related IR patterns. | `src/bin2llvmir/optimizations/x87_fpu/x87_fpu.cpp` |
| `retdec-main-detection` | `MainDetection` | Detects likely `main` entry and links startup wrappers to user code entry. | `src/bin2llvmir/optimizations/main_detection/main_detection.cpp` |
| `retdec-idioms-libgcc` | `IdiomsLibgcc` | Recognizes libgcc helper/compiler-runtime idioms and simplifies them. | `src/bin2llvmir/optimizations/idioms_libgcc/idioms_libgcc.cpp` |
| `retdec-inst-opt` | `InstructionOptimizer` | Custom instruction-level cleanup/canonicalization on LLVM IR. | `src/bin2llvmir/optimizations/inst_opt/inst_opt_pass.cpp` |
| `retdec-cond-branch-opt` | `CondBranchOpt` | Simplifies conditional branch patterns and branch conditions. | `src/bin2llvmir/optimizations/cond_branch_opt/cond_branch_opt.cpp` |
| `retdec-syscalls` | `SyscallFixer` | Identifies syscall patterns and converts them to clearer semantic calls/metadata. | `src/bin2llvmir/optimizations/syscalls/syscalls.cpp` |
| `retdec-stack` | `StackAnalysis` | Recovers stack-frame semantics and stack-based accesses/objects. | `src/bin2llvmir/optimizations/stack/stack.cpp` |
| `retdec-constants` | `ConstantsAnalysis` | Rewrites/annotates constants to more meaningful forms (addresses, symbolic values). | `src/bin2llvmir/optimizations/constants/constants.cpp` |
| `retdec-param-return` | `ParamReturn` | Infers function parameters and return values, then updates signatures/call sites. | `src/bin2llvmir/optimizations/param_return/param_return.cpp` |
| `retdec-inst-opt-rda` | `InstructionRdaOptimizer` | Instruction optimization driven by reaching-definitions dataflow analysis. | `src/bin2llvmir/optimizations/inst_opt_rda/inst_opt_rda_pass.cpp` |
| `retdec-simple-types` | `SimpleTypesAnalysis` | Recovers simple data types from IR usage and propagates inferred types. | `src/bin2llvmir/optimizations/simple_types/simple_types.cpp` |
| `retdec-write-dsm` | `DsmWriter` | Emits disassembly output from the current module state. | `src/bin2llvmir/optimizations/writer_dsm/writer_dsm.cpp` |
| `retdec-remove-asm-instrs` | `AsmInstructionRemover` | Removes helper/mapping assembly pseudo-instructions from final IR. | `src/bin2llvmir/optimizations/asm_inst_remover/asm_inst_remover.cpp` |
| `retdec-class-hierarchy` | `ClassHierarchyAnalysis` | Reconstructs class hierarchy relations (notably C++) from recovered artifacts. | `src/bin2llvmir/optimizations/class_hierarchy/hierarchy_analysis.cpp` |
| `retdec-select-fncs` | `SelectFunctions` | Keeps/decompiles selected functions and prunes unrelated code paths. | `src/bin2llvmir/optimizations/select_functions/select_functions.cpp` |
| `retdec-unreachable-funcs` | `UnreachableFuncs` | Removes unreachable functions from module call graph roots. | `src/bin2llvmir/optimizations/unreachable_funcs/unreachable_funcs.cpp` |
| `retdec-register-localization` | `RegisterLocalization` | Converts global/register-like state into more local SSA-friendly form. | `src/bin2llvmir/optimizations/register_localization/register_localization.cpp` |
| `retdec-value-protect` | `ValueProtect` | Marks/protects critical values against destructive simplifications before output. | `src/bin2llvmir/optimizations/value_protect/value_protect.cpp` |
| `retdec-idioms` | `Idioms` | General compiler-idiom recognition and replacement into higher-level constructs. | `src/bin2llvmir/optimizations/idioms/idioms.cpp` |
| `retdec-stack-ptr-op-remove` | `StackPointerOpsRemove` | Eliminates explicit stack-pointer arithmetic ops after higher-level recovery. | `src/bin2llvmir/optimizations/stack_pointer_ops/stack_pointer_ops.cpp` |
| `retdec-remove-phi` | `PhiRemover` | Lowers/removes PHI nodes near pipeline end for downstream compatibility/output. | `src/bin2llvmir/optimizations/phi_remover/phi_remover.cpp` |
| `retdec-write-ll` | `LlvmIrWriter` | Writes current LLVM IR (`.ll`) output. | `src/bin2llvmir/optimizations/writer_ll/writer_ll.cpp` |
| `retdec-write-bc` | `BitcodeWriter` | Writes current LLVM bitcode (`.bc`) output. | `src/bin2llvmir/optimizations/writer_bc/writer_bc.cpp` |
| `retdec-write-config` | `ConfigWriter` | Writes current analyzed configuration metadata. | `src/bin2llvmir/optimizations/writer_config/writer_config.cpp` |
| `retdec-dump-module` | `DumpModule` | Debug/diagnostic pass to dump module state. | `src/bin2llvmir/optimizations/dump_module/dump_module.cpp` |
| `retdec-types-propagation` | `TypesPropagator` | Propagates known type information through IR uses/defs. | `src/bin2llvmir/optimizations/types_propagator/types_propagator.cpp` |

## Notes on usage/order

- Default decompiler pipeline ordering is in `src/retdec-decompiler/decompiler-config.json` (`llvmPasses`).
- The earliest `bin2llvmir` stages are `retdec-provider-init` then `retdec-decoder`, followed by recovery/cleanup passes.
- Not all registered passes are necessarily in the default list for every run profile.
- `retdec-types-propagation` is registered, but `types_propagator.cpp` is not listed in `src/bin2llvmir/CMakeLists.txt` in this snapshot.

## Most important pass (my pick)

**`retdec-decoder` (`Decoder`)** is the most important pass.

Why:

1. It is the lifter that actually turns input binary code into LLVM IR; without it, later passes have nothing meaningful to optimize.
2. It is at the front of the default pipeline (`retdec-provider-init` -> `retdec-decoder`) in `src/retdec-decompiler/decompiler-config.json`.
3. Even the minimal disassembly path in `src/retdec/retdec.cpp` runs just provider initialization + decoder to produce a module.
