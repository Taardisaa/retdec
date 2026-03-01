# Decompiler Pass Analysis (Decoder + Full Pipeline)

This document has two parts:

1. A deep dive into what `retdec-decoder` (the `Decoder` LLVM pass) really does internally.
2. A catalog of all passes in the default decompiler pipeline (`decompParams.llvmPasses`).

Primary evidence:

- Decoder pass registration and core flow: `src/bin2llvmir/optimizations/decoder/decoder.cpp`
- Decoder internals split across files: `src/bin2llvmir/optimizations/decoder/*.cpp`
- Pass pipeline source of truth: `src/retdec-decompiler/decompiler-config.json`

---

## 1) Decoder pass deep dive (`retdec-decoder`)

`retdec-decoder` is a single LLVM `ModulePass` (`RegisterPass<Decoder>`) that performs binary lifting from machine code to LLVM IR. It is registered as:

- ID: `retdec-decoder`
- Description: `Input binary to LLVM IR decoding`
- Source: `src/bin2llvmir/optimizations/decoder/decoder.cpp`

### Internal stages executed by `Decoder::run()`

From `Decoder::run()` in `src/bin2llvmir/optimizations/decoder/decoder.cpp`, the pass executes these phases in order:

1. `initTranslator()`
   - Chooses architecture/backend mode for capstone2llvmir (`x86`, `ARM/Thumb`, `ARM64`, `MIPS`, `PPC`) and endianness.
   - Source: `src/bin2llvmir/optimizations/decoder/decoder_init.cpp`
2. `initDryRunCsInstruction()`
   - Allocates a Capstone instruction object used by dry-run validation.
3. `initEnvironment()`
   - Syncs translator metadata into module/config:
     - asm<->llvm mapping global
     - pseudo call/return/branch/cond-branch functions
     - register globals into ABI/config register model
4. `initRanges()`
   - Builds decode ranges from sections/segments or selected ranges from config.
   - Tracks primary vs alternative ranges and architecture alignment.
5. `initJumpTargets()`
   - Seeds queue of initial targets from config/entry/imports/exports/symbols/debug/static/vtables.
6. `decode()`
   - Main worklist loop: pop jump target, translate instructions, create/split basic blocks/functions, discover new jump targets.
7. `resolvePseudoCalls()`
   - Revisits unresolved pseudo control-flow calls and fixes/removes invalid artifacts.
8. `patternsRecognize()`
   - Applies post-decode control-flow patterns:
     - terminating-call function detection/splitting
     - statically linked pattern handling
9. `finalizePseudoCalls()`
   - Erases pseudo intrinsics and cleans ABI-specific call/return-address stores.
10. `initConfigFunctions()`
    - Writes recovered function info into config model.
11. `initializeGpReg_mips()`
    - MIPS-specific GP register post-init.

### What makes decoder technically important

- It is the only pass that creates the initial LLVM IR from binary bytes; all later passes assume this IR exists.
- It does architecture-specific dry-runs and mode-switch handling (e.g., ARM mode, MIPS32/64 fallback).
- It reconstructs control flow from pseudo calls into real `call`/`br`/`switch` structures.
- It recognizes jump-table switches from symbolic-expression patterns and materializes `SwitchInst`.

### "LLVM passes in decoder" clarification

Decoder itself is one LLVM pass (`ModulePass`), but internally runs many lifting phases as regular C++ methods. It does **not** invoke a nested LLVM pass manager. In other words:

- External pass manager sees one pass: `retdec-decoder`.
- Inside that pass, RetDec implements its own decode pipeline.

---

## 2) Full default pipeline pass catalog

Source: `src/retdec-decompiler/decompiler-config.json` (`decompParams.llvmPasses`).

Computed from current config snapshot:

- Total pass invocations: **142**
- Unique pass IDs: **62**
- Unique RetDec pass IDs (`retdec-*`): **27**
- Unique LLVM/core pass IDs: **35**

### 2.1 RetDec pass IDs (in pipeline) - meaning and frequency

| Pass ID | Freq | Meaning (brief) |
|---|---:|---|
| `retdec-provider-init` | 1 | Initialize analysis providers (config/image/debug/ABI/etc.) used by later passes. |
| `retdec-decoder` | 1 | Decode/lift input machine code into LLVM IR and recover core CFG skeleton. |
| `retdec-x86-addr-spaces` | 1 | Normalize x86-specific address-space artifacts in generated IR. |
| `retdec-x87-fpu` | 1 | Recover/normalize x87 stack-FPU behavior. |
| `retdec-main-detection` | 1 | Detect likely `main` and startup-to-user entry linkage. |
| `retdec-idioms-libgcc` | 1 | Recognize/simplify libgcc helper idioms. |
| `retdec-inst-opt` | 5 | RetDec instruction-level canonicalization and cleanup. |
| `retdec-cond-branch-opt` | 1 | Simplify conditional branch forms. |
| `retdec-syscalls` | 1 | Recover/fix syscall semantics and call representation. |
| `retdec-stack` | 1 | Recover stack-frame semantics and stack object accesses. |
| `retdec-constants` | 1 | Resolve/rewrite constants into more meaningful forms. |
| `retdec-param-return` | 1 | Infer/update function parameters and return values. |
| `retdec-inst-opt-rda` | 1 | RetDec instruction optimization driven by reaching definitions. |
| `retdec-simple-types` | 2 | Recover/propagate simple type information. |
| `retdec-write-dsm` | 1 | Emit disassembly output. |
| `retdec-remove-asm-instrs` | 1 | Remove asm-mapping helper instructions from IR stream. |
| `retdec-class-hierarchy` | 1 | Reconstruct C++ class hierarchy hints. |
| `retdec-select-fncs` | 1 | Keep selected functions and prune others as configured. |
| `retdec-unreachable-funcs` | 1 | Remove unreachable functions from recovered module. |
| `retdec-register-localization` | 1 | Localize register-like state into SSA-friendlier structure. |
| `retdec-value-protect` | 2 | Protect critical recovered values from harmful late transforms. |
| `retdec-stack-ptr-op-remove` | 1 | Remove residual explicit stack-pointer arithmetic patterns. |
| `retdec-idioms` | 2 | Recognize broader compiler idioms and lift to cleaner forms. |
| `retdec-remove-phi` | 1 | Remove/lower PHI nodes for downstream compatibility/output constraints. |
| `retdec-write-ll` | 1 | Write resulting LLVM IR text (`.ll`). |
| `retdec-write-bc` | 1 | Write resulting LLVM bitcode (`.bc`). |
| `retdec-llvmir2hll` | 1 | Convert LLVM IR to high-level language representation/output. |

### 2.2 LLVM/core pass IDs (in pipeline) - meaning and frequency

| Pass ID | Freq | Meaning (brief) |
|---|---:|---|
| `verify` | 2 | Run LLVM IR verifier for structural/semantic correctness checks. |
| `instcombine` | 15 | Canonical peephole/algebraic instruction combining and simplification. |
| `tbaa` | 4 | Type-based alias-analysis metadata pass used by memory optimizations. |
| `basicaa` | 4 | Baseline alias analysis for pointer disambiguation. |
| `simplifycfg` | 10 | Simplify control-flow graph (merge blocks, fold branches, remove clutter). |
| `early-cse` | 4 | Early common-subexpression elimination for simple redundancies. |
| `globalopt` | 2 | Optimize global variables/initializers and related uses. |
| `mem2reg` | 2 | Promote stack allocas to SSA registers where possible. |
| `lazy-value-info` | 4 | Query/provide inferred value-range information lazily. |
| `jump-threading` | 4 | Thread jumps through known conditions to reduce branches. |
| `correlated-propagation` | 4 | Propagate condition correlations to simplify dependent branches. |
| `reassociate` | 2 | Reassociate expressions to expose optimization opportunities. |
| `loops` | 3 | Loop pass manager driver/group pass anchor. |
| `loop-simplify` | 4 | Canonicalize loop structure (preheaders/latches/exits). |
| `lcssa` | 6 | Put loop values into LCSSA form for safe loop transforms. |
| `loop-rotate` | 2 | Rotate loops to canonical do-while-like shape. |
| `licm` | 2 | Loop-invariant code motion. |
| `loop-simplifycfg` | 2 | CFG simplification specialized for loop regions. |
| `aa` | 2 | Alias-analysis aggregation pipeline entry point. |
| `loop-accesses` | 2 | Analyze loop memory accesses/dependences. |
| `loop-load-elim` | 2 | Eliminate redundant loop loads when provably safe. |
| `indvars` | 2 | Canonicalize and simplify induction variables. |
| `loop-idiom` | 2 | Recognize loop idioms (e.g., memset/memcpy-like patterns). |
| `loop-deletion` | 2 | Delete loops proven to have no side effects/impact. |
| `gvn` | 2 | Global value numbering / redundancy elimination. |
| `sccp` | 2 | Sparse conditional constant propagation. |
| `dse` | 2 | Dead store elimination. |
| `bdce` | 2 | Bit-tracking dead code elimination. |
| `adce` | 2 | Aggressive dead code elimination. |
| `strip-dead-prototypes` | 2 | Remove unused/dead function prototypes/declarations. |
| `globaldce` | 2 | Remove dead globals/functions unreachable from roots. |
| `constmerge` | 2 | Merge duplicate global constants. |
| `constprop` | 2 | Constant propagation/folding. |
| `sink` | 1 | Sink instructions to colder/later blocks where legal. |
| `scalar-evolution` | 1 | Scalar-evolution analysis for loop/value progression. |

---

## 3) Most important passes (practical ranking)

1. `retdec-decoder` - without lifting, there is no decompilation pipeline.
2. `retdec-param-return` - heavily impacts function signatures and readability.
3. `retdec-simple-types` + `retdec-idioms` - major readability/high-level reconstruction gains.
4. `instcombine` + `simplifycfg` - repeated canonical cleanup backbone around RetDec-specific transforms.
